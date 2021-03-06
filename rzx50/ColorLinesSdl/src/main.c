/****************************************************************************

Color Lines SDL

****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS  // For warning C4996: 'sprintf': This function or variable may be unsafe.

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>


void MenuStart();


#define VERSION_MAJOR    0
#define VERSION_MINOR    3

#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   272

#define FONT_WIDTH          8
#define FONT_HEIGHT         16

#define POSX_BOARD_LEFT     130
#define POSX_TILES_LEFT     (POSX_BOARD_LEFT + 2)
#define POSY_BOARD_TOP      30
#define POSY_TILES_TOP      (POSY_BOARD_TOP + 2)
#define POSX_NEXT_LEFT      (POSX_BOARD_LEFT + 110)

#define POSX_BOARD_RIGHT    (POSX_BOARD_LEFT + 4 + 9 * 24)

enum GameMode
{
    GAMEMODE_MENU = 0,
    GAMEMODE_PLAY = 1,
};

enum BallEnum
{
    BALL_NONE       = 0,
    BALL_RED        = 1,
    BALL_GREEN      = 2,
    BALL_BLUE       = 3,
    BALL_YELLOW     = 4,
    BALL_CYAN       = 5,
    BALL_MAGENTA    = 6,
    BALL_BROWN      = 7,
    BALLCOLORCOUNT  = 7
};

SDL_Surface *g_pSurface = NULL;
SDL_Surface *g_pSprites = NULL;
SDL_Surface *g_pFont = NULL;
enum GameMode g_GameMode = GAMEMODE_MENU;
int g_okQuit = 0;
int g_Board[9][9];
int g_Score;
int g_KingScore;
int g_NextColors[3];
int g_TileCursorX, g_TileCursorY;
int g_okShowTileCursor = 0;
int g_okTileCursorPhase = 0;
int g_TileSelectX, g_TileSelectY;

enum SpriteEnum
{
    SPRITE_KING = 0,
    SPRITE_PRETENDER = 1,
    SPRITE_TILE = 2,
    SPRITE_TILE_RED = 3,
    SPRITE_TILE_GREEN = 4,
    SPRITE_TILE_BLUE = 5,
    SPRITE_TILE_YELLOW = 6,
    SPRITE_TILE_CYAN = 7,
    SPRITE_TILE_MAGENTA = 8,
    SPRITE_TILE_BROWN = 9,
    SPRITE_NEXT = 10,
    SPRITE_NEXT_RED = 11,
    SPRITE_NEXT_GREEN = 12,
    SPRITE_NEXT_BLUE = 13,
    SPRITE_NEXT_YELLOW = 14,
    SPRITE_NEXT_CYAN = 15,
    SPRITE_NEXT_MAGENTA = 16,
    SPRITE_NEXT_BROWN = 17,
    SPRITECOUNT = 18
};
int SPRITECOORDS[SPRITECOUNT * 4] =
{
      1,  1,  70, 94,  // SPRITE_KING
     72,  1,  66, 94,
      1, 96,  24, 24,  // SPRITE_TILE
     26, 96,  24, 24,
     51, 96,  24, 24,
     76, 96,  24, 24,
    101, 96,  24, 24,
    126, 96,  24, 24,
    151, 96,  24, 24,
    176, 96,  24, 24,
      1, 121, 11, 10,  // SPRITE_NEXT
     13, 121, 11, 10,
     25, 121, 11, 10,
     37, 121, 11, 10,
     49, 121, 11, 10,
     61, 121, 11, 10,
     73, 121, 11, 10,
     85, 121, 11, 10,
};

void DrawSprite(int sprite, int x, int y)
{
    SDL_Rect src, dest;

    int spriteX = SPRITECOORDS[sprite * 4];
    int spriteY = SPRITECOORDS[sprite * 4 + 1];
    int spriteW = SPRITECOORDS[sprite * 4 + 2];
    int spriteH = SPRITECOORDS[sprite * 4 + 3];

    src.x = spriteX;  src.y = spriteY;
    src.w = spriteW; src.h = spriteH;
    dest.x = x;  dest.y = y;
    dest.w = spriteW; dest.h = spriteH;

    SDL_BlitSurface(g_pSprites, &src, g_pSurface, &dest);
}

void DrawText(int x, int y, char *str)
{
    int charline, charperline;
    char ch;
    SDL_Rect src, dest;

    if (str == NULL) return;
    if (y >= SCREEN_HEIGHT) return;

    charperline = g_pFont->w / FONT_WIDTH;

    while (*str != 0)
    {
        ch = *str;
        charline = ((int)ch) / charperline;

        src.x = ((int)ch) % charperline * FONT_WIDTH;
        src.y = charline * FONT_HEIGHT;
        src.w = FONT_WIDTH;  src.h = FONT_HEIGHT;

        dest.x = x;  dest.y = y;
        dest.w = FONT_WIDTH;  dest.h = FONT_HEIGHT;

        SDL_BlitSurface(g_pFont, &src, g_pSurface, &dest);

        x += FONT_WIDTH;
        if (x >= SCREEN_WIDTH) break;

        str++;
    }
}

void DrawTileCursor()
{
    int x, y;
    int cursorColorLum;
    Uint32 cursorColor;
    SDL_Rect rc;

    x = POSX_TILES_LEFT + g_TileCursorX * 24;
    y = POSY_TILES_TOP + g_TileCursorY * 24;

    cursorColorLum = g_okTileCursorPhase - 16;
    if (cursorColorLum < 0) cursorColorLum = -cursorColorLum;
    cursorColorLum *= 15;
    cursorColor = SDL_MapRGB(g_pSurface->format, cursorColorLum,cursorColorLum,cursorColorLum);

    rc.x = x;  rc.y = y;  rc.w = 24;  rc.h = 1;
    SDL_FillRect(g_pSurface, &rc, cursorColor);
    rc.x = x;  rc.y = y;  rc.w = 1;  rc.h = 24;
    SDL_FillRect(g_pSurface, &rc, cursorColor);
    rc.x = x + 24;  rc.y = y;  rc.w = 1;  rc.h = 24;
    SDL_FillRect(g_pSurface, &rc, cursorColor);
    rc.x = x;  rc.y = y + 24;  rc.w = 24;  rc.h = 1;
    SDL_FillRect(g_pSurface, &rc, cursorColor);

    g_okTileCursorPhase = (g_okTileCursorPhase + 1) % 32;
}

void DrawTileSelect()
{
    int x, y;
    Uint32 cursorColor;
    SDL_Rect rc;

    x = POSX_TILES_LEFT + g_TileSelectX * 24;
    y = POSY_TILES_TOP + g_TileSelectY * 24;

    cursorColor = SDL_MapRGB(g_pSurface->format, 255,255,0);

    rc.x = x;  rc.y = y;  rc.w = 24;  rc.h = 1;
    SDL_FillRect(g_pSurface, &rc, cursorColor);
    rc.x = x;  rc.y = y;  rc.w = 1;  rc.h = 24;
    SDL_FillRect(g_pSurface, &rc, cursorColor);
    rc.x = x + 24;  rc.y = y;  rc.w = 1;  rc.h = 24;
    SDL_FillRect(g_pSurface, &rc, cursorColor);
    rc.x = x;  rc.y = y + 24;  rc.w = 24;  rc.h = 1;
    SDL_FillRect(g_pSurface, &rc, cursorColor);
}

void DrawGameScreen()
{
    int i, j, tile;
    SDL_Rect rc;
    char buf[10];

    // Clear screen
    rc.x = rc.y = 0;  rc.w = SCREEN_WIDTH; rc.h = SCREEN_HEIGHT;
    SDL_FillRect(g_pSurface, &rc, SDL_MapRGB(g_pSurface->format, 0,0,0));

    //TODO: Draw columns according to score
    DrawSprite(SPRITE_KING, POSX_BOARD_LEFT - 96, 75);
    DrawSprite(SPRITE_PRETENDER, POSX_BOARD_RIGHT + 26, 75);

    // Draw board
    rc.x = POSX_BOARD_LEFT;  rc.y = POSY_BOARD_TOP;
    rc.w = rc.h = 24 * 9 + 4;
    SDL_FillRect(g_pSurface, &rc, SDL_MapRGB(g_pSurface->format, 173,170,173));

    for (i = 0; i < 9; i++)
    {
        for (j = 0; j < 9; j++)
        {
            tile = SPRITE_TILE + g_Board[i][j];
            DrawSprite(tile, POSX_TILES_LEFT + j * 24, POSY_TILES_TOP + i * 24);

            //sprintf(buf, "%d", g_Board[i][j]);  // DEBUG
            //DrawText(POSX_TILES_LEFT + j * 24, POSY_TILES_TOP + i * 24, buf);  // DEBUG
        }
    }

    // Draw cursor
    if (g_TileSelectX >= 0 && g_TileSelectY >= 0)
    {
        DrawTileSelect();
    }
    if (g_okShowTileCursor)
    {
        DrawTileCursor();
    }

    // Draw score
    sprintf(buf, "%d", g_KingScore);
    DrawText(POSX_BOARD_LEFT - 100, 6, buf);
    sprintf(buf, "% 6d", g_Score); 
    DrawText(POSX_BOARD_RIGHT + 100 - FONT_WIDTH * 6, 6, buf);

    // Draw next colors
    DrawText(POSX_NEXT_LEFT - FONT_WIDTH * 6, 6, "Next:");
    DrawSprite(SPRITE_NEXT + g_NextColors[0], POSX_NEXT_LEFT, 8);
    DrawSprite(SPRITE_NEXT + g_NextColors[1], POSX_NEXT_LEFT + 16, 8);
    DrawSprite(SPRITE_NEXT + g_NextColors[2], POSX_NEXT_LEFT + 32, 8);
}

void GamePutRandomBall()
{
    int i, freecount, place, ball;
    int *pBoard;

    pBoard = (int*)g_Board;

    freecount = 0;
    for (i = 0; i < 9 * 9; i++)
    {
        if (*(pBoard + i) == BALL_NONE)
            freecount++;
    }

    if (freecount == 0) return;  // No place to put a ball

    if (freecount == 1)
        place = 0;
    else
        place = rand() % freecount;

    freecount = 0;
    for (i = 0; i < 9 * 9; i++)
    {
        if (*(pBoard + i) == BALL_NONE)
        {
            if (freecount == place)
            {
                // Get ball color from "next colors"
                ball = g_NextColors[0];
                g_NextColors[0] = g_NextColors[1];
                g_NextColors[1] = g_NextColors[2];
                g_NextColors[2] = rand() % BALLCOLORCOUNT + 1;

                // Place the ball
                *(pBoard + i) = ball;

                //g_Score++;  //DEBUG
                return;
            }

            freecount++;
        }
    }
}

void GamePutThreeRandomBalls()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        GamePutRandomBall();
        DrawGameScreen();
        SDL_Flip(g_pSurface);
        SDL_Delay(100);
    }
}

void GameStart()
{
    // Clear board
    memset(g_Board, 0, sizeof(g_Board));
    // Clear score
    g_Score = 0;
    g_KingScore = 100;  //TODO
    g_TileCursorX = g_TileCursorY = 4;  // Cursor to center of the board
    g_okShowTileCursor = 0;  // Hide cursor
    g_TileSelectX = g_TileSelectY = -1;  // No selection

    g_NextColors[0] = rand() % BALLCOLORCOUNT + 1;
    g_NextColors[1] = rand() % BALLCOLORCOUNT + 1;
    g_NextColors[2] = rand() % BALLCOLORCOUNT + 1;

    DrawGameScreen();
    SDL_Flip(g_pSurface);
    SDL_Delay(100);

    // Put first three random balls
    GamePutThreeRandomBalls();

    g_okShowTileCursor = 1;  // Show cursor

    g_GameMode = GAMEMODE_PLAY;
}

int GameCheckPathExists(int x1, int y1, int x2, int y2)
{
    int i, j;
    int waveboard[9][9];

    memcpy(waveboard, g_Board, sizeof(waveboard));

    waveboard[y1][x1] = -1;  // Wave seed

    for (;;)
    {
        // Wave step
        int okProcess = 0;
        for (i = 0; i < 9; i++)
        {
            for (j = 0; j < 9; j++)
            {
                if (waveboard[i][j] != 0) continue;  // Skip balls and wave

                if (j < 8 && waveboard[i][j + 1] < 0)  // Right
                    waveboard[i][j] = waveboard[i][j + 1] - 1;
                if (j > 0 && waveboard[i][j - 1] < waveboard[i][j])  // Left
                    waveboard[i][j] = waveboard[i][j - 1] - 1;
                if (i < 8 && waveboard[i + 1][j] < waveboard[i][j])  // Down
                    waveboard[i][j] = waveboard[i + 1][j] - 1;
                if (i > 0 && waveboard[i - 1][j] < waveboard[i][j])  // Down
                    waveboard[i][j] = waveboard[i - 1][j] - 1;

                if (waveboard[i][j] < 0) okProcess = 1;  // Wave still moving
            }
        }

        if (waveboard[y2][x2] < 0)
            return 1;
        if (!okProcess) break;  // Wave stopped
    }

    return 0;
}

int GameMoveSelectedBall()
{
    int ball;

    // Check move rules; return 0 if the move isn't valid
    if (!GameCheckPathExists(g_TileSelectX, g_TileSelectY, g_TileCursorX, g_TileCursorY))
        return 0;

    g_okShowTileCursor = 0;  // Hide cursor

    ball = g_Board[g_TileSelectY][g_TileSelectX];
    g_Board[g_TileSelectY][g_TileSelectX] = BALL_NONE;
    g_TileSelectX = g_TileSelectY = -1;  // Clear selection

    DrawGameScreen();
    SDL_Flip(g_pSurface);
    SDL_Delay(100);

    g_Board[g_TileCursorY][g_TileCursorX] = ball;

    DrawGameScreen();
    SDL_Flip(g_pSurface);
    SDL_Delay(100);

    g_okShowTileCursor = 1;  // Show cursor

    return 1;
}

int GameCheckToRemove()
{
    int i, j, c, dx, dy;
    int ball, count;
    int bestball, bestcount, bestx = 0, besty = 0, bestdx = 0, bestdy = 0;

    bestball = BALL_NONE;  bestcount = 0;

    // Check horizontal lines
    for (i = 0; i < 9; i++)
    {
        ball = BALL_NONE;  count = 0;
        for (j = 0; j < 9; j++)
        {
            if (ball != BALL_NONE && g_Board[i][j] == ball)
                count++;
            else
            {
                if (count >= 5 && count > bestcount)
                {
                    bestball = ball;  bestcount = count;
                    bestx = j - count;  besty = i;  bestdx = 1;  bestdy = 0;
                }
                ball = g_Board[i][j];
                count = 1;
            }
        }
        if (count >= 5 && count > bestcount)
        {
            bestball = ball;  bestcount = count;
            bestx = j - count;  besty = i;  bestdx = 1;  bestdy = 0;
        }
    }

    // Check vertical lines
    for (j = 0; j < 9; j++)
    {
        ball = BALL_NONE;  count = 0;
        for (i = 0; i < 9; i++)
        {
            if (ball != BALL_NONE && g_Board[i][j] == ball)
                count++;
            else
            {
                if (count >= 5 && count > bestcount)
                {
                    bestball = ball;  bestcount = count;
                    bestx = j;  besty = i - count;  bestdx = 0;  bestdy = 1;
                }
                ball = g_Board[i][j];
                count = 1;
            }
        }
        if (count >= 5 && count > bestcount)
        {
            bestball = ball;  bestcount = count;
            bestx = j;  besty = i - count;  bestdx = 0;  bestdy = 1;
        }
    }

    // Check diagonal lines: from SW to NE
    dx = 1;  dy = -1;
    for (c = 0; c < 9; c++)
    {
        i = (c <= 4) ? c + 4 : 8;
        j = (c <= 4) ? 0 : c - 4;

        ball = BALL_NONE;  count = 0;
        for (;;)
        {
            if (ball != BALL_NONE && g_Board[i][j] == ball)
                count++;
            else
            {
                if (count >= 5 && count > bestcount)
                {
                    bestball = ball;  bestcount = count;
                    bestx = j - count;  besty = i + count;  bestdx = dx;  bestdy = dy;
                }
                ball = g_Board[i][j];
                count = 1;
            }

            i += dy;  j += dx;
            if (i >= 9 || j >= 9)
            {
                if (count >= 5 && count > bestcount)
                {
                    bestball = ball;  bestcount = count;
                    bestx = j - count;  besty = i + count;  bestdx = dx;  bestdy = dy;
                }
                break;
            }
        }
    }

    // Check diagonal lines: from NW to SE
    dx = 1;  dy = 1;
    for (c = 0; c < 9; c++)
    {
        i = (c <= 4) ? 4 - c : 0;
        j = (c <= 4) ? 0 : c - 4;

        ball = BALL_NONE;  count = 0;
        for (;;)
        {
            if (ball != BALL_NONE && g_Board[i][j] == ball)
                count++;
            else
            {
                if (count >= 5 && count > bestcount)
                {
                    bestball = ball;  bestcount = count;
                    bestx = j - count;  besty = i - count;  bestdx = dx;  bestdy = dy;
                }
                ball = g_Board[i][j];
                count = 1;
            }

            i += dy;  j += dx;
            if (i >= 9 || j >= 9)
            {
                if (count >= 5 && count > bestcount)
                {
                    bestball = ball;  bestcount = count;
                    bestx = j - count;  besty = i - count;  bestdx = dx;  bestdy = dy;
                }
                break;
            }
        }
    }

    if (bestball == BALL_NONE)
        return 0;  // Balls to remove was not found

    // Remove the balls
    i = besty;  j = bestx;
    for (c = 0; c < bestcount; c++)
    {
        g_Board[i][j] = BALL_NONE;
        g_Score += 10;

        DrawGameScreen();
        SDL_Flip(g_pSurface);
        SDL_Delay(25);

        i += bestdy;  j += bestdx;
    }

    return 1;
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
        case SDLK_ESCAPE:
	case SDLK_RETURN:  // SELECT button on Dingoo
            MenuStart();
            break;
        case SDLK_LEFT:    // D-PAD LEFT on Dingoo
            g_TileCursorX--;
            if (g_TileCursorX < 0) g_TileCursorX = 8;
            break;
        case SDLK_RIGHT:   // D-PAD RIGHT on Dingoo
            g_TileCursorX++;
            if (g_TileCursorX >= 9) g_TileCursorX = 0;
            break;
        case SDLK_UP:      // D-PAD UP on Dingoo
            g_TileCursorY--;
            if (g_TileCursorY < 0) g_TileCursorY = 8;
            break;
        case SDLK_DOWN:    // D-PAD DOWN on Dingoo
            g_TileCursorY++;
            if (g_TileCursorY >= 9) g_TileCursorY = 0;
            break;
        case SDLK_LCTRL:   // A button on Dingoo
        case SDLK_LALT:    // B button on Dingoo
        case SDLK_SPACE:
            if (g_Board[g_TileCursorY][g_TileCursorX] == BALL_NONE)
            {
                if (g_TileSelectX >= 0 && g_TileSelectY >= 0)  // We have selection and user selects the destination
                {
                    // Check rules and Move the ball
                    if (GameMoveSelectedBall())
                    {
                        if (!GameCheckToRemove())
                        {
                            GamePutThreeRandomBalls();
                            GameCheckToRemove();
                        }
                    }
                }
            }
            else  // If we have a ball under the cursor then select it
            {
                g_TileSelectX = g_TileCursorX;
                g_TileSelectY = g_TileCursorY;
            }
            break;
        default:  // Do nothing
            break;
        }
    }
}

void MenuStart()
{
    SDL_Surface *tempSurface;
    SDL_Surface *pSurfaceTitle;
    SDL_Rect src, dest;
    char buffer[20];

    // Show title screen
    tempSurface = SDL_LoadBMP("ColorLinesData/title.bmp");
    if (tempSurface != NULL)
    {
        pSurfaceTitle = SDL_DisplayFormat(tempSurface);
        SDL_FreeSurface(tempSurface);

        src.x = 0;  src.y = 0;
        src.w = SCREEN_WIDTH; src.h = SCREEN_HEIGHT;
        dest.x = 0;  dest.y = 0;
        dest.w = SCREEN_WIDTH; dest.h = SCREEN_HEIGHT;
        SDL_BlitSurface(pSurfaceTitle, &src, g_pSurface, &dest);

        SDL_FreeSurface(pSurfaceTitle);
    }

    sprintf(buffer, "ver. %d.%d", VERSION_MAJOR, VERSION_MINOR);
    DrawText(SCREEN_WIDTH - FONT_WIDTH * 9, 5, buffer);

#ifdef _WIN32
    DrawText(5, 226, "Press");
    DrawText(5, 240, "  \"SPACE\" to start,");
    DrawText(5, 254, "  \"ESC\" to exit");
#else
    DrawText(5, 226, "Press");
    DrawText(5, 240, "  \"A\" to start,");
    DrawText(5, 254, "  \"SELECT\" to exit");
#endif

    SDL_Flip(g_pSurface);

    g_GameMode = GAMEMODE_MENU;
}

void MenuProcessEvent(SDL_Event evt)
{
    if (evt.type == SDL_KEYDOWN)
    {
        switch (evt.key.keysym.sym)
        {
        case SDLK_ESCAPE:
        case SDLK_RETURN:  // SELECT button on Dingoo
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

#ifdef _WIN32
#undef main  //HACK for error LNK1561: entry point must be defined
#endif

int main(int argc, char * argv[])
{
    int flags = 0;
    SDL_Event evt;
    SDL_Surface *tempSurface;

#ifdef _WIN32
    _putenv("SDL_VIDEO_WINDOW_POS=250,200");
#endif

    // Init randomizer
    srand(SDL_GetTicks());

    // Init SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 255;  // Unable to initialize SDL
    // Prepare screen surface
    g_pSurface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, flags);
    if (g_pSurface == NULL)
        return 254;  // Unable to set video mode

#ifdef _WIN32
    SDL_WM_SetCaption("Color Lines SDL", "Color Lines SDL");
#else
    SDL_ShowCursor(SDL_DISABLE);
#endif

    // Load font
    tempSurface = SDL_LoadBMP("ColorLinesData/font.bmp");
    if (tempSurface == NULL)
        return 253;  // Unable to load bitmap
    g_pFont = SDL_DisplayFormat(tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load sprites
    tempSurface = SDL_LoadBMP("ColorLinesData/sprites.bmp");
    if (tempSurface == NULL)
        return 253;  // Unable to load bitmap
    g_pSprites = SDL_DisplayFormat(tempSurface);
    SDL_FreeSurface(tempSurface);

    MenuStart();

    while (!g_okQuit)
    {
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

        if (g_GameMode == GAMEMODE_PLAY)
        {
            DrawGameScreen();
            SDL_Flip(g_pSurface);
        }

        SDL_Delay(20);
    }

    SDL_FreeSurface(g_pSprites);
    SDL_FreeSurface(g_pFont);

    SDL_Quit();

    return 0;
}
