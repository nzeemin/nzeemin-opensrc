// level.cpp

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

//#include <stdlib.h>
//#include <stdio.h>
#include <math.h>

#include <SDL.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////


#include "towers.inc"

struct TowerStruct
{
    const char* name;
    Uint8 colorr, colorg, colorb;
    unsigned int time;
    int robot;
    int height;
    const char** data;
}
static g_Mission1Towers[] = 
{
    { "Test Tower",         140,150,120,    300, 0, 19,     TestTower },  //DEBUG
    { "Tower of Eyes",      200,60,60,      500, 0, 48,     Mission1Tower1 },
    { "Realm of Robots",    120,120,205,    600, 1, 72,     Mission1Tower2 },
    { "Trap of Tricks",     170,170,170,    700, 2, 50,     Mission1Tower3 },
    { "Slippery Slide",     170,125,92,     800, 3, 67,     Mission1Tower4 },
    { "Broken Path",        0,155,155,      900, 4, 97,     Mission1Tower5 },
    { "Swimmers Delight",   226,90,90,     1000, 5, 65,     Mission1Tower6 },
    { "Nasty One",          130,221,130,   1100, 6, 81,     Mission1Tower7 },
    { "Edge of Doom",       226,136,0,     1200, 7, 97,     Mission1Tower8 },
};


/////////////////////////////////////////////////////////////////////////////


/* tower block flags */
#define TBF_NONE     0x0000
#define TBF_EMPTY    0x0001 /* block is not solid */
#define TBF_PLATFORM 0x0002 /* block is a platform */
#define TBF_STATION  0x0004 /* block is a lift station */
#define TBF_DEADLY   0x0008 /* block is deadly */
#define TBF_ROBOT    0x0010 /* block is a robot */


struct TowerBlockStruct
{
    const char *nam; /* name */
    char       ch;   /* representation in saved tower file */
    Uint16     tf;   /* flags; TBF_foo */
}
static TowerBlocks[NUM_TBLOCKS] =
{
    { "space",            ' ', TBF_EMPTY },
    { "lift top stop",    'v', TBF_EMPTY|TBF_STATION },
    { "lift middle stop", '+', TBF_EMPTY|TBF_STATION },
    { "lift bottom stop",  0,  TBF_EMPTY|TBF_STATION },
    { "robot 1",          '1', TBF_EMPTY|TBF_DEADLY|TBF_ROBOT },
    { "robot 2",          '2', TBF_EMPTY|TBF_DEADLY|TBF_ROBOT },
    { "robot 3",          '3', TBF_EMPTY|TBF_DEADLY|TBF_ROBOT },
    { "robot 4",          '4', TBF_EMPTY|TBF_DEADLY|TBF_ROBOT },
    { "robot 5",          '5', TBF_EMPTY|TBF_DEADLY|TBF_ROBOT },
    { "robot 6",          '6', TBF_EMPTY|TBF_DEADLY|TBF_ROBOT },
    { "robot 7",          '7', TBF_EMPTY|TBF_DEADLY|TBF_ROBOT },
    { "stick",            '!', 0 },
    { "step",             '-', TBF_PLATFORM },
    { "vanisher step",    '.', TBF_PLATFORM },
    { "slider > step",    '>', TBF_PLATFORM },
    { "slider < step",    '<', TBF_PLATFORM },
    { "box",              'b', TBF_NONE },
    { "door",             '#', TBF_EMPTY },
    { "target door",      'T', TBF_EMPTY },
    { "stick top",         0,  TBF_STATION },
    { "stick middle",      0,  TBF_STATION },
    { "stick bottom",      0,  TBF_STATION },
    { "lift top",          0,  TBF_STATION|TBF_PLATFORM },
    { "lift middle",       0,  TBF_STATION|TBF_PLATFORM },
    { "lift bottom",      '^', TBF_STATION|TBF_PLATFORM },
    { "stick at door",     0,  0 },
    { "stick at target",   0,  0 },
    { "lift at door",      0 , TBF_STATION|TBF_PLATFORM },
    { "lift at target",    0 , TBF_STATION|TBF_PLATFORM },
};


int g_Level_TowerNumber = -1;  // Number of the current tower
int g_Level_TowerHeight = 0;
static Uint8 g_Level_Tower[256][TOWERWID];  // Current tower data


int Level_GetTowerCount()
{
    return sizeof(g_Mission1Towers) / sizeof(TowerStruct);
}

Uint8 conv_char2towercode(char ch)
{
  if (ch)
      for (int x = 0; x < NUM_TBLOCKS; x++)
          // we can do that because we use only chars below 128
          if (ch == TowerBlocks[x].ch) return x;
  return TB_EMPTY;
}

void Level_SelectTower(int tower)
{
    if (tower < 0 || (size_t)tower >= sizeof(g_Mission1Towers) / sizeof(TowerStruct))
        return;  // Out of range
    
    g_Level_TowerNumber = tower;
    g_Level_TowerHeight = g_Mission1Towers[g_Level_TowerNumber].height;

    // Prepare g_Level_Tower
    memset(g_Level_Tower, 0, sizeof(g_Level_Tower));
    const char** data = g_Mission1Towers[g_Level_TowerNumber].data;
    for (int row = 0; row < g_Level_TowerHeight; row++)
    {
        const char* line = data[g_Level_TowerHeight - row - 1];
        for (int col = 0; col < TOWERWID; col++)
        {
            g_Level_Tower[row][col] = conv_char2towercode(line[col]);
        }
    }
}

const char* Level_GetTowerName()
{
    if (g_Level_TowerNumber < 0) return NULL;

    return g_Mission1Towers[g_Level_TowerNumber].name;
}
int Level_GetTowerSize()
{
    if (g_Level_TowerNumber < 0) return 0;

    return g_Level_TowerHeight;
}
unsigned int Level_GetTowerTime()
{
    if (g_Level_TowerNumber < 0) return 0;

    return g_Mission1Towers[g_Level_TowerNumber].time;
}

Uint8 Level_GetTowerBlock(int row, int col)
{
    if (g_Level_TowerNumber < 0) return 0;
    if (col < 0 || col >= TOWERWID) return 0;

    return g_Level_Tower[row][col];
}

Uint32 Level_GetTowerColor()
{
    if (g_Level_TowerNumber < 0) return 0;

    SDL_Surface *pSurface = SDL_GetVideoSurface();
    if (pSurface == NULL) return 0;

    return SDL_MapRGB(pSurface->format,
        g_Mission1Towers[g_Level_TowerNumber].colorr,
        g_Mission1Towers[g_Level_TowerNumber].colorg,
        g_Mission1Towers[g_Level_TowerNumber].colorb);
}

int Level_IsPlatform(Uint8 data)
{
    return ((TowerBlocks[data].tf & TBF_PLATFORM) != 0);
}
int Level_IsEmpty(Uint8 data)
{
    return ((TowerBlocks[data].tf & TBF_EMPTY) != 0);
}
int Level_IsStation(Uint8 data)
{
    return ((TowerBlocks[data].tf & TBF_STATION) != 0);
}
int Level_IsRobot(Uint8 data)
{
    return ((TowerBlocks[data].tf & TBF_ROBOT) != 0);
}

void Level_Platform2Stick(int row, int col)
{
    if (g_Level_Tower[row][col] == TB_ELEV_TOP) g_Level_Tower[row][col] = TB_STICK_TOP;
    else if (g_Level_Tower[row][col] == TB_ELEV_MIDDLE) g_Level_Tower[row][col] = TB_STICK_MIDDLE;
    else if (g_Level_Tower[row][col] == TB_ELEV_BOTTOM) g_Level_Tower[row][col] = TB_STICK_BOTTOM;
    else if (g_Level_Tower[row][col] == TB_STEP) g_Level_Tower[row][col] = TB_STICK;
}
void Level_Stick2Platform(int row, int col)
{
    if (g_Level_Tower[row][col] == TB_STICK_TOP) g_Level_Tower[row][col] = TB_ELEV_TOP;
    else if (g_Level_Tower[row][col] == TB_STICK_MIDDLE) g_Level_Tower[row][col] = TB_ELEV_MIDDLE;
    else if (g_Level_Tower[row][col] == TB_STICK_BOTTOM) g_Level_Tower[row][col] = TB_ELEV_BOTTOM;
    else if (g_Level_Tower[row][col] == TB_STICK_DOOR) g_Level_Tower[row][col] = TB_ELEV_DOOR;
    else if (g_Level_Tower[row][col] == TB_STICK_DOOR_TARGET) g_Level_Tower[row][col] = TB_ELEV_DOOR_TARGET;
    else if (g_Level_Tower[row][col] == TB_STICK) g_Level_Tower[row][col] = TB_STEP;
}
void Level_Stick2Empty(int row, int col)
{
    if (g_Level_Tower[row][col] == TB_STICK_TOP) g_Level_Tower[row][col] = TB_STATION_TOP;
    else if (g_Level_Tower[row][col] == TB_STICK_MIDDLE) g_Level_Tower[row][col] = TB_STATION_MIDDLE;
    else if (g_Level_Tower[row][col] == TB_STICK_BOTTOM) g_Level_Tower[row][col] = TB_STATION_BOTTOM;
    else if (g_Level_Tower[row][col] == TB_STICK_DOOR_TARGET) g_Level_Tower[row][col] = TB_DOOR_TARGET;
    else if (g_Level_Tower[row][col] == TB_STICK_DOOR) g_Level_Tower[row][col] = TB_DOOR;
    else if (g_Level_Tower[row][col] == TB_STICK) g_Level_Tower[row][col] = TB_EMPTY;
}
void Level_Empty2Stick(int row, int col)
{
    if (g_Level_Tower[row][col] == TB_STATION_TOP) g_Level_Tower[row][col] = TB_STICK_TOP;
    else if (g_Level_Tower[row][col] == TB_STATION_MIDDLE) g_Level_Tower[row][col] = TB_STICK_MIDDLE;
    else if (g_Level_Tower[row][col] == TB_STATION_BOTTOM) g_Level_Tower[row][col] = TB_STICK_BOTTOM;
    else if (g_Level_Tower[row][col] == TB_DOOR) g_Level_Tower[row][col] = TB_STICK_DOOR;
    else if (g_Level_Tower[row][col] == TB_DOOR_TARGET) g_Level_Tower[row][col] = TB_STICK_DOOR_TARGET;
    else if (g_Level_Tower[row][col] == TB_EMPTY) g_Level_Tower[row][col] = TB_STICK;
}
void Level_Platform2Empty(int row, int col)
{
    if (g_Level_Tower[row][col] == TB_ELEV_TOP) g_Level_Tower[row][col] = TB_STATION_TOP;
    else if (g_Level_Tower[row][col] == TB_ELEV_MIDDLE) g_Level_Tower[row][col] = TB_STATION_MIDDLE;
    else if (g_Level_Tower[row][col] == TB_ELEV_BOTTOM) g_Level_Tower[row][col] = TB_STATION_BOTTOM;
    else if (g_Level_Tower[row][col] == TB_ELEV_DOOR_TARGET) g_Level_Tower[row][col] = TB_DOOR_TARGET;
    else if (g_Level_Tower[row][col] == TB_ELEV_DOOR) g_Level_Tower[row][col] = TB_DOOR;
    else if (g_Level_Tower[row][col] == TB_STEP) g_Level_Tower[row][col] = TB_EMPTY;
}

void Level_RemoveVanishStep(int row, int col)
{
    if (g_Level_Tower[row][col] == TB_STEP_VANISHER)
    {
        g_Level_Tower[row][col] = TB_EMPTY;
    }
}
void Level_ClearBlock(int row, int col)
{
    g_Level_Tower[row][col] = TB_EMPTY;
}

Uint8 Level_PutPlatform(int row, int col)
{
    Uint8 erg = g_Level_Tower[row][col];

    g_Level_Tower[row][col] = TB_ELEV_BOTTOM;

    return erg;
}

void Level_Restore(int row, int col, Uint8 data)
{
    g_Level_Tower[row][col] = data;
}

static int InsideCyclicIntervall(int x, int start, int end, int cycle)
{
    while (x < start) x += cycle;
    while (x >= end) x -= cycle;

    return (x >= start) && (x < end);
}

// Returns true, if the given figure can be at the given position without colliding with fixed objects of the tower
//   typ = 0 - Pogo, 1 - robot, 2 - snowball
int Level_TestFigure(float fangle, int vert, int back, int fore, int typ, int height, int width)
{
    int anglepos = ((int)(fangle / ANGLE_ROTATION)) % TOWER_ANGLECOUNT;
    int iback = ((anglepos + back) / 8) % TOWERWID;
    int ifore = (((anglepos + fore) / 8) + 1) % TOWERWID;

    int y = vert / POSY_BRICK_HEIGHT;
    vert = (vert % POSY_BRICK_HEIGHT) / 2;

    int x = 0;
    switch (typ)
    {
    case 0:  // Pogo
        x = (vert == 3) ? 3 : 2;
        break;
    case 1:  // Robot
        x = (vert == 0) ? 2 : 3;
        break;
    case 2:  // Snowball
        x = (vert == 0) ? 0 : 1;
        break;
    }

    int k, t;
    do
    {
        k = x;
        do
        {
            if (Level_IsPlatform(Level_GetTowerBlock(k + y, iback)))
                return FALSE;
            else if (Level_IsStick(Level_GetTowerBlock(k + y, iback)))
            {
                t = iback * 8 + height;
                if (InsideCyclicIntervall(anglepos, t, t + width, TOWER_ANGLECOUNT))
                    return FALSE;
            }
            else if (Level_GetTowerBlock(k + y, iback) == TB_BOX)
            {
                t = iback * 8 + height;
                if (InsideCyclicIntervall(anglepos, t, t + width, TOWER_ANGLECOUNT))
                {
                    if (typ == 2)
                        Main_SnowballHitsBox(k + y, iback);
                    return FALSE;
                }
            }
            k--;
        }
        while (k != -1);
        iback = (iback + 1) % TOWERWID;
    }
    while (iback != ifore);

    return TRUE;
}
