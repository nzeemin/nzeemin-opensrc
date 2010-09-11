// robot.cpp

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////


static struct ObjectStruct
{
    float           angle;
    int             level;
    RobotKindEnum   kind;
    int             subkind;
    int             futurekind;
    int             time;
} g_Robot_Objects[MAX_OBJECTS];

int g_Robot_ReadyLine;
int g_Robot_ReadyIndex;

int g_NextCrossTimer;


RobotKindEnum Robot_GetKind(int rob) { return g_Robot_Objects[rob].kind; }
float Robot_GetAngle(int rob) { return g_Robot_Objects[rob].angle; }
int Robot_GetLevel(int rob) { return g_Robot_Objects[rob].level; }

void Robot_Initialize()
{
    for (int b = 0; b < MAX_OBJECTS; b++) {
        g_Robot_Objects[b].kind = OBJ_KIND_NOTHING;
        g_Robot_Objects[b].time = -1;
    }

    g_Robot_ReadyLine = 0;
    g_Robot_ReadyIndex = 0;

    g_NextCrossTimer = 125;
}

void Robot_New(int toplevel)
{
    if (g_Robot_ReadyLine >= Level_GetTowerSize())
        return;

    for (int t = 0; t < MAX_OBJECTS; t++)
    {
        if (g_Robot_Objects[t].kind != OBJ_KIND_NOTHING) continue;

        // Find next robot to create
        int okRobotFound = false;
        for (;;)
        {
            okRobotFound = Level_IsRobot(Level_GetTowerData(g_Robot_ReadyLine, g_Robot_ReadyIndex));
            if (okRobotFound) break;
            g_Robot_ReadyIndex = (g_Robot_ReadyIndex + 1) % 16;
            if (g_Robot_ReadyIndex == 0)
            {
                g_Robot_ReadyLine++;
                return;  // Robot not found
            }
        }

        // Prepare data for robot
        g_Robot_Objects[t].kind = OBJ_KIND_FREEZEBALL;  //STUB
        g_Robot_Objects[t].subkind = 1;  //STUB
        g_Robot_Objects[t].futurekind = OBJ_KIND_FREEZEBALL;  //STUB
        g_Robot_Objects[t].angle = g_Robot_ReadyIndex * ANGLE_BLOCK + ANGLE_HALFBLOCK;
        g_Robot_Objects[t].level = g_Robot_ReadyLine * POSY_BRICK_HEIGHT;

        Level_ClearBlock(g_Robot_ReadyLine, g_Robot_ReadyIndex);
    }
}

void Robot_Update()
{
    //TODO
}


/////////////////////////////////////////////////////////////////////////////
