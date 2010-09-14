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
    RobotKindEnum   futurekind;
    int             time;
} g_Robot_Objects[MAX_OBJECTS];

int g_Robot_ReadyLine;
int g_Robot_ReadyIndex;

int g_Robot_NextCrossTimer;
int g_Robot_CrossDirection;


RobotKindEnum Robot_GetKind(int rob) { return g_Robot_Objects[rob].kind; }
float Robot_GetAngle(int rob) { return g_Robot_Objects[rob].angle; }
int Robot_GetLevel(int rob) { return g_Robot_Objects[rob].level; }
int Robot_GetTime(int rob) { return g_Robot_Objects[rob].time; }

void Robot_Initialize()
{
    for (int b = 0; b < MAX_OBJECTS; b++) {
        g_Robot_Objects[b].kind = OBJ_KIND_NOTHING;
        g_Robot_Objects[b].time = -1;
    }

    g_Robot_ReadyLine = 0;
    g_Robot_ReadyIndex = 0;

    g_Robot_NextCrossTimer = TIMEOUT_ROBOT_CROSS;
    g_Robot_CrossDirection = -1;
}

void Robot_PrepareNewRobot(int t)
{
    Uint8 robotTB = Level_GetTowerBlock(g_Robot_ReadyLine, g_Robot_ReadyIndex);

    switch (robotTB)
    {
    case TB_ROBOT1:  // freeze ball
        g_Robot_Objects[t].subkind = 1;
        g_Robot_Objects[t].futurekind = OBJ_KIND_FREEZEBALL;
        break;
    case TB_ROBOT2:  // jumping ball moving from the start
        g_Robot_Objects[t].subkind = 1;
        g_Robot_Objects[t].futurekind = OBJ_KIND_JUMPBALL;
        break;
    case TB_ROBOT3:  // jumping ball standing and then moving to you
        g_Robot_Objects[t].subkind = 0;
        g_Robot_Objects[t].futurekind = OBJ_KIND_JUMPBALL;
        break;
    case TB_ROBOT4:  // robot, up down
        g_Robot_Objects[t].subkind = 1;
        g_Robot_Objects[t].futurekind = OBJ_KIND_ROBOT_VERT;
        break;
    case TB_ROBOT5:  // robot, up down fast
        g_Robot_Objects[t].subkind = 2;
        g_Robot_Objects[t].futurekind = OBJ_KIND_ROBOT_VERT;
        break;
    case TB_ROBOT6:  // robot left right
        g_Robot_Objects[t].subkind = 1;
        g_Robot_Objects[t].futurekind = OBJ_KIND_ROBOT_HORIZ;
        break;
    case TB_ROBOT7:  // robot left right fast
        g_Robot_Objects[t].subkind = 2;
        g_Robot_Objects[t].futurekind = OBJ_KIND_ROBOT_HORIZ;
        break;
    }

    g_Robot_Objects[t].kind = OBJ_KIND_APPEAR;
    g_Robot_Objects[t].angle = g_Robot_ReadyIndex * ANGLE_BLOCK + ANGLE_HALFBLOCK;
    g_Robot_Objects[t].level = g_Robot_ReadyLine * POSY_BRICK_HEIGHT;
    g_Robot_Objects[t].time = 0;
}

void Robot_New(int toplevel)
{
    if (g_Robot_ReadyLine >= Level_GetTowerSize())
        return;

    int toprow = toplevel / POSY_BRICK_HEIGHT + 9;
    for (int t = 0; t < MAX_OBJECTS; t++)
    {
        if (g_Robot_Objects[t].kind != OBJ_KIND_NOTHING) continue;

        if (g_Robot_ReadyLine > toprow)
        {
            if (g_Robot_NextCrossTimer == -1) return;
            g_Robot_NextCrossTimer--;
            if (g_Robot_NextCrossTimer != -1) return;

            // Fill in the data for cross robot
            g_Robot_Objects[t].kind = OBJ_KIND_CROSS;
            g_Robot_Objects[t].subkind = g_Robot_CrossDirection;
            g_Robot_Objects[t].angle = (float)((g_Robot_CrossDirection > 0) ? POSX_VIEWPORT_WIDTH / 2 : -POSX_VIEWPORT_WIDTH / 2);
            g_Robot_Objects[t].level = toplevel;
            g_Robot_Objects[t].time = 0;
        }
        else
        {
            // Find next robot to create
            int okRobotFound = false;
            for (;;)
            {
                okRobotFound = Level_IsRobot(Level_GetTowerBlock(g_Robot_ReadyLine, g_Robot_ReadyIndex));
                if (okRobotFound) break;
                g_Robot_ReadyIndex = (g_Robot_ReadyIndex + 1) % 16;
                if (g_Robot_ReadyIndex == 0)
                {
                    g_Robot_ReadyLine++;
                    return;  // Robot not found
                }
            }

            Robot_PrepareNewRobot(t);

            Level_ClearBlock(g_Robot_ReadyLine, g_Robot_ReadyIndex);
        }
    }
}

void Robot_MoveHorizontal(int t)
{
    g_Robot_Objects[t].angle += ANGLE_ROTATION * g_Robot_Objects[t].subkind;
    if (g_Robot_Objects[t].angle < 0.0f) g_Robot_Objects[t].angle += ANGLE_360;
    else if (g_Robot_Objects[t].angle >= ANGLE_360) g_Robot_Objects[t].angle -= ANGLE_360;

    //TODO
}

void Robot_UpdateCross(int t)
{
    g_Robot_Objects[t].angle -= g_Robot_Objects[t].subkind;
    g_Robot_Objects[t].time -= g_Robot_Objects[t].subkind;

    if ((int)g_Robot_Objects[t].angle == 0)
        g_Robot_Objects[t].subkind *= 2;  // Speed up cross after the cross reached the middle

    if (g_Robot_CrossDirection > 0 && g_Robot_Objects[t].angle < -POSX_VIEWPORT_WIDTH / 2 ||
        g_Robot_CrossDirection < 0 && g_Robot_Objects[t].angle > POSX_VIEWPORT_WIDTH / 2)
    {
        g_Robot_Objects[t].kind = OBJ_KIND_NOTHING;
        g_Robot_Objects[t].time = 0;
        g_Robot_NextCrossTimer = TIMEOUT_ROBOT_CROSS;
        g_Robot_CrossDirection *= -1;
    }
}

void Robot_Update()
{
    for (int t = 0; t < MAX_OBJECTS; t++)
    {
        switch (g_Robot_Objects[t].kind)
        {
            case OBJ_KIND_NOTHING:
                break;
            case OBJ_KIND_CROSS:
                Robot_UpdateCross(t);
                break;
            case OBJ_KIND_DISAPPEAR:
                if (g_Robot_Objects[t].time < 12)
                    g_Robot_Objects[t].time++;
                else
                    g_Robot_Objects[t].kind = OBJ_KIND_NOTHING;
                break;
            case OBJ_KIND_APPEAR:
                if (g_Robot_Objects[t].time < 12)
                    g_Robot_Objects[t].time++;
                else
                {
                    g_Robot_Objects[t].kind = g_Robot_Objects[t].futurekind;
                    g_Robot_Objects[t].time = 0;
                }
                break;
            case OBJ_KIND_FREEZEBALL_FROZEN:
                g_Robot_Objects[t].time--;
                if (g_Robot_Objects[t].time > 0)
                    break;
                g_Robot_Objects[t].kind = OBJ_KIND_FREEZEBALL;
                //NOTE: don't put break here!
            case OBJ_KIND_FREEZEBALL:
                g_Robot_Objects[t].time++;  //STUB
                //TODO
                break;
            case OBJ_KIND_FREEZEBALL_FALLING:
                g_Robot_Objects[t].time++;  //STUB
                //TODO
                break;
            case OBJ_KIND_JUMPBALL:
                g_Robot_Objects[t].time++;  //STUB
                //TODO: Robot_UpdateJumpBall(t);
                break;
            case OBJ_KIND_ROBOT_HORIZ:
                g_Robot_Objects[t].time++;  //STUB
                //TODO
                break;
            case OBJ_KIND_ROBOT_VERT:
                g_Robot_Objects[t].time++;  //STUB
                //TODO
                break;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
