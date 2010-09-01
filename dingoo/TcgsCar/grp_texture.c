/* ---------------------------------------------------------- */
/*  grp_texture.c                                             */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   grp_texture.c                                        */
/*     テクスチャー管理クラス                             */
/*                                                        */
/*--------------------------------------------------------*/

/*------------------------------------------------------------- */
/** @file
    @brief		テクスチャー管理
    @author		K.Kunikane (rerofumi)
    @since		Sep.19.2005
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
#ifdef __MACH__ // __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include "SDL.h"
#include "grp_texture.h"

/*-------------------------------*/
/* local define                  */
/*-------------------------------*/

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
 #define RMASK 0xff000000
 #define GMASK 0x00ff0000
 #define BMASK 0x0000ff00
 #define AMASK 0x000000ff
 #define RMASK16 0x0000f800
 #define GMASK16 0x000007c0
 #define BMASK16 0x0000003f
 #define AMASK16 0x00000001
#else
 #define RMASK 0x000000ff
 #define GMASK 0x0000ff00
 #define BMASK 0x00ff0000
 #define AMASK 0xff000000
 #define RMASK16 0x0000001f
 #define GMASK16 0x000003e0
 #define BMASK16 0x00007c00
 #define AMASK16 0x00008000
#endif

/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/*-------------------------------*/
/* local function                */
/*-------------------------------*/

/* -------------------------------------------------------------- */
/* --- テクスチャ管理クラス                                       */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- コンストラクタ・デストラクタ         */
TGameTexture *TGameTexture_Create(void)
{
  int  i;
  TGameTexture *class;

  class = malloc(sizeof(TGameTexture));
  if (class == NULL) {
    return(0);
  }
  for(i=0; i<TEXTUREMAX; i++) {
    class->texture_id = i;
    class->bitmap[i] = NULL;
  }

  return(class);
}

void TGameTexture_Destroy(TGameTexture *class)
{
  int  i;
  if (class == NULL) {
    return;
  }

  /* ----- ロード中のテクスチャを解放する */
  for(i=0; i<TEXTUREMAX; i++) {
    if (class->bitmap[i] != NULL) {
      SDL_FreeSurface(class->bitmap[i]);
      class->bitmap[i] = NULL;
    }
  }

  /* ----- インスタンスの解放 */
  free(class);
}


/* ---------------------------------------- */
/* --- テクスチャーの読み込み、登録         */
/* ---------------------------------------- */
void TGameTexture_Load(TGameTexture *class,
		       int num,
		       char *filename,
                       SDL_Surface *GameScreen,
                       int preconv)
{
  SDL_Surface  *plane, *standard;
  void *nonalign;
  int  msize, loop;
  unsigned short *pixdst;
  unsigned long  *pixsrc;

  standard = 0;
  nonalign = 0;
  msize = 0;
  loop = 0;
  pixdst = 0;
  pixsrc = 0;
  if (class == NULL) {\
    return;
  }

  /* ----- テクスチャ番号が不正だったら終了 */
  if (num < 0) return;
  if (num > TEXTUREMAX) return;

  /* ----- 既にテクスチャがあったら解放 */
  if (class->bitmap[num] != NULL) {
    SDL_FreeSurface(class->bitmap[num]);
    class->bitmap[num] = NULL;
  }

  /* ----- テクスチャーの読み込み */
  plane = IMG_Load(filename);
  if (plane == NULL) {
    class->bitmap[num] = NULL;
    return;
  }
#ifdef NOTPSP
  /* --- Normal SDL work for PC */
  if (preconv == TRUE) {
    class->bitmap[num] = SDL_ConvertSurface(plane,
					    GameScreen->format,
					    SDL_SWSURFACE);
    if (plane != NULL) {
      SDL_FreeSurface(plane);
    }
  }
  else {
    class->bitmap[num] = plane;
  }
#else
  /* --- PSP 向けにテクスチャを加工する */

  /* --- PSP で変換要求があった場合は16bitに落とす */
  class->bitmap[num] = plane;

  /* --- PSPではDMA転送で 16byte align に無いと都合が悪いので変換  */
  nonalign = class->bitmap[num]->pixels;
  msize = (class->bitmap[num]->w * class->bitmap[num]->h) * class->bitmap[num]->format->BytesPerPixel;
  if ((preconv == TRUE) && (class->bitmap[num]->format->BytesPerPixel == 4)) {
    /* --- 16bit 減色して保持 */
    class->bitmap[num]->pixels = (void*)memalign(16, (msize / 2));
    msize = (class->bitmap[num]->w * class->bitmap[num]->h);
    pixdst = (unsigned short *)class->bitmap[num]->pixels;
    pixsrc = (unsigned long *)nonalign;
    for(loop=0; loop<msize; loop++) {
      *pixdst =
	(*pixsrc & 0x80000000) >> 16 |
	(*pixsrc & 0x00f80000) >> 9 |
	(*pixsrc & 0x0000f800) >> 6 |
	(*pixsrc & 0x000000f8) >> 3;
      pixdst++;
      pixsrc++;
    }
    class->bitmap[num]->format->BytesPerPixel = 2;
    class->bitmap[num]->format->BitsPerPixel = 16;
    class->bitmap[num]->format->Rmask = RMASK16;
    class->bitmap[num]->format->Gmask = GMASK16;
    class->bitmap[num]->format->Bmask = BMASK16;
    class->bitmap[num]->format->Amask = AMASK16;
  }
  else {
    /* --- 32bit 通常モード */
    class->bitmap[num]->pixels = (void*)memalign(16, msize);
    memcpy(class->bitmap[num]->pixels, nonalign, msize);
  }
  free(nonalign);

#endif
}


/* ---------------------------------------- */
/* --- テクスチャーの渡し                   */
/* ---------------------------------------- */
SDL_Surface  *TGameTexture_GetTexture(TGameTexture *class,
				      int index)
{

  if (class == NULL) {
    return(NULL);
  }
  return(class->bitmap[index]);
}
