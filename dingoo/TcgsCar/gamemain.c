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
/*     �Q�[���{�̂̃��C���t���[                        */
/*                                                        */
/*--------------------------------------------------------*/

/*------------------------------------------------------------- */
/** @file
    @brief		�Q�[���{�̃X�e�b�v
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

/* - �Q�[���X�^�[�g�܂ł̎���(�V�O�i�����������������) */
#define  GAME_TIME_START    140

/* - �V�O�i���̃^�C�~���O */
#define  SIGNAL_TIME_READY       60
#define  SIGNAL_TIME_GO         120
#define  SIGNAL_TIME_BGM        200
#define  SIGNAL_TIME_DISAPPEAR  240

/* - �S�̂̃X�s�[�h(MAX�l) */
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
/* --- �Q�[�����C���X�e�b�v                                       */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- �R���X�g���N�^�E�f�X�g���N�^         */
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

  /* - ��������܂����� */

  /* --- �C���X�^���X�̉�� */
  free(class);
}

/* ---------------------------------------- */
/* --- �Q�[���I�����f (0=playing, 1=end)      */
/* ---------------------------------------- */
int TGameMain_GetState(TGameMain *class)
{
  return class->game_state;
}


/* ---------------------------------------- */
/* --- �Q�[�����C��                         */
/* ---------------------------------------- */
int TGameMain_Poll(TGameMain *class,
		   int counter)
{
  int  skip;
  int  i;

  if (class == NULL) return(FALSE);

  skip = TRUE;

  /* -------------------------------- */
  /* --- �{�����[���R���g���[�� */
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
  /* --- �Q�[�����C���X�e�b�v */
  
  switch(class->step) {

    /* --- �Q�[���^�C�g���S������ */
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

    /* --- �^�C�g����ʂ̕\������ */
  case TitleInit:
    /* �Q�[�����̉�ʏ����Ă��� */
    class->mycar->DispSw = FALSE;
    class->signal->DispSw = FALSE;
    for(i=0; i<ENEMYMAX; i++) {
      class->enemy[i]->DispSw = FALSE;
    }
    /* �^�C�g����ʂ̕\�� */
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
    /* pause�������Ă��� */
    class->pause->DispSw = FALSE;
    /* ���� */
    class->titletimer = 0;
    class->step = TitleLoop;
    break;

    /* --- �^�C�g�����(�{�^�����������҂�) */
  case TitleLoop:
    /* ��(�w�i) */
    car_disp_road(class, class->road_position);
    class->road_position += 1;
    /* �L�[������ */
    if (((class->titletimer / 60) & 1) == 0) {
      class->title_inkey->DispSw = TRUE;
    }
    else {
      class->title_inkey->DispSw = FALSE;
    }
    class->titletimer += 1;
    /* �L�[�������ꂽ�玟�� */
    if (InputJoyKeyTriger(0) &
	(IN_Button1|IN_Button2|IN_Button3|IN_Button4|IN_Button5|IN_Button6)) {
      /* �������L�[�ɂ���ă����_���V�[�h�ݒ� */
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

    /* --- �Q�[���X�^�[�g����A�s�[�� */
  case TitleAdvatize:
    /* ��(�w�i) */
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

    /* --- �Q�[���Ɉڂ�O���� */
  case GameInit:
    /* �^�C�g���������Ă��� */
    class->title->DispSw = FALSE;
    class->title_inkey->DispSw = FALSE;
    /* ���낢�돉���� */
    car_game_init(class);
    class->signal_timer = 0;
    class->step_timer = 0;
    class->step = GameReady;
    break;

    /* --- �V�O�i���\���A��`���ǂ� */
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
      /* �N���b�V���}�[�N1 */
      class->mycar->x -= 4;
      class->mycar->y -= 4;
      class->mycar->w = 32;
      class->mycar->h = 32;
      class->mycar->tx = 448;
      class->mycar->ty = 0;
      /* BGM�~�߂Ď��� */
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
      /* �N���b�V���}�[�N2 */
      class->mycar->x -= 8;
      class->mycar->y -= 8;
      class->mycar->w = 48;
      class->mycar->h = 48;
      class->mycar->tx = 432;
      class->mycar->ty = 32;
    }
    if (class->step_timer == 120) {
      /* GAMEOVER �\�� */
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
      /* �L�[�҂��� */
      class->step = GameOver2;
    }
    break;

  case GameOver2:
    /* �L�[�������ꂽ�玟�� */
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
/*  �Q�[���{��                                          */
/* ---------------------------------------------------- */

/* -------------------------------------- */
/* --- �Ԃ悯�Q�[�������� */
void car_game_init(TGameMain *class)
{
  car_disp_road(class, 0);
  /* ���������낢�� */
  class->my_pos = 120;
  class->game_score = 0;
  class->road_position = 0;
  class->speed = 0;
  class->speed_max = game_level[0];
  /* �����̎Ԃ�\�� */
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
/* --- �Ԃ悯�Q�[���{�� */
int  car_game_main(TGameMain *class, int timer)
{
  int  i, exec;
  int  l;

  exec = TRUE;

  /* --- �A�N�Z�� */
  if (class->speed < class->speed_max) {
    class->speed += 1;
  }
  if (class->speed > class->speed_max) {
    class->speed = class->speed_max;
  }
  DEBUGPRINT("speed: %d", class->speed);

  /* --- �i�� */
  class->road_position += class->speed;
  class->game_score = (class->road_position / 0x20) * 10;
  car_disp_road(class, (class->road_position / 0x10));
  car_disp_score(class, class->game_score, TRUE);
  class->mycar->ty = ((class->road_position / 0x200) & 1) * 32;

  /* --- ��Փx */
  l = class->road_position / 400000;
  if (l >= LEVEL_MAX) {
    l = LEVEL_MAX - 1;
  }
  class->speed_max = game_level[l];

  /* --- �n���h�� */
  if ((class->my_pos > 32) && ((InputJoyKey(0) & IN_Up) != 0)) {
    class->my_pos -= 3;
  }
  if ((class->my_pos < 240-32) && ((InputJoyKey(0) & IN_Down) != 0)) {
    class->my_pos += 3;
  }
  class->mycar->y = class->my_pos - 12;

  /* --- �G�o�� */
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

  /* --- �G�ړ� */
  for(i=0; i<ENEMYMAX; i++) {
    if (class->enemy[i]->DispSw == TRUE) {
      class->enemy[i]->x -= (class->speed / 0x20);
      if (class->enemy[i]->x < -24) {
	class->enemy[i]->DispSw = FALSE;
      }
    }
  }

  /* --- �G�Ƃ̏Փ˔��� */
  if (car_game_hit_check(class) == TRUE) {
    exec = FALSE;
  }

  return(exec);
}

/* ---------------------------------------------------- */
/*  Local routine                                       */
/* ---------------------------------------------------- */

/* -------------------------------------- */
/* ---�@�G�ԓo�� */
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
/* ---�@�G�ԂƂ̏Փ˃`�F�b�N */
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
/* --- ���H(�w�i)�̕\���A�X�N���[�� */
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
/* --- �M���@�̕\���T�u���[�`�� */
void car_disp_signal(TGameMain *class, int timer)
{
  /* -- ���łɗp�ς� */
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
/* --- �X�R�A(10�i��)�̕\���T�u���[�`�� */
void car_disp_score(TGameMain *class, int num, int playmode)
{
  int  i, j;
  int  c;
  int  top;

  int x, y;
  int n, o;

  /* --- "SCORE" �̕\�� */
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
  
  /* x, y : �\���ʒu */
  /* n : �\�������� */
  /* o : Obj�ԍ� */
  /* num : �\�����鐔�� */
  o = 1;
  n = 8;
  x = 320 - 4 - 16;
  y = 64;
  if (playmode == FALSE) {
    y += 20;
  }
  /* --- ���l��\������ */
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
    /* -- �\�� */
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

