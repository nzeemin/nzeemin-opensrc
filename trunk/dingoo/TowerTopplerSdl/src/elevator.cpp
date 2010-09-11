// elevator.cpp

#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////


static struct ElevatorStruct
{
    float   angle;
    int     level;
    int     time;
} g_Elevators[MAX_ELE];

int g_Elevator_Active;  // Active elevator number: 0..MAX_ELE, -1 - not selected


void Elevator_Initialize()
{
    for (Uint8 t = 0; t < MAX_ELE; t++)
        g_Elevators[t].time = -1;

    g_Elevator_Active = -1;
}

void Elevator_Select(int row, int col)
{
    //TODO
}

void Elevator_Activate(int direction)
{
    //TODO
}

void Elevator_Move()
{
    //TODO
}

int Elevator_IsAtStop()
{
    return 0; //TODO
}

void Elevator_Deactivate()
{
    //TODO
}

void Elevator_Update()
{
    //TODO
}


/////////////////////////////////////////////////////////////////////////////
