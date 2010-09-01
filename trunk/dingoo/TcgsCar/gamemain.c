/* ---------------------------------------------------------- */
/*  gamemain.c                                                */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* The cheap game show - "CAR"                            */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   gamemain.c                                           */
/*     ゲーム本体のメインフロー                        */
/*                                                        */
/*--------------------------------------------------------*/

/*------------------------------------------------------------- */
/** @file
    @brief		ゲーム本体ステップ
    @author		K.Kunikane (rerofumi)
    @since		Oct.25.2005
*/
/*-----------------------------------------------------
  Copyright (C) 2005 rerofumi <rero2@yuumu.org>
  All Rights Reserved.
  ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <stdlib.h>
#include <math.h>

#include "gamemain.h"
#include "input.h"
#include "sound.h"
#include "debug.h"

/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/* - ゲームスタートまでの時間(シグナルが消えるよりも早い) */
#define  GAME_TIME_START    140

/* - シグナルのタイミング */
#define  SIGNAL_TIME_READY       60
#define  SIGNAL_TIME_GO         120
#define  SIGNAL_TIME_BGM        200
#define  SIGNAL_TIME_DISAPPEAR  240

/* - 全体のスピード(MAX値) */
#define  LEVEL_MAX  10
int  game_level[LEVEL_MAX] = {
  0x080, 0x090, 0x0a0, 0x0b0, 0x0c0,
  0x0d0, 0x0e0, 0x0f0, 0x100, 0x110,
};


/*-------------------------------*/
/* local function                */
/*-------------------------------*/

void car_game_init(TGameMain *class);
int  car_game_main(TGameMain *class, int timer);
void  car_game_appear_enemy(TGameMain *class);
int  car_game_hit_check(TGameMain *class);
void car_disp_road(TGameMain *class, int position);
void car_disp_signal(TGameMain *class, int timer);
void car_disp_score(TGameMain *class, int num, int playscore);


/* -------------------------------------------------------------- */
/* --- ゲームメインステップ                                       */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- コンストラクタ・デストラクタ         */
TGameMain *TGameMain_Create(TGameScreen *mainscreen)
{
  TGameMain *class;

  class = malloc(sizeof(TGameMain));
  if (class == NULL) return(0);

  class->screen = mainscreen;
  class->step = Initialize;
  class->sound_volume = 88;
  class->game_state = 0;

  return(class);
}

void TGameMain_Destroy(TGameMain *class)
{
  if (class == NULL) return;

  /* - 何かありましたら */

  /* --- インスタンスの解放 */
  free(class);
}

/* ---------------------------------------- */
/* --- ゲーム終了判断 (0=playing, 1=end)      */
/* ---------------------------------------- */
int TGameMain_GetState(TGameMain *class)
{
  return class->game_state;
}


/* ---------------------------------------- */
/* --- ゲームメイン                         */
/* ---------------------------------------- */
int TGameMain_Poll(TGameMain *class,
		   int counter)
{
  int  skip;
  int  i;

  if (class == NULL) return(FALSE);

  skip = TRUE;

  /* -------------------------------- */
  /* --- ボリュームコントロール */
#ifdef __GP2X__
  i = InputJoyKeyTriger(0);
  if ((i & IN_GP_VOLUP) != 0) {
    if (class->sound_volume < 128) {
      class->sound_volume += 16;
      if (class->sound_volume > 128) {
	class->sound_volume = 128;
      }
      SoundVolume(class->sound_volume);
    }
  }
  if ((i & IN_GP_VOLDOWN) != 0) {
    if (class->sound_volume > 0) {
      class->sound_volume -= 16;
      if (class->sound_volume < 0) {
	class->sound_volume = 0;
      }
      SoundVolume(class->sound_volume);
    }
  }
  DEBUGPRINT("VOLUME : %d", class->sound_volume);
#endif

  /* -------------------------------- */
  /* --- ゲームメインステップ */
  
  switch(class->step) {

    /* --- ゲームタイトル全初期化 */
  case Initialize:
    TGameScreen_LoadTexture(class->screen, 0, "texture.png");
    class->tex = TGameScreen_GetTexture(class->screen, 0);
    SDL_SetColorKey(class->tex,
		    SDL_SRCCOLORKEY,
		    SDL_MapRGB(class->tex->format,
			       0x80, 0x80, 0x80));
    for(i=0; i<7; i++) {
      class->road[i] = TGameScreen_GetSprite(class->screen, i);
    }
    for(i=0; i<ENEMYMAX; i++) {
      class->enemy[i] = TGameScreen_GetSprite(class->screen, i+7);
      class->enemy[i]->DispSw = FALSE;
    }
    for(i=0; i<9; i++) {
      class->score[i] = TGameScreen_GetSprite(class->screen, i+1+7+ENEMYMAX+1+1+1);
    }
    class->mycar = TGameScreen_GetSprite(class->screen, 7+ENEMYMAX);
    class->signal = TGameScreen_GetSprite(class->screen, 1+7+ENEMYMAX);
    class->title = TGameScreen_GetSprite(class->screen, 1+7+ENEMYMAX+1);
    class->title_inkey = TGameScreen_GetSprite(class->screen, 1+7+ENEMYMAX+1+1);
    class->pause = TGameScreen_GetSprite(class->screen, 1+7+ENEMYMAX+1+1+1+11);
    class->mycar->DispSw = FALSE;
    class->titletimer = 0;
    class->road_position = 0;
    class->high_score = 0;
    class->game_score = 0;
    class->step = TitleInit;
    skip = FALSE;
    break;

    /* --- タイトル画面の表示準備 */
  case TitleInit:
    /* ゲーム中の画面消しておく */
    class->mycar->DispSw = FALSE;
    class->signal->DispSw = FALSE;
    for(i=0; i<ENEMYMAX; i++) {
      class->enemy[i]->DispSw = FALSE;
    }
    /* タイトル画面の表示 */
    class->title->DispSw = TRUE;
    class->title->x = 180;
    class->title->y = 28;
    class->title->w = 79;
    class->title->h = 192;
    class->title->tx = 305;
    class->title->ty = 0;
    class->title->TextureId = 0;
    class->title->Texture = class->tex;
    class->title->alpha = 255;
    class->title_inkey->DispSw = TRUE;
    class->title_inkey->x = 100;
    class->title_inkey->y = 24;
    class->title_inkey->w = 16;
    class->title_inkey->h = 192;
    class->title_inkey->tx = 288;
    class->title_inkey->ty = 0;
    class->title_inkey->TextureId = 0;
    class->title_inkey->Texture = class->tex;
    class->title_inkey->alpha = 255;
    car_disp_road(class, class->road_position);
    car_disp_score(class, class->high_score, FALSE);
    /* pauseも消しておく */
    class->pause->DispSw = FALSE;
    /* 次へ */
    class->titletimer = 0;
    class->step = TitleLoop;
    break;

    /* --- タイトル画面(ボタン押し下げ待ち) */
  case TitleLoop:
    /* 道(背景) */
    car_disp_road(class, class->road_position);
    class->road_position += 1;
    /* キー押して */
    if (((class->titletimer / 60) & 1) == 0) {
      class->title_inkey->DispSw = TRUE;
    }
    else {
      class->title_inkey->DispSw = FALSE;
    }
    class->titletimer += 1;
    /* キーが押されたら次へ */
    if (InputJoyKeyTriger(0) &
	(IN_Button1|IN_Button2|IN_Button3|IN_Button4|IN_Button5|IN_Button6)) {
      /* 押したキーによってランダムシード設定 */
      if (InputJoyKey(0) & IN_Button2) {
	srand(135);
      }
      else if (InputJoyKey(0) & IN_Button3) {
	srand(246);
      }
      else if (InputJoyKey(0) & IN_Button4) {
	srand(789);
      }
      else {
	srand(class->titletimer);
      }
      SoundSE(1);
      class->titletimer = 0;
      class->step = TitleAdvatize;
    }
    /* pause */
    {
      if (InputJoyKeyTriger(0) & IN_Button7) {
	class->pause_return_step = class->step;
	class->step = PauseInit;
      }
    }
    break;

    /* --- ゲームスタートだよアピール */
  case TitleAdvatize:
    /* 道(背景) */
    car_disp_road(class, class->road_position);
    class->road_position += 1;
    class->titletimer += 1;
    if (((class->titletimer / 3) & 1) == 0) {
      class->title_inkey->DispSw = TRUE;
    }
    else {
      class->title_inkey->DispSw = FALSE;
    }
    if (class->titletimer > 90) {
      class->step = GameInit;
    }
    break;

    /* --- ゲームに移る前処理 */
  case GameInit:
    /* タイトルを消しておく */
    class->title->DispSw = FALSE;
    class->title_inkey->DispSw = FALSE;
    /* いろいろ初期化 */
    car_game_init(class);
    class->signal_timer = 0;
    class->step_timer = 0;
    class->step = GameReady;
    break;

    /* --- シグナル表示、よ～いどん */
  case GameReady:
    car_disp_signal(class, class->signal_timer);
    class->signal_timer += 1;
    class->step_timer += 1;
    if (class->step_timer >= GAME_TIME_START) {
      class->step_timer = 0;
      class->step = GameMain;
    }
    break;

  case GameMain:
    car_disp_signal(class, class->signal_timer);
    class->signal_timer += 1;
    class->step_timer += 1;
    if (car_game_main(class, class->step_timer) == FALSE) {
      /* クラッシュマーク1 */
      class->mycar->x -= 4;
      class->mycar->y -= 4;
      class->mycar->w = 32;
      class->mycar->h = 32;
      class->mycar->tx = 448;
      class->mycar->ty = 0;
      /* BGM止めて次へ */
      SoundMusicStop();
      SoundSE(4);
      class->step_timer = 0;
      class->step = GameOver1;
    }
    /* pause */
    else {
      if (InputJoyKeyTriger(0) & IN_Button7) {
	class->pause_return_step = class->step;
	class->step = PauseInit;
      }
    }
    break;

  case GameOver1:
    class->step_timer += 1;
    if (class->step_timer == 4) {
      /* クラッシュマーク2 */
      class->mycar->x -= 8;
      class->mycar->y -= 8;
      class->mycar->w = 48;
      class->mycar->h = 48;
      class->mycar->tx = 432;
      class->mycar->ty = 32;
    }
    if (class->step_timer == 120) {
      /* GAMEOVER 表示 */
      class->signal->DispSw = TRUE;
      class->signal->x = 200;
      class->signal->y = (240 - 160) / 2;
      class->signal->w = 32;
      class->signal->h = 160;
      class->signal->tx = 256;
      class->signal->ty = 0;
      class->signal->TextureId = 0;
      class->signal->Texture = class->tex;
      class->signal->alpha = 255;
      /* キー待ちへ */
      class->step = GameOver2;
    }
    break;

  case GameOver2:
    /* キーが押されたら次へ */
    if (InputJoyKeyTriger(0) &
	(IN_Button1|IN_Button2|IN_Button3|IN_Button4|IN_Button5|IN_Button6)) {
      if (class->game_score > class->high_score) {
	class->high_score = class->game_score;
      }
      class->step = TitleInit;
    }
    break;

  case PauseInit:
    SoundSE(5);
    class->step = Pause;
    class->pause->DispSw = TRUE;
    class->pause->x = 140;
    class->pause->y = 32;
    class->pause->w = 96;
    class->pause->h = 208;
    class->pause->tx = 208;
    class->pause->ty = 272;
    class->pause->TextureId = 0;
    class->pause->Texture = class->tex;
    class->pause->alpha = 255;
    break;

  case Pause:
    i = InputJoyKeyTriger(0);
    if (IN_Button7 == i) {
      SoundSE(2);
      class->pause->DispSw = FALSE;
      class->step = class->pause_return_step;
    }
    if (IN_Button8 == i) {
      SoundMusicStop();
      SoundSE(2);
      class->pause->DispSw = FALSE;
      class->step = TitleInit;
    }
    if (IN_Button3 == i) {
      SoundMusicStop();
      SoundSE(2);
      class->pause->DispSw = FALSE;
      class->game_state = 1;
    }
    break;
   
  }

  return(skip);
}


/* ---------------------------------------------------- */
/*  ゲーム本体                                          */
/* ---------------------------------------------------- */

/* -------------------------------------- */
/* --- 車よけゲーム初期化 */
void car_game_init(TGameMain *class)
{
  car_disp_road(class, 0);
  /* 初期化いろいろ */
  class->my_pos = 120;
  class->game_score = 0;
  class->road_position = 0;
  class->speed = 0;
  class->speed_max = game_level[0];
  /* 自分の車を表示 */
  class->mycar->DispSw = TRUE;
  class->mycar->x = 32;
  class->mycar->y = class->my_pos - 12;
  class->mycar->w = 24;
  class->mycar->h = 24;
  class->mycar->tx = 488;
  class->mycar->ty = 0;
  class->mycar->TextureId = 0;
  class->mycar->Texture = class->tex;
  class->mycar->alpha = 255;
}

/* -------------------------------------- */
/* --- 車よけゲーム本体 */
int  car_game_main(TGameMain *class, int timer)
{
  int  i, exec;
  int  l;

  exec = TRUE;

  /* --- アクセル */
  if (class->speed < class->speed_max) {
    class->speed += 1;
  }
  if (class->speed > class->speed_max) {
    class->speed = class->speed_max;
  }
  DEBUGPRINT("speed: %d", class->speed);

  /* --- 進む */
  class->road_position += class->speed;
  class->game_score = (class->road_position / 0x20) * 10;
  car_disp_road(class, (class->road_position / 0x10));
  car_disp_score(class, class->game_score, TRUE);
  class->mycar->ty = ((class->road_position / 0x200) & 1) * 32;

  /* --- 難易度 */
  l = class->road_position / 400000;
  if (l >= LEVEL_MAX) {
    l = LEVEL_MAX - 1;
  }
  class->speed_max = game_level[l];

  /* --- ハンドル */
  if ((class->my_pos > 32) && ((InputJoyKey(0) & IN_Up) != 0)) {
    class->my_pos -= 3;
  }
  if ((class->my_pos < 240-32) && ((InputJoyKey(0) & IN_Down) != 0)) {
    class->my_pos += 3;
  }
  class->mycar->y = class->my_pos - 12;

  /* --- 敵出現 */
  if (timer > 200) {
    if ((timer % (12 * 0xc0 / class->speed)) == 0) {
      car_game_appear_enemy(class);
      if ((rand() % class->speed) > 0x30) {
	car_game_appear_enemy(class);
      }
      if ((rand() % class->speed) > 0x80) {
	car_game_appear_enemy(class);
      }
    }
  }

  /* --- 敵移動 */
  for(i=0; i<ENEMYMAX; i++) {
    if (class->enemy[i]->DispSw == TRUE) {
      class->enemy[i]->x -= (class->speed / 0x20);
      if (class->enemy[i]->x < -24) {
	class->enemy[i]->DispSw = FALSE;
      }
    }
  }

  /* --- 敵との衝突判定 */
  if (car_game_hit_check(class) == TRUE) {
    exec = FALSE;
  }

  return(exec);
}

/* ---------------------------------------------------- */
/*  Local routine                                       */
/* ---------------------------------------------------- */

/* -------------------------------------- */
/* ---　敵車登場 */
void  car_game_appear_enemy(TGameMain *class)
{
  int  i;

  for(i=0; i<ENEMYMAX; i++) {
    if (class->enemy[i]->DispSw == FALSE) {
      class->enemy[i]->DispSw = TRUE;
      class->enemy[i]->x = 350;
      class->enemy[i]->y = (rand() % 9) * 24 + 24 - 12;
      class->enemy[i]->w = 24;
      class->enemy[i]->h = 24;
      class->enemy[i]->tx = 488;
      class->enemy[i]->ty = 64;
      class->enemy[i]->TextureId = 0;
      class->enemy[i]->Texture = class->tex;
      class->enemy[i]->alpha = 255;
      break;
    }
  }
}

/* -------------------------------------- */
/* ---　敵車との衝突チェック */
int  car_game_hit_check(TGameMain *class)
{
  int  i, result;
  int  d;

  result = FALSE;

  for(i=0; i<ENEMYMAX; i++) {
    if (class->enemy[i]->DispSw == TRUE) {
      d = class->enemy[i]->x - 32;
      if ((d > 0) && (d < 20)) {
	d = (class->enemy[i]->y + 12) - class->my_pos;
	if ((d > -16) && (d < 16)) {
	  result = TRUE;
	  break;
	}
      }
    }
  }

  return(result);
}

/* -------------------------------------- */
/* --- 道路(背景)の表示、スクロール */
void car_disp_road(TGameMain *class, int position)
{
  int  i;

  position = (0 - position) % 192;
  for(i=0; i<6; i++) {
    class->road[i]->DispSw = TRUE;
    class->road[i]->x = position;
    class->road[i]->y = 0;
    class->road[i]->w = 96;
    class->road[i]->h = 240;
    class->road[i]->tx = 320+(i%2)*96;
    class->road[i]->ty = 240;
    class->road[i]->TextureId = 0;
    class->road[i]->Texture = class->tex;
    class->road[i]->alpha = 255;
    position += 96;
  }
}

/* -------------------------------------- */
/* --- 信号機の表示サブルーチン */
void car_disp_signal(TGameMain *class, int timer)
{
  /* -- すでに用済み */
  if (timer > SIGNAL_TIME_DISAPPEAR) {
    return;
  }
  
  switch(timer) {

  case SIGNAL_TIME_READY:
    class->signal->DispSw = TRUE;
    class->signal->x = 200;
    class->signal->y = 48;
    class->signal->w = 64;
    class->signal->h = 144;
    class->signal->tx = 448;
    class->signal->ty = 96;
    class->signal->TextureId = 0;
    class->signal->Texture = class->tex;
    class->signal->alpha = 255;
    SoundSE(2);
    break;

  case SIGNAL_TIME_GO:
    class->signal->tx = 384;
    SoundSE(3);
    break;

  case SIGNAL_TIME_BGM:
    SoundMusic(1);
    break;

  case SIGNAL_TIME_DISAPPEAR:
    class->signal->DispSw = FALSE;
    break;
  }
}

/* -------------------------------------- */
/* --- スコア(10進数)の表示サブルーチン */
void car_disp_score(TGameMain *class, int num, int playmode)
{
  int  i, j;
  int  c;
  int  top;

  int x, y;
  int n, o;

  /* --- "SCORE" の表示 */
  if (playmode == TRUE) {
    class->score[0]->DispSw = TRUE;
    class->score[0]->x = 320 - 4 - 16;
    class->score[0]->y = 4;
    class->score[0]->w = 16;
    class->score[0]->h = 64;
    class->score[0]->tx = 224;
    class->score[0]->ty = 0;
    class->score[0]->TextureId = 0;
    class->score[0]->Texture = class->tex;
    class->score[0]->alpha = 255;
  }
  else {
    /* HIGHSCORE */
    class->score[0]->DispSw = TRUE;
    class->score[0]->x = 320 - 4 - 16;
    class->score[0]->y = 4;
    class->score[0]->w = 16;
    class->score[0]->h = 80;
    class->score[0]->tx = 224;
    class->score[0]->ty = 64;
    class->score[0]->TextureId = 0;
    class->score[0]->Texture = class->tex;
    class->score[0]->alpha = 255;
  }
  
  /* x, y : 表示位置 */
  /* n : 表示文字数 */
  /* o : Obj番号 */
  /* num : 表示する数字 */
  o = 1;
  n = 8;
  x = 320 - 4 - 16;
  y = 64;
  if (playmode == FALSE) {
    y += 20;
  }
  /* --- 数値を表示する */
  top = FALSE;
  j = 1;
  for(i=0; i<n; i++) {
    j = j * 10;
  }
  if (num >= j) num = j - 1;
  for(i=0; i<n; i++) {
    c = num / (j / 10);
    num = num % (j / 10);
    if (c > 9) c = 9;
    if (i == (n - 1)) top = FALSE;
    /* -- 表示 */
    if ((c == 0) && (top == TRUE)) {
      class->score[o]->DispSw = FALSE;
    }
    else {
      top = FALSE;
      class->score[o]->DispSw = TRUE;
      class->score[o]->x = x;
      class->score[o]->y = y;
      class->score[o]->w = 16;
      class->score[o]->h = 14;
      class->score[o]->tx = 240;
      class->score[o]->ty = (c * 16);
      class->score[o]->TextureId = 0;
      class->score[o]->Texture = class->tex;
      class->score[o]->alpha = 255;
    }
    y = y + 14;
    o = o + 1;
    j = j / 10;
  }
}

