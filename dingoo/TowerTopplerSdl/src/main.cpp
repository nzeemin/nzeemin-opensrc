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
int     g_PogoDirection;        // Pogo direction: 0 - left, 1 - right
int     g_PogoMovtPhase;        // Pogo movement phase
int     g_FallingDirection;     // Pogo falling direction: -1, 0, 1
float   g_Pogo180Angle;         // Target angle for 180 degree rotation
int     g_ElevDirection;        // Elevator movement direction: -1 or 1
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
    SPRITE_SNOWBALL,
    SPRITE_LIFE,
    SPRITE_FONT_TIMER,
    SPRITE_FONT_8X8,
    SPRITE_ROBOT,
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
    {   1,  42,   8,   8 },  // SPRITE_SNOWBALL
    {   1,  53,  11,   9 },  // SPRITE_LIFE
    {   1,  88, 160,  23 },  // SPRITE_FONT_TIMER
    {   1, 113, 128,  40 },  // SPRITE_FONT_8X8
    { 163,  85,  20,  20 },  // SPRITE_ROBOT
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
    int starsX = (int)((ANGLE_360 - g_TowerAngle) * STARS_MULTIPLIER);
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
void DrawStepBlock(int x1, int x2, int sx1, int sx2, int y, int isprevplatform, int isnextplatform)
{
    SDL_Rect rc;

    if (sx1 < sx2)
    {
        rc.x = POSX_TOWER_CENTER + sx1;  rc.y = y;
        rc.w = sx2 - sx1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorLite);
    }
    if (x1 < sx1 && !isprevplatform)
    {
        rc.x = POSX_TOWER_CENTER + x1;  rc.y = y;
        rc.w = sx1 - x1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
    }
    {  // Small bar at right edge of the step
        rc.x = POSX_TOWER_CENTER + sx2 - 1;  rc.y = y;
        rc.w = 1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColor);
    }
    if (sx2 < x2 && !isnextplatform)
    {
        rc.x = POSX_TOWER_CENTER + sx2;  rc.y = y;
        rc.w = x2 - sx2;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColorDark);
    }
    {  // Small bar at left edge of the step
        rc.x = POSX_TOWER_CENTER + sx1;  rc.y = y;
        rc.w = 1;  rc.h = POSY_BRICK_HEIGHT;
        SDL_FillRect(g_Screen, &rc, g_TowerColor);
    }
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
        else if (towerlinetb == TB_ELEV_BOTTOM)
        {
            int ex = (int)(sincenter * TOWER_ELEVC_RADIUS + 0.5);  // Elevator center position

            DrawElevatorBlock(ex, y);
        }
        // Steps
        else if (Level_IsPlatform(towerlinetb))
        {
            DrawStepBlock(x1, x2, sx1, sx2, y,
                Level_IsPlatform(towerlineprevtb), Level_IsPlatform(towerlinenexttb));
        }
        // Sticks
        else if (Level_IsStick(towerlinetb))
        {
            int ex = (int)(sincenter * TOWER_STICKC_RADIUS + 0.5);  // Stick center position

            DrawStickBlock(ex, y);
        }
    }
}

void DrawRobots()
{
    for (int rob = 0; rob < MAX_OBJECTS; rob++)
    {
        if (Robot_GetKind(rob) == OBJ_KIND_NOTHING || Robot_GetKind(rob) == OBJ_KIND_CROSS)
            continue;

        float angle = fmod(Robot_GetAngle(rob) + ANGLE_360 - g_TowerAngle, ANGLE_360);
        if (angle < ANGLE_90 + ANGLE_BLOCK || angle > ANGLE_270 - ANGLE_BLOCK)
        {
            float cosrob = sin(angle * PI / ANGLE_180);
            int x = (int)(cosrob * TOWER_ROBOTC_RADIUS + 0.5);
            int y = POSY_POGO_BASE - Robot_GetLevel(rob) + g_TowerLevel;
            
            DrawSprite(SPRITE_ROBOT, POSX_TOWER_CENTER + x - 10, y - 20);
        }
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

    // Draw tower body
    DrawTowerBricks();

    DrawRobots();

    // Draw tower extras: steps, doors, elevators etc.
    float angle = 0.0f;
    for (int i = 0; i < 16; i++, angle += ANGLE_BLOCK)
    {
        float angle1 = fmod(angle + ANGLE_360 - g_TowerAngle, ANGLE_360);
        float angle2 = fmod(angle1 + ANGLE_BLOCK, ANGLE_360);

        if (angle2 < ANGLE_90 + ANGLE_BLOCK || angle1 > ANGLE_270 - ANGLE_BLOCK)
        {
            DrawTowerColumn(i, angle1, angle2);
        }
    }
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
        if (g_PogoMovtPhase < 12)
            sprite = g_PogoDirection ? SPRITE_POGO_R_WALK5 : SPRITE_POGO_L_WALK5;
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

    if (g_PogoState == POGO_ELEV && g_ElevDirection != 0)
    {
        //TODO: Draw moving elevator platform under Pogo
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


void GameStart()
{
    // Prepare Pogo
    g_PogoState = POGO_STAY;
    g_PogoDirection = 1;
    g_ControlState = CONTROL_NONE;
    g_SnowballDirection = 0;

    // Prepare tower
    g_TowerAngle = ANGLE_HALFBLOCK;
    g_TowerLevel = POSY_BRICK_HEIGHT * 2;
    g_TowerTopLineY = 0;

    Level_SelectTower(0);

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
        case SDLK_LCTRL:   // A button on Dingoo
        case SDLK_SPACE:
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
    g_BaseLineIndex = (int)(angle / ANGLE_BLOCK);
    g_BaseLineTB = TB_EMPTY;
    if (g_BaseLine >= 0 && g_BaseLine < g_TowerHeight)
        g_BaseLineTB = Level_GetTowerBlock(g_BaseLine, g_BaseLineIndex);
    g_CurLineTB = TB_EMPTY;
    if (g_CurLine >= 0 && g_CurLine < g_TowerHeight)
        g_CurLineTB = Level_GetTowerBlock(g_CurLine, g_BaseLineIndex);
}

void SnowballUpdate()
{
    if (g_SnowballDirection != 0)
    {
        float delta = ANGLE_ROTATION * 2;
        g_SnowballAngle += delta * g_SnowballDirection;
        if (g_SnowballAngle > ANGLE_360) g_SnowballAngle -= ANGLE_360;
        else if (g_SnowballAngle < 0.0f) g_SnowballAngle += ANGLE_360;
        g_SnowballAnglePassed += delta;
        if (fmod(g_SnowballAnglePassed + 0.01f, ANGLE_ROTATION * 2 * 4) < 0.02f)
            g_SnowballLevel++;

        if (g_SnowballAnglePassed > ANGLE_90 + ANGLE_HALFBLOCK)
            g_SnowballDirection = 0;
    }
}

// Process long-time logic
// Returns: 1 - process futher logic and controls, 0 - don't process other logic
int GameProcessLogicCont()
{
    // Pogo moves thru door and tunnel
    if (g_PogoState == POGO_DOOR)
    {
        if (g_PogoMovtPhase < 0)  // Part 1: Move in
        {
            g_PogoMovtPhase++;
            return 0;
        }
        if (fabs(g_TowerAngle - g_Pogo180Angle) > 0.01f)  // Part 2: 180 degree rotation
        {
            g_MoveX = g_PogoDirection ? 3 : -3;
            return 0;
        }
        if (g_PogoMovtPhase < 12)  // Part 3: Move out
        {
            g_PogoMovtPhase++;
            return 0;
        }
        // Rotation finished
        g_PogoMovtPhase = 0;
        g_PogoState = POGO_STAY;
        return 1;
    }

    // Pogo jumps
    if (g_PogoState == POGO_JUMP)
    {
        if (g_PogoMovtPhase < 8)
        {
            g_PogoMovtPhase++;
            g_MoveY = 1;
            g_MoveX = g_PogoDirection ? 1 : -1;
            return 0;
        }
        if (g_PogoMovtPhase < 10)  // Horz
        {
            g_PogoMovtPhase++;
            g_MoveX = g_PogoDirection ? 1 : -1;
            return 0;
        }
        else  // Jump finished, falling
        {
            g_PogoState = POGO_FALL;
            g_FallingDirection = g_PogoDirection ? 1 : -1;
            g_PogoMovtPhase = 0;
            return 1;
        }
    }

    // Elevator
    if (g_PogoState == POGO_ELEV)
    {
        if (g_PogoMovtPhase != 0)  // Phase for center Pogo to the elevator platform
        {
            float comp = fmod(g_TowerAngle, ANGLE_BLOCK) - ANGLE_HALFBLOCK;
            if (fabs(comp) > 0.01)
            {
                g_MoveX = (comp > 0.0f) ? -1 : 1;
                return 0;
            }
            g_PogoMovtPhase = 0;
        }

        if (g_TowerLevel % POSY_BRICK_HEIGHT == 0)
        {
            if (Elevator_IsAtStop())  // Arrived to station, stop the elevator
            {
                Elevator_Deactivate();
                g_PogoState = POGO_STAY;
                g_ElevDirection = 0;
                return 1;
            }
            else
            {
                Elevator_Move();
            }
        }

        g_MoveY = g_ElevDirection;
        return 0;
    }

    //TODO: turning

    // Drowns
    if (g_PogoState == POGO_DROWN)
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
    if (g_TowerLevel <= 0)  // Drown
    {
        g_MoveY = -1;
        g_PogoState = POGO_DROWN;
        g_PogoMovtPhase = 0;
        return 0;
    }

    if (g_PogoState == POGO_FALL)
    {
        if (g_TowerLevel % 8 == 0 && Level_IsPlatform(g_BaseLineTB))
        {
            g_PogoState = POGO_STAY;  // Restore state after landing
        }
        else
        {
            g_MoveX = g_FallingDirection;
            g_MoveY = -1;
            return 0;
        }
    }

    if (g_PogoState != POGO_FALL)
    {
        if (g_TowerLevel % 8 == 0 && g_BaseLineTB == TB_STEP_VANISHER)
        {
            Level_RemoveVanishStep(g_BaseLine, g_BaseLineIndex);
        }

        if (g_TowerLevel % 8 != 0 && Level_IsEmpty(g_CurLineTB))
        {
            g_FallingDirection = 0;
            if (g_PogoState == POGO_WALK)
                g_FallingDirection = g_PogoState ? 1 : -1;
            g_PogoState = POGO_FALL;
            g_MoveX = g_FallingDirection;
            g_MoveY = -1;
            return 0;
        }

        if (Level_IsPlatform(g_CurLineTB))  // Climb up
        {
            g_MoveY = 2;
            g_PogoState = POGO_STAY;
            return 0;
        }

        if (Level_IsEmpty(g_BaseLineTB))
        {
            g_PogoState = POGO_FALL;
            g_FallingDirection = 0;
            g_MoveY = -1;
            g_MoveX = g_PogoDirection ? 1 : -1;
            return 0;
        }
    }

    int sliding = Level_IsSliding(g_BaseLineTB);
    if (sliding)
    {
        g_MoveX = sliding;
    }

    return 1;
}

// Put Pogo to center of the current tower block
void GameAlignTowerAngle()
{
     float comp = fmod(g_TowerAngle, ANGLE_BLOCK) - ANGLE_HALFBLOCK;
     g_TowerAngle -= comp;
     if (g_TowerAngle > ANGLE_360) g_TowerAngle -= ANGLE_360;
     if (g_TowerAngle < 0.0f) g_TowerAngle += ANGLE_360;
}

// Process controlled logic like moving / turning / jumping
void GameProcessControls()
{
    // Walking left and right
    if (g_ControlState & CONTROL_LEFT)
    {
        if (!g_PogoDirection)
        {
            g_MoveX = -1;
            if (g_PogoState == POGO_STAY)
            {
                g_PogoState = POGO_WALK;
                g_PogoMovtPhase = 0;
            }
            else if (g_PogoState == POGO_WALK)
                g_PogoMovtPhase++;
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
            g_MoveX = 1;
            if (g_PogoState == POGO_STAY)
            {
                g_PogoState = POGO_WALK;
                g_PogoMovtPhase = 0;
            }
            else if (g_PogoState == POGO_WALK)
                g_PogoMovtPhase++;
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
        return;
    }

    // Door and 180 degree rotation
    if ((g_ControlState & CONTROL_UP) && g_PogoState == POGO_STAY && Level_IsDoor(g_CurLineTB))
    {
        GameAlignTowerAngle();  //TODO: Move to 180 degree rotation motion phase
        g_Pogo180Angle = fmod(g_TowerAngle + ANGLE_180, ANGLE_360);
        g_PogoState = POGO_DOOR;
        g_PogoMovtPhase = -12;
        return;
    }

    // Jump
    if ((g_ControlState & CONTROL_UP) && g_PogoState == POGO_WALK)
    {
        g_MoveY = -1;
        g_PogoState = POGO_JUMP;
        g_PogoMovtPhase = 0;
    }

    // Throw snowball
    if ((g_ControlState & CONTROL_FIRE) && g_SnowballDirection == 0)
    {
        g_SnowballAngle = g_TowerAngle;
        g_SnowballLevel = g_TowerLevel + 6;
        g_SnowballDirection = g_PogoDirection ? 1 : -1;
        g_SnowballAnglePassed = 0;
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

    // Check if Pogo died in the water
    if (g_TowerLevel < -POSY_POGO_HEIGHT * 2)
    {
        //TODO: Move to main() function level
        GameStart();  // Restart level
        return;
    }

    SnowballUpdate();
    Robot_New(g_TowerTopLineY);
    Robot_Update();
    Elevator_Update();

    if (GameProcessLogicCont())
    {
        if (GameProcessLogic())
        {
            GameProcessControls();
        }
    }

    GameMoveTower();
}

/////////////////////////////////////////////////////////////////////////////
// Menu

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


/////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#undef main  //HACK for VC error LNK1561: entry point must be defined
#endif

int main(int argc, char * argv[])
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

            DrawGameScreen();
            SDL_Flip(g_Screen);
            frames++;

            // Delay
            const int FRAME_TICKS = 30;  // 33.3 frames per second
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

/////////////////////////////////////////////////////////////////////////////
