/* ---------------------------------------------------------- */
/*  debug.c                                                   */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   debug.c                                             */
/*     �f�o�b�O�p�`�F�b�N���[�`��                         */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: debug.pp,v 1.3 2002/07/11 17:21:47 rero2 Exp $ */


/*------------------------------------------------------------- */
/** @file
    @brief		�f�o�b�O�`�F�b�N���[�`��
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

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "grp_screen.h"

/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/* ----- �f�o�b�O���[�`�����L�����ۂ�(ASCII�������o���Ă邩) */
int  UseDebug;

/* ----- �A�X�L�[�t�H���g�e�N�X�`���[ */
SDL_Surface *AsciiFont;

/* ----- �A�X�L�[�v���[�� */
SDL_Surface *AsciiPlane;

/* ----- �\�������� */
char StockString[MessageMax][128];
int  StockNum;

/* ----- �A�X�L�[�e�N�X�`���[�� */
char *TextureName = "ascii.bmp";

/* --- �f�o�b�O�t�H���g������ */
char  debug_line[128];


/* -------------------------------------------------------------- */
/* --- �f�o�b�O�p�`�F�b�N���[�`��                                 */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- ���b�Z�[�W�̃��C���\��    */
void  print_msg(char *mes, int disp_x, int disp_y)
{
  int  i, l;
  int  c;
  SDL_Rect  rect1, rect2;

  l = strlen(mes);
  rect1.w = 8;
  rect1.h = 8;
  rect2.w = 8;
  rect2.h = 8;
  for(i=0; i<l; i++) {
    c = mes[i];
    rect1.x = (c % 16) * 8;
    rect1.y = (c / 16) * 8;
    rect2.x = disp_x;
    rect2.y = disp_y;
    SDL_BlitSurface(AsciiFont, &rect1, AsciiPlane, &rect2);
    disp_x = disp_x + 8;
  }
}



/* ---------------------------------------- */
/* --- �f�o�b�O�t�F�C�X������               */
/* ---------------------------------------- */
void  TDebugInit(TGameScreen *screen,
		 int Width, int Height, int Depth)
{
  SDL_Surface *plane;

  StockNum = 0;
  UseDebug = 1;
  plane = SDL_LoadBMP(TextureName);
  if (plane == NULL) {
    UseDebug = 0;
    return;
  }
  AsciiFont = SDL_ConvertSurface(plane, screen->Screen->format, SDL_SWSURFACE);
  if (AsciiFont == NULL) {
    UseDebug = 0;
  }
  SDL_SetColorKey(AsciiFont, SDL_SRCCOLORKEY, 0x000000);
#ifdef __GP2X__
  AsciiPlane = SDL_CreateRGBSurface(SDL_SWSURFACE,
				    Width, Height, Depth,
				    screen->Screen->format->Rmask,
				    screen->Screen->format->Gmask,
				    screen->Screen->format->Bmask,
				    screen->Screen->format->Amask);
  SDL_SetColorKey(AsciiPlane, SDL_SRCCOLORKEY, 0);
#else
  AsciiPlane = SDL_CreateRGBSurface(SDL_SWSURFACE,
				    Width, Height, Depth,
				    DRmask,
				    DGmask,
				    DBmask,
				    DAmask);
#endif
  if (AsciiPlane == NULL) {
    UseDebug = 0;
  }
  SDL_FreeSurface(plane);
}


/* ---------------------------------------- */
/* --- �f�o�b�O�t�F�C�X���                 */
/* ---------------------------------------- */
void  TDebugFree()
{
  SDL_FreeSurface(AsciiPlane);
  SDL_FreeSurface(AsciiFont);
  UseDebug = 0;
}


/* ---------------------------------------- */
/* --- �f�o�b�O�t�F�C�X�̒���`��           */
/* ---------------------------------------- */
void  TDebugDisp(TGameScreen *screen)
{
  int  disp_x, disp_y;
  int  disp_edge;
  SDL_Rect  rect1, rect2;
  int  i;

  /* --- �������o���Ă��Ȃ��������� */
  if (UseDebug == 0) {
    return;
  }

  /* --- �\���L���[�ɐς܂�Ă��镪�����\�� */
  disp_x = 0;
  disp_y = 0;
  disp_edge = 0;
  for(i=0; i<StockNum; i++) {
    print_msg(StockString[i], disp_x, disp_y);
    if (disp_edge < (strlen(StockString[i]) * 8)) {
      disp_edge = strlen(StockString[i]) * 8;
    }
    disp_y = disp_y + 8;
  }

  /* --- �A�X�L�[�v���[�����X�N���[���� */
  if (disp_edge > 0) {
//    SDL_UpdateRect(AsciiPlane, 0, 0, disp_edge, disp_y);
    rect1.x = 0;
    rect1.y = 0;
    rect1.w = disp_edge;
    rect1.h = disp_y;
    rect2.x = 0;
    rect2.y = 0;
    rect2.w = disp_edge;
    rect2.h = disp_y;
    SDL_BlitSurface(AsciiPlane, &rect1, screen->Screen, &rect2);
    SDL_FillRect(AsciiPlane, 0, 0x00000000);
  }
  StockNum = 0;  
}


/* ---------------------------------------- */
/* --- �f�o�b�O���b�Z�[�W�̃L���[�C���O     */
/* ---------------------------------------- */
void  TDebugPrint(char *mes)
{
  if (StockNum < MessageMax) {
    strncpy(StockString[StockNum], mes, 127);
    StockNum = StockNum + 1;
  }
}


/* ---------------------------------------- */
/* --- �����𕶎���ɕϊ�                   */
/* ---------------------------------------- */
void  IntToStr(int num, char *buf)
{
  snprintf(buf, 127, "%d", num);
}


/* ---------------------------------------- */
/* --- 16�i���\���p                         */
/* ---------------------------------------- */
void  IntToHex(int num, int length, char *buf)
{
  snprintf(buf, 127, "%x", num);
}

