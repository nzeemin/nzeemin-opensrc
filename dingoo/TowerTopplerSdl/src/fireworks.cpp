// fireworks.cpp
//
// The fireworks code based on the blog post:
// "How To: Create a Simple Fireworks Effect in OpenGL and SDL"
// http://r3dux.org/2010/10/how-to-create-a-simple-fireworks-effect-in-opengl/

#include <SDL.h>

#include "main.h"


/////////////////////////////////////////////////////////////////////////////


const int FIREWORKS = 12;           // Number of fireworks
const int FIREWORK_PARTICLES = 32;  // Number of particles per firework

struct FireworkStruct
{
    float   x[FIREWORK_PARTICLES];
    float   y[FIREWORK_PARTICLES];
    float   xSpeed[FIREWORK_PARTICLES];
    float   ySpeed[FIREWORK_PARTICLES];
    int     framesUntilLaunch;
    float   particleSize;
    int     hasExploded;
    int     red, green, blue;
    float   alpha;
}
static g_Fireworks[FIREWORKS];

const float Firework_GRAVITY = 0.08f;
const float Firework_baselineYSpeed = -3.0f;
const float Firework_maxYSpeed = -3.5f;


void Firework_Start(FireworkStruct* firework)
{
    // Pick an initial x location and  random x/y speeds
    float xLoc = (rand() / (float)RAND_MAX) * SCREEN_WIDTH;
    float xSpeedVal = -2 + (rand() / (float)RAND_MAX) * 4.0f;
    float ySpeedVal = Firework_baselineYSpeed + ((float)rand() / (float)RAND_MAX) * Firework_maxYSpeed;
 
    // Set initial x/y location and speeds
    for (int loop = 0; loop < FIREWORK_PARTICLES; loop++)
    {
        firework->x[loop] = xLoc;
        firework->y[loop] = SCREEN_HEIGHT + 10.0f; // Push the particle location down off the bottom of the screen
        firework->xSpeed[loop] = xSpeedVal;
        firework->ySpeed[loop] = ySpeedVal;
    }

    firework->red   = 192 + (rand() % 64);
    firework->green = 192 + (rand() % 64);
    firework->blue  = 192 + (rand() % 64);
    firework->alpha = 1.0f;

    firework->framesUntilLaunch = ((int)rand() % 400);

    firework->hasExploded = FALSE;
}

void Firework_Move(FireworkStruct* firework)
{
    for (int loop = 0; loop < FIREWORK_PARTICLES; loop++)
    {
        // Once the firework is ready to launch start moving the particles
        if (firework->framesUntilLaunch <= 0)
        {
            firework->x[loop] += firework->xSpeed[loop];
            firework->y[loop] += firework->ySpeed[loop];
            firework->ySpeed[loop] += Firework_GRAVITY;
        }
    }
    firework->framesUntilLaunch--;

    // Once a fireworks speed turns positive (i.e. at top of arc) - blow it up!
    if (firework->ySpeed[0] > 0.0f)
    {
        for (int loop2 = 0; loop2 < FIREWORK_PARTICLES; loop2++)
        {
            // Set a random x and y speed beteen -4 and + 4
            firework->xSpeed[loop2] = -4 + (rand() / (float)RAND_MAX) * 8;
            firework->ySpeed[loop2] = -4 + (rand() / (float)RAND_MAX) * 8;
        }
 
        firework->hasExploded = true;
    }
}

void Firework_Explode(FireworkStruct* firework)
{
    for (int loop = 0; loop < FIREWORK_PARTICLES; loop++)
    {
        // Dampen the horizontal speed by 1% per frame
        firework->xSpeed[loop] *= 0.99f;
 
        // Move the particle
        firework->x[loop] += firework->xSpeed[loop];
        firework->y[loop] += firework->ySpeed[loop];
 
        // Apply gravity to the particle's speed
        firework->ySpeed[loop] += Firework_GRAVITY;
    }
 
    // Fade out the particles (alpha is stored per firework, not per particle)
    if (firework->alpha > 0.0f)
    {
        firework->alpha -= 0.01f;
    }
    else // Once the alpha hits zero reset the firework
    {
        Firework_Start(firework);
    }
}

void Firework_Initialize()
{
    ::memset(g_Fireworks, 0, sizeof(g_Fireworks));
    for (int i = 0; i < FIREWORKS; i++)
        Firework_Start(g_Fireworks + i);
}

void Firework_Update(SDL_Surface *screen)
{
    SDL_Rect rc;

    // Draw fireworks
    for (int i = 0; i < FIREWORKS; i++)
    {
        FireworkStruct* firework = g_Fireworks + i;
        Uint32 color;
        if (firework->hasExploded)
            color = SDL_MapRGB(screen->format,
                (int)(firework->red * firework->alpha),
                (int)(firework->green * firework->alpha),
                (int)(firework->blue * firework->alpha));
        else
            color = SDL_MapRGB(screen->format, 255, 255, 0);
        for (int p = 0; p < FIREWORK_PARTICLES; p++)
        {
            rc.x = (int)(firework->x[p] + 0.5f);
            rc.y = (int)(firework->y[p] + 0.5f);
            rc.w = rc.h = 2;
            SDL_FillRect(screen, &rc, color);
        }
    }

    for (int i = 0; i < FIREWORKS; i++)
    {
        if (!g_Fireworks[i].hasExploded)
            Firework_Move(g_Fireworks + i);
        else
            Firework_Explode(g_Fireworks + i);
    }
}


/////////////////////////////////////////////////////////////////////////////
