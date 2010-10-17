/****************************************************************************

Tower Toppler SDL
for Dingoo A320 native OS

****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

//#include <stdlib.h>
//#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////
// Globals

SDL_Surface *g_Screen = NULL;
SDL_Surface *g_Sprites = NULL;
SDL_Surface *g_TowerBricks = NULL;  // Buffer surface for 2 lines of tower bricks
Uint32      g_colorBlack = 0;

enum GameMode
{
    GAMEMODE_MENU = 0,
    GAMEMODE_PLAY = 1,
} g_GameMode = GAMEMODE_MENU;

enum GameResultEnum
{
    GAME_NONE,
    GAME_FINISHED,      // The tower has been finished successfully
    GAME_DIED,          // The toppler died
    GAME_ABORTED,       // The game was aborted
} g_GameResult = GAME_NONE;

int g_okQuit = FALSE;
//int g_Score;

int     g_TowerCount;           // Total towers in the mission
int     g_TowerNumber;          // Number of the current tower

int     g_TowerHeight = 32;     // Tower height in brick lines
float   g_TowerAngle = 0;       // Current tower rotation angle, in degree (0.0f .. 359.5f)
int     g_TowerLevel = 0;       // Viewport level by Y coord, in pixels
Uint32  g_TowerColor;
Uint32  g_TowerColorDark;
Uint32  g_TowerColorLite;
float   g_LastTowerAngle = -1.0f;  // Tower rotation angle, in degree (0.0f .. 359.5f), for which g_TowerBricks is prepared

int     g_GlobalTime;           // Global timer: increments every frame
int     g_MoveX = 0;            // Tower move in the current frame: -1, 0, 1
int     g_MoveY = 0;            // Tower move in the current frame: -2, 0, 2
int     g_ControlState;         // See ControlStateEnum
int     g_PogoState;            // See PogoStateEnum
int     g_PogoSubstate;
int     g_PogoDirection;        // Pogo direction: 0 - left, 1 - right
int     g_PogoMovtPhase;        // Pogo movement phase
int     g_FallingDirection;     // Pogo falling direction: -1, 0, 1
int     g_FallingMinimum;
int     g_FallingHowMuch;
int     g_ToppleMin;
int     g_JumpDirection;
int     g_JumpHow;
int     g_JumpHowLong;
float   g_Pogo180Rotates;       // Number of tower rotate quants for 180 degree rotation
int     g_ElevDirection;        // Elevator movement direction: -1 or 1
int     g_SnowballTime;
float   g_SnowballAnglePassed;
float   g_SnowballAngle;
int     g_SnowballLevel;
int     g_SnowballDirection = 0;

int     g_WaterY;               // Water level, used in drawing process
int     g_TowerTopLine;         // Top tower block line to draw, used in drawing process
int     g_TowerTopLineY;        // Y-position of the top tower block line to draw, used in drawing process

int     g_CurLine;              // Tower line behind Pogo, used in game logic processing
Uint8   g_CurLineTB;            // Tower block behind Pogo, used in game logic processing
int     g_BaseLine;             // Tower line under Pogo, used in game logic processing
int     g_BaseLineIndex;
Uint8   g_BaseLineTB;           // Tower block under Pogo, used in game logic processing

int     g_LastDelay = 0;        //DEBUG: Last delay value, milliseconds
int     g_LastFps = 0;          //DEBUG: Last Frames-per-Second value

struct StarsStruct
{
    int x, y;
}
g_Stars[STARCOUNT];

enum ControlStateEnum
{
    CONTROL_NONE    = 0x0000,
    CONTROL_LEFT    = 0x0001,
    CONTROL_RIGHT   = 0x0002,
    CONTROL_UP      = 0x0004,
    CONTROL_DOWN    = 0x0008,
    CONTROL_FIRE    = 0x0010,
};

int Main_GetTowerLevel() { return g_TowerLevel; }
float Main_GetTowerAngle() { return g_TowerAngle; }
int Main_IsPogoWalking() { return g_PogoState == POGO_WALK; }


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
    SPRITE_POGO_R_TOPPLE1,
    SPRITE_POGO_R_TOPPLE2,
    SPRITE_POGO_R_TOPPLE3,
    SPRITE_POGO_R_TOPPLE4,
    SPRITE_POGO_L_TOPPLE1,
    SPRITE_POGO_L_TOPPLE2,
    SPRITE_POGO_L_TOPPLE3,
    SPRITE_POGO_L_TOPPLE4,
    SPRITE_POGO_TUNIN1,
    SPRITE_POGO_TUNIN2,
    SPRITE_POGO_TUNIN3,
    SPRITE_POGO_TUNOUT1,
    SPRITE_POGO_TUNOUT2,
    SPRITE_POGO_TUNOUT3,
    SPRITE_POGO_DROWN1,
    SPRITE_POGO_DROWN2,
    SPRITE_POGO_DROWN3,
    SPRITE_SNOWBALL,
    SPRITE_LIFE,
    SPRITE_FONT_TIMER,
    SPRITE_FONT_8X8,
    SPRITE_ROBOT_APPEAR1,
    SPRITE_ROBOT_APPEAR2,
    SPRITE_ROBOT_APPEAR3,
    SPRITE_ROBOT_APPEAR4,
    SPRITE_ROBOT_BALL1,
    SPRITE_ROBOT_BALL2,
    SPRITE_ROBOT_BALL3,
    SPRITE_ROBOT_BALL4,
    SPRITE_ROBOT_EYE1,
    SPRITE_ROBOT_EYE2,
    SPRITE_ROBOT_EYE3,
    SPRITE_ROBOT_EYE4,
    SPRITE_ROBOT_CROSS1,
    SPRITE_ROBOT_CROSS2,
    SPRITE_ROBOT_CROSS3,
    SPRITE_ROBOT_CROSS4,
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
    { 126,  42,  24,  20 },  // SPRITE_POGO_R_TOPPLE1
    { 101,  42,  24,  20 },  // SPRITE_POGO_R_TOPPLE2
    {  76,  42,  24,  20 },  // SPRITE_POGO_R_TOPPLE3
    { 126,   0,  24,  20 },  // SPRITE_POGO_R_TOPPLE4 -- the same as SPRITE_POGO_R_WALK5
    {   1,  42,  24,  20 },  // SPRITE_POGO_L_TOPPLE1
    {  26,  42,  24,  20 },  // SPRITE_POGO_L_TOPPLE2
    {  51,  42,  24,  20 },  // SPRITE_POGO_L_TOPPLE3
    { 101,  21,  24,  20 },  // SPRITE_POGO_L_TOPPLE4 -- the same as SPRITE_POGO_L_WALK5
    {   1,  63,  24,  20 },  // SPRITE_POGO_TUNIN1
    {  26,  63,  24,  20 },  // SPRITE_POGO_TUNIN2
    {  51,  63,  24,  20 },  // SPRITE_POGO_TUNIN3
    {  76,  63,  24,  20 },  // SPRITE_POGO_TUNOUT1
    { 101,  63,  24,  20 },  // SPRITE_POGO_TUNOUT2
    { 126,  63,  24,  20 },  // SPRITE_POGO_TUNOUT3
    { 176,  42,  24,  20 },  // SPRITE_POGO_DROWN1
    { 201,  42,  24,  20 },  // SPRITE_POGO_DROWN2
    { 226,  42,  24,  20 },  // SPRITE_POGO_DROWN3
    { 130, 119,   8,   8 },  // SPRITE_SNOWBALL
    { 130, 109,  11,   9 },  // SPRITE_LIFE
    {   1,  85-23, 160,  23 },  // SPRITE_FONT_TIMER ('0'-'9' only)
    {   1, 109, 128,  48 },  // SPRITE_FONT_8X8
    { 226,  84,  20,  20 },  // SPRITE_ROBOT_APPEAR1
    { 205,  84,  20,  20 },  // SPRITE_ROBOT_APPEAR1
    { 184,  84,  20,  20 },  // SPRITE_ROBOT_APPEAR1
    { 163,  84,  20,  20 },  // SPRITE_ROBOT_APPEAR1
    { 163, 105,  20,  20 },  // SPRITE_ROBOT_BALL1
    { 184, 105,  20,  20 },  // SPRITE_ROBOT_BALL2
    { 205, 105,  20,  20 },  // SPRITE_ROBOT_BALL3
    { 226, 105,  20,  20 },  // SPRITE_ROBOT_BALL4
    { 163, 126,  20,  20 },  // SPRITE_ROBOT_EYE1
    { 184, 126,  20,  20 },  // SPRITE_ROBOT_EYE2
    { 205, 126,  20,  20 },  // SPRITE_ROBOT_EYE3
    { 226, 126,  20,  20 },  // SPRITE_ROBOT_EYE4
    { 163, 147,  20,  20 },  // SPRITE_ROBOT_CROSS1
    { 184, 147,  20,  20 },  // SPRITE_ROBOT_CROSS2
    { 205, 147,  20,  20 },  // SPRITE_ROBOT_CROSS3
    { 226, 147,  20,  20 },  // SPRITE_ROBOT_CROSS4
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

void DrawText(int sprite, int x, int y, int charw, int charh, const char *str)
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
        if (ch >= ' ')
        {
            charline = ((int)ch - ' ') / charperline;

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
void DrawTextBase(int x, int y, const char *str)
{
    DrawText(SPRITE_FONT_8X8, x, y, 8, 8, str);
}


/////////////////////////////////////////////////////////////////////////////
// Drawing

void DrawPrepareTowerBricks()
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
        float angle = ANGLE_90 - ANGLE_BLOCK + fmod(ANGLE_360 - g_TowerAngle, ANGLE_BLOCK);
        if (line == 1) angle += ANGLE_HALFBLOCK;
        angle = fmod(angle, ANGLE_360);
        for (int i = 0; i < 8; i++)
        {
            float fx = sin(angle * PI / ANGLE_180) * TOWER_RADIUS;
            int x = (int)(fx + 0.5);
            rc.x = TOWER_RADIUS + x - 1;  rc.y = y;
            rc.w = 2;  rc.h = POSY_BRICK_HEIGHT - 1;
            SDL_FillRect(g_TowerBricks, &rc, g_TowerColorDark);

            angle -= ANGLE_BLOCK;
            if (angle < 0) angle += ANGLE_360;
        }

        y += POSY_BRICK_HEIGHT;
    }

    g_LastTowerAngle = g_TowerAngle;
}

void DrawGameIndicators()
{
    char buffer[16];

    //// Draw score
    //DrawTextBase(POSX_VIEWPORT_MARGIN, 8, "SCORE");
    //sprintf(buffer, "%08d", 0);
    //DrawTextBase(POSX_VIEWPORT_MARGIN, 18, buffer);

    //sprintf(buffer, "STATE %2d", g_PogoState);  //DEBUG
    //DrawTextBase(POSX_VIEWPORT_MARGIN, 8, buffer);  //DEBUG
    sprintf(buffer, "A%4d", (int)(g_TowerAngle * 10));  //DEBUG
    DrawTextBase(POSX_VIEWPORT_MARGIN + 80, 8, buffer);  //DEBUG
    sprintf(buffer, "L%4d", (int)g_TowerLevel);  //DEBUG
    DrawTextBase(POSX_VIEWPORT_MARGIN + 80, 18, buffer);  //DEBUG

    sprintf(buffer, "DEL %2d", g_LastDelay);  //DEBUG
    DrawTextBase(SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 11*10, 8, buffer);  //DEBUG
    sprintf(buffer, "FPS %2d", g_LastFps);  //DEBUG
    DrawTextBase(SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 11*10, 18, buffer);  //DEBUG

    //// Draw lives left
    //DrawTextBase(SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 8*5, 8, "LIVES");
    //DrawSprite(SPRITE_LIFE, SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 12, 18);
    //DrawSprite(SPRITE_LIFE, SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 12*2, 18);
    //DrawSprite(SPRITE_LIFE, SCREEN_WIDTH - POSX_VIEWPORT_MARGIN - 12*3, 18);

    //// Draw time
    //DrawText(SPRITE_FONT_TIMER, SCREEN_WIDTH / 2 - 16*4/2, 8,  16,23, "1000");
}

void DrawBackground()
{
    SDL_Rect rc;

    // Draw background stars
    int starsY = g_TowerLevel % STARS_BACK_HEIGHT;
    int starsX = STARS_BACK_WIDTH - (int)(g_TowerAngle / ANGLE_ROTATION) * STARS_MULTIPLIER;
    Uint32 colorStars = SDL_MapRGB(g_Screen->format, 255,255,255);
    for (int i = 0; i < STARCOUNT; i++)
    {
        int x = g_Stars[i].x - starsX;
        if (x < 0) x += STARS_BACK_WIDTH;
        if (x >= 0 && x < SCREEN_WIDTH)
        {
            int stary = (g_Stars[i].y + starsY) % STARS_BACK_HEIGHT;
            if (stary > POSY_VIEWPORT_TOP && stary < POSY_VIEWPORT_BOTTOM && stary < g_WaterY)
            {
                rc.x = x;  rc.y = stary;  rc.w = rc.h = 2;
                SDL_FillRect(g_Screen, &rc, colorStars);
            }
        }
    }
}

void DrawWater()
{
    SDL_Rect rc;

    if (g_WaterY < POSY_VIEWPORT_BOTTOM)
    {
        int waterh = POSY_VIEWPORT_BOTTOM - g_WaterY;
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

void DrawTowerBricks()
{
    DrawPrepareTowerBricks();

    SDL_Rect src, dest;
    int line = g_TowerTopLine;
    int y = g_TowerTopLineY;

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
            src.x = src.y = 0;
            src.w = g_TowerBricks->w;  src.h = POSY_BRICK_HEIGHT;
            dest.x = SCREEN_WIDTH / 2 - TOWER_RADIUS;  dest.y = y - 1;
            dest.w = g_TowerBricks->w;  dest.h = POSY_BRICK_HEIGHT;
            SDL_BlitSurface(g_TowerBricks, &src, g_Screen, &dest);

            line++;
            y += POSY_BRICK_HEIGHT;
            break;
        }

        src.x = src.y = 0;
        src.w = g_TowerBricks->w;  src.h = g_TowerBricks->h;
        dest.x = SCREEN_WIDTH / 2 - TOWER_RADIUS;  dest.y = y - 1;
        dest.w = g_TowerBricks->w;  dest.h = g_TowerBricks->h;
        SDL_BlitSurface(g_TowerBricks, &src, g_Screen, &dest);

        line += 2;
        y += POSY_BRICK_HEIGHT * 2;

        if (y >= POSY_VIEWPORT_BOTTOM || y >= g_WaterY)
            break;
    }
}

void DrawTopEmptyBlock(int x1, int x2, int ix1, int ix2, int y)
{
    SDL_Rect rc;
    if (x2 > x1)
    {
        rc.x = POSX_TOWER_CENTER + x1;  rc.y = y - 1;
        rc.w = x2 - x1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_colorBlack);
    }
    int jx1 = (x2 < ix1) ? x2 : ix1;
    if (x1 < jx1)
    {
        rc.x = POSX_TOWER_CENTER + x1;  rc.y = y - 1;
        rc.w = jx1 - x1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
    }
    int jx2 = (ix2 < x1) ? x1 : ix2;
    if (jx2 < x2)
    {
        rc.x = POSX_TOWER_CENTER + jx2;  rc.y = y - 1;
        rc.w = x2 - jx2;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
    }
}

void DrawDoorBlock(int x1, int x2, int y)
{
    SDL_Rect rc;

    if (x2 > x1 + 4)  // Black part
    {
        rc.x = POSX_TOWER_CENTER + x1;  rc.y = y;
        rc.w = x2 - x1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_colorBlack);
    }
    if (x2 > x1)  // Door bars at left and right
    {
        rc.x = POSX_TOWER_CENTER + x1;  rc.y = y;
        rc.w = 2;  rc.h = POSY_BRICK_HEIGHT - 1;
        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
        rc.x = POSX_TOWER_CENTER + x2 - 2;  rc.y = y;
        rc.w = 2;  rc.h = POSY_BRICK_HEIGHT - 1;
        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
    }
}
void DrawStickBlock(int ex, int y)
{
    SDL_Rect rc;

    rc.x = POSX_TOWER_CENTER + ex - TOWER_STICK_RADIUS;  rc.y = y;
    rc.w = TOWER_STICK_RADIUS * 2;  rc.h = POSY_BRICK_HEIGHT - 1;
    SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
    rc.x = POSX_TOWER_CENTER + ex + TOWER_STICK_RADIUS - 4;  rc.y = y;
    rc.w = 2;  rc.h = POSY_BRICK_HEIGHT - 1;
    SDL_FillRect(g_Screen, &rc, g_TowerColor);
}
void DrawElevatorBlock(int ex, int y)
{
    SDL_Rect rc;

    rc.x = POSX_TOWER_CENTER + ex - TOWER_ELEV_RADIUS;  rc.y = y;
    rc.w = TOWER_ELEV_RADIUS * 2;  rc.h = POSY_BRICK_HEIGHT - 1;
    SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
    rc.x = POSX_TOWER_CENTER + ex + TOWER_ELEV_RADIUS - 6;  rc.y = y;
    rc.w = 4;  rc.h = POSY_BRICK_HEIGHT - 1;
    SDL_FillRect(g_Screen, &rc, g_TowerColor);
}
void DrawStepBlock(int x1, int x2, int sx1, int sx2, int y)
{
    SDL_Rect rc;

    if (sx1 < sx2)
    {
        rc.x = POSX_TOWER_CENTER + sx1;  rc.y = y;
        rc.w = sx2 - sx1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
    }
    if (x1 < sx1)
    {
        rc.x = POSX_TOWER_CENTER + x1;  rc.y = y;
        rc.w = sx1 - x1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
    }
    if (sx2 < x2 || sx1 < sx2) {  // Small bar at right edge of the step
        rc.x = POSX_TOWER_CENTER + sx2 - 1;  rc.y = y;
        rc.w = 1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColor);
    }
    if (sx2 < x2)
    {
        rc.x = POSX_TOWER_CENTER + sx2;  rc.y = y;
        rc.w = x2 - sx2;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
    }
    if (x1 < sx1 || sx1 < sx2)  // Small bar at left edge of the step
    {
        rc.x = POSX_TOWER_CENTER + sx1;  rc.y = y;
        rc.w = 1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColor);
    }
}
void DrawFlashBox(int ex, int y)
{
    SDL_Rect rc;

    Uint32 color = (g_GlobalTime % 21 < 3) ? g_TowerColor : (g_GlobalTime % 21 > 17) ? g_TowerColorDark : g_TowerColorLite;

    rc.x = POSX_TOWER_CENTER + ex - TOWER_STICK_RADIUS;  rc.y = y;
    rc.w = TOWER_STICK_RADIUS * 2;  rc.h = POSY_BRICK_HEIGHT - 1;
    SDL_FillRect(g_Screen, &rc, color);
    rc.x = POSX_TOWER_CENTER + ex + TOWER_STICK_RADIUS - 1;  rc.y = y;
    rc.w = 1;  rc.h = POSY_BRICK_HEIGHT - 1;
    SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
    rc.x = POSX_TOWER_CENTER + ex - TOWER_STICK_RADIUS;  rc.y = y;
    rc.w = 1;  rc.h = POSY_BRICK_HEIGHT - 1;
    SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
}

// Draw tower extras (steps, doors, elevators etc.) for all line in one column
void DrawTowerColumn(int i, float angle1, float angle2)
{
    float sin1 = sin(angle1 * PI / ANGLE_180);
    float sin2 = sin(angle2 * PI / ANGLE_180);
    float sincenter = sin((angle1 + ANGLE_HALFBLOCK) * PI / ANGLE_180);
    int x1 = (int)(sin1 * TOWER_RADIUS + 0.5);
    int x2 = (int)(sin2 * TOWER_RADIUS + 0.5);
    int sx1 = (int)(sin1 * TOWER_STEPS_RADIUS + 0.5);
    int sx2 = (int)(sin2 * TOWER_STEPS_RADIUS + 0.5);

    int line = g_TowerTopLine;
    for (int y = g_TowerTopLineY; y < POSY_VIEWPORT_BOTTOM; y += POSY_BRICK_HEIGHT, line--)
    {
        if (line < 0) break;

        if (line == g_TowerHeight)  // Top line
        {
            if ((i & 1) == 0)
            {
                int ix1 = (int)(sin1 * TOWER_INNER_RADIUS + 0.5);
                int ix2 = (int)(sin2 * TOWER_INNER_RADIUS + 0.5);
                DrawTopEmptyBlock(x1, x2, ix1, ix2, y);
            }
            continue;
        }

        Uint8 towerlinetb = Level_GetTowerBlock(line, i);  // Current tower block to draw
        int iprev = i - 1;  if (iprev < 0) iprev = 15;
        Uint8 towerlineprevtb = Level_GetTowerBlock(line, iprev);  // Previous block
        int inext = i + 1;  if (inext >= 16) inext = 0;
        Uint8 towerlinenexttb = Level_GetTowerBlock(line, inext);  // Next block

        // Doors
        if (Level_IsDoor(towerlinetb))
        {
            DrawDoorBlock(x1, x2, y);
        }
        // Elevators
        else if (towerlinetb == TB_ELEV_BOTTOM || towerlinetb == TB_ELEV_TOP || towerlinetb == TB_ELEV_MIDDLE)
        {
            int ex = (int)(sincenter * TOWER_ELEVC_RADIUS + 0.5);  // Elevator center position

            DrawElevatorBlock(ex, y);
        }
        // Steps
        else if (Level_IsPlatform(towerlinetb))
        {
            DrawStepBlock(x1, x2, sx1, sx2, y);
        }
        // Sticks
        else if (Level_IsStick(towerlinetb))
        {
            int ex = (int)(sincenter * TOWER_STICKC_RADIUS + 0.5);  // Stick center position

            DrawStickBlock(ex, y);
        }
        else if (towerlinetb == TB_BOX)
        {
            int ex = (int)(sincenter * TOWER_STICKC_RADIUS + 0.5);  // Stick center position

            DrawFlashBox(ex, y);
        }
    }
}

void DrawRobots()
{
    for (int rob = 0; rob < MAX_OBJECTS; rob++)
    {
        RobotKindEnum kind = Robot_GetKind(rob);
        if (kind == OBJ_KIND_NOTHING || kind == OBJ_KIND_CROSS)
            continue;

        float angle = fmod(Robot_GetAngle(rob) + ANGLE_360 - g_TowerAngle, ANGLE_360);
        if (angle < ANGLE_90 + ANGLE_BLOCK || angle > ANGLE_270 - ANGLE_BLOCK)
        {
            float cosrob = sin(angle * PI / ANGLE_180);
            int x = (int)(cosrob * TOWER_ROBOTC_RADIUS + 0.5);
            int y = POSY_POGO_BASE - Robot_GetLevel(rob) + g_TowerLevel;
            
            int time = Robot_GetTime(rob);
            int sprite = SPRITE_ROBOT_BALL1;
            if (kind == OBJ_KIND_APPEAR)
                sprite = SPRITE_ROBOT_APPEAR1 + time / 3;
            else if (kind == OBJ_KIND_DISAPPEAR)
                sprite = SPRITE_ROBOT_APPEAR1 + (12 - time) / 3;
            else if (kind == OBJ_KIND_ROBOT_VERT)
            {
                // Most of time the eye is opened, and blinks time to time
                time = (time % (21*5)) / 5 - 10;  // -10..10
                if (time > 0)
                {
                    if (time < 8) time = 0; else time -= 7;  // 0..3
                }
                else
                {
                    if (time > -8) time = 0; else time += 7;  // -3..0
                }
                if (time < 0) time = -time;  // 0..3
                sprite = SPRITE_ROBOT_EYE1 + time;
            }
            else
                sprite = SPRITE_ROBOT_BALL1 + (time % 20) / 5;
            //TODO: Draw other kinds

            DrawSprite(sprite, POSX_TOWER_CENTER + x - 10, y - 20);
        }
    }
}

void DrawRobotCross()
{
    for (int rob = 0; rob < MAX_OBJECTS; rob++)
    {
        if (Robot_GetKind(rob) != OBJ_KIND_CROSS) continue;

        int time = Robot_GetTime(rob);  if (time < 0) time = -time;
        int sprite = SPRITE_ROBOT_CROSS1;
        sprite += (Robot_GetTime(rob) > 0) ? (time % 12) / 3 : 3 - (time % 12) / 3;
        int x = ((int)Robot_GetAngle(rob));
        int y = POSY_POGO_BASE - Robot_GetLevel(rob) + g_TowerLevel;

        DrawSprite(sprite, POSX_TOWER_CENTER + x - 10, y - 20);

        break;
    }
}

void DrawTower()
{
    // Calculate first block line to draw
    int y = g_WaterY - POSY_BRICK_HEIGHT;
    int line = 0;
    while (y > POSY_VIEWPORT_TOP && line < g_TowerHeight)  //TODO: Optimize the loop
    {
        y -= POSY_BRICK_HEIGHT;
        line++;
    }
    g_TowerTopLine = line;
    g_TowerTopLineY = y;

    int towerpos = (int)(g_TowerAngle / ANGLE_ROTATION);

    // Draw tower blocks behind the tower
    for (int j = 63; j >= 31; j--)  // 63..31
    {
        int pos = (j + towerpos) % TOWER_ANGLECOUNT;
        if (pos % 8 == 0)
        {
            float angle1 = j * ANGLE_ROTATION;
            float angle2 = angle1 + ANGLE_BLOCK;
            DrawTowerColumn(pos / 8, angle1, angle2);
        }
    }
    for (int j = 64; j < 96; j++)  // 64..95
    {
        int pos = (j + towerpos) % TOWER_ANGLECOUNT;
        if (pos % 8 == 0)
        {
            float angle1 = j * ANGLE_ROTATION;
            float angle2 = angle1 + ANGLE_BLOCK;
            DrawTowerColumn(pos / 8, angle1, angle2);
        }
    }

    // Draw tower body
    DrawTowerBricks();

    // Draw tower blocks in front of the tower
    for (int j = 32; j >= 0; j--)  // 32..0
    {
        int pos = (j + towerpos) % TOWER_ANGLECOUNT;
        if (pos % 8 == 0)
        {
            float angle1 = j * ANGLE_ROTATION;
            float angle2 = angle1 + ANGLE_BLOCK;
            DrawTowerColumn(pos / 8, angle1, angle2);
        }
    }
    for (int j = 96; j < 128; j++)  // 96..127
    {
        int pos = (j + towerpos) % TOWER_ANGLECOUNT;
        if (pos % 8 == 0)
        {
            float angle1 = j * ANGLE_ROTATION;
            float angle2 = angle1 + ANGLE_BLOCK;
            DrawTowerColumn(pos / 8, angle1, angle2);
        }
    }

    //DEBUG DrawRobots();
}

void DrawPogo()
{
    int y = POSY_POGO_BASE - POSY_POGO_HEIGHT;
    int sprite;
    switch (g_PogoState)
    {
    case POGO_STAY:
    case POGO_ELEV:
        sprite = g_PogoDirection ? SPRITE_POGO_R_STAY : SPRITE_POGO_L_STAY;
        break;
    case POGO_WALK:
        sprite = g_PogoDirection ? SPRITE_POGO_R_WALK1 : SPRITE_POGO_L_WALK1;
        sprite = sprite + g_PogoMovtPhase / 2 % 8;
        break;
    case POGO_DOOR:
        if (g_PogoMovtPhase == 0) return;  // Invisible while tower rotates
        if (g_PogoMovtPhase < 0)  // Moving into
            sprite = SPRITE_POGO_TUNIN1 + (g_PogoMovtPhase + 12) / 4;
        else if (g_PogoMovtPhase > 0)  // Moving out
            sprite = SPRITE_POGO_TUNOUT1 + (g_PogoMovtPhase - 1) / 4;
        break;
    case POGO_FALL:
        sprite = g_PogoDirection ? SPRITE_POGO_R_WALK5 : SPRITE_POGO_L_WALK5;
        break;
    case POGO_JUMP:
        sprite = g_PogoDirection ? SPRITE_POGO_R_WALK5 : SPRITE_POGO_L_WALK5;
        break;
    case POGO_DROWN:
        if (g_PogoMovtPhase < 3)
            sprite = g_PogoDirection ? SPRITE_POGO_R_WALK5 : SPRITE_POGO_L_WALK5;
        else
        {
            sprite = SPRITE_POGO_DROWN1 + (g_PogoMovtPhase / 8) % 3;
            y = g_WaterY - POSY_POGO_HEIGHT - 2;
        }
        break;
    case POGO_TOPPLE:
        sprite = g_PogoDirection ? SPRITE_POGO_R_TOPPLE1 : SPRITE_POGO_L_TOPPLE1;
        sprite += (g_PogoSubstate % 16) / 4;
        break;
    default:
        return;  // Pogo is not visible
    }

    DrawSprite(sprite, POSX_VIEWPORT_CENTER - POSX_POGO_HALFWIDTH, y);

    if (g_PogoState == POGO_ELEV && g_ElevDirection != 0)
    {
        // Draw moving elevator platform under Pogo
        DrawElevatorBlock(0, POSY_POGO_BASE + 1);
    }
}

void DrawSnowball()
{
    if (g_SnowballDirection == 0) return;

    float angle = fmod(g_SnowballAngle + ANGLE_360 - g_TowerAngle, ANGLE_360);
    if (angle < ANGLE_90 + ANGLE_BLOCK || angle > ANGLE_270 - ANGLE_BLOCK)
    {
        float sinsnb = sin(angle * PI / ANGLE_180);
        int x = (int)(sinsnb * TOWER_SNOWBALLC_RADIUS + 0.5);
        int y = POSY_POGO_BASE - g_SnowballLevel + g_TowerLevel;
        DrawSprite(SPRITE_SNOWBALL, POSX_TOWER_CENTER + x - 4, y - 4);
    }
}

void DrawGameScreen()
{
    // Clear top area and viewport
    SDL_Rect rc;
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

        DrawSnowball();
        DrawPogo();
        //DrawRobotCross();

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

/////////////////////////////////////////////////////////////////////////////


void Stars_Initialize()
{
    for (int i = 0; i < STARCOUNT; i++)
    {
        g_Stars[i].x = rand() % STARS_BACK_WIDTH;
        g_Stars[i].y = rand() % STARS_BACK_HEIGHT;
    }
}

void GameProcessEvent(SDL_Event evt)
{
    if (evt.type == SDL_KEYDOWN)
    {
        switch (evt.key.keysym.sym)
        {
        case SDLK_PAUSE:   // POWER UP button on Dingoo
            g_okQuit = TRUE;
            g_GameResult = GAME_ABORTED;
            break;
        case SDLK_ESCAPE:  // SELECT button on Dingoo
            g_GameResult = GAME_ABORTED;
            break;
        case SDLK_LCTRL:   // A button on Dingoo
        case SDLK_SPACE:   // X button on Dingoo
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
        case SDLK_LCTRL:   // A button on Dingoo
        case SDLK_SPACE:   // X button on Dingoo
            g_ControlState &= ~CONTROL_FIRE;
            break;
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

void GameCalculatePosition()
{
    // Find tower blocks under Pogo (g_BaseLine) and behind Pogo (curline)
    g_BaseLine = (g_TowerLevel / 8) - 1;
    g_CurLine = g_BaseLine + 1;
    float angle = g_TowerAngle;
    g_BaseLineIndex = (int)(angle / ANGLE_BLOCK) % TOWERWID;
    g_BaseLineTB = TB_EMPTY;
    if (g_BaseLine >= 0 && g_BaseLine < g_TowerHeight)
        g_BaseLineTB = Level_GetTowerBlock(g_BaseLine, g_BaseLineIndex);
    g_CurLineTB = TB_EMPTY;
    if (g_CurLine >= 0 && g_CurLine < g_TowerHeight)
        g_CurLineTB = Level_GetTowerBlock(g_CurLine, g_BaseLineIndex);
}

void SnowballUpdate()
{
    /* the snowball moves up this fiels specifies by how much */
    static long schusshoch[12] = { 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0 };

    if (g_SnowballDirection == 0) return;

    int dy = 1;
    if (g_SnowballTime < 12)
        dy = schusshoch[g_SnowballTime];
    else
        if (fmod(g_SnowballAnglePassed + 0.01f, ANGLE_ROTATION * 2 * 4) < 0.02f)
            dy++;
    dy += g_SnowballLevel;
    g_SnowballTime++;

    float delta = ANGLE_ROTATION * 2;
    g_SnowballAngle += delta * g_SnowballDirection;
    if (g_SnowballAngle > ANGLE_360) g_SnowballAngle -= ANGLE_360;
    else if (g_SnowballAngle < 0.0f) g_SnowballAngle += ANGLE_360;
    g_SnowballAnglePassed += delta;

    if (g_SnowballAnglePassed > ANGLE_90 + ANGLE_HALFBLOCK)
    {
        g_SnowballDirection = 0;
        return;
    }

    if (g_SnowballAnglePassed > ANGLE_HALFBLOCK &&
        !Level_TestFigure(g_SnowballAngle, g_SnowballLevel, -1, 0, 2, 2, 5))
    {
        g_SnowballDirection = 0;
    }
}

void Main_SnowballHitsBox(int row, int col)
{
    Level_ClearBlock(row, col);
    //TODO: Add 50 points to score
}

int GameMovePogo(int dx, int dy)
{
    int test = Level_TestFigure(
        fmod(g_TowerAngle + (g_MoveX + dx) * ANGLE_ROTATION + ANGLE_360, ANGLE_360),
        g_TowerLevel + (g_MoveY + dy) * 2,
        -3, 2, 0, 0, 9);
    if (!test)
        return FALSE;

    g_MoveX += dx;
    g_MoveY += dy;
    return TRUE;
}

void GamePogoFalling(int nr)
{
    g_PogoState = POGO_FALL;
    g_PogoSubstate = 0;

    switch (nr)
    {
    case 0:
        g_FallingDirection = 0;
        g_FallingMinimum = -1;
        g_FallingHowMuch = 1;
        break;
    case 1:
        g_FallingDirection = g_PogoDirection ? 1 : -1;
        g_FallingMinimum = -1;
        g_FallingHowMuch = 1;
        break;
    case 2:
        g_FallingDirection = g_PogoDirection ? -1 : 1;
        g_FallingMinimum = -1;
        g_FallingHowMuch = 1;
        break;
    case 3:
        g_FallingDirection = 0;
        g_FallingMinimum = -1;
        g_FallingHowMuch = 3;
        break;
    }
}

void GamePogoTopple()
{
    g_PogoState = POGO_TOPPLE;
    g_PogoSubstate = 0;
}

// Tests the underground of the animal at the given position returning
//   0 if everything is all right
//   1 if there is no underground below us (fall vertical)
//   2 if there is no underground behind us (fall backwards)
//   3 if there is no underground in front of us (fall forwards)
int GameTestUnderground()
{
    static unsigned char unter[4 * 8] = {
        0x11, 0x20, 0x02, 0x00,
        0x11, 0x00, 0x32, 0x00,
        0x11, 0x00, 0x32, 0x00,
        0x11, 0x00, 0x11, 0x00,
        0x11, 0x00, 0x11, 0x00,
        0x11, 0x00, 0x11, 0x00,
        0x11, 0x23, 0x00, 0x00,
        0x11, 0x23, 0x00, 0x00
    };

    Uint8 data;
    int c, erg;

    int r = g_BaseLine;

    c = ((int)(g_TowerAngle / ANGLE_ROTATION) + (TOWER_ANGLECOUNT - 4)) / 8 % 16;
    data = Level_GetTowerBlock(r, c);
    erg = (Level_IsEmpty(data) || Level_IsDoor(data)) ? 0 : 2;

    c = (((int)(g_TowerAngle / ANGLE_ROTATION) + (TOWER_ANGLECOUNT - 4)) / 8 + 1) % 16;
    data = Level_GetTowerBlock(r, c);
    if (!Level_IsEmpty(data) && !Level_IsDoor(data)) erg++;

    erg = unter[((int)(g_TowerAngle / ANGLE_ROTATION + 0.01) % 8) * 4 + erg];

    return g_PogoDirection ? erg & 0xf : erg >> 4;
}

void GamePogoTestCollision()
{
    if (g_PogoState == POGO_DROWN ||
        g_PogoState == POGO_DOOR ||
        g_PogoState == POGO_TOPPLE)
        return;

    int nr = Robot_PogoCollison(g_TowerAngle, g_TowerLevel);
    if (nr != -1)
    {
        if (Robot_GetKind(nr) == OBJ_KIND_CROSS)
            g_ToppleMin = 12;
        else
            g_ToppleMin = 16;

        //TODO
        GamePogoTopple();
    }
    //TODO
}

void GameProcessLogic()
{
    /* the height differences for jumping */
    static long jump0[12] = { 3, 2, 2, 1, 1, 0, 0, -1, -1, -2, -2, -3 };
    static long jump1[7] = { 2, 2, 1, 0, -1, -2, -2 };
    
    /* the vertical movement for toppling */
    static long toppler2[16] = { 3, 2, 1, 1, 1, 0, 0, -1, -2, -2, -3, -3, -3, -3, -3, -4 };

    int inh, b;

    if (g_PogoState != POGO_DROWN && g_TowerLevel <= 0)  // Drown
    {
        g_PogoState = POGO_DROWN;
        g_PogoMovtPhase = 0;
    }

    //DEBUG GamePogoTestCollision();

    switch (g_PogoState)
    {

    case POGO_FALL:
        if (g_PogoSubstate == 0)
        {
            g_FallingMinimum++;
            if (!GameMovePogo(g_FallingDirection, 0))
                g_FallingDirection = 0;
            if (GameMovePogo(0, -g_FallingHowMuch))
            {
                g_FallingHowMuch++;
                if (g_FallingHowMuch > 4) g_FallingHowMuch = 4;
            }
            else
            {
                do
                {
                    g_FallingHowMuch--;
                }
                while (g_FallingHowMuch != 0 && !GameMovePogo(0, -g_FallingHowMuch));

                if (g_FallingHowMuch != 0)
                {
                    g_FallingHowMuch++;
                    if (g_FallingHowMuch > 4) g_FallingHowMuch = 4;
                }
                else
                {
                    if (g_FallingDirection == 0 || g_FallingMinimum >= 2)
                    {
                        g_PogoSubstate++;
                        g_PogoSubstate++;
                        if (g_PogoSubstate == 3)
                            g_PogoState = POGO_STAY;
                    }
                    else
                    {
                        g_FallingHowMuch++;
                        if (g_FallingHowMuch > 4) g_FallingHowMuch = 4;
                    }
                }
            }
        }
        else
        {
            g_PogoSubstate++;
            if (g_PogoSubstate == 3)
                g_PogoState = POGO_STAY;
        }
        break;

    case POGO_JUMP:
        GameMovePogo(g_JumpDirection, 0);

        if (g_JumpHow == 0)
            inh = jump0[g_PogoSubstate];
        else
            inh = jump1[g_PogoSubstate];

        if (GameMovePogo(0, inh))
        {
            g_PogoSubstate++;
            if (g_PogoSubstate >= g_JumpHowLong)
            {
                if (GameMovePogo(0, -1))
                    GamePogoFalling(3);
                else
                {
                    g_PogoState = POGO_FALL;
                    g_PogoSubstate = 1;
                }
            }
        }
        else
        {
            b = inh;
            do { if (inh < 0) inh++; else inh--; }
            while ((inh != 0) && !GameMovePogo(0, inh));

            if (b < 0)
            {
                g_PogoState = POGO_STAY;
            }
            else
            {
                g_PogoSubstate++;
                if (g_PogoSubstate >= g_JumpHowLong)
                {
                    if (GameMovePogo(0, -1))
                        GamePogoFalling(3);
                    else
                    {
                        g_PogoState = POGO_FALL;
                        g_PogoSubstate = 1;
                    }
                }
            }
        }
        break;

    case POGO_DOOR:  // Pogo moves thru door and tunnel
        if (g_PogoMovtPhase < 0)  // Part 1: Move in
        {
            // Move Pogo to door center
            float comp = fmod(g_TowerAngle, ANGLE_BLOCK) - ANGLE_ROTATION * 4;
            if (fabs(comp) > 0.01)
            {
                g_MoveX = (comp > 0.0f) ? -1 : 1;
                return;
            }

            if (g_CurLineTB == TB_DOOR_TARGET)  // Entered target door - game finished
            {
                g_GameResult = GAME_FINISHED;
                return;
            }

            g_Pogo180Rotates = TOWER_ANGLECOUNT / 2;

            g_PogoMovtPhase++;
            return;
        }
        if (g_Pogo180Rotates > 0)  // Part 2: 180 degree rotation
        {
            g_MoveX = g_PogoDirection ? 2 : -2;
            g_Pogo180Rotates -= 2;
            return;
        }
        if (g_PogoMovtPhase < 12)  // Part 3: Move out
        {
            g_PogoMovtPhase++;
            return;
        }
        // Rotation finished
        g_PogoMovtPhase = 0;
        g_PogoState = POGO_STAY;
        break;

    case POGO_ELEV:
        if (g_PogoMovtPhase != 0)  // Phase for center Pogo to the elevator platform
        {
            float comp = fmod(g_TowerAngle, ANGLE_BLOCK) - ANGLE_ROTATION * 4;
            if (fabs(comp) > 0.01)
            {
                g_MoveX = (comp > 0.0f) ? -1 : 1;
                return;
            }
            g_PogoMovtPhase = 0;
        }

        if (g_TowerLevel % POSY_BRICK_HEIGHT == 0)
        {
            if (Elevator_IsAtStop())  // Arrived to station, stop the elevator
            {
                Elevator_Deactivate();
                g_BaseLineTB = Level_GetTowerBlock(g_BaseLine, g_BaseLineIndex);  // Update tower block after Elevator_Deactivate() call
                g_PogoState = POGO_STAY;
                g_ElevDirection = 0;
                return;
            }
            else
            {
                Elevator_Move();
            }
        }

        g_MoveY = g_ElevDirection;
        break;

    case POGO_TOPPLE:
        if (g_PogoSubstate < g_ToppleMin)
        {
            g_MoveY = toppler2[(g_PogoSubstate < 15) ? g_PogoSubstate : 15];

            g_PogoSubstate++;
        }
        else
        {
            g_MoveY = -4;
            //TODO
            if (GameMovePogo(0, 0))
                GamePogoFalling(3);
        }
        break;

    //TODO: case POGO_TURN:

    case POGO_DROWN:
        if (g_TowerLevel > -POSY_POGO_HEIGHT)
        {
            g_MoveY = -1;
            return;
        }
        if (g_PogoMovtPhase > 24)  // Pogo died in the water
        {
            g_GameResult = GAME_DIED;
            return;
        }
        g_PogoMovtPhase++;
        break;

    case POGO_STAY:
    case POGO_WALK:
        if (g_TowerLevel % 8 == 0 && g_BaseLineTB == TB_STEP_VANISHER)
        {
            Level_RemoveVanishStep(g_BaseLine, g_BaseLineIndex);
        }

        int test = GameTestUnderground();
        if (test == 1)
            GamePogoFalling(0);
        else if (test == 2)
            GamePogoFalling(1);
        else if (test == 3)
            GamePogoFalling(2);
        else
        {
            if ((g_ControlState & (CONTROL_LEFT | CONTROL_RIGHT)) == 0)
            {
                int sliding = Level_IsSliding(g_BaseLineTB);
                if (sliding)
                {
                    GameMovePogo(sliding, 0);
                }
            }
        }
        break;
    }
}

// Process controlled logic like moving / turning / jumping
void GameProcessControls()
{
    if (g_PogoState != POGO_STAY && g_PogoState != POGO_WALK)
        return;

    // Walking left and right
    if (g_ControlState & CONTROL_LEFT)
    {
        if (!g_PogoDirection)
        {
            if (g_PogoState == POGO_STAY)
            {
                g_PogoState = POGO_WALK;
                g_PogoMovtPhase = 0;
            }
            else if (g_PogoState == POGO_WALK)
                g_PogoMovtPhase++;
            if (!GameMovePogo(-1, 0))
            {
                g_PogoState = POGO_JUMP;
                g_PogoSubstate = 0;
                g_JumpDirection = -1;
                g_JumpHow = 1;
                g_JumpHowLong = 7;
            }
        }
        else
        {
            g_PogoState = POGO_STAY;  //TODO: Start turning process
            g_PogoDirection = 0;
        }
    }
    else if (g_ControlState & CONTROL_RIGHT)
    {
        if (g_PogoDirection)
        {
            if (g_PogoState == POGO_STAY)
            {
                g_PogoState = POGO_WALK;
                g_PogoMovtPhase = 0;
            }
            else if (g_PogoState == POGO_WALK)
                g_PogoMovtPhase++;
            if (!GameMovePogo(1, 0))
            {
                g_PogoState = POGO_JUMP;
                g_PogoSubstate = 0;
                g_JumpDirection = 1;
                g_JumpHow = 1;
                g_JumpHowLong = 7;
            }
        }
        else
        {
            g_PogoState = POGO_STAY;  //TODO: Start turning process
            g_PogoDirection = 1;
        }
    }
    else
    {
        if (g_PogoState != POGO_STAY)
            g_PogoState = POGO_STAY;
    }

    // Elevator up
    if ((g_ControlState & CONTROL_UP) && Level_IsUpStation(g_BaseLineTB))
    {
        Elevator_Select(g_BaseLine, g_BaseLineIndex);
        g_PogoState = POGO_ELEV;
        g_ElevDirection = 1;
        g_PogoMovtPhase = 1;
        Elevator_Activate(1);
        g_MoveY = g_ElevDirection;
        return;
    }
    // Elevator down
    if ((g_ControlState & CONTROL_DOWN) && Level_IsDownStation(g_BaseLineTB))
    {
        Elevator_Select(g_BaseLine, g_BaseLineIndex);
        g_PogoState = POGO_ELEV;
        g_ElevDirection = -1;
        g_PogoMovtPhase = 1;
        Elevator_Activate(-1);
        g_MoveY = g_ElevDirection;
        return;
    }

    // Door and 180 degree rotation
    if ((g_ControlState & CONTROL_UP) && Level_IsDoor(g_CurLineTB))
    {
        g_PogoState = POGO_DOOR;
        g_PogoMovtPhase = -12;
        return;
    }

    // Jump
    if ((g_ControlState & CONTROL_FIRE) && g_PogoState == POGO_WALK)
    {
        g_PogoState = POGO_JUMP;
        g_PogoSubstate = 0;
        g_JumpDirection = g_PogoDirection ? 1 : -1;
        g_JumpHow = 0;
        g_JumpHowLong = 12;
        g_PogoMovtPhase = 0;
    }

    // Throw snowball
    if ((g_ControlState & CONTROL_FIRE) && g_PogoState == POGO_STAY && g_SnowballDirection == 0)
    {
        g_SnowballAngle = g_TowerAngle;
        g_SnowballLevel = g_TowerLevel + 6;
        g_SnowballDirection = g_PogoDirection ? 1 : -1;
        g_SnowballAnglePassed = 0;
        g_SnowballTime = 0;
        g_ControlState &= ~CONTROL_FIRE;  // Reset signal -- do not auto-fire
    }
}

void Main_PogoSideMove()
{
    if (g_PogoState != POGO_STAY && g_PogoState != POGO_WALK && g_PogoState != POGO_JUMP)
        return;

    if (GameMovePogo(0, 0)) return;

    int i = 1;
    while (TRUE)
    {
        if (GameMovePogo(i, 0)) return;
        if (GameMovePogo(-i, 0)) return;
        i++;
    }
}

void GameMoveTower()
{
    if (g_MoveX != 0)
    {
        g_TowerAngle += ANGLE_ROTATION * g_MoveX;
        if (g_TowerAngle >= ANGLE_360) g_TowerAngle -= ANGLE_360;
        else if (g_TowerAngle < 0.0f) g_TowerAngle += ANGLE_360;
        g_MoveX = 0;
    }
    if (g_MoveY != 0)
    {
        g_TowerLevel += g_MoveY * 2;
        g_MoveY = 0;
    }
}

void GameProcess()
{
    GameCalculatePosition();

    SnowballUpdate();
    Robot_New(g_TowerLevel);
    //DEBUG Robot_Update();
    Elevator_Update();

    GameProcessLogic();
    GameProcessControls();

    GameMoveTower();

    g_GlobalTime++;
}

void GameStartTower()
{
    char buffer[20];

    g_GlobalTime = 0;
    g_GameResult = GAME_NONE;

    // Prepare Pogo
    g_PogoState = POGO_STAY;
    g_PogoDirection = 1;
    g_ControlState = CONTROL_NONE;
    g_SnowballDirection = 0;

    // Prepare tower
    g_TowerAngle = ANGLE_HALFBLOCK;
    g_TowerLevel = POSY_BRICK_HEIGHT * 2;
    g_TowerTopLineY = 0;
    g_LastTowerAngle = -1.0f;  // Reset g_TowerBricks

    Level_SelectTower(g_TowerNumber);

    g_TowerHeight = Level_GetTowerSize();
    g_TowerColor = Level_GetTowerColor();

    // Prepare tower colors
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

    Robot_Initialize();
    Elevator_Initialize();

    // Show tower starting screen
    ClearScreen();

    sprintf(buffer, "TOWER %2d", (g_TowerNumber + 1));
    DrawTextBase(SCREEN_WIDTH / 2 - 8*8/2, SCREEN_HEIGHT / 2 - 8, buffer);
    const char* towername = Level_GetTowerName();
    DrawTextBase(SCREEN_WIDTH / 2 - strlen(towername)*8/2, SCREEN_HEIGHT / 2 + 8, towername);

    SDL_Flip(g_Screen);
    SDL_Delay(1000);

    ClearScreen();
}

void GameStart()
{
    Stars_Initialize();

    g_GameMode = GAMEMODE_PLAY;

    g_TowerCount = Level_GetTowerCount();
    g_TowerNumber = FIRST_TOWER;
    GameStartTower();
}


/////////////////////////////////////////////////////////////////////////////
// Menu

// Switch to menu mode and draw menu screen
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

    const int y = 120;
    DrawTextBase(POSX_VIEWPORT_MARGIN, y+0*8, "Weird things are happening on Planet");
    DrawTextBase(POSX_VIEWPORT_MARGIN, y+1*8, "Nebulus.  Someone or something has");
    DrawTextBase(POSX_VIEWPORT_MARGIN, y+2*8, "begun building huge towers on the sea");
    DrawTextBase(POSX_VIEWPORT_MARGIN, y+3*8, "bottom without having a construction");
    DrawTextBase(POSX_VIEWPORT_MARGIN, y+4*8, "license...");

    DrawTextBase(SCREEN_WIDTH / 2 - 8*16/2, SCREEN_HEIGHT - 30, "PRESS A TO START");
    DrawTextBase(SCREEN_WIDTH / 2 - 8*20/2, SCREEN_HEIGHT - 20, "PRESS SELECT TO EXIT");
    
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
            g_okQuit = TRUE;
            break;
        case SDLK_LCTRL:   // A button on Dingoo
        case SDLK_LALT:    // B button on Dingoo
        case SDLK_SPACE:
        case SDLK_RETURN:  // START button on Dingoo
            GameStart();
            break;
        default:  // Do nothing
            break;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#undef main  //HACK for VC error LNK1561: entry point must be defined
#endif

int main()
{
#ifdef _WIN32
    SDL_putenv("SDL_VIDEO_WINDOW_POS=300,200");
#endif

    // Init randomizer
    srand(SDL_GetTicks());

    // Init SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 255;  // Unable to initialize SDL
#ifdef _WIN32
    SDL_WM_SetCaption("Tower Toppler SDL", "Tower Toppler SDL");
#else
    SDL_ShowCursor(SDL_DISABLE);
#endif
    // Prepare screen surface
    g_Screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0);
    if (g_Screen == NULL)
        return 254;  // Unable to set video mode

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
                g_okQuit = TRUE;
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

            DrawGameScreen();
            SDL_Flip(g_Screen);
            frames++;

            // Delay
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

            if (g_GameResult == GAME_ABORTED)
                MenuStart();
            else if (g_GameResult == GAME_FINISHED)
            {
                g_TowerNumber++;
                if (g_TowerNumber < g_TowerCount)
                    GameStartTower();
                else
                    MenuStart();
            }
            else if (g_GameResult == GAME_DIED)
            {
                //TODO: g_Lifes--; Check if no lifes left
                GameStartTower();
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

/////////////////////////////////////////////////////////////////////////////
