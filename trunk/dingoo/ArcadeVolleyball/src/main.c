/****************************************************************************
Arcade Volleyball

TODO:
  - AI

****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <dingoo/ucos2.h>
#include <dingoo/entry.h>

#include <sml/graphics.h>
#include <sml/display.h>
#include <sml/control.h>
#include <sml/timer.h>
#include <sml/fixmath.h>

#include "images.h"
#include "hif.h"

#define UNUSED __attribute__ (( unused ))

#define VERSION_MAJOR    0
#define VERSION_MINOR    4

gfx_font* g_gameFontSmall = NULL;
display*  g_gameDisplay  = NULL;
bool      g_gameRunning  = true;

gfx_texture* g_texBall1 = NULL;
gfx_texture* g_texBall2 = NULL;
gfx_texture* g_texPlayer1 = NULL;
gfx_texture* g_texPlayer1Jump = NULL;
gfx_texture* g_texPlayer2 = NULL;
gfx_texture* g_texPlayer2Jump = NULL;

bool     error = false;
char*    errorMsg = "Unknown error.";

enum GameMode
{
    GAMEMODE_MENU = 0,
    GAMEMODE_PLAY = 1,
};
enum GameMode g_gameMode = GAMEMODE_MENU;

enum GameMenuItem
{
    GAMEMENU_P1P2 = 0,
    GAMEMENU_P1AI = 1,
    GAMEMENU_AIP2 = 2,
    GAMEMENU_AIAI = 3,
    GAMEMENU_EXIT = 4,
    GAMEMENU_LAST = 4,
};
enum GameMenuItem g_gameMenuItem = GAMEMENU_P1P2;

#define BALL_GRAVITY 2
#define BALL_RADIUS 12
#define BALL_MAX_SPEED 600
#define POSY_BALL_BASE 50
#define POSY_FIELD_TOP 20
#define POSY_FIELD_BOTTOM 220
#define POSX_FIELD_RIGHT (320 - 3)
#define POSX_FIELD_LEFT (3)
#define POSY_NET_TOP (220 - 96)
#define POSX_NET (320 / 2)
#define POSY_PLAYER_BASE POSY_FIELD_BOTTOM
#define POSY_PLAYER_TOPJUMP POSY_FIELD_BOTTOM - 40
#define PLAYER_MOVE_SPEED 2
#define PLAYER_JUMP_SPEED 2
#define PLAYER_HEIGHT 26
#define PLAYER_RADIUS 12
#define POSX_PLAYER1_LEFT_LIMIT (4 + 15)
#define POSX_PLAYER1_RIGHT_LIMIT (160 - 2 - 15)
#define POSX_PLAYER2_LEFT_LIMIT (160 + 2 + 15)
#define POSX_PLAYER2_RIGHT_LIMIT (320 - 4 - 15)
#define POSX_PLAYER1_BASE (320 / 4)
#define POSX_PLAYER2_BASE (320 / 4 * 3)

int32_t g_gameBallPosX = 320 / 2;
int32_t g_gameBallPosY = 60;
int32_t g_gameBallSpeedX = 0;  // pixels * 100
int32_t g_gameBallSpeedY = 0;  // pixels * 100
int32_t g_gameBallDeltaX = 0;  // pixels * 100
int32_t g_gameBallDeltaY = 0;  // pixels * 100
int32_t g_gameBallSpriteTick = 0;
int32_t g_gamePlayer1PosY = POSY_PLAYER_BASE;
int32_t g_gamePlayer1Score = 0;
int32_t g_gamePlayer1PosX = POSX_PLAYER1_BASE;
int32_t g_gamePlayer1SpeedY = 0;
bool    g_gamePlayer1AI = false;
int     g_gamePlayer1AIControl = 0;  // CONTROL_DPAD_LEFT, CONTROL_DPAD_RIGHT, CONTROL_DPAD_UP
int32_t g_gamePlayer2PosY = POSY_PLAYER_BASE;
int32_t g_gamePlayer2PosX = POSX_PLAYER2_BASE;
int32_t g_gamePlayer2SpeedY = 0;
int32_t g_gamePlayer2Score = 0;
bool    g_gamePlayer2AI = false;
int     g_gamePlayer2AIControl = 0;  // CONTROL_DPAD_LEFT, CONTROL_DPAD_RIGHT, CONTROL_DPAD_UP

bool g_gameBallTouchNetTop = false;
bool g_gameBallTouchPlayer = false;


bool loadGame()
{
    g_gameFontSmall = (gfx_font*)gfx_font_load_hif_from_buffer(hifFontSmall);
    if (g_gameFontSmall == NULL)
    {
        errorMsg = "Unable to load graphics.";
        return false;
    }

    g_texBall1 = gfx_tex_load_hif_from_buffer(hifBall1);
    g_texBall2 = gfx_tex_load_hif_from_buffer(hifBall2);
    g_texPlayer1 = gfx_tex_load_hif_from_buffer(hifPlayer1);
    g_texPlayer1Jump = gfx_tex_load_hif_from_buffer(hifPlayer1Jump);
    g_texPlayer2 = gfx_tex_load_hif_from_buffer(hifPlayer2);
    g_texPlayer2Jump = gfx_tex_load_hif_from_buffer(hifPlayer2Jump);

    return true;
}

void unloadGame()
{
    gfx_font_delete(g_gameFontSmall);

    gfx_tex_delete(g_texBall1);
    gfx_tex_delete(g_texBall2);
    gfx_tex_delete(g_texPlayer1);
    gfx_tex_delete(g_texPlayer1Jump);
    gfx_tex_delete(g_texPlayer2);
    gfx_tex_delete(g_texPlayer2Jump);
}

void drawScreen()
{
    char buffer[20];

    gfx_render_target_clear(gfx_color_rgb(0, 0, 0));

    gfx_font_print_center(0, g_gameFontSmall, "Arcade Volleyball");
    sprintf(buffer, "v%d.%d", VERSION_MAJOR, VERSION_MINOR);
    gfx_font_print(320 - 4 * 8, 0, g_gameFontSmall, buffer);

    gfx_color colorFrame = gfx_color_rgb(160,160,160);
    gfx_line_draw(3, 20, 320 - 3, 20, colorFrame);
    gfx_line_draw(3, 20, 3, 220, colorFrame);
    gfx_line_draw(320 - 3, 20, 320 - 3, 220, colorFrame);
    gfx_line_draw(3, 220, 320 - 3, 220, colorFrame);

    gfx_rect_fill_draw(320 / 2 - 2, 220 - 96, 4, 96, colorFrame);

    if (g_gameMode == GAMEMODE_MENU)
    {
        // Copyright titles
        gfx_font_print_center(24, g_gameFontSmall, "MSDOS ver. 1988 COMPUTE! Publications");
        gfx_font_print_center(40, g_gameFontSmall, "Dingoo ver. 2010 Nikita Zimin");

        // Menu items
        gfx_font_print(90 - 4 * 8, 64 + g_gameMenuItem * 16, g_gameFontSmall, "-->");
        gfx_font_print(90, 64 + 00, g_gameFontSmall, "Player1 vs Player2");
        gfx_font_print(90, 64 + 16, g_gameFontSmall, "Player1 vs AI");
        gfx_font_print(90, 64 + 32, g_gameFontSmall, "     AI vs Player2");
        gfx_font_print(90, 64 + 48, g_gameFontSmall, "     AI vs AI");
        gfx_font_print(90, 64 + 64, g_gameFontSmall, "       Exit");

    }
    else if (g_gameMode == GAMEMODE_PLAY)
    {
        // Draw ball
        gfx_texture* ballSprite = ((g_gameBallSpriteTick >> 5) & 1) ? g_texBall1 : g_texBall2;
        gfx_tex_draw(g_gameBallPosX - 12, g_gameBallPosY - 11, ballSprite);
        g_gameBallSpriteTick++;
        //gfx_line_draw(g_gameBallPosX, g_gameBallPosY, g_gameBallPosX + g_gameBallSpeedX / 10, g_gameBallPosY + g_gameBallSpeedY / 10, gfx_color_rgb(0,0,210));

        // Player signs
        gfx_font_print(0, 240 - 16, g_gameFontSmall, g_gamePlayer1AI ? "AI" : "Player1");
        gfx_font_print(320 - 7*8, 240 - 16, g_gameFontSmall, g_gamePlayer2AI ? "     AI" : "Player2");
    }

    if (g_gamePlayer1PosY < POSY_PLAYER_BASE)
        gfx_tex_draw(g_gamePlayer1PosX - 15, g_gamePlayer1PosY - 26, g_texPlayer1Jump);
    else
        gfx_tex_draw(g_gamePlayer1PosX - 15, g_gamePlayer1PosY - 26, g_texPlayer1);
    if (g_gamePlayer2PosY < POSY_PLAYER_BASE)
        gfx_tex_draw(g_gamePlayer2PosX - 15, g_gamePlayer2PosY - 26, g_texPlayer2Jump);
    else
        gfx_tex_draw(g_gamePlayer2PosX - 15, g_gamePlayer2PosY - 26, g_texPlayer2);

    // Show score
    sprintf(buffer, "%d : %d", g_gamePlayer1Score, g_gamePlayer2Score);
    gfx_font_print_center(240 - 16, g_gameFontSmall, buffer);
}

void gameRoundStart()
{
    g_gameBallPosX = POSX_NET;
    g_gameBallPosY = POSY_BALL_BASE;
    g_gameBallSpeedX = g_gameBallSpeedY = 0;
    g_gameBallDeltaX = g_gameBallDeltaY = 0;

    g_gameBallTouchNetTop = false;
    g_gameBallTouchPlayer = false;

    g_gamePlayer1PosY = POSY_PLAYER_BASE;
    g_gamePlayer1PosX = POSX_PLAYER1_BASE;
    g_gamePlayer1SpeedY = 0;
    g_gamePlayer1AIControl = 0;
    g_gamePlayer2PosY = POSY_PLAYER_BASE;
    g_gamePlayer2PosX = POSX_PLAYER2_BASE;
    g_gamePlayer2SpeedY = 0;
    g_gamePlayer2AIControl = 0;

    // Pause
    display_flip(g_gameDisplay);
    OSTimeDly(20);
}

void gameStart()
{
    g_gameMode = GAMEMODE_PLAY;

    g_gamePlayer1Score = g_gamePlayer2Score = 0;

    gameRoundStart();
}

void gameFinish()
{
    g_gameMode = GAMEMODE_MENU;
}

void processControlsMenu()
{
	if (control_just_pressed(CONTROL_DPAD_UP))
    {
        if (g_gameMenuItem > 0)
            g_gameMenuItem--;
    }
	if (control_just_pressed(CONTROL_DPAD_DOWN))
    {
        if (g_gameMenuItem < GAMEMENU_LAST)
            g_gameMenuItem++;
    }
    if (control_just_pressed(CONTROL_BUTTON_SELECT) ||
        control_just_pressed(CONTROL_BUTTON_START))
    {
        switch (g_gameMenuItem)
        {
        case GAMEMENU_P1P2:
            g_gamePlayer1AI = false;
            g_gamePlayer2AI = false;
            gameStart();
            break;
        case GAMEMENU_P1AI:
            g_gamePlayer1AI = false;
            g_gamePlayer2AI = true;
            gameStart();
            break;
        case GAMEMENU_AIP2:
            g_gamePlayer1AI = true;
            g_gamePlayer2AI = false;
            gameStart();
            break;
        case GAMEMENU_AIAI:
            g_gamePlayer1AI = true;
            g_gamePlayer2AI = true;
            gameStart();
            break;
        case GAMEMENU_EXIT:
            g_gameRunning = false;
            break;
        }
    }
}

void processControlsPlay()
{
    // Player 1 controls
	if (((!g_gamePlayer1AI && control_still_pressed(CONTROL_DPAD_LEFT)) || g_gamePlayer1AIControl == CONTROL_DPAD_LEFT) && 
        g_gamePlayer1PosX > POSX_PLAYER1_LEFT_LIMIT)  // Left
    {
        g_gamePlayer1PosX -= PLAYER_MOVE_SPEED;
    }
	if (((!g_gamePlayer1AI && control_still_pressed(CONTROL_DPAD_RIGHT)) || g_gamePlayer1AIControl == CONTROL_DPAD_RIGHT) &&
        g_gamePlayer1PosX < POSX_PLAYER1_RIGHT_LIMIT)  // Right
    {
        g_gamePlayer1PosX += PLAYER_MOVE_SPEED;
    }
	if (((!g_gamePlayer1AI && control_still_pressed(CONTROL_DPAD_UP)) || g_gamePlayer1AIControl == CONTROL_DPAD_UP) &&
        g_gamePlayer1PosY == POSY_PLAYER_BASE)  // Jump
    {
        g_gamePlayer1SpeedY = -PLAYER_JUMP_SPEED;
    }

    // Player 2 controls
	if (((!g_gamePlayer2AI && control_still_pressed(CONTROL_BUTTON_Y)) || g_gamePlayer2AIControl == CONTROL_DPAD_LEFT) &&
        g_gamePlayer2PosX > POSX_PLAYER2_LEFT_LIMIT)  // Left
    {
        g_gamePlayer2PosX -= PLAYER_MOVE_SPEED;
    }
	if (((!g_gamePlayer2AI && control_still_pressed(CONTROL_BUTTON_A)) || g_gamePlayer2AIControl == CONTROL_DPAD_RIGHT) &&
        g_gamePlayer2PosX < POSX_PLAYER2_RIGHT_LIMIT)  // Right
    {
        g_gamePlayer2PosX += PLAYER_MOVE_SPEED;
    }
	if (((!g_gamePlayer2AI && control_still_pressed(CONTROL_BUTTON_X)) || g_gamePlayer2AIControl == CONTROL_DPAD_UP) &&
        g_gamePlayer2PosY == POSY_PLAYER_BASE)  // Jump
    {
        g_gamePlayer2SpeedY = -PLAYER_JUMP_SPEED;
    }

	if (control_just_pressed(CONTROL_BUTTON_START))
    {
        gameFinish();
    }
}

int32_t compareDistance(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t radius)
{
    int32_t dx = x1 - x2;
    int32_t dy = y1 - y2;
    int32_t result2 = dx * dx + dy * dy;
    int32_t radius2 = radius * radius;
    if (result2 == radius2) return 0;
    return (result2 > radius2) ? 1 : -1;
}

void processCollisions()
{
    // Check ball touch floor
    if (g_gameBallPosY + BALL_RADIUS >= POSY_FIELD_BOTTOM)
    {
        //g_gameBallSpeedY = -g_gameBallSpeedY * 4 / 5;
        //g_gameBallDeltaY = 0;
        //g_gameBallPosY = POSY_FIELD_BOTTOM - BALL_RADIUS - 1;

        // Adjust score
        if (g_gameBallPosX > POSX_NET)  // Player 1 score
            g_gamePlayer1Score++;
        else
            g_gamePlayer2Score++;

        // Pause
        display_flip(g_gameDisplay);
        OSTimeDly(50);

        // Start next round
        gameRoundStart();
    }
    // Check ball touch ceiling
    if (g_gameBallPosY - BALL_RADIUS <= POSY_FIELD_TOP)
    {
        g_gameBallSpeedY = -g_gameBallSpeedY * 4 / 5;
        g_gameBallDeltaY = 0;
        g_gameBallPosY = POSY_FIELD_TOP + BALL_RADIUS + 1;
    }
    if (g_gameBallPosX + BALL_RADIUS >= POSX_FIELD_RIGHT)
    {
        g_gameBallSpeedX = -g_gameBallSpeedX * 4 / 5;
        g_gameBallDeltaX = 0;
        g_gameBallPosX = POSX_FIELD_RIGHT - BALL_RADIUS - 1;
    }
    if (g_gameBallPosX - BALL_RADIUS <= POSX_FIELD_LEFT)
    {
        g_gameBallSpeedX = -g_gameBallSpeedX * 4 / 5;
        g_gameBallDeltaX = 0;
        g_gameBallPosX = POSX_FIELD_LEFT + BALL_RADIUS + 1;
    }

    // Check ball collision with net top
    if (g_gameBallPosY <= POSY_NET_TOP &&
        g_gameBallSpeedY > 0 &&
        compareDistance(g_gameBallPosX, g_gameBallPosY, 320 / 2, POSY_NET_TOP, BALL_RADIUS) < 0)
    {
        if (g_gameBallTouchNetTop)
        {
            // Do nothing, we already worked it out
        }
        else if (g_gameBallSpeedX == 0)
        {
            g_gameBallSpeedX = (rand() > RAND_MAX / 2) ? 20 : -20;
            g_gameBallSpeedY = -g_gameBallSpeedY * 4 / 5;
            g_gameBallDeltaY = 0;
        }
        else
        {
            // Calculate projection of ball speed vector to vector from ball center to net top point
            int32_t dx = (POSX_NET - g_gameBallPosX);
            int32_t dy = (POSY_NET_TOP - g_gameBallPosY);
            int32_t sx = g_gameBallSpeedX;
            int32_t sy = g_gameBallSpeedY;
            int32_t dlen = fix16_to_int(fix16_sqrt(fix16_from_int(dx * dx + dy * dy)));
            int32_t dprolen = (dx * sx + dy * sy) / dlen;
            int32_t dprox = dprolen * dx / dlen;
            int32_t dproy = dprolen * dy / dlen;

            //gfx_line_draw(g_gameBallPosX, g_gameBallPosY, POSX_NET, POSY_NET_TOP, gfx_color_rgb(0,120,120));
            //gfx_line_draw(g_gameBallPosX, g_gameBallPosY, g_gameBallPosX - dprox / 10, g_gameBallPosY - dproy / 10, gfx_color_rgb(120,0,0));

            // Adjust speed
            g_gameBallSpeedX = sx - dprox;
            g_gameBallSpeedY = sy - dproy;
            g_gameBallDeltaX = g_gameBallSpeedX;
            g_gameBallDeltaY = g_gameBallSpeedY;

            //display_flip(g_gameDisplay);
            //OSTimeDly(50);
        }

        g_gameBallTouchNetTop = true;
    }
    else
    {
        g_gameBallTouchNetTop = false;
    }

    // Check ball collision with net side
    if (g_gameBallPosY > POSY_NET_TOP &&
        g_gameBallPosX < POSX_NET && g_gameBallPosX + BALL_RADIUS >= POSX_NET)
    {
        g_gameBallSpeedX = -g_gameBallSpeedX * 4 / 5;
        g_gameBallDeltaX = 0;
        if (g_gameBallSpeedX == 0) g_gameBallSpeedX = -20;
    }
    else if (g_gameBallPosY > POSY_NET_TOP &&
        g_gameBallPosX > POSX_NET && g_gameBallPosX - BALL_RADIUS <= POSX_NET)
    {
        g_gameBallSpeedX = -g_gameBallSpeedX * 4 / 5;
        g_gameBallDeltaX = 0;
        if (g_gameBallSpeedX == 0) g_gameBallSpeedX = 20;
    }

    // Check ball and player collision
    if (g_gameBallPosX < POSX_NET &&
        compareDistance(g_gameBallPosX, g_gameBallPosY, g_gamePlayer1PosX, g_gamePlayer1PosY - 15,
            BALL_RADIUS + PLAYER_RADIUS) <= 0)
    {
        if (g_gameBallTouchPlayer)
        {
            //Do nothing: already worked it out
        }
        else
        {
            int32_t dx = (g_gamePlayer1PosX - g_gameBallPosX);
            int32_t dy = ((g_gamePlayer1PosY - 15) - g_gameBallPosY);
            int32_t sx = g_gameBallSpeedX;
            int32_t sy = g_gameBallSpeedY;
            int32_t dlen = fix16_to_int(fix16_sqrt(fix16_from_int(dx * dx + dy * dy)));
            int32_t dprolen = (dx * sx + dy * sy) / dlen;
            int32_t dprox = dprolen * dx / dlen;
            int32_t dproy = dprolen * dy / dlen;

            // Adjust speed
            g_gameBallSpeedX = sx - dprox * 6 / 4;
            g_gameBallSpeedY = sy - dproy * 6 / 4;
            g_gameBallDeltaX = g_gameBallSpeedX;
            g_gameBallDeltaY = g_gameBallSpeedY;

            //g_gameBallSpeedY = g_gameBallSpeedY * 5 / 4;
            if (g_gamePlayer1SpeedY < 0)
                g_gameBallSpeedY += g_gamePlayer1SpeedY * 120;
        }
        g_gameBallTouchPlayer = true;
    }
    else if (g_gameBallPosX > POSX_NET &&
        compareDistance(g_gameBallPosX, g_gameBallPosY, g_gamePlayer2PosX, g_gamePlayer2PosY - 15,
            BALL_RADIUS + PLAYER_RADIUS) <= 0)
    {
        if (g_gameBallTouchPlayer)
        {
            //Do nothing: already worked it out
        }
        else
        {
            int32_t dx = (g_gamePlayer2PosX - g_gameBallPosX);
            int32_t dy = ((g_gamePlayer2PosY - 15) - g_gameBallPosY);
            int32_t sx = g_gameBallSpeedX;
            int32_t sy = g_gameBallSpeedY;
            int32_t dlen = fix16_to_int(fix16_sqrt(fix16_from_int(dx * dx + dy * dy)));
            int32_t dprolen = (dx * sx + dy * sy) / dlen;
            int32_t dprox = dprolen * dx / dlen;
            int32_t dproy = dprolen * dy / dlen;

            // Adjust speed
            g_gameBallSpeedX = sx - dprox * 6 / 4;
            g_gameBallSpeedY = sy - dproy * 6 / 4;
            g_gameBallDeltaX = g_gameBallSpeedX;
            g_gameBallDeltaY = g_gameBallSpeedY;

            if (g_gamePlayer2SpeedY < 0)
                g_gameBallSpeedY += g_gamePlayer2SpeedY * 120;
        }
        g_gameBallTouchPlayer = true;
    }
    else
    {
        g_gameBallTouchPlayer = false;
    }
}

void processLogic()
{
    if (g_gameMode != GAMEMODE_PLAY)
        return;

    // Move the ball
    g_gameBallDeltaX += g_gameBallSpeedX;
    g_gameBallDeltaY += g_gameBallSpeedY;

    if (g_gameBallDeltaX > 100 || g_gameBallDeltaX < -100)
    {
        g_gameBallPosX += g_gameBallDeltaX / 100;
        g_gameBallDeltaX = g_gameBallDeltaX % 100;
    }
    if (g_gameBallDeltaY > 100 || g_gameBallDeltaY < -100)
    {
        g_gameBallPosY += g_gameBallDeltaY / 100;
        g_gameBallDeltaY = g_gameBallDeltaY % 100;
    }

    // Gravity on ball
    g_gameBallSpeedY += BALL_GRAVITY;

    processCollisions();

    // Limit maximum ball speed
    if (g_gameBallSpeedX > BALL_MAX_SPEED) g_gameBallSpeedX = BALL_MAX_SPEED;
    if (g_gameBallSpeedX < -BALL_MAX_SPEED) g_gameBallSpeedX = -BALL_MAX_SPEED;
    if (g_gameBallSpeedY > BALL_MAX_SPEED) g_gameBallSpeedY = BALL_MAX_SPEED;
    if (g_gameBallSpeedY < -BALL_MAX_SPEED) g_gameBallSpeedY = -BALL_MAX_SPEED;

    // Player jumps
    if (g_gamePlayer1SpeedY != 0)
    {
        g_gamePlayer1PosY += g_gamePlayer1SpeedY;
        if (g_gamePlayer1SpeedY < 0 &&  g_gamePlayer1PosY <= POSY_PLAYER_TOPJUMP)
        {
            g_gamePlayer1PosY = POSY_PLAYER_TOPJUMP;
            g_gamePlayer1SpeedY = PLAYER_JUMP_SPEED;
        }
        else if (g_gamePlayer1SpeedY > 0 && g_gamePlayer1PosY >= POSY_PLAYER_BASE)
        {
            g_gamePlayer1PosY = POSY_PLAYER_BASE;
            g_gamePlayer1SpeedY = 0;
        }
    }
    if (g_gamePlayer2SpeedY != 0)
    {
        g_gamePlayer2PosY += g_gamePlayer2SpeedY;
        if (g_gamePlayer2SpeedY < 0 &&  g_gamePlayer2PosY <= POSY_PLAYER_TOPJUMP)
        {
            g_gamePlayer2PosY = POSY_PLAYER_TOPJUMP;
            g_gamePlayer2SpeedY = PLAYER_JUMP_SPEED;
        }
        else if (g_gamePlayer2SpeedY > 0 && g_gamePlayer2PosY >= POSY_PLAYER_BASE)
        {
            g_gamePlayer2PosY = POSY_PLAYER_BASE;
            g_gamePlayer2SpeedY = 0;
        }
    }
}

int processAI(bool player1)
{
    int32_t playerPosX = player1 ? g_gamePlayer1PosX : g_gamePlayer2PosX;

    bool isBallOnOtherSide = player1 ? (g_gameBallPosX - BALL_RADIUS > POSX_NET) : (g_gameBallPosX + BALL_RADIUS < POSX_NET);
    if (isBallOnOtherSide)  // Ball on other side - just walking around
    {
        // Go to field center
        int32_t centerPos = player1 ? (POSX_NET - POSX_NET / 2) : (POSX_NET + POSX_NET / 2);
        if (centerPos < playerPosX)
            return CONTROL_DPAD_LEFT;
        if (centerPos > playerPosX)
            return CONTROL_DPAD_RIGHT;
    }
    else  // Ball on our side
    {
        // If the ball to low then jump
        if (g_gameBallPosY + BALL_RADIUS + 50 > POSY_FIELD_BOTTOM - PLAYER_HEIGHT)
            return CONTROL_DPAD_UP;

        // If the ball at left, go left
        if (g_gameBallPosX < playerPosX)
            return CONTROL_DPAD_LEFT;
        // If the ball at right, go right
        if (g_gameBallPosX > playerPosX)
            return CONTROL_DPAD_RIGHT;
    }

    return 0;
}

int main(int argc UNUSED, char** argv UNUSED)
{
    int ref = EXIT_SUCCESS;

	srand(OSTimeGet());

    control_init();

    g_gameDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565 | DISPLAY_BUFFER_STATIC), NULL, NULL);
    if (g_gameDisplay == NULL)
    {
        control_term();
        return ref;
    }
    gfx_init(g_gameDisplay);

    gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
    display_flip(g_gameDisplay);

    control_lock(timer_resolution / 4);

    if (!loadGame())
        error = true;

    drawScreen();
    display_flip(g_gameDisplay);
        
    int sysref;
    while (g_gameRunning)
    {
        sysref = _sys_judge_event(NULL);
        if (sysref < 0) {
            ref = sysref;
            break;
        }

        drawScreen();
        
        control_poll();
        if (g_gameMode == GAMEMODE_MENU)
            processControlsMenu();
        else
        {
            if (g_gamePlayer1AI)
                g_gamePlayer1AIControl = processAI(true);
            if (g_gamePlayer2AI)
                g_gamePlayer2AIControl = processAI(false);
            processControlsPlay();
        }
        processLogic();

        display_flip(g_gameDisplay);

        OSTimeDly(1);
    }
    
    unloadGame();
    gfx_term();
    display_delete(g_gameDisplay);
    control_term();

    return ref;
}
