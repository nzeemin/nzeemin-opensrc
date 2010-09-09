/****************************************************************************

Tower Toppler SDL
for Dingoo A320 native OS

****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <SDL.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////


void MenuStart();


/////////////////////////////////////////////////////////////////////////////

#define VERSION_MAJOR    0
#define VERSION_MINOR    1

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240

const float PI = 3.14159265358979323f;

#define TOWER_RADIUS            75
#define TOWER_STEPS_RADIUS      (TOWER_RADIUS + 30)
#define TOWER_LIFTC_RADIUS      (TOWER_RADIUS + 15)     // Distance between tower center and lift center
#define TOWER_LIFT_RADIUS       14                      // Radius of lift platform
#define TOWER_STICKC_RADIUS     (TOWER_RADIUS + 15)
#define TOWER_STICK_RADIUS      6
#define POSY_BRICK_HEIGHT       8
#define POSX_VIEWPORT_MARGIN    6
#define POSY_VIEWPORT_TOP       44
#define POSY_VIEWPORT_BOTTOM    SCREEN_HEIGHT - 16
#define POSY_VIEWPORT_HEIGHT    180
#define POSX_VIEWPORT_WIDTH     (SCREEN_WIDTH - 2 * POSX_VIEWPORT_MARGIN)
#define POSX_VIEWPORT_CENTER    (SCREEN_WIDTH / 2)
#define POSX_TOWER_CENTER       POSX_VIEWPORT_CENTER
#define POSY_POGO_BASE          (POSY_VIEWPORT_TOP + 110)
#define POSX_POGO_HALFWIDTH     12
#define POSY_POGO_HEIGHT        20

#define STARS_MULTIPLIER        3
#define STARS_BACK_HEIGHT       (POSY_VIEWPORT_HEIGHT * 2)
#define STARS_BACK_WIDTH        (360 * STARS_MULTIPLIER + SCREEN_WIDTH)
#define STARCOUNT               50


/////////////////////////////////////////////////////////////////////////////
// Globals

SDL_Surface *g_Screen = NULL;
SDL_Surface *g_Sprites = NULL;
SDL_Surface *g_TowerBricks = NULL;  // Buffer surface for 2 lines of tower bricks
Uint32      g_colorBlack = 0;
Uint32      g_colorTarget;

enum GameMode
{
    GAMEMODE_MENU = 0,
    GAMEMODE_PLAY = 1,
} g_GameMode = GAMEMODE_MENU;
int g_okQuit = 0;
//int g_Score;

int     g_TowerHeight = 32;     // Tower height in brick lines
float   g_TowerAngle = 0;       // Current tower rotation angle, in degree (0.0f .. 359.5f)
int     g_TowerLevel = 0;       // Viewport level by Y coord, in pixels
Uint32  g_TowerColor;
Uint32  g_TowerColorDark;
Uint32  g_TowerColorLite;
float   g_LastTowerAngle = -1.0f;  // Tower rotation angle, in degree (0.0f .. 359.5f), for which g_TowerBricks is prepared

int     g_MoveX = 0;            // Tower move in the current frame: -1, 0, 1
int     g_MoveY = 0;            // Tower move in the current frame: -2, 0, 2
int     g_ControlState;         // See ControlStateEnum
int     g_PogoState;            // See PogoStateEnum
int     g_PogoMovtPhase;        // Pogo movement phase
float   g_Pogo180Angle;         // Target angle for 180 degree rotation

int     g_WaterY;               // Water level, used in drawing process
Uint8   g_CurLineTB;            // Tower block behind Pogo, used in game logic processing
Uint8   g_BaseLineTB;           // Tower block under Pogo, used in game logic processing

int     g_LastDelay = 0;        //DEBUG: Last delay value, milliseconds
int     g_LastFps = 0;          //DEBUG: Last Frames-per-Second value

struct StarsStruct
{
    int x, y;
}
g_Stars[STARCOUNT];

enum PogoStateEnum
{
    // Masks and masked values
    POGO_MASK_DIR   = 0x1000,  // Direction mask
    POGO_L          = 0x0000,
    POGO_R          = 0x1000,
    POGO_MASK_STATE = 0x00ff,  // State mask
    // States
    POGO_STAY       = 0x0001,
    POGO_WALK       = 0x0002,
    POGO_DOOR       = 0x0004,  // Door and 180 degree rotation
    POGO_JUMP       = 0x0008,
    POGO_LIFT       = 0x0010,  // Lifting on elevator
    POGO_FALL       = 0x0020,
    POGO_DROWN      = 0x0040,
    // Combined values
    POGO_L_STAY     = POGO_STAY | POGO_L,
    POGO_L_WALK     = POGO_WALK | POGO_L,
    POGO_L_DOOR     = POGO_DOOR | POGO_L,
    POGO_L_JUMP     = POGO_JUMP | POGO_L,
    POGO_L_FALL     = POGO_FALL | POGO_L,
    POGO_L_DROWN    = POGO_DROWN | POGO_L,
    POGO_R_STAY     = POGO_STAY | POGO_R,
    POGO_R_WALK     = POGO_WALK | POGO_R,
    POGO_R_DOOR     = POGO_DOOR | POGO_R,
    POGO_R_JUMP     = POGO_JUMP | POGO_R,
    POGO_R_FALL     = POGO_FALL | POGO_R,
    POGO_R_DROWN    = POGO_DROWN | POGO_R,
};

enum ControlStateEnum
{
    CONTROL_NONE    = 0x0000,
    CONTROL_LEFT    = 0x0001,
    CONTROL_RIGHT   = 0x0002,
    CONTROL_UP      = 0x0004,
    CONTROL_DOWN    = 0x0008,
    CONTROL_FIRE    = 0x0010,
};


/////////////////////////////////////////////////////////////////////////////
// Sprites

enum SpriteEnum
{
    SPRITE_POGO_R_STAY,
    SPRITE_POGO_R_WALK1,
    SPRITE_POGO_R_WALK2,
    SPRITE_POGO_R_WALK3,
    SPRITE_POGO_R_WALK4,
    SPRITE_POGO_R_WALK5,
    SPRITE_POGO_R_WALK6,
    SPRITE_POGO_R_WALK7,
    SPRITE_POGO_R_WALK8,
    SPRITE_POGO_L_STAY,
    SPRITE_POGO_L_WALK1,
    SPRITE_POGO_L_WALK2,
    SPRITE_POGO_L_WALK3,
    SPRITE_POGO_L_WALK4,
    SPRITE_POGO_L_WALK5,
    SPRITE_POGO_L_WALK6,
    SPRITE_POGO_L_WALK7,
    SPRITE_POGO_L_WALK8,
    SPRITE_POGO_TUNIN1,
    SPRITE_POGO_TUNIN2,
    SPRITE_POGO_TUNIN3,
    SPRITE_POGO_TUNOUT1,
    SPRITE_POGO_TUNOUT2,
    SPRITE_POGO_TUNOUT3,
    SPRITE_POGO_DROWN1,
    SPRITE_POGO_DROWN2,
    SPRITE_POGO_DROWN3,
    SPRITE_LIFE,
    SPRITE_FONT_TIMER,
    SPRITE_FONT_8X8,
    SPRITECOUNT
};

struct SpriteStruct
{
    int x, y, w, h;
}
g_SpriteCoords[SPRITECOUNT] =   // List of sprites coords, in order of SpriteEnum
{
    {   1,   0,  24,  20 },  // SPRITE_POGO_R_STAY
    {  26,   0,  24,  20 },  // SPRITE_POGO_R_WALK1
    {  51,   0,  24,  20 },  // SPRITE_POGO_R_WALK2
    {  76,   0,  24,  20 },  // SPRITE_POGO_R_WALK3
    { 101,   0,  24,  20 },  // SPRITE_POGO_R_WALK4
    { 126,   0,  24,  20 },  // SPRITE_POGO_R_WALK5
    { 151,   0,  24,  20 },  // SPRITE_POGO_R_WALK6
    { 176,   0,  24,  20 },  // SPRITE_POGO_R_WALK7
    { 201,   0,  24,  20 },  // SPRITE_POGO_R_WALK8
    { 226,  21,  24,  20 },  // SPRITE_POGO_L_STAY
    { 201,  21,  24,  20 },  // SPRITE_POGO_L_WALK1
    { 176,  21,  24,  20 },  // SPRITE_POGO_L_WALK2
    { 151,  21,  24,  20 },  // SPRITE_POGO_L_WALK3
    { 126,  21,  24,  20 },  // SPRITE_POGO_L_WALK4
    { 101,  21,  24,  20 },  // SPRITE_POGO_L_WALK5
    {  76,  21,  24,  20 },  // SPRITE_POGO_L_WALK6
    {  51,  21,  24,  20 },  // SPRITE_POGO_L_WALK7
    {  26,  21,  24,  20 },  // SPRITE_POGO_L_WALK8
    {   1,  63,  24,  20 },  // SPRITE_POGO_TUNIN1
    {  26,  63,  24,  20 },  // SPRITE_POGO_TUNIN2
    {  51,  63,  24,  20 },  // SPRITE_POGO_TUNIN3
    {  76,  63,  24,  20 },  // SPRITE_POGO_TUNOUT1
    { 101,  63,  24,  20 },  // SPRITE_POGO_TUNOUT2
    { 126,  63,  24,  20 },  // SPRITE_POGO_TUNOUT3
    { 176,  42,  24,  20 },  // SPRITE_POGO_DROWN1
    { 201,  42,  24,  20 },  // SPRITE_POGO_DROWN2
    { 226,  42,  24,  20 },  // SPRITE_POGO_DROWN3
    {   1,  54,  11,   9 },  // SPRITE_LIFE
    {   1,  88, 160,  23 },  // SPRITE_FONT_TIMER
    {   1, 113, 128,  40 },  // SPRITE_FONT_8X8
};


/////////////////////////////////////////////////////////////////////////////
// Service routines

void ClearScreen()
{
    SDL_Rect rc;
    rc.x = rc.y = 0;  rc.w = SCREEN_WIDTH; rc.h = SCREEN_HEIGHT;
    SDL_FillRect(g_Screen, &rc, g_colorBlack);
}

void DrawSprite(int sprite, int x, int y)
{
    SDL_Rect src, dest;

    int spriteX = g_SpriteCoords[sprite].x;
    int spriteY = g_SpriteCoords[sprite].y;
    int spriteW = g_SpriteCoords[sprite].w;
    int spriteH = g_SpriteCoords[sprite].h;

    src.x = spriteX;  src.y = spriteY;
    src.w = spriteW; src.h = spriteH;
    dest.x = x;  dest.y = y;
    dest.w = spriteW; dest.h = spriteH;

    SDL_BlitSurface(g_Sprites, &src, g_Screen, &dest);
}

void DrawText(int sprite, int x, int y, int charw, int charh, char *str)
{
    int charline;
    char ch;
    SDL_Rect src, dest;

    if (str == NULL) return;
    if (y >= SCREEN_HEIGHT) return;

    int spriteX = g_SpriteCoords[sprite].x;
    int spriteY = g_SpriteCoords[sprite].y;

    const int charperline = 16;

    while (*str != 0)
    {
        ch = *str;
        if (ch >= '0')
        {
            charline = ((int)ch - '0') / charperline;

            src.x = spriteX + ((int)ch) % charperline * charw;
            src.y = spriteY + charline * charh;
            src.w = charw;  src.h = charh;

            dest.x = x;  dest.y = y;
            dest.w = charw;  dest.h = charh;

            SDL_BlitSurface(g_Sprites, &src, g_Screen, &dest);
        }
        x += charw;
        if (x >= SCREEN_WIDTH) break;

        str++;
    }
}
// Draw text with base 8x8 font
void DrawTextBase(int x, int y, char *str)
{
    DrawText(SPRITE_FONT_8X8, x, y, 8, 8, str);
}


/////////////////////////////////////////////////////////////////////////////

void GamePrepareTowerBricks()
{
    if (g_LastTowerAngle == g_TowerAngle)
        return;  // g_TowerBricks already prepared for the angle

    SDL_Rect rc;

    // Fill bitmap with tower color
    rc.x = 0;  rc.y = 0;
    rc.w = TOWER_RADIUS * 2 + 1;  rc.h = POSY_BRICK_HEIGHT * 2 + 1;
    SDL_FillRect(g_TowerBricks, &rc, g_TowerColor);

    // Draw left and right border line
    rc.x = 0;  rc.y = 0;
    rc.w = 1;  rc.h = g_TowerBricks->h;
    SDL_FillRect(g_TowerBricks, &rc, g_TowerColorDark);
    rc.x = g_TowerBricks->w - 1;  rc.y = 0;
    rc.w = 1;  rc.h = g_TowerBricks->h;
    SDL_FillRect(g_TowerBricks, &rc, g_TowerColorDark);

    // Draw top horz line
    rc.x = 0;  rc.y = 0;
    rc.w = TOWER_RADIUS * 2 + 1;  rc.h = 1;
    SDL_FillRect(g_TowerBricks, &rc, g_TowerColorDark);
    
    int y = 1;
    for (int line = 0; line < 2; line++)
    {
        // Draw bottom horz line
        rc.x = 0;  rc.y = (line + 1) * POSY_BRICK_HEIGHT;
        rc.w = TOWER_RADIUS * 2 + 1;  rc.h = 1;
        SDL_FillRect(g_TowerBricks, &rc, g_TowerColorDark);

        // Draw vert lines
        float angle = g_TowerAngle;
        if (line  == 1) angle += 11.25f;
        angle = fmod(angle, 22.5f);
        for (int i = 0; i < 8; i++)
        {
            float fx = cos(angle * PI / 180.0f) * TOWER_RADIUS;
            int x = (int)(fx + 0.5);
            rc.x = TOWER_RADIUS + x - 1;  rc.y = y;
            rc.w = 2;  rc.h = POSY_BRICK_HEIGHT - 1;
            SDL_FillRect(g_TowerBricks, &rc, g_TowerColorDark);

            angle += 22.5f;  // 16 bricks
            if (angle >= 180.0f) break;
        }

        y += POSY_BRICK_HEIGHT;
    }

    g_LastTowerAngle = g_TowerAngle;
}

void DrawGameIndicators()
{
    char buffer[16];

    //TODO: Draw score
    //DrawTextBase(POSX_VIEWPORT_MARGIN, 8, "SCORE");
    //DrawTextBase(POSX_VIEWPORT_MARGIN, 18, "00000000");  //TODO
    sprintf(buffer, "ANGLE %4d", (int)(g_TowerAngle * 10));  //DEBUG
    DrawTextBase(POSX_VIEWPORT_MARGIN, 8, buffer);  //DEBUG
    sprintf(buffer, "LEVEL %4d", (int)g_TowerLevel);  //DEBUG
    DrawTextBase(POSX_VIEWPORT_MARGIN, 18, buffer);  //DEBUG

    sprintf(buffer, "DELAY %4d", g_LastDelay);  //DEBUG
    DrawTextBase(SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 8*10, 8, buffer);  //DEBUG
    sprintf(buffer, "FPS   %4d", g_LastFps);  //DEBUG
    DrawTextBase(SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 8*10, 18, buffer);  //DEBUG

    //TODO: Draw lives left
    //DrawTextBase(SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 8*5, 8, "LIVES");
    //DrawSprite(SPRITE_LIFE, SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 12*1, 12);
    //DrawSprite(SPRITE_LIFE, SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 12*2, 12);
    //DrawSprite(SPRITE_LIFE, SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 12*3, 12);

    //TODO: Draw time
    DrawText(SPRITE_FONT_TIMER, SCREEN_WIDTH / 2 - 16*4/2, 8,  16,23, "1000");
}

void DrawBackground()
{
    SDL_Rect rc;

    // Draw background stars
    int starsY = g_TowerLevel % STARS_BACK_HEIGHT;
    int starsX = (int)((360.0f - g_TowerAngle) * STARS_MULTIPLIER);
    Uint32 colorStars = SDL_MapRGB(g_Screen->format, 255,255,255);
    for (int i = 0; i < STARCOUNT; i++)
    {
        if (g_Stars[i].x >= starsX && g_Stars[i].x < starsX + SCREEN_WIDTH)
        {
            int stary = g_Stars[i].y + starsY;
            if (stary > STARS_BACK_HEIGHT) stary -= STARS_BACK_HEIGHT;
            if (stary > POSY_VIEWPORT_TOP && stary < POSY_VIEWPORT_BOTTOM && stary < g_WaterY)
            {
                rc.x = g_Stars[i].x - starsX;  rc.y = stary;  rc.w = rc.h = 2;
                SDL_FillRect(g_Screen, &rc, colorStars);
            }
        }
    }
}

void DrawWater()
{
    if (g_WaterY < POSY_VIEWPORT_BOTTOM)
    {
        int waterh = POSY_VIEWPORT_BOTTOM - g_WaterY;
        SDL_Rect rc;
        rc.x = POSX_VIEWPORT_MARGIN;  rc.y = g_WaterY;
        rc.w = POSX_VIEWPORT_WIDTH;  rc.h = 4;
        for (int i = 0; i < waterh; i += 4)
        {
            Uint8 waterg = (8 + i < 255) ? 8 + i : 255;
            Uint8 waterb = (68 + i < 255) ? 68 + i : 255;
            SDL_FillRect(g_Screen, &rc, SDL_MapRGB(g_Screen->format, 8,waterg,waterb));
            rc.y += 4;
        }
    }
}

void DrawTower()
{
    GamePrepareTowerBricks();

    SDL_Rect rc;
    SDL_Rect src, dest;

    // Find first brick line to draw
    int y = g_WaterY - POSY_BRICK_HEIGHT;
    int line = 0;
    while (y > POSY_VIEWPORT_TOP && line < g_TowerHeight)  //TODO: Optimize the loop
    {
        y -= POSY_BRICK_HEIGHT;
        line++;
    }

    int topline = line;
    int topliney = y;

    // Draw bricks
    if ((line & 1) == 0)  // Start with even line => draw one line
    {
        src.x = 0;  src.y = POSY_BRICK_HEIGHT;
        src.w = g_TowerBricks->w;  src.h = POSY_BRICK_HEIGHT;
        dest.x = SCREEN_WIDTH / 2 - TOWER_RADIUS;  dest.y = y - 1;
        dest.w = g_TowerBricks->w;  dest.h = POSY_BRICK_HEIGHT;
        SDL_BlitSurface(g_TowerBricks, &src, g_Screen, &dest);

        line++;
        y += POSY_BRICK_HEIGHT;
    }
    while (1)
    {
        if (y + POSY_BRICK_HEIGHT >= POSY_VIEWPORT_BOTTOM)
        {
            src.x = 0;  src.y = 0;
            src.w = g_TowerBricks->w;  src.h = POSY_BRICK_HEIGHT;
            dest.x = SCREEN_WIDTH / 2 - TOWER_RADIUS;  dest.y = y - 1;
            dest.w = g_TowerBricks->w;  dest.h = POSY_BRICK_HEIGHT;
            SDL_BlitSurface(g_TowerBricks, &src, g_Screen, &dest);

            line++;
            y += POSY_BRICK_HEIGHT;
            break;
        }

        src.x = 0;  src.y = 0;
        src.w = g_TowerBricks->w;  src.h = g_TowerBricks->h;
        dest.x = SCREEN_WIDTH / 2 - TOWER_RADIUS;  dest.y = y - 1;
        dest.w = g_TowerBricks->w;  dest.h = g_TowerBricks->h;
        SDL_BlitSurface(g_TowerBricks, &src, g_Screen, &dest);

        line += 2;
        y += POSY_BRICK_HEIGHT * 2;

        if (y >= POSY_VIEWPORT_BOTTOM || y >= g_WaterY)
            break;
    }

    line = topline;
    y = topliney;

    // Draw tower extras
    for (; y < POSY_VIEWPORT_BOTTOM; y += POSY_BRICK_HEIGHT)
    {
        // Draw tower steps, doors, elevators etc.
        float angle = 0.0f;
        for (int i = 0; i < 16; i++)
        {
            int iprev = i - 1;  if (iprev < 0) iprev = 15;
            Uint8 towerlinech = Level_GetTowerData(line, 15 - i);  // Current object to draw
            Uint8 towerlineprevch = Level_GetTowerData(line, 15 - iprev);  // Previous object

            float angle1 = angle + g_TowerAngle;
            if (angle1 >= 360.0f) angle1 -= 360.0f;
            float angle2 = angle1 + 22.5f;
            if (angle2 >= 360.0f) angle2 -= 360.0f;

            if (angle2 < 180.0f + 22.5f || angle1 > 360.0f - 22.5f)
            {
                float fx1 = cos(angle1 * PI / 180.0f) * TOWER_RADIUS;
                int x1 = (int)(fx1 + 0.5);
                float fx2 = cos(angle2 * PI / 180.0f) * TOWER_RADIUS;
                int x2 = (int)(fx2 + 0.5);

                // Doors
                if (Level_IsDoor(towerlinech))
                {
                    Uint32 colorDoor = (towerlinech == TB_DOOR_TARGET) ? g_colorTarget : g_colorBlack;
                    if (x1 > x2)
                    {
                        rc.x = POSX_TOWER_CENTER + x2;  rc.y = y;
                        rc.w = x1 - x2;  rc.h = POSY_BRICK_HEIGHT;
                        SDL_FillRect(g_Screen, &rc, colorDoor);
                    }
                    if (x1 >= x2)
                    {
                        // Door bars at left and right
                        rc.x = POSX_TOWER_CENTER + x2;  rc.y = y;
                        rc.w = 2;  rc.h = POSY_BRICK_HEIGHT - 1;
                        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
                        rc.x = POSX_TOWER_CENTER + x1 - 2;  rc.y = y;
                        rc.w = 2;  rc.h = POSY_BRICK_HEIGHT - 1;
                        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
                    }
                }
                // Elevators
                else if (towerlinech == TB_ELEV_BOTTOM)
                {
                    float fex = cos((angle1 + 11.25f) * PI / 180.0f) * TOWER_LIFTC_RADIUS;
                    int ex = (int)(fex + 0.5);  // Lift center position
                    {
                        rc.x = POSX_TOWER_CENTER + ex - TOWER_LIFT_RADIUS;  rc.y = y;
                        rc.w = TOWER_LIFT_RADIUS * 2;  rc.h = POSY_BRICK_HEIGHT - 1;
                        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
                    }
                }
                // Steps
                else if (Level_IsPlatform(towerlinech))
                {
                    float fsx1 = cos(angle1 * PI / 180.0f) * TOWER_STEPS_RADIUS;
                    int sx1 = (int)(fsx1 + 0.5);
                    float fsx2 = cos(angle2 * PI / 180.0f) * TOWER_STEPS_RADIUS;
                    int sx2 = (int)(fsx2 + 0.5);
                    if (sx1 > sx2)
                    {
                        rc.x = POSX_TOWER_CENTER + sx2;  rc.y = y;
                        rc.w = sx1 - sx2;  rc.h = POSY_BRICK_HEIGHT;
                        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
                    }
                    if (x1 > sx1 && !Level_IsPlatform(towerlineprevch))
                    {
                        rc.x = POSX_TOWER_CENTER + sx1;  rc.y = y;
                        rc.w = x1 - sx1;  rc.h = POSY_BRICK_HEIGHT;
                        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
                    }
                    {  // Small bar at right edge of the step
                        rc.x = POSX_TOWER_CENTER + sx1 - 1;  rc.y = y;
                        rc.w = 1;  rc.h = POSY_BRICK_HEIGHT;
                        SDL_FillRect(g_Screen, &rc, g_TowerColor);
                    }
                    if (sx2 > x2)
                    {
                        rc.x = POSX_TOWER_CENTER + x2;  rc.y = y;
                        rc.w = sx2 - x2;  rc.h = POSY_BRICK_HEIGHT;
                        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
                    }
                    {  // Small bar at left edge of the step
                        rc.x = POSX_TOWER_CENTER + sx2;  rc.y = y;
                        rc.w = 1;  rc.h = POSY_BRICK_HEIGHT;
                        SDL_FillRect(g_Screen, &rc, g_TowerColor);
                    }
                }
                // Sticks
                else if (towerlinech == TB_STICK)
                {
                    float fex = cos((angle1 + 11.25f) * PI / 180.0f) * TOWER_STICKC_RADIUS;
                    int ex = (int)(fex + 0.5);  // Stick center position
                    {
                        rc.x = POSX_TOWER_CENTER + ex - TOWER_STICK_RADIUS;  rc.y = y;
                        rc.w = TOWER_STICK_RADIUS * 2;  rc.h = POSY_BRICK_HEIGHT - 1;
                        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
                    }
                }
            }
            
            angle += 22.5f;  // 16 bricks
        }

        line--;
        if (line < 0) break;
    }
}

void DrawPogo()
{
    int y = POSY_POGO_BASE - POSY_POGO_HEIGHT;
    int sprite;
    switch (g_PogoState)
    {
    case POGO_R_STAY:   sprite = SPRITE_POGO_R_STAY;  break;
    case POGO_L_STAY:   sprite = SPRITE_POGO_L_STAY;  break;
    case POGO_R_WALK:
    case POGO_L_WALK:
        sprite = (g_PogoState & POGO_MASK_DIR) ? SPRITE_POGO_R_WALK1 : SPRITE_POGO_L_WALK1;
        sprite = sprite + g_PogoMovtPhase / 2 % 8;
        break;
    case POGO_L_DOOR:
    case POGO_R_DOOR:
        if (g_PogoMovtPhase == 0) return;  // Invisible while tower rotates
        if (g_PogoMovtPhase < 0)  // Moving into
            sprite = SPRITE_POGO_TUNIN1 + (g_PogoMovtPhase + 12) / 4;
        else if (g_PogoMovtPhase > 0)  // Moving out
            sprite = SPRITE_POGO_TUNOUT1 + (g_PogoMovtPhase - 1) / 4;
        break;
    case POGO_L_FALL:   sprite = SPRITE_POGO_L_WALK5;  break;
    case POGO_R_FALL:   sprite = SPRITE_POGO_R_WALK5;  break;
    case POGO_L_JUMP:   sprite = SPRITE_POGO_L_WALK5;  break;
    case POGO_R_JUMP:   sprite = SPRITE_POGO_R_WALK5;  break;
    case POGO_L_DROWN:
    case POGO_R_DROWN:
        if (g_PogoMovtPhase < 12)
            sprite = (g_PogoState & POGO_MASK_DIR) ? SPRITE_POGO_R_WALK5 : SPRITE_POGO_L_WALK5;
        else
        {
            sprite = SPRITE_POGO_DROWN1 + (g_PogoMovtPhase / 6) % 3;
            y = g_WaterY - POSY_POGO_HEIGHT;
        }
        break;
    default:
        return;  // Pogo is not visible
    }

    DrawSprite(sprite, POSX_VIEWPORT_CENTER - POSX_POGO_HALFWIDTH, y);
}

void DrawGameScreen()
{
    SDL_Rect rc;

    // Clear top area and viewport
    rc.x = POSX_VIEWPORT_MARGIN;  rc.y = 8;  rc.w = POSX_VIEWPORT_WIDTH; rc.h = POSY_VIEWPORT_BOTTOM - 8;
    SDL_FillRect(g_Screen, &rc, g_colorBlack);

    // Water level
    g_WaterY = POSY_POGO_BASE + g_TowerLevel + 1;

    // Set clipping to viewport
    SDL_Rect rcViewport;
    rcViewport.x = POSX_VIEWPORT_MARGIN;  rcViewport.y = POSY_VIEWPORT_TOP;
    rcViewport.w = POSX_VIEWPORT_WIDTH;  rcViewport.h = POSY_VIEWPORT_HEIGHT;
    SDL_SetClipRect(g_Screen, &rcViewport);
    {
        DrawBackground();

        DrawTower();

        DrawPogo();

        DrawWater();
    }
    SDL_SetClipRect(g_Screen, NULL);

    DrawGameIndicators();

    ////DEBUG: Show prepared bricks
    //SDL_Rect src, dest;
    //src.x = 0;  src.y = 0;
    //src.w = g_TowerBricks->w;  src.h = g_TowerBricks->h;
    //dest.x = SCREEN_WIDTH / 2 - TOWER_RADIUS;  dest.y = 20;
    //dest.w = g_TowerBricks->w;  dest.h = g_TowerBricks->h;
    //SDL_BlitSurface(g_TowerBricks, &src, g_Screen, &dest);
}

void GameStart()
{
    // Prepare Pogo
    g_PogoState = POGO_R_STAY;
    g_ControlState = CONTROL_NONE;

    // Prepare tower
    g_TowerAngle = 90.0f + 11.25f;
    g_TowerLevel = POSY_BRICK_HEIGHT * 2;

    Level_SelectTower(0);

    g_TowerHeight = Level_GetTowerSize();
    g_TowerColor = Level_GetTowerColor();

    // Prepare colors
    Uint8 r, g, b;
    SDL_GetRGB(g_TowerColor, g_Screen->format, &r,&g,&b);
    Uint16 rd, gd, bd;
    rd = ((Uint16)r) * 2 / 4;
    gd = ((Uint16)g) * 2 / 4;
    bd = ((Uint16)b) * 2 / 4;
    g_TowerColorDark = SDL_MapRGB(g_Screen->format, (Uint8)rd,(Uint8)gd,(Uint8)bd);
    Uint16 rl, gl, bl;
    rl = (Uint16)r * 3 / 2;  if (rl > 255) rl = 255;
    gl = (Uint16)g * 3 / 2;  if (gl > 255) gl = 255;
    bl = (Uint16)b * 3 / 2;  if (bl > 255) bl = 255;
    g_TowerColorLite = SDL_MapRGB(g_Screen->format, (Uint8)rl,(Uint8)gl,(Uint8)bl);

    //TODO: Move to program initialize routine
    // Prepare stars
    for (int i = 0; i < STARCOUNT; i++)
    {
        g_Stars[i].x = rand() % STARS_BACK_WIDTH;
        g_Stars[i].y = rand() % STARS_BACK_HEIGHT;
    }

    ClearScreen();

    g_GameMode = GAMEMODE_PLAY;
}

void GameProcessEvent(SDL_Event evt)
{
    if (evt.type == SDL_KEYDOWN)
    {
        switch (evt.key.keysym.sym)
        {
        case SDLK_PAUSE:   // POWER UP button on Dingoo
            g_okQuit = 1;
            break;
        case SDLK_ESCAPE:  // SELECT button on Dingoo
            MenuStart();
            break;
        case SDLK_LCTRL:   // A button on Dingoo
        case SDLK_SPACE:
            g_ControlState |= CONTROL_FIRE;
            break;
        case SDLK_LEFT:
            g_ControlState |= CONTROL_LEFT;
            g_ControlState &= ~CONTROL_RIGHT;
            break;
        case SDLK_RIGHT:
            g_ControlState |= CONTROL_RIGHT;
            g_ControlState &= ~CONTROL_LEFT;
            break;
        case SDLK_UP:
            g_ControlState |= CONTROL_UP;
            g_ControlState &= ~CONTROL_DOWN;
            break;
        case SDLK_DOWN:
            g_ControlState |= CONTROL_DOWN;
            g_ControlState &= ~CONTROL_UP;
            break;
        default:  // Do nothing
            break;
        }
    }
    else if (evt.type == SDL_KEYUP)
    {
        switch (evt.key.keysym.sym)
        {
        case SDLK_LEFT:
            g_ControlState &= ~CONTROL_LEFT;
            break;
        case SDLK_RIGHT:
            g_ControlState &= ~CONTROL_RIGHT;
            break;
        case SDLK_UP:
            g_ControlState &= ~CONTROL_UP;
            break;
        case SDLK_DOWN:
            g_ControlState &= ~CONTROL_DOWN;
            break;
        default:  // Do nothing
            break;
        }
    }
}

// Process long-time logic
// Returns: 1 - process futher logic and controls, 0 - don't process other logic
int GameProcessLogicCont()
{
    // Pogo moves thru door and tunnel
    if ((g_PogoState & POGO_MASK_STATE) == POGO_DOOR)
    {
        if (g_PogoMovtPhase < 0)  // Part 1: Move in
        {
            g_PogoMovtPhase++;
            return 0;
        }
        if (fabs(g_TowerAngle - g_Pogo180Angle) > 0.01f)  // Part 2: 180 degree rotation
        {
            g_MoveX = (g_PogoState & POGO_MASK_DIR) ? 3 : -3;
            return 0;
        }
        if (g_PogoMovtPhase < 12)  // Part 3: Move out
        {
            g_PogoMovtPhase++;
            return 0;
        }
        // Rotation finished
        g_PogoMovtPhase = 0;
        g_PogoState = POGO_STAY | (g_PogoState & POGO_MASK_DIR);
        return 1;
    }

    // Pogo jumps
    if ((g_PogoState & POGO_MASK_STATE) == POGO_JUMP)
    {
        if (g_PogoMovtPhase < 8)
        {
            g_PogoMovtPhase++;
            g_MoveY = 1;
            g_MoveX = (g_PogoState & POGO_MASK_DIR) ? 1 : -1;
            return 0;
        }
        if (g_PogoMovtPhase < 10)  // Horz
        {
            g_PogoMovtPhase++;
            g_MoveX = (g_PogoState & POGO_MASK_DIR) ? 1 : -1;
            return 0;
        }
        else  // Jump finished, falling
        {
            g_PogoState = POGO_FALL | (g_PogoState & POGO_MASK_DIR);
            g_PogoMovtPhase = 0;
            return 1;
        }
    }

    //TODO: turning

    // Drowns
    if ((g_PogoState & POGO_MASK_STATE) == POGO_DROWN)
    {
        g_PogoMovtPhase++;
        g_MoveY = -1;
        return 0;
    }

    return 1;
}

// Process non-controlled logic like falling, collisions and so on
// Returns: 1 - process controls, 0 - don't process controls
int GameProcessLogic()
{
    // Find tower blocks under Pogo (baseline) and behind Pogo (curline)
    int baseline = (g_TowerLevel / 8) - 1;
    int curline = baseline + 1;
    float angle = fmod(g_TowerAngle - 90.0f + 360.0f, 360.0f);
    int linei = (int)(angle / 22.5f);
    g_BaseLineTB = TB_EMPTY;
    if (baseline >= 0 && baseline < g_TowerHeight)
        g_BaseLineTB = Level_GetTowerData(baseline, linei);
    g_CurLineTB = TB_EMPTY;
    if (curline >= 0 && curline < g_TowerHeight)
        g_CurLineTB = Level_GetTowerData(curline, linei);

    // Check if Pogo drown or falling
    if (g_TowerLevel <= 0)
    {
        g_MoveY = -1;
        g_PogoState = POGO_DROWN | (g_PogoState & POGO_MASK_DIR);
        g_PogoMovtPhase = 0;
        return 0;
    }
    if (g_TowerLevel % 8 != 0 && Level_IsEmpty(g_CurLineTB))
    {
        g_MoveY = -1;
        g_MoveX = ((g_PogoState & POGO_MASK_DIR) == POGO_L) ? -1 : 1;
        g_PogoState = POGO_FALL | (g_PogoState & POGO_MASK_DIR);
        return 0;
    }

    if (g_BaseLineTB == TB_STEP_VANISHER)
    {
        Level_RemoveVanishStep(baseline, linei);
        g_MoveY = -1;
        g_PogoState = POGO_FALL | (g_PogoState & POGO_MASK_DIR);
        return 0;
    }

    if (Level_IsPlatform(g_CurLineTB))  // Climb up
    {
        g_MoveY = 2;
        g_PogoState = POGO_STAY | (g_PogoState & POGO_MASK_DIR);
        return 0;
    }

    if (Level_IsEmpty(g_BaseLineTB))
    {
        g_MoveY = -1;
        g_MoveX = ((g_PogoState & POGO_MASK_DIR) == POGO_L) ? -1 : 1;
        g_PogoState = POGO_FALL | (g_PogoState & POGO_MASK_DIR);
        return 0;
    }

    if ((g_PogoState & POGO_MASK_STATE) == POGO_FALL && Level_IsPlatform(g_BaseLineTB))
    {
        g_PogoState = POGO_STAY | (g_PogoState & POGO_MASK_DIR);  // Restore state after landing
    }

    int sliding = Level_IsSliding(g_BaseLineTB);
    if (sliding)
    {
        g_MoveX = sliding;
    }

    return 1;
}

// Process controlled logic like moving / turning / jumping
void GameProcessControls()
{
    // Walking left and right
    if (g_ControlState & CONTROL_LEFT)
    {
        if ((g_PogoState & POGO_MASK_DIR) == POGO_L)
        {
            g_MoveX = -1;
            if (g_PogoState == POGO_L_STAY)
            {
                g_PogoState = POGO_L_WALK;
                g_PogoMovtPhase = 0;
            }
            else if (g_PogoState == POGO_L_WALK)
                g_PogoMovtPhase++;
        }
        else
            g_PogoState = POGO_L_STAY;  //TODO: Start turning process
    }
    else if (g_ControlState & CONTROL_RIGHT)
    {
        if ((g_PogoState & POGO_MASK_DIR) == POGO_R)
        {
            g_MoveX = 1;
            if (g_PogoState == POGO_R_STAY)
            {
                g_PogoState = POGO_R_WALK;
                g_PogoMovtPhase = 0;
            }
            else if (g_PogoState == POGO_R_WALK)
                g_PogoMovtPhase++;
        }
        else
            g_PogoState = POGO_R_STAY;  //TODO: Start turning process
    }
    else
    {
        if (g_PogoState != POGO_L_STAY && g_PogoState != POGO_R_STAY)
            g_PogoState = POGO_L_STAY | (g_PogoState & POGO_MASK_DIR);
    }

    //TODO: Lift up and lift down
    // Door
    if ((g_ControlState & CONTROL_UP) && (g_PogoState & POGO_MASK_STATE) == POGO_STAY && Level_IsDoor(g_CurLineTB))
    {
        //TODO: Normalize angle first
        g_Pogo180Angle = fmod(g_TowerAngle + 180.0f, 360.0f);
        g_PogoState = POGO_DOOR | (g_PogoState & POGO_MASK_DIR);
        g_PogoMovtPhase = -12;
        return;
    }
    // Jump
    if ((g_ControlState & CONTROL_UP) && (g_PogoState & POGO_MASK_STATE) == POGO_WALK)
    {
        g_MoveY = -1;
        g_PogoState = POGO_JUMP | (g_PogoState & POGO_MASK_DIR);
        g_PogoMovtPhase = 0;
    }

    if (g_ControlState & CONTROL_FIRE)
    {
        //TODO: Throw snowball
    }
}

void GameProcess()
{
    // Check if Pogo died in the water
    if (g_TowerLevel < -POSY_POGO_HEIGHT * 2)
    {
        GameStart();  // Restart level
        return;
    }

    if (!GameProcessLogicCont())
        return;
    if (!GameProcessLogic())
        return;

    GameProcessControls();
}

void GameMoveTower()
{
    if (g_MoveX != 0)
    {
        g_TowerAngle += 2.5f * g_MoveX;
        if (g_TowerAngle >= 360.0f)
            g_TowerAngle -= 360.0f;
        else if (g_TowerAngle < 0.0f)
            g_TowerAngle += 360.0f;
        g_MoveX = 0;
    }
    if (g_MoveY != 0)
    {
        g_TowerLevel += g_MoveY * 2;
        g_MoveY = 0;
    }
}

void MenuStart()
{
    char buffer[16];

    ClearScreen();

    DrawTextBase(SCREEN_WIDTH / 2 - 8* 7/2, 30, "NEBULUS");
    DrawTextBase(SCREEN_WIDTH / 2 - 8*33/2, 38, "ORIGINAL VER 1987 JOHN M PHILLIPS");

    DrawTextBase(SCREEN_WIDTH / 2 - 8*17/2, 60, "TOWER TOPPLER SDL");
    DrawTextBase(SCREEN_WIDTH / 2 - 8*33/2, 68, "  DINGOO VER 2010 NIKITA ZIMIN   ");
    sprintf(buffer, "VERSION %d.%d", VERSION_MAJOR, VERSION_MINOR);
    DrawTextBase(SCREEN_WIDTH / 2 - 8*11/2, 76, buffer);
    DrawTextBase(SCREEN_WIDTH / 2 - 8*11/2, 84, __DATE__);

    DrawTextBase(SCREEN_WIDTH / 2 - 8*16/2, 180, "PRESS A TO START");
    DrawTextBase(SCREEN_WIDTH / 2 - 8*20/2, 188, "PRESS SELECT TO EXIT");
    
    SDL_Flip(g_Screen);

    g_GameMode = GAMEMODE_MENU;
}

void MenuProcessEvent(SDL_Event evt)
{
    if (evt.type == SDL_KEYDOWN)
    {
        switch (evt.key.keysym.sym)
        {
        case SDLK_ESCAPE:  // SELECT button on Dingoo
        case SDLK_PAUSE:   // POWER UP button on Dingoo
            g_okQuit = 1;
            break;
        case SDLK_LCTRL:   // A button on Dingoo
        case SDLK_LALT:    // B button on Dingoo
        case SDLK_SPACE:
            GameStart();
            break;
        default:  // Do nothing
            break;
        }
    }
}

#ifdef _WIN32
#undef main  //HACK for VC error LNK1561: entry point must be defined
#endif

int main(int argc, char * argv[])
{
#ifdef _WIN32
    _putenv("SDL_VIDEO_WINDOW_POS=250,200");
#endif

    // Init randomizer
    srand(SDL_GetTicks());

    // Init SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 255;  // Unable to initialize SDL
    // Prepare screen surface
    g_Screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0);
    if (g_Screen == NULL)
        return 254;  // Unable to set video mode

#ifdef _WIN32
    SDL_WM_SetCaption("Tower Toppler SDL", "Tower Toppler SDL");
#else
    SDL_ShowCursor(SDL_DISABLE);
#endif

    // Various initializations
    {
        // Load sprites
        SDL_Surface *tempSurface = SDL_LoadBMP("TowerTopplerData/sprites.bmp");
        if (tempSurface == NULL)
            return 253;  // Unable to load bitmap
        g_Sprites = SDL_DisplayFormat(tempSurface);
        SDL_SetColorKey(g_Sprites, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(g_Screen->format, 178,0,255));
        SDL_FreeSurface(tempSurface);

        // Prepare surface for tower brick lines
        g_TowerBricks = SDL_CreateRGBSurface(0,
            TOWER_RADIUS * 2 + 1, POSY_BRICK_HEIGHT * 2 + 1, g_Screen->format->BitsPerPixel,
            g_Screen->format->Rmask, g_Screen->format->Gmask, g_Screen->format->Bmask, g_Screen->format->Amask); 

        g_colorBlack = SDL_MapRGB(g_Screen->format, 0,0,0);
        g_colorTarget = SDL_MapRGB(g_Screen->format, 40,96,40);
    }

    MenuStart();

    Uint32 ticksLast;
    Uint32 ticksLastFps = SDL_GetTicks();
    int frames = 0;
    while (!g_okQuit)
    {
        ticksLast = SDL_GetTicks();  // Time at frame start
        SDL_Event evt;
        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
            {
                g_okQuit = 1;
                break;
            }
            else
            {
                if (g_GameMode == GAMEMODE_MENU)
                    MenuProcessEvent(evt);
                else
                    GameProcessEvent(evt);
            }
        }

        if (g_GameMode == GAMEMODE_MENU)
            SDL_Delay(50);
        else
        {
            GameProcess();  // Process game logic and controls
            GameMoveTower();

            DrawGameScreen();
            SDL_Flip(g_Screen);
            frames++;

            // Delay
            const int FRAME_TICKS = 32;  // 31.25 frames per second
            Uint32 ticksNew = SDL_GetTicks();
            Uint32 ticksElapsed = ticksNew - ticksLast;
            g_LastDelay = 0;
            if (ticksElapsed < FRAME_TICKS)
            {
                g_LastDelay = FRAME_TICKS - ticksElapsed;
                SDL_Delay(FRAME_TICKS - ticksElapsed);
            }
            ticksLast = ticksNew;
            
            if (ticksLast - ticksLastFps > 1000)  //DEBUG: FPS calculation
            {
                g_LastFps = frames;
                frames = 0;
                ticksLastFps += 1000;
            }
        }
    }

    // Free memory
    SDL_FreeSurface(g_Screen);
    SDL_FreeSurface(g_Sprites);
    SDL_FreeSurface(g_TowerBricks);

    SDL_Quit();

    return 0;
}
