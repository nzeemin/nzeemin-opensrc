// level.cpp

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include "main.h"


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
    { "stick",            '!', /*TBF_PLATFORM*/ },
    { "step",             '-', TBF_PLATFORM },
    { "vanisher step",    '.', TBF_PLATFORM },
    { "slider > step",    '>', TBF_PLATFORM },
    { "slider < step",    '<', TBF_PLATFORM },
    { "box",              'b', TBF_NONE },
    { "door",             '#', TBF_EMPTY },
    { "target door",      'T', TBF_EMPTY },
    { "stick top",         0,  TBF_STATION/*|TBF_PLATFORM*/ },
    { "stick middle",      0,  TBF_STATION/*|TBF_PLATFORM*/ },
    { "stick bottom",      0,  TBF_STATION/*|TBF_PLATFORM*/ },
    { "lift top",          0,  TBF_STATION|TBF_PLATFORM },
    { "lift middle",       0,  TBF_STATION|TBF_PLATFORM },
    { "lift bottom",      '^', TBF_STATION|TBF_PLATFORM },
    { "stick at door",     0,  },
    { "stick at target",   0,  },
    { "lift at door",      0 , TBF_STATION|TBF_PLATFORM },
    { "lift at target",    0 , TBF_STATION|TBF_PLATFORM },
};

#include "towers.inc"

struct TowerStruct
{
    const char* name;
    Uint8 colorr, colorg, colorb;
    unsigned int time;
    int robot;
    int height;
    const char** data;
};

static TowerStruct g_Mission1Towers[] = 
{
    { "Tower of Eyes",      200,60,60,      500, 0, 48,     Mission1Tower1 },
    { "Realm of Robots",    120,120,205,    600, 1, 72,     Mission1Tower2 },
};


int g_Level_TowerNumber = -1;  // Number of the current tower
int g_Level_TowerHeight = 0;
static Uint8 g_Level_Tower[256][TOWERWID];  // Current tower data


int Level_GetTowerCount()
{
    return sizeof(g_Mission1Towers) / sizeof(TowerStruct);
}

Uint8 conv_char2towercode(char ch) {
  if (ch)
      for (int x = 0; x < NUM_TBLOCKS; x++)
          // we can do that because we use only chars below 128
          if (ch == TowerBlocks[x].ch) return x;
  return TB_EMPTY;
}

void Level_SelectTower(int tower)
{
    if (tower < 0 || tower >= sizeof(g_Mission1Towers) / sizeof(TowerStruct))
        return;  // Out of range
    
    g_Level_TowerNumber = tower;
    g_Level_TowerHeight = g_Mission1Towers[g_Level_TowerNumber].height;

    // Prepare g_Level_Tower
    memset(g_Level_Tower, 0, sizeof(g_Level_Tower));
    const char** data = g_Mission1Towers[g_Level_TowerNumber].data;
    for (int row = 0; row < g_Level_TowerHeight; row++)
    {
        const char* line = g_Mission1Towers[g_Level_TowerNumber].data[g_Level_TowerHeight - row - 1];
        for (int col = 0; col < TOWERWID; col++)
        {
            g_Level_Tower[row][col] = conv_char2towercode(line[col]);
        }
    }
}

int Level_GetTowerSize()
{
    if (g_Level_TowerNumber < 0) return 0;

    return g_Level_TowerHeight;
}

Uint8 Level_GetTowerData(int row, int col)
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
        g_Mission1Towers[g_Level_TowerNumber].colorr, g_Mission1Towers[g_Level_TowerNumber].colorg, g_Mission1Towers[g_Level_TowerNumber].colorb);
}

int Level_IsPlatform(Uint8 data)
{
    return ((TowerBlocks[data].tf & TBF_PLATFORM) != 0);
}
int Level_IsEmpty(Uint8 data)
{
    return ((TowerBlocks[data].tf & TBF_EMPTY) != 0);
}

void Level_RemoveVanishStep(int row, int col)
{
    if (g_Level_Tower[row][col] == TB_STEP_VANISHER)
    {
        g_Level_Tower[row][col] = TB_EMPTY;
    }
}
