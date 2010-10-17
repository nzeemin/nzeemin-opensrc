// robot.cpp

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

//#include <stdlib.h>
//#include <stdio.h>
#include <math.h>

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

int Robot_PogoCollison(float angle, int level)
{
    int j;

    for (int t = 0; t < MAX_OBJECTS; t++)
    {
        if (g_Robot_Objects[t].kind != OBJ_KIND_CROSS &&
            g_Robot_Objects[t].kind != OBJ_KIND_NOTHING &&
            g_Robot_Objects[t].kind != OBJ_KIND_DISAPPEAR &&
            g_Robot_Objects[t].kind != OBJ_KIND_APPEAR)
        {
            float iangle = fmod(angle + ANGLE_360 - g_Robot_Objects[t].angle, ANGLE_360);
            j = g_Robot_Objects[t].level - level;
            if ((iangle < ANGLE_ROTATION * 4 || iangle > ANGLE_360 - ANGLE_ROTATION * 4) &&
                (-8 < j) && (j < 8))
                return t;
        }
        else if (g_Robot_Objects[t].kind == OBJ_KIND_CROSS)
        {
            int xpos = (int)(g_Robot_Objects[t].angle);
            if (xpos + POSX_ROBOT_HALFWIDTH >= -POSX_POGO_HALFWIDTH && xpos - POSX_ROBOT_HALFWIDTH <= POSX_POGO_HALFWIDTH &&
                g_Robot_Objects[t].level <= level + POSY_POGO_HEIGHT && g_Robot_Objects[t].level + POSY_ROBOT_HEIGHT >= level)
            {
                return t;
            }
        }
    }

    return -1;
}

//TODO: Robot_SnowballCollision()

// Returns the index of the figure the given figure (nr) collides with or -1 if there is no such object
int Robot_FigureCollision(int nr)
{
    return -1;  //TODO
}

// Returns true, if the robot cannot be at the given position without colliding
int Robot_TestEr(int t)
{
    int test = Level_TestFigure(g_Robot_Objects[t].angle, g_Robot_Objects[t].level, -2, 1, 1, 1, 8);
    return !test;
}

// Tests the underground of the given object (only used for freeze ball) returns
//   0 if nothing needs to be done
//   1 for falling
//   2 for reverse direction
int Robot_TestUnderground(int nr)
{
    int row, col;
    Uint8 data;

    row = g_Robot_Objects[nr].level / POSY_BRICK_HEIGHT - 1;
    col = (int)(g_Robot_Objects[nr].angle / ANGLE_ROTATION) / 8;
    data = Level_GetTowerBlock(row, col);

    if (data == TB_BOX) return 0;
    if (Level_IsPlatform(data) || Level_IsStick(data))
    {
        if (Level_IsElevator(data))
            return 2;
        else
            return 0;
    }

    if ((int)(g_Robot_Objects[nr].angle / ANGLE_ROTATION) % 16 < 2)
    {
        Uint8 dataprev = Level_GetTowerBlock(row, (col - 1) % 16);
        if (Level_IsEmpty(dataprev)) return 1;
        if (Level_IsDoor(dataprev)) return 1;
        if ((g_Robot_Objects[nr].subkind & 0x80) == 0)
            return 2;
        else
            return 0;
    }

    if ((int)(g_Robot_Objects[nr].angle / ANGLE_ROTATION) % 16 > 6)
    {
        Uint8 dataprev = Level_GetTowerBlock(row, (col - 1) % 16);
        if (Level_IsEmpty(dataprev)) return 1;
        if (Level_IsDoor(dataprev)) return 1;
        if ((g_Robot_Objects[nr].subkind & 0x80) == 0)
            return 2;
        else
            return 0;
    }

    return 1;
}

void Robot_MoveHorizontal(int t)
{
    g_Robot_Objects[t].angle += ANGLE_ROTATION * g_Robot_Objects[t].subkind;
    if (g_Robot_Objects[t].angle < 0.0f) g_Robot_Objects[t].angle += ANGLE_360;
    else if (g_Robot_Objects[t].angle >= ANGLE_360) g_Robot_Objects[t].angle -= ANGLE_360;

    if (Robot_TestEr(t) || Robot_FigureCollision(t) != -1)
    {
        g_Robot_Objects[t].subkind = -g_Robot_Objects[t].subkind;

        g_Robot_Objects[t].angle += ANGLE_ROTATION * g_Robot_Objects[t].subkind;
        if (g_Robot_Objects[t].angle < 0.0f) g_Robot_Objects[t].angle += ANGLE_360;
        else if (g_Robot_Objects[t].angle >= ANGLE_360) g_Robot_Objects[t].angle -= ANGLE_360;
    }
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

// Remove objects that drop below the screen
int Robot_CheckVerticalPosition(int level, int t)
{
    if (g_Robot_Objects[t].level + POSY_ROBOT_KILL < level)
    {
        g_Robot_Objects[t].kind = OBJ_KIND_DISAPPEAR;
        g_Robot_Objects[t].time = 0;
        return TRUE;
    }
    else
        return FALSE;
}

// Checks if the robot is at a position that it can not be if not remove it
int Robot_CheckValidPosition(int t)
{
    if (Robot_TestEr(t))
    {
        g_Robot_Objects[t].kind = OBJ_KIND_DISAPPEAR;
        g_Robot_Objects[t].time = 0;
        return TRUE;
    }
    else
        return FALSE;
}

void Robot_Drown(int t)
{
    g_Robot_Objects[t].level = 0;
    g_Robot_Objects[t].time = 0;
    g_Robot_Objects[t].kind = OBJ_KIND_DISAPPEAR;
}

void Robot_Update()
{
    /* the vertical movement of the jumping ball */
    static long jumping_ball[11] = {
        2, 2, 1, 1, 0, 0, -1, -1, -2, -2, -4
    };

    int h;

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
                if (Robot_CheckVerticalPosition(Main_GetTowerLevel(), t) ||
                    Robot_CheckValidPosition(t))
                    break;

                switch (Robot_TestUnderground(t))
                {
                case 1:
                    g_Robot_Objects[t].kind = OBJ_KIND_FREEZEBALL_FALLING;
                    g_Robot_Objects[t].time = 10;
                    break;
                case 2:
                    g_Robot_Objects[t].subkind = -g_Robot_Objects[t].subkind;
                    break;
                }

                Robot_MoveHorizontal(t);
                break;

            case OBJ_KIND_FREEZEBALL_FALLING:
                if (Robot_CheckVerticalPosition(Main_GetTowerLevel(), t) ||
                    Robot_CheckValidPosition(t))
                    break;

                Robot_MoveHorizontal(t);

                if (g_Robot_Objects[t].level + jumping_ball[g_Robot_Objects[t].time] < 0)
                {
                    Robot_Drown(t);
                    break;
                }

                h = jumping_ball[g_Robot_Objects[t].time];
                while (h != 0)
                {
                    g_Robot_Objects[t].level += h;
                    if (!Robot_TestEr(t) || Robot_FigureCollision(t) != -1) break;
                    g_Robot_Objects[t].level -= h;

                    if (h > 0) h--; else h++;
                }

                if (h == 0 && jumping_ball[g_Robot_Objects[t].time] < 0)
                {
                    g_Robot_Objects[t].kind = OBJ_KIND_FREEZEBALL;
                    g_Robot_Objects[t].time = 0;
                }
                break;

            case OBJ_KIND_JUMPBALL:
                if (Robot_CheckVerticalPosition(Main_GetTowerLevel(), t) ||
                    Robot_CheckValidPosition(t))
                    break;

                h = g_Robot_Objects[t].subkind;
                Robot_MoveHorizontal(t);

                if (g_Robot_Objects[t].level + jumping_ball[g_Robot_Objects[t].time] < 0)
                {
                    Robot_Drown(t);
                    break;
                }

                h = jumping_ball[g_Robot_Objects[t].time];
                while (h != 0)
                {
                    g_Robot_Objects[t].level += h;
                    if (!Robot_TestEr(t) || Robot_FigureCollision(t) != -1) break;
                    g_Robot_Objects[t].level -= h;

                    if (h > 0) h--; else h++;
                }

                // Ball is bouncing
                if (h == 0 && jumping_ball[g_Robot_Objects[t].time] < 0)
                {
                    g_Robot_Objects[t].time = 0;  // restart bounce cyclus

                    // start the bounding ball moving sideways in direction of the animal
                    if (g_Robot_Objects[t].subkind == 0 &&
                        Main_IsPogoWalking() && g_Robot_Objects[t].level == Main_GetTowerLevel())
                    {
                        // check if the animal is near enough to the ball
                        float w = fmod(g_Robot_Objects[t].angle + ANGLE_360 - Main_GetTowerAngle(), ANGLE_360);
                        if (w < ANGLE_90 || w > ANGLE_270)
                            g_Robot_Objects[t].subkind = (w < ANGLE_180) ? -1 : 1;
                    }
                    break;
                }

                g_Robot_Objects[t].time++;
                if (g_Robot_Objects[t].time > 10) g_Robot_Objects[t].time = 10; 
                break;

            case OBJ_KIND_ROBOT_HORIZ:
                if (Robot_CheckVerticalPosition(Main_GetTowerLevel(), t) ||
                    Robot_CheckValidPosition(t))
                    break;

                Robot_MoveHorizontal(t);

                g_Robot_Objects[t].time++;
                break;

            case OBJ_KIND_ROBOT_VERT:
                if (Robot_CheckVerticalPosition(Main_GetTowerLevel(), t) ||
                    Robot_CheckValidPosition(t))
                    break;

                if ((g_Robot_Objects[t].level + g_Robot_Objects[t].subkind) < 0)
                    Robot_Drown(t);
                else
                {
                    g_Robot_Objects[t].level += g_Robot_Objects[t].subkind;

                    if (Robot_TestEr(t) || Robot_FigureCollision(t) != -1)
                    {
                        g_Robot_Objects[t].subkind *= -1;
                        g_Robot_Objects[t].level += g_Robot_Objects[t].subkind;
                    }
                }

                g_Robot_Objects[t].time++;
                break;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
