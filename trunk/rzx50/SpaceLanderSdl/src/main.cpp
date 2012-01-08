//
// This source file is a part of Space Lander game.
// 
// Copyright (C) 2011-2012 by Dmitry Simakov <dosimakov@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <SDL.h>
#include <SDL_mixer.h> 
#include <SDL_framerate.h>
#include <SDL_gfxPrimitives.h>
#include "lander.h"
#include "level.h"

// TODO(DS) Move this to configuration file
#define FPS 30
#define SCREEN_WIDTH 480 
#define SCREEN_HEIGHT 272 
#define AUDIO_RATE 22050
#define AUDIO_FORMAT AUDIO_S16
#define AUDIO_CHANNELS 2
#define AUDIO_BUFFER 512
#define GAME_MAX_WAIT_FRAMES (FPS*3+FPS/2)
#define LANDER_FUEL_CAPACITY 10000
#define LANDER_ENGINE_SOUND_FILE  "data/lander_engine.wav"
#define LANDER_EXPLODE_SOUND_FILE "data/lander_explode.wav"
#define GAME_WINDOW_TITLE "Space Lander v 0.6.0"
#define GAME_LANDER_ZOOM_IN_ALT 180 
#define GAME_LANDER_ZOOM_OUT_ALT 300
#define GAME_LANDER_ZOOM_IN 1.0f  
#define GAME_LANDER_ZOOM_OUT 0.35f

// game modes
enum 
{ 
    GAME_IDLE,
    GAME_PLAY,
    GAME_WAIT, // landing or crash
    GAME_PAUSE,
    GAME_OVER,
};

// Global variables only for this module
static SDL_Surface*  DISPLAY = 0;
static SDL_Joystick* JOYSTICK = 0;
static bool          JOYSTICK_CONTROL = false;
static bool          RUNNING = true;
static Level         LEVEL;
static Lander        LANDER;
static float         THROTTLE = 0;
static float         LAST_THROTTLE = 0;
static FPSmanager    FPS_MANAGER;
static Mix_Chunk*    ENGINE_SOUND = 0;
static Mix_Chunk*    EXPLODE_SOUND = 0;
static int           GAME_MODE = GAME_IDLE;
static int           WAIT_ELAPSED_FRAMES = 0;
static bool          ZOOM_FIRST_TIME_FLAG = false; 
static float         LAST_ZOOM_OUT_X = 0;
static float         LAST_ZOOM_OUT_Y = 0;

// Global variables shared across all modules
int                  COLLISION = LEVEL_IN_FLIGHT;
ScreenInfo           SCREEN;
Game                 GAME;


/**
 * Makes screenshot.
 *
 * Output format is BMP, the picture is saved to the working directory,
 * with filename format: YYYY_MM_DD_HHMMSS[_N].bmp, where YYYY_MM_DD_HHMMSS is 
 * current timestamp and _N is optional suffix if file with the same name 
 * exists (trying to make more than one screenshot per second).
 */
void game_make_screenshot()
{
    // No display - no deal...
    if (!DISPLAY)
        return;

    // Make current timestamp.
    std::string ts;
    util_current_time_to_string(ts);

    // If file exists try adding suffix to make it unique
    if (util_check_filename(ts + ".bmp"))
    {
        // Maximum attempts
        const int max_attempts = 24;

        for (int i=0; i<max_attempts; ++i)
        {
            // Make the file name
            std::ostringstream oss;
            oss << "_" << i << ".bmp";

            if (!util_check_filename(ts + oss.str()))
            {
                ts += oss.str();
                break;
            }
        }

        // Exceeds limit of attempts - no uniq filename generated
        if (*ts.rbegin() != 'p')
            throw std::runtime_error("game_make_screenshot could not generate filename");
    }
    else
    {
        ts += ".bmp";
    }
    
    // Finally call SDL's built-in function to save the screenshot, no need to
    // check return value.
    SDL_SaveBMP(DISPLAY, ts.c_str());
}

void game_handle_zoom(bool force_zoom_out)
{
    if (force_zoom_out ||
        (LANDER.altitude >=GAME_LANDER_ZOOM_OUT_ALT && 
         equal(SCREEN.zoom, GAME_LANDER_ZOOM_IN) && ZOOM_FIRST_TIME_FLAG))
    {
        SCREEN.zoom = GAME_LANDER_ZOOM_OUT;
        if (!force_zoom_out)
        {
            float left_trigger_screen = GAME.screen_width/2;
            float bottom_trigger_screen = GAME.screen_height - GAME.game_scroll_ytrigger;
            float left_trigger_units = left_trigger_screen / SCREEN.zoom;
            float bottom_trigger_units = GAME.screen_height - (bottom_trigger_screen) / SCREEN.zoom;
            
            // smart zoom, when we go back to zoom out mode, just restore the previous settings if so
            if (!equal(LAST_ZOOM_OUT_Y, 0.0f))
                SCREEN.yscroll = LAST_ZOOM_OUT_Y;
            else
                SCREEN.yscroll = - bottom_trigger_units + (LANDER.pos.y - LANDER.altitude);
                
            if (!equal(LAST_ZOOM_OUT_X, 0.0f))
                SCREEN.xscroll = LAST_ZOOM_OUT_X;
            else
                SCREEN.xscroll = left_trigger_units - LANDER.pos.x;
        }
    }
    else
    if (LANDER.altitude < GAME_LANDER_ZOOM_IN_ALT && equal(SCREEN.zoom, GAME_LANDER_ZOOM_OUT))
    {
        // save current zoom out settings to restore them when we go back to zoom out mode
        LAST_ZOOM_OUT_X = SCREEN.xscroll;
        LAST_ZOOM_OUT_Y = SCREEN.yscroll;

        SCREEN.zoom = GAME_LANDER_ZOOM_IN;
        ZOOM_FIRST_TIME_FLAG = true;
        float left_trigger_screen = GAME.screen_width/2;
        float bottom_trigger_screen = GAME.screen_height - GAME.screen_height/2;// - GAME.game_scroll_ytrigger;
        float left_trigger_units = left_trigger_screen / SCREEN.zoom;
        float bottom_trigger_units = GAME.screen_height - (bottom_trigger_screen) / SCREEN.zoom;
        SCREEN.yscroll = - bottom_trigger_units + LANDER.pos.y;
        SCREEN.xscroll = left_trigger_units - LANDER.pos.x;
    }
}

void game_handle_scrolling()
{
    if (!LANDER.explode)
    {
        float x, y;

        // scroll triggers
        float right_trigger_screen = GAME.screen_width - GAME.game_scroll_xtrigger;
        float left_trigger_screen = GAME.game_scroll_xtrigger;
        float top_trigger_screen = GAME.game_scroll_ytrigger;
        float bottom_trigger_screen = GAME.screen_height - GAME.game_scroll_ytrigger;
        float right_trigger_units = right_trigger_screen / SCREEN.zoom;
        float left_trigger_units = left_trigger_screen / SCREEN.zoom;
        float top_trigger_units = GAME.screen_height - (top_trigger_screen) / SCREEN.zoom;
        float bottom_trigger_units = GAME.screen_height - (bottom_trigger_screen) / SCREEN.zoom;

        x = level_map_to_screen_x(LANDER.pos.x);
        y = level_map_to_screen_y(LANDER.pos.y);

        if (y < top_trigger_screen && LANDER.vel.y > 0)
            SCREEN.yscroll = - top_trigger_units + LANDER.pos.y;

        if (y > bottom_trigger_screen && LANDER.vel.y < 0)
            SCREEN.yscroll = - bottom_trigger_units + LANDER.pos.y;

        if (x > right_trigger_screen && LANDER.vel.x > 0)
            SCREEN.xscroll = right_trigger_units - LANDER.pos.x;

        if (x < left_trigger_screen && LANDER.vel.x < 0)
            SCREEN.xscroll = left_trigger_units - LANDER.pos.x;
    }
}

// Launches the lander, if new_game is true - full tank of fuel and reset 
// score.
void game_launch_lander(bool new_game)
{
    // Prepare lander for launching
    lander_init(LANDER,                  // Lander 
                GAME.lander_launch_xpos, // Launching X position in world space
                GAME.lander_launch_ypos, // Launching Y position in world space
                GAME.lander_size,        // Lander size in world units
                Color::WHITE,            // Lander color
                Color::WHITE);           // Lander engine flame color

    // Set lanching velocity
    LANDER.vel.set(GAME.lander_launch_xspeed, GAME.lander_launch_yspeed);

    // Refill fuel and reset score in case of new game.
    if (new_game)
    {
        LANDER.score = 0;
        LANDER.last_landing_score = 0;
        LANDER.fuel = GAME.lander_fuel_capacity;
    }
    
    // Generate areas with bonuses for landing
    level_generate_best_landing_areas(LEVEL, LANDER, 3);

    // We must start in Zoom-Out mode, so call zoom handing with forced zoom-outing. 
    game_handle_zoom(true);
    
    // Tune scrolling in a way to make the launching position on intersection of the
    // Left and top scrolling trigger lines.
    float left_trigger_screen = GAME.game_scroll_xtrigger;
    float top_trigger_screen = GAME.game_scroll_ytrigger;
    float left_trigger_units = left_trigger_screen / SCREEN.zoom;
    float top_trigger_units = GAME.screen_height - (top_trigger_screen) / SCREEN.zoom;
    SCREEN.yscroll = - top_trigger_units + LANDER.pos.y;
    SCREEN.xscroll = left_trigger_units - LANDER.pos.x;
}

void game_initialize()
{
    int bits_per_pixel = 32;

    //
    // reset random seed
    //
    srand(SDL_GetTicks());
    
    //
    // init SDL
    //
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        std::ostringstream oss;
        oss << "SDL_Init fails: " << SDL_GetError();
        throw std::runtime_error(oss.str());
    }
    
    SDL_ShowCursor(SDL_DISABLE);

    //
    // init video
    //
    if (!(DISPLAY = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, bits_per_pixel, SDL_HWSURFACE | SDL_DOUBLEBUF)))
    {
        std::ostringstream oss;
        oss << "SDL_SetVideoMode fails: " << SDL_GetError() << ". "
            << "Requested configuration width=" << SCREEN_WIDTH << " "
            << "height=" << SCREEN_HEIGHT << " "
            << "bpp=" << bits_per_pixel;
        throw std::runtime_error(oss.str());
    } 
  
    // set window icon and caption
    SDL_WM_SetCaption(GAME_WINDOW_TITLE, 0);


    //
    // init audio
    //
    if (Mix_OpenAudio(AUDIO_RATE, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_BUFFER))
    {
        std::ostringstream oss;
        oss << "Mix_OpenAudio fails: " << Mix_GetError() 
            << ". Requested configuration: "
            << "rate=" << AUDIO_RATE << " format=" << AUDIO_FORMAT 
            << " channels=" << AUDIO_CHANNELS << " buffers=" << AUDIO_BUFFER;
        throw std::runtime_error(oss.str());
    }

    // 
    // init joystick
    //
    if (SDL_NumJoysticks())
    {
        // open the first joystick if any 

        SDL_JoystickEventState(SDL_ENABLE);
        JOYSTICK = SDL_JoystickOpen(0);
    }

    if (!(ENGINE_SOUND = Mix_LoadWAV(LANDER_ENGINE_SOUND_FILE)))
    {
        std::ostringstream oss;
        oss << "Mix_LoadWAV fails: "
            << Mix_GetError() << ". "
            << "requested sound name '" << LANDER_ENGINE_SOUND_FILE << "'";
        throw std::runtime_error(oss.str());
    }
    if (!(EXPLODE_SOUND = Mix_LoadWAV(LANDER_EXPLODE_SOUND_FILE)))
    {
        std::ostringstream oss;
        oss << "Mix_LoadWAV fails: "
            << Mix_GetError() << ". "
            << "requested sound name '" << LANDER_EXPLODE_SOUND_FILE << "'";
        throw std::runtime_error(oss.str());
    }


    //
    // init fps manager
    //
    SDL_initFramerate(&FPS_MANAGER);
    SDL_setFramerate(&FPS_MANAGER, FPS);

    //
    // init game settings
    //
    GAME.screen_width = SCREEN_WIDTH;
    GAME.screen_height = SCREEN_HEIGHT;

    GAME.level_map_width = GAME.screen_width * 3;
    GAME.level_map_height = GAME.screen_height;
    GAME.level_max_stars = 50;
    GAME.level_allowed_landing_roll = 5;
    GAME.level_allowed_soft_landing_xspeed = 15;
    GAME.level_allowed_soft_landing_yspeed = 15;
    GAME.level_allowed_hard_landing_xspeed = 25;
    GAME.level_allowed_hard_landing_yspeed = 25;
    GAME.level_minimum_intersection_with_landing_area = 85;  

    GAME.level_landing_size_factor = 4;
    GAME.lander_air_friction = 0.01;
    GAME.level_gravity = -9.8;
    GAME.lander_engine_power = GAME.level_gravity * 3;

    GAME.lander_module_explode_xspeed = -1;
    GAME.lander_module_explode_yspeed = 1;
    GAME.lander_base_explode_xspeed = 1;
    GAME.lander_base_explode_yspeed = 1;
    GAME.lander_left_leg_explode_xspeed = -1;
    GAME.lander_left_leg_explode_yspeed = 0.5;
    GAME.lander_right_leg_explode_xspeed = 1;
    GAME.lander_right_leg_explode_yspeed = 0.5;
    GAME.lander_nozzle_explode_xspeed = -0.2;
    GAME.lander_nozzle_explode_yspeed = 1;
    GAME.lander_fuel_speed = 10;
    GAME.lander_fuel_capacity = 1000;

    GAME.fps = FPS;
    GAME.lander_rot_step = 10;
    GAME.lander_throttle_step = 0.2;
    GAME.lander_fuel_speed = 10;
    GAME.lander_launch_xpos = 110;
    GAME.lander_launch_ypos = 1000;
    GAME.lander_launch_xspeed = 90;
    GAME.lander_launch_yspeed = 0;
    GAME.lander_launch_angle = 90;
    GAME.lander_size = 4;
    GAME.game_scroll_ytrigger = 100;
    GAME.game_scroll_xtrigger = 70;
    GAME.game_show_debug_objects = false;//turn it on for debugging
    GAME.game_speed_limit = 400; // unit

    GAME.level_builder_max_height = 800;
    GAME.level_builder_min_height = 20;

    // 
    // init level and launch lander 
    // 
    game_launch_lander(true);
    level_init(LEVEL, LANDER, Color::WHITE);
    level_generate_best_landing_areas(LEVEL, LANDER, 3);
}

void game_cleanup()
{   
    //
    // cleanup resources
    //
    if (ENGINE_SOUND)
    {
        Mix_FreeChunk(ENGINE_SOUND);
        ENGINE_SOUND = 0;
    }
    if (EXPLODE_SOUND)
    {
        Mix_FreeChunk(EXPLODE_SOUND);
        EXPLODE_SOUND = 0;
    }

    // close joystick if any
    if (JOYSTICK)
    {
        SDL_JoystickClose(0);
        JOYSTICK = 0;
    }
    
    //
    // cleanup audio
    //
    Mix_CloseAudio();

    //
    // cleanup video
    //
    if (DISPLAY)
    {
        SDL_FreeSurface(DISPLAY);
        DISPLAY = 0;
    }

    SDL_Quit();
}

void game_handle_event(SDL_Event & event)
{
    // Make screenshot
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
        game_make_screenshot();

    // No Yoda conditions :)
    if (event.type == SDL_QUIT) 
        RUNNING = false;

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
        RUNNING = false;
}

void game_handle_input()
{
    Uint8* key_table;
    Uint8 key_state;
    
    key_table = SDL_GetKeyState(0);
    key_state = SDL_GetMouseState(0, 0);

    if (GAME_MODE == GAME_PLAY)
    {
        if (JOYSTICK_CONTROL)
        {
            float yjoy = std::abs(SDL_JoystickGetAxis(JOYSTICK, 1)) / 32768.0; 
            bool left_button = SDL_JoystickGetAxis(JOYSTICK, 3) < -32768.0 / 2; 
            bool right_button = SDL_JoystickGetAxis(JOYSTICK, 3) > 32768.0 / 2; 

            THROTTLE = yjoy;

            if (left_button)
                LANDER.angle_deg -= GAME.lander_rot_step;
            if (right_button)
                LANDER.angle_deg += GAME.lander_rot_step;
        }
        else
        {
            if (key_table[SDLK_LEFT])
            {
                LANDER.angle_deg -= GAME.lander_rot_step;
            }

            if (key_table[SDLK_RIGHT])
            {
                LANDER.angle_deg += GAME.lander_rot_step;
            }

            if (key_table[SDLK_UP] || key_table[SDLK_LCTRL])
            {
                THROTTLE += GAME.lander_throttle_step;
            }
            else
            {
                THROTTLE -= GAME.lander_throttle_step;
            }
        }

        if (THROTTLE > 1)
            THROTTLE = 1;
        if (THROTTLE < 0)
            THROTTLE = 0;

        if (LANDER.fuel == 0)
            THROTTLE = 0;
    }
    else
    {
        THROTTLE = 0;
    }

    if (GAME_MODE == GAME_IDLE)
    {   
        if (key_table[SDLK_LCTRL])
        {
            GAME_MODE = GAME_PLAY;
            game_launch_lander(true);
        }
    }
}

void game_sound_update()
{
    // Fade-out engine sound time in microseconds
    float fade_time_ms = (20.0/FPS) * 1000;

    //
    // TODO(DS) smarter sound manage !!!!!!!!! several channels
    //

    if (GAME_MODE == GAME_PLAY)
    {
        // If throttle is not 0 and chanel is not fading out or playing then
        // start playing engine sound.
        if(!equal(THROTTLE, 0.0f) && 
           !Mix_Playing(1) && 
           !Mix_FadingChannel(1))
        {
            Mix_PlayChannel(1, ENGINE_SOUND, -1);
        }

        // If throttle is 0 and it was not 0 last time and the channel is not 
        // fading out currently - then fade it out.
        if (equal(THROTTLE, 0.0f) && 
            !equal(LAST_THROTTLE, THROTTLE) && 
            !Mix_FadingChannel(1))
        {
            Mix_FadeOutChannel(1, fade_time_ms);
        }

        LAST_THROTTLE = THROTTLE;
    }
    else if (GAME_MODE == GAME_WAIT)
    {
        // depends on collision 
        if (COLLISION == LEVEL_CRASHED && WAIT_ELAPSED_FRAMES == 0)
        {
            // play explode sound
            Mix_HaltChannel(1);
            Mix_PlayChannel(1, EXPLODE_SOUND, 0);
        }
        else if (COLLISION == LEVEL_LANDED || COLLISION == LEVEL_LANDED_HARD)
        {
            // fade out engine sound if so
            if (Mix_Playing(1) && !Mix_FadingChannel(1))
            {
                Mix_FadeOutChannel(1, fade_time_ms);
            }
        }
    }
}

void game_draw_texts()
{
    if (GAME_MODE == GAME_PLAY || GAME_MODE == GAME_WAIT)
    {
        // FIXME(DS)
        // For the following operations blocks {} are used, because I dont know
        // a good way to clean up the std::ostringstream object, and from other 
        // side I dont want to declare lots of std::ostringstream objects for 
        // every draw_text call. 
        // That's why I am using scope operator here.
        int x = 10, y = 10, step = 10;
        {
            // Draw score 
            std::ostringstream oss;
            oss << std::setw(10) << std::left << "SCORE" 
                << std::right << std::setw(5) 
                << (int)LANDER.score;
            draw_text(oss.str(),    // text 
                      vec_t(x, y),  // text position
                      Color::RED,   // text color
                      false,        // world space
                      false,        // center by x
                      false);       // center by y
        }
        {
            // Draw altitude 
            y += step; 
            std::ostringstream oss;
            oss << std::setw(10) << std::left << "ALTITUDE" 
                << std::right << std::setw(5) 
                << (int)LANDER.altitude;
            draw_text(oss.str(),    // text 
                      vec_t(x, y),  // text position
                      Color::RED,   // text color
                      false,        // world space
                      false,        // center by x
                      false);       // center by y
        }
        {
            // Draw horizontal speed
            y += step; 
            std::ostringstream oss;
            oss << std::setw(10) << std::left << "H-SPEED" 
                << std::right << std::setw(5) 
                << (int)LANDER.vel.x;
            draw_text(oss.str(),    // text 
                      vec_t(x, y),  // text position
                      Color::RED,   // text color
                      false,        // world space
                      false,        // center by x
                      false);       // center by y
        }
        {
            // Draw vertical speed
            y += step; 
            std::ostringstream oss;
            oss << std::setw(10) << std::left << "V-SPEED" 
                << std::right << std::setw(5)
                << (int)LANDER.vel.y;
            draw_text(oss.str(),    // text 
                      vec_t(x, y),  // text position
                      Color::RED,   // text color
                      false,        // world space
                      false,        // center by x
                      false);       // center by y
        }
        {
            // Draw left fuel 
            y += step; 
            std::ostringstream oss;
            oss << std::setw(10) << std::left << "FUEL" 
                << std::right << std::setw(5) 
                << (int)LANDER.fuel;
            draw_text(oss.str(),    // text 
                      vec_t(x, y), // text position
                      Color::RED,   // text color
                      false,        // world space
                      false,        // center by x
                      false);       // center by y
        }

        if (GAME_MODE == GAME_WAIT)
        {
            std::ostringstream oss;
            if (COLLISION == LEVEL_CRASHED)
                oss << "CRASHED";
            else if (COLLISION == LEVEL_LANDED)
                oss << "LANDED, score = " << LANDER.last_landing_score;
            else if (COLLISION == LEVEL_LANDED_HARD)
                oss << "LANDED HARD, score = " << LANDER.last_landing_score;

            draw_text(oss.str(),                     // text 
                      vec_t(GAME.screen_width/2, 
                            GAME.screen_height/2),   // text position
                      Color::RED,                    // text color
                      false,                         // world space 
                      true,                          // center text by x
                      true);                         // center text by y
        }
    }
    else if (GAME_MODE == GAME_OVER)
    {
        std::ostringstream oss;
        oss << "GAME OVER, total score " << LANDER.score;
        draw_text(oss.str(),                     // text 
                  vec_t(GAME.screen_width/2, 
                        GAME.screen_height/2),   // text position
                  Color::RED,                    // text color
                  false,                         // world space 
                  true,                          // center text by x
                  true);                         // center text by y
    }
    else if (GAME_MODE == GAME_IDLE)
    {
        std::ostringstream oss;
        oss << "CLICK TO START";
        draw_text(oss.str(),                     // text 
                  vec_t(GAME.screen_width/2, 
                        GAME.screen_height/2),   // text position
                  Color::RED,                    // text color
                  false,                         // world space 
                  true,                          // center text by x
                  true);                         // center text by y
    }
}

void game_main_loop()
{
    SDL_Event event;

    while(RUNNING)
    {
        // poll events
        while(SDL_PollEvent(&event)) 
            game_handle_event(event); 

        // update game objects
        game_handle_input();
        game_sound_update();

        // clear the screen
        SDL_FillRect(DISPLAY, 0, SDL_MapRGB(DISPLAY->format,0,0,0));

        //
        // handle game states
        //

        if (GAME_MODE == GAME_PLAY || GAME_MODE == GAME_IDLE)
        {
            // gameplay or idle - update physics and check collisions
            // in case of idle - relaunch LANDER without fuel on each 
            // collision

            lander_update_physics(LANDER, THROTTLE, 1.0/FPS);
            lander_update_points(LANDER, THROTTLE);
            COLLISION = level_check_collisions(LEVEL, LANDER);
            game_handle_zoom(false);
            game_handle_scrolling();
            if (COLLISION != LEVEL_IN_FLIGHT && GAME_MODE == GAME_IDLE)
            {
                game_launch_lander(false);
                COLLISION = LEVEL_IN_FLIGHT;
            }
            if (COLLISION != LEVEL_IN_FLIGHT)
            {
                GAME_MODE = GAME_WAIT;
            }
        }
        else 
        if (GAME_MODE == GAME_WAIT)
        {
            // landing or crash
            // in case of crash - run explosion animation then 
            // depend on the fuel relaunch LANDER without fuel renew 
            // or switch to Game Over

            if (WAIT_ELAPSED_FRAMES == 0)
            {
                if (COLLISION == LEVEL_CRASHED)
                {
                    lander_explode(LANDER);
                    ZOOM_FIRST_TIME_FLAG = false;
                }
            }
            lander_update_points(LANDER, 0);
            WAIT_ELAPSED_FRAMES++;

            // next mode is ?
            if (WAIT_ELAPSED_FRAMES == GAME_MAX_WAIT_FRAMES)
            {
                if (LANDER.fuel > 0)
                {
                    GAME_MODE = GAME_PLAY;
                    game_launch_lander(false);
                }
                else
                {
                    GAME_MODE = GAME_OVER;
                }
                WAIT_ELAPSED_FRAMES = 0;
            }
        }
        else
        if (GAME_MODE == GAME_OVER)
        {   
            // in case of game over show game over text on the
            // screen then switch to idle mode

            WAIT_ELAPSED_FRAMES++;
            if (WAIT_ELAPSED_FRAMES == GAME_MAX_WAIT_FRAMES)
            {
                game_launch_lander(true);
                GAME_MODE = GAME_IDLE;
                WAIT_ELAPSED_FRAMES = 0;
            }
        }

        // draw scroll lines
        if (GAME.game_show_debug_objects)
        {
            // Top horizontal line
            draw_line(vec_t(0, GAME.game_scroll_ytrigger),
                      vec_t(GAME.screen_width, GAME.game_scroll_ytrigger),
                      Color::GREEN,
                      false, false);
            // Left vertical line
            draw_line(vec_t(GAME.game_scroll_xtrigger, 0),
                      vec_t(GAME.game_scroll_xtrigger, GAME.screen_height),
                      Color::GREEN,
                      false, false);
            // Bottom horizontal line 
            draw_line(vec_t(0, GAME.screen_height-GAME.game_scroll_ytrigger),
                      vec_t(GAME.screen_width, GAME.screen_height-GAME.game_scroll_ytrigger),
                      Color::GREEN,
                      false, false);
            // Right vertical line 
            draw_line(vec_t(GAME.screen_width-GAME.game_scroll_xtrigger, 0), 
                      vec_t(GAME.screen_width-GAME.game_scroll_xtrigger, GAME.screen_height),
                      Color::GREEN,
                      false, false);
        }

        // show text informations (content depend on the game state)
        game_draw_texts();

        // draw the level map
        level_draw(LEVEL, true);

        // draw the lander if we are not in game over state
        if (GAME_MODE != GAME_OVER)
            lander_draw(LANDER, true);
        
        // make the screen visible 
        SDL_Flip(DISPLAY);

        // finally - add frame delay for good FPS independent 
        // animation
        SDL_framerateDelay(&FPS_MANAGER);
    }
}


int main(int argc, char * argv[])
{
    try
    {
        game_initialize();
        game_main_loop();
    }
    catch (std::exception & e)
    {
        game_cleanup();
        std::cerr << "Exception:\n    " << e.what() << std::endl;
        return -1;
    }

    game_cleanup();
    std::cerr << "Bye!\n";
    return 0;
}
