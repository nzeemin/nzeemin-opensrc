// elevator.cpp

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////


static struct ElevatorStruct
{
    int     col;
    int     row;
    int     time;
    int     bg;
} g_Elevators[MAX_ELE];

int g_Elevator_Selected;  // Selected elevator number: 0..MAX_ELE, -1 - not selected
int g_Elevator_Direction;


void Elevator_Initialize()
{
    for (Uint8 t = 0; t < MAX_ELE; t++)
        g_Elevators[t].time = -1;

    g_Elevator_Selected = -1;
}

void Elevator_Select(int row, int col)
{
    int what = 0;
    for (int t = 0; t < MAX_ELE; t++)
    {
        if ((g_Elevators[t].time == -1) && (what == 0))
        {
            what = 1;
            g_Elevator_Selected = t;
        }
        if ((g_Elevators[t].col == col) && (g_Elevators[t].row == row))
        {
            what = 2;
            g_Elevator_Selected = t;
        }
    }

    g_Elevators[g_Elevator_Selected].col = col;
    g_Elevators[g_Elevator_Selected].row = row;
    g_Elevators[g_Elevator_Selected].time = -1;
}

void Elevator_Activate(int direction)
{
    Level_Platform2Stick(g_Elevators[g_Elevator_Selected].row, g_Elevators[g_Elevator_Selected].col);
    g_Elevator_Direction = direction;
    if (direction == -1)
        Elevator_Move();
}

void Elevator_Move()
{
    if (g_Elevator_Direction == 1)
    {
        g_Elevators[g_Elevator_Selected].row++;
        Level_Empty2Stick(g_Elevators[g_Elevator_Selected].row, g_Elevators[g_Elevator_Selected].col);
    }
    else
    {
        Level_Stick2Empty(g_Elevators[g_Elevator_Selected].row, g_Elevators[g_Elevator_Selected].col);
        g_Elevators[g_Elevator_Selected].row--;
    }
}

int Elevator_IsAtStop()
{
    if (g_Elevator_Direction == 1)
        return Level_IsStation(
            Level_GetTowerBlock(g_Elevators[g_Elevator_Selected].row + 1, g_Elevators[g_Elevator_Selected].col));
    else
        return Level_IsStation(
            Level_GetTowerBlock(g_Elevators[g_Elevator_Selected].row, g_Elevators[g_Elevator_Selected].col));
}

void Elevator_Deactivate()
{
    if (g_Elevator_Direction == 1)
        Elevator_Move();

    Level_Stick2Platform(g_Elevators[g_Elevator_Selected].row, g_Elevators[g_Elevator_Selected].col);

    int ae = g_Elevator_Selected;
    g_Elevator_Selected = -1;

    if (!Level_IsStation(Level_GetTowerBlock(g_Elevators[ae].row, g_Elevators[ae].col)))
    {
        g_Elevators[ae].bg = Level_PutPlatform(g_Elevators[ae].row, g_Elevators[ae].col);
    }
    else
    {
        if (Level_IsBottomStation(Level_GetTowerBlock(g_Elevators[ae].row, g_Elevators[ae].col)))
            return;
        else
            g_Elevators[ae].bg = Level_GetTowerBlock(g_Elevators[ae].row, g_Elevators[ae].col);
    }

    g_Elevators[ae].time = TIMEOUT_ELEVATOR_RETURN;
}

void Elevator_Update()
{
    for (Uint8 t = 0; t < MAX_ELE; t++)
    {
        if (g_Elevators[t].time == 0)
        {
            Level_Restore(g_Elevators[t].row, g_Elevators[t].col, g_Elevators[t].bg);
            Level_Platform2Empty(g_Elevators[t].row, g_Elevators[t].col);
            g_Elevators[t].row--;
            Level_Stick2Platform(g_Elevators[t].row, g_Elevators[t].col);
            if (Level_IsBottomStation(Level_GetTowerBlock(g_Elevators[t].row, g_Elevators[t].col)))
            {
                g_Elevators[t].time = -1;
                //TODO: top_sidemove
            }
            else
            {
                g_Elevators[t].bg = Level_PutPlatform(g_Elevators[t].row, g_Elevators[t].col);
            }
        }

        if (g_Elevators[t].time > 0)
            g_Elevators[t].time--;
    }
}


/////////////////////////////////////////////////////////////////////////////
