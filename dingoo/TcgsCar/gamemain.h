/* ---------------------------------------------------------- */
/*  gamemain.h                                                */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   gamemain.h                                           */
/*     ゲームの本体フロー                                 */
/*                                                        */
/*--------------------------------------------------------*/

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include "SDL.h"
#include "grp_screen.h"

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* --- ゲームステップのラベル */
enum {
  Initialize,
  TitleInit,
  TitleLoop,
  TitleAdvatize,
  GameInit,
  GameReady,
  GameMain,
  GameOver1,
  GameOver2,
  PauseInit,
  Pause
} MainStep;

#define  ENEMYMAX  256

/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

typedef struct {
  int  titletimer;
  SDL_Surface  *tex;
  TGameSprite  *check;
  TGameScreen  *screen;
  int  step;
  int  sound_volume;
  int  game_state;

  /* ----- ここから下は各ゲーム専用のワークとして使ってください */
  TGameSprite  *road[7];
  TGameSprite  *mycar;
  TGameSprite  *enemy[ENEMYMAX];
  TGameSprite  *signal;
  TGameSprite  *title;
  TGameSprite  *title_inkey;
  TGameSprite  *score[9];
  TGameSprite  *pause;

  int  high_score;
  int  game_score;
  int  road_position;
  int  signal_timer;
  int  step_timer;
  int  speed;
  int  speed_max;

  int  my_pos;

  int pause_return_step;

} TGameMain, *PTGameMain;

/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

TGameMain *TGameMain_Create(TGameScreen *mainscreen);
void TGameMain_Destroy(TGameMain *class);

int TGameMain_Poll(TGameMain *class,
		   int  counter);

int TGameMain_GetState(TGameMain *class);


#endif //GAMEMAIN_H
