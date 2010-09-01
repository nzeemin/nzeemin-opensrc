/* ---------------------------------------------------------- */
/*  input.c                                                   */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   input.c                                              */
/*     ���̓}�l�[�W��                                     */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id:  $ */


/*------------------------------------------------------------- */
/** @file
    @brief		���̓}�l�[�W��
    @author		K.Kunikane (rerofumi)
    @since		Jul.27.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include "input.h"
#include "SDL.h"


/*-------------------------------*/
/* define                        */
/*-------------------------------*/

#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (14)
#define GP2X_BUTTON_Y               (15)
#define GP2X_BUTTON_L               (10)
#define GP2X_BUTTON_R               (11)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/* ----- �C�x���g */
SDL_Event  event;

/* ----- ���[�U�[�̃Q�[������ */
SDL_Joystick  *JoyPtr[JOY_NUM_MAX];
unsigned long  JoyKey[JOY_NUM_MAX];
unsigned long  JoyKeyGP2X[JOY_NUM_MAX];
unsigned long  JoyStick[JOY_NUM_MAX];
unsigned long  JoyKeyPast[JOY_NUM_MAX];
unsigned long  JoyStickPast[JOY_NUM_MAX];

/* ----- �W���C�X�e�B�b�N�̏�� */
/*  �܂��A�ꉞ���Ƃ���  */
int JoyStickAxes[JOY_NUM_MAX];
int JoyStickButtons[JOY_NUM_MAX];

/* ----- �I���L�[�t���O */
int  AppExit;

/* -------------------------------------------------------------- */
/* --- ���͊Ǘ�                                                   */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- �L�[�̔��ʏ���                       */
/* ---------------------------------------- */
void  key_read_down(SDL_KeyboardEvent *key)
{
  if (key->keysym.sym == SDLK_UP)
    JoyKey[0] = JoyKey[0] | IN_Up;
  if (key->keysym.sym == SDLK_DOWN)
    JoyKey[0] = JoyKey[0] | IN_Down;
  if (key->keysym.sym == SDLK_LEFT)
    JoyKey[0] = JoyKey[0] | IN_Left;
  if (key->keysym.sym == SDLK_RIGHT)
    JoyKey[0] = JoyKey[0] | IN_Right;
  if (key->keysym.sym == SDLK_z)
    JoyKey[0] = JoyKey[0] | IN_Button1;
  if (key->keysym.sym == SDLK_x)
    JoyKey[0] = JoyKey[0] | IN_Button2;
  if (key->keysym.sym == SDLK_c)
    JoyKey[0] = JoyKey[0] | IN_Button3;
  if (key->keysym.sym == SDLK_v)
    JoyKey[0] = JoyKey[0] | IN_Button4;
  if (key->keysym.sym == SDLK_a)
    JoyKey[0] = JoyKey[0] | IN_Button5;
  if (key->keysym.sym == SDLK_s)
    JoyKey[0] = JoyKey[0] | IN_Button6;
  if (key->keysym.sym == SDLK_d)
    JoyKey[0] = JoyKey[0] | IN_Button7;
  if (key->keysym.sym == SDLK_f)
    JoyKey[0] = JoyKey[0] | IN_Button8;
  if (key->keysym.sym == SDLK_SPACE)
    JoyKey[0] = JoyKey[0] | IN_Button1;

  if (key->keysym.sym == SDLK_q)
    AppExit = 1;
  if (key->keysym.sym == SDLK_ESCAPE)
    AppExit = 1;
}

void  key_read_up(SDL_KeyboardEvent *key)
{
  if (key->keysym.sym == SDLK_UP)
    JoyKey[0] = JoyKey[0] & (~(IN_Up));
  if (key->keysym.sym == SDLK_DOWN)
    JoyKey[0] = JoyKey[0] & (~(IN_Down));
  if (key->keysym.sym == SDLK_LEFT)
    JoyKey[0] = JoyKey[0] & (~(IN_Left));
  if (key->keysym.sym == SDLK_RIGHT)
    JoyKey[0] = JoyKey[0] & (~(IN_Right));
  if (key->keysym.sym == SDLK_z)
    JoyKey[0] = JoyKey[0] & (~(IN_Button1));
  if (key->keysym.sym == SDLK_x)
    JoyKey[0] = JoyKey[0] & (~(IN_Button2));
  if (key->keysym.sym == SDLK_c)
    JoyKey[0] = JoyKey[0] & (~(IN_Button3));
  if (key->keysym.sym == SDLK_v)
    JoyKey[0] = JoyKey[0] & (~(IN_Button4));
  if (key->keysym.sym == SDLK_a)
    JoyKey[0] = JoyKey[0] & (~(IN_Button5));
  if (key->keysym.sym == SDLK_s)
    JoyKey[0] = JoyKey[0] & (~(IN_Button6));
  if (key->keysym.sym == SDLK_d)
    JoyKey[0] = JoyKey[0] & (~(IN_Button7));
  if (key->keysym.sym == SDLK_f)
    JoyKey[0] = JoyKey[0] & (~(IN_Button8));
  if (key->keysym.sym == SDLK_SPACE)
    JoyKey[0] = JoyKey[0] & (~(IN_Button1));
}

/* ---------------------------------------- */
/* --- �W���C�X�e�B�b�N�̔��ʏ���           */
/* ---------------------------------------- */

void  joy_read_stick(SDL_JoyAxisEvent *stick)
{
  if (stick->which < JOY_NUM_MAX) {
    switch(stick->axis) {
      /* --- ������ */
    case 0:
      JoyStick[stick->which] &= (~(IN_Left|IN_Right));
      if (stick->value < (-JOY_STICK_DITHER)) {
	JoyStick[stick->which] = JoyStick[stick->which] | IN_Left;
       }
      if (stick->value > JOY_STICK_DITHER) {
	JoyStick[stick->which] = JoyStick[stick->which] | IN_Right;
      }
      break;

      /* --- �c���� */
    case 1:
      JoyStick[stick->which] &= (~(IN_Up|IN_Down));
      if (stick->value < (-JOY_STICK_DITHER)) {
	JoyStick[stick->which] = JoyStick[stick->which] | IN_Up;
       }
      if (stick->value > JOY_STICK_DITHER) {
	JoyStick[stick->which] = JoyStick[stick->which] | IN_Down;
      }
      break;
    }
  }
}


void  joy_read_button_up_pc(SDL_JoyButtonEvent *btn)
{
  if (btn->which < JOY_NUM_MAX) {
    if ((btn->button == 0) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button1));
    if ((btn->button == 1) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button2));
    if ((btn->button == 2) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button3));
    if ((btn->button == 3) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button4));
    if ((btn->button == 4) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button5));
    if ((btn->button == 5) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button6));
    if ((btn->button == 6) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button7));
    if ((btn->button == 7) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button8));
  }
}

void  joy_read_button_up_gp2x(SDL_JoyButtonEvent *btn)
{
#ifdef __GP2X__
  if (btn->which < JOY_NUM_MAX) {
    if ((btn->button == GP2X_BUTTON_UP) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_Up));
    if ((btn->button == GP2X_BUTTON_DOWN) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_Down));
    if ((btn->button == GP2X_BUTTON_LEFT) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_Left));
    if ((btn->button == GP2X_BUTTON_RIGHT) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_Right));
    if ((btn->button == GP2X_BUTTON_UPLEFT) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_GP_UPLEFT));
    if ((btn->button == GP2X_BUTTON_UPRIGHT) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_GP_UPRIGHT));
    if ((btn->button == GP2X_BUTTON_DOWNLEFT) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_GP_DOWNLEFT));
    if ((btn->button == GP2X_BUTTON_DOWNRIGHT) && (btn->state == SDL_RELEASED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] & (~(IN_GP_DOWNRIGHT));
    if ((btn->button == GP2X_BUTTON_A) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button1));
    if ((btn->button == GP2X_BUTTON_B) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button2));
    if ((btn->button == GP2X_BUTTON_X) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button3));
    if ((btn->button == GP2X_BUTTON_Y) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button4));
    if ((btn->button == GP2X_BUTTON_L) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button5));
    if ((btn->button == GP2X_BUTTON_R) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button6));
    if ((btn->button == GP2X_BUTTON_START) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button7));
    if ((btn->button == GP2X_BUTTON_SELECT) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button8));
    if ((btn->button == GP2X_BUTTON_VOLUP) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button9));
    if ((btn->button == GP2X_BUTTON_VOLDOWN) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button10));
    if ((btn->button == GP2X_BUTTON_CLICK) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button11));
  }
  /* --- stick encode */
  JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Up | IN_Down | IN_Left | IN_Right));
  if ((JoyKeyGP2X[btn->which] & IN_Up) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Up;
  if ((JoyKeyGP2X[btn->which] & IN_Down) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Down;
  if ((JoyKeyGP2X[btn->which] & IN_Left) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Left;
  if ((JoyKeyGP2X[btn->which] & IN_Right) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Right;
  if ((JoyKeyGP2X[btn->which] & IN_GP_UPLEFT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Up;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Left;
  }
  if ((JoyKeyGP2X[btn->which] & IN_GP_UPRIGHT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Up;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Right;
  }
  if ((JoyKeyGP2X[btn->which] & IN_GP_DOWNLEFT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Down;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Left;
  }
  if ((JoyKeyGP2X[btn->which] & IN_GP_DOWNRIGHT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Down;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Right;
  }
#endif
}


void  joy_read_button_up_psp(SDL_JoyButtonEvent *btn)
{
  if (btn->which < JOY_NUM_MAX) {
    /* �� */
    if ((btn->button == 0) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button3));
    /* �� */
    if ((btn->button == 1) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button1));
    /* �~ */
    if ((btn->button == 2) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button2));
    /* �� */
    if ((btn->button == 3) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button4));
    /* L */
    if ((btn->button == 4) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button5));
    /* R */
    if ((btn->button == 5) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Button6));
    /* �� */
    if ((btn->button == 6) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Down));
    /* �� */
    if ((btn->button == 7) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Left));
    /* �� */
    if ((btn->button == 8) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Up));
    /* �� */
    if ((btn->button == 9) && (btn->state == SDL_RELEASED))
      JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Right));
  }
}


void  joy_read_button_down_pc(SDL_JoyButtonEvent *btn)
{
  if (btn->which < JOY_NUM_MAX) {
    if ((btn->button == 0) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button1;
    if ((btn->button == 1) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button2;
    if ((btn->button == 2) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button3;
    if ((btn->button == 3) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button4;
    if ((btn->button == 4) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button5;
    if ((btn->button == 5) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button6;
    if ((btn->button == 6) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button7;
    if ((btn->button == 7) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button8;
  }
}

void  joy_read_button_down_gp2x(SDL_JoyButtonEvent *btn)
{
#ifdef __GP2X__
  if (btn->which < JOY_NUM_MAX) {
    if ((btn->button == GP2X_BUTTON_UP) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_Up;
    if ((btn->button == GP2X_BUTTON_DOWN) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_Down;
    if ((btn->button == GP2X_BUTTON_LEFT) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_Left;
    if ((btn->button == GP2X_BUTTON_RIGHT) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_Right;
    if ((btn->button == GP2X_BUTTON_UPLEFT) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_GP_UPLEFT;
    if ((btn->button == GP2X_BUTTON_UPRIGHT) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_GP_UPRIGHT;
    if ((btn->button == GP2X_BUTTON_DOWNLEFT) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_GP_DOWNLEFT;
    if ((btn->button == GP2X_BUTTON_DOWNRIGHT) && (btn->state == SDL_PRESSED))
      JoyKeyGP2X[btn->which] = JoyKeyGP2X[btn->which] | IN_GP_DOWNRIGHT;
    if ((btn->button == GP2X_BUTTON_A) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button1;
    if ((btn->button == GP2X_BUTTON_B) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button2;
    if ((btn->button == GP2X_BUTTON_X) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button3;
    if ((btn->button == GP2X_BUTTON_Y) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button4;
    if ((btn->button == GP2X_BUTTON_L) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button5;
    if ((btn->button == GP2X_BUTTON_R) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button6;
    if ((btn->button == GP2X_BUTTON_START) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button7;
    if ((btn->button == GP2X_BUTTON_SELECT) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button8;
    if ((btn->button == GP2X_BUTTON_VOLUP) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button9;
    if ((btn->button == GP2X_BUTTON_VOLDOWN) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button10;
    if ((btn->button == GP2X_BUTTON_CLICK) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button11;
  }

  /* --- stick encode */
  JoyKey[btn->which] = JoyKey[btn->which] & (~(IN_Up | IN_Down | IN_Left | IN_Right));
  if ((JoyKeyGP2X[btn->which] & IN_Up) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Up;
  if ((JoyKeyGP2X[btn->which] & IN_Down) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Down;
  if ((JoyKeyGP2X[btn->which] & IN_Left) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Left;
  if ((JoyKeyGP2X[btn->which] & IN_Right) != 0)
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Right;
  if ((JoyKeyGP2X[btn->which] & IN_GP_UPLEFT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Up;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Left;
  }
  if ((JoyKeyGP2X[btn->which] & IN_GP_UPRIGHT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Up;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Right;
  }
  if ((JoyKeyGP2X[btn->which] & IN_GP_DOWNLEFT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Down;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Left;
  }
  if ((JoyKeyGP2X[btn->which] & IN_GP_DOWNRIGHT) != 0) {
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Down;
    JoyKey[btn->which] = JoyKey[btn->which] | IN_Right;
  }

  /* --- Exit control */
  if ((JoyKey[btn->which] & (IN_Button5|IN_Button6|IN_Button11)) == (IN_Button5|IN_Button6|IN_Button11))
    AppExit = 1;
#endif
}

void  joy_read_button_down_psp(SDL_JoyButtonEvent *btn)
{
  if (btn->which < JOY_NUM_MAX) {
    /* �� */
    if ((btn->button == 0) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button3;
    /* �� */
    if ((btn->button == 1) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button1;
    /* �~ */
    if ((btn->button == 2) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button2;
    /* �� */
    if ((btn->button == 3) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button4;
    /* L */
    if ((btn->button == 4) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button5;
    /* R */
    if ((btn->button == 5) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Button6;
    /* �� */
    if ((btn->button == 6) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Down;
    /* �� */
    if ((btn->button == 7) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Left;
    /* �� */
    if ((btn->button == 8) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Up;
    /* �� */
    if ((btn->button == 9) && (btn->state == SDL_PRESSED))
      JoyKey[btn->which] = JoyKey[btn->which] | IN_Right;
  }
}


/* ---------------------------------------- */
/* --- ���͑��u�̏����ݒ�                   */
/* ---------------------------------------- */
void  InputInit(void)
{
  int i;
  int JoyNum;

  for(i=0; i<JOY_NUM_MAX; i++) {
    JoyPtr[i] = 0;
    JoyStickAxes[i] = 0;
    JoyStickButtons[i] = 0;
    JoyKey[i] = 0;
    JoyStick[i] = 0;
  }
  /* ----- �W���C�X�e�B�b�N�̑��݊m�F */
  JoyNum = SDL_NumJoysticks();
  if (JoyNum > JOY_NUM_MAX) {
    JoyNum = JOY_NUM_MAX;
  }
  for(i=0; i<(JoyNum + 1); i++) {
    JoyPtr[i] = SDL_JoystickOpen(i);
    if (JoyPtr[i] != NULL) {
      /* --- �W���C�X�e�B�b�N�̎�ނ��`�F�b�N */
      JoyStickAxes[i] = SDL_JoystickNumAxes(JoyPtr[i]);
      JoyStickButtons[i] = SDL_JoystickNumButtons(JoyPtr[i]);
    }
  }
}


/* ---------------------------------------- */
/* --- ���͑��u�̉��                       */
/* ---------------------------------------- */
void  InputFree(void)
{
  int i;

  /* ----- �W���C�X�e�B�b�N�̉�� */
  for(i=0; i<JOY_NUM_MAX; i++) {
    if (JoyPtr[i] != 0) {
      SDL_JoystickClose(JoyPtr[i]);
    }
  }
}



/* ---------------------------------------- */
/* --- �L�[�̒���I�ǂ݂Ƃ�                 */
/* ---------------------------------------- */
void  InputPoll(void)
{
  int  i;
  AppExit = 0;

  /* ----- for Triger */
  for(i=0; i<JOY_NUM_MAX; i++) {
    JoyKeyPast[i] = JoyKey[i];
    JoyStickPast[i] = JoyStick[i];
  }

  /* ----- Key Event Read */
  while(SDL_PollEvent(&event) != 0) {
    switch(event.type) {
    case SDL_KEYUP:
      key_read_up(&event.key);
      break;

    case SDL_KEYDOWN:
      key_read_down(&event.key);
      break;

    case SDL_JOYBUTTONUP:
#ifdef  NOTPSP
#ifdef __GP2X__
      joy_read_button_up_gp2x(&event.jbutton);
#else
      joy_read_button_up_pc(&event.jbutton);
#endif
#else
      joy_read_button_up_psp(&event.jbutton);
#endif
      break;

    case SDL_JOYBUTTONDOWN:
#ifdef  NOTPSP
#ifdef __GP2X__
      joy_read_button_down_gp2x(&event.jbutton);
#else
      joy_read_button_down_pc(&event.jbutton);
#endif
#else
      joy_read_button_down_psp(&event.jbutton);
#endif
      break;

    case SDL_JOYAXISMOTION:
      joy_read_stick(&event.jaxis);
      break;

    case SDL_QUIT:
      AppExit = 1;
      break;
    }
  }
}


/* ---------------------------------------- */
/* --- �A�v���P�[�V�����I���L�[�̔���       */
/* ---------------------------------------- */
int  InputExit(void)
{
  return(AppExit);  
}


/* ---------------------------------------- */
/* --- ���[�U�[���͂̓ǂ݂Ƃ�               */
/* ---------------------------------------- */
int InputJoyKey(int side)
{
  int r1, r2;

  r1 = r2 = 0;
  if (side < JOY_NUM_MAX) {
    r1 = JoyKey[side];
    r2 = JoyStick[side];
    r1 |= r2;
  }

  return(r1);
}


/* ---------------------------------------- */
/* --- ���[�U�[���͂̃g���K�[�ǂ݂Ƃ�       */
/* ---------------------------------------- */
int  InputJoyKeyTriger(int side)
{
  int  r1, r2;

  r1 = r2 = 0;
  if (side < JOY_NUM_MAX) {
    r1 = (JoyKey[side] ^ JoyKeyPast[side]) & JoyKey[side];
    r2 = (JoyStick[side] ^ JoyStickPast[side]) & JoyStick[side];
    r1 |= r2;
  }
  return(r1);
}

