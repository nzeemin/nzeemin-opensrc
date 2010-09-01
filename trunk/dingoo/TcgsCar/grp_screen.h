/* ---------------------------------------------------------- */
/*  grp_screen.h                                              */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   debug.pp                                             */
/*     デバッグ用チェックルーチン                         */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: debug.pp,v 1.3 2002/07/11 17:21:47 rero2 Exp $ */


/*------------------------------------------------------------- */
/** @file
    @brief		画面表示管理
    @author		K.Kunikane (rerofumi)
    @since		Jul.27.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/


#ifndef GRP_SCREEN_H
#define GRP_SCREEN_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include "SDL.h"

#include "grp_texture.h"
#include "grp_sprite.h"

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

/* --- 管理する最大スプライト数 */
#define  SPRITEMAX  1024


#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
 #define DRmask 0xff000000
 #define DGmask 0x00ff0000
 #define DBmask 0x0000ff00
 #define DAmask 0x000000ff
#else
 #define DRmask 0x000000ff
 #define DGmask 0x0000ff00
 #define DBmask 0x00ff0000
 #define DAmask 0xff000000
#endif


/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

typedef struct {
  SDL_Surface *Screen;
  TGameTexture *Texture;
  TGameSprite  *Sprites[SPRITEMAX];
  int Width;
  int Height;
  int Depth;
  /// 現在描画ターゲットになっている GU フレームバッファのポインタ
  unsigned char *current_buffer_ptr;
  /// PSPでのGU描画パケットエリア
  unsigned int  *packet;
} TGameScreen, *PTGameScreen;


/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

TGameScreen *TGameScreen_Create(int width, int height, int depth);
void TGameScreen_Destroy(TGameScreen *class);
void TGameScreen_SetWMName(TGameScreen *class, char *name);
void TGameScreen_DispScreen(TGameScreen *class);
void TGameScreen_UpdateScreen(TGameScreen *class);
void TGameScreen_SwapScreen(TGameScreen *class);

TGameSprite *TGameScreen_GetSprite(TGameScreen *class, int id);
SDL_Surface *TGameScreen_GetTexture(TGameScreen *class, int id);
void TGameScreen_LoadTexture(TGameScreen *class, int num, char *filename);
void TGameScreen_LoadTexturePure(TGameScreen *class, int num, char *filename);


#endif //GRP_SCREEN_H






