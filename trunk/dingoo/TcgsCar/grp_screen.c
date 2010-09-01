/* ---------------------------------------------------------- */
/*  debug.c                                                   */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   grp_screen.c                                         */
/*     画面管理クラス                                     */
/*                                                        */
/*--------------------------------------------------------*/


/*------------------------------------------------------------- */
/** @file
    @brief		描画スクリーン管理
    @author		K.Kunikane (rerofumi)
    @since		Jul.27.2005
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <stdlib.h>

#include "grp_screen.h"
#include "grp_table_int.h"

#include "debug.h"


/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/*-------------------------------*/
/* local function                */
/*-------------------------------*/

/* --- スプライトを一枚スクリーンに貼り付ける */
void Render(TGameScreen *class, TGameSprite *spr);
void RenderZoomRot(TGameScreen *class, TGameSprite *spr);


/* -------------------------------------------------------------- */
/* --- スクリーン管理クラス                                       */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- コンストラクタ・デストラクタ         */
TGameScreen *TGameScreen_Create(int width, int height, int depth)
{
  int  i;

  TGameScreen *class;

  /* --- インスタンスの生成 */
  class = malloc(sizeof(TGameScreen));
  if (class == 0) {
    return(0);
  }

  /* ----- スクリーンの生成 */
  class->Screen = SDL_SetVideoMode(width, height, depth, SDL_SWSURFACE);
  class->Texture = TGameTexture_Create();
  class->Width = width;
  class->Height = height;
  class->Depth = depth;
  /* --- 初期化 */
  for(i=0; i<SPRITEMAX; i++) {
    class->Sprites[i] = TGameSprite_Create();
  }

  /* ----- デバッグプリントの初期化 */
#ifdef DEBUG
#ifdef __GP2X__
  TDebugInit(class, width, height, 32);
#else
  TDebugInit(class, width, height, depth);
#endif
#endif

  /* --- ポインタを返して終了 */
  return(class);
}

void TGameScreen_Destroy(TGameScreen *class)
{
  int  i;

  if (class) {
    /* ----- テクスチャの解放 */
    TGameTexture_Destroy(class->Texture);
    /* ----- スクリーンの解放 */
    SDL_FreeSurface(class->Screen);
    /* ----- スプライトの解放 */
    for(i=0; i<SPRITEMAX; i++) {
      TGameSprite_Destroy(class->Sprites[i]);
    }
    /* ----- デバッグも終了 */
#ifdef DEBUG
    TDebugFree();
#endif
    /* ----- インスタンスの破棄 */
    free(class);
  }
}

/* ---------------------------------------- */
/* --- ウィンドウタイトルの設定             */
void TGameScreen_SetWMName(TGameScreen *class, char *name)
{
  SDL_WM_SetCaption(name, name);
}


/* ---------------------------------------- */
/* --- フレームタイミングによる全描画       */
void TGameScreen_DispScreen(TGameScreen *class)
{
  int  i;

  /* --- スプライト描画 */
  for(i=0; i<SPRITEMAX; i++) {
    if ((class->Sprites[i]->DispSw == TRUE) &&
	(class->Sprites[i]->Texture != NULL)) {
      /* --- スプライトの描画 */
      if ((class->Sprites[i]->zoomx == 1.0) &&
	  (class->Sprites[i]->zoomy == 1.0) &&
	  (class->Sprites[i]->rotation_z == 0.0)) {
	/* 回転拡大縮小なし、単純矩形コピー */
	Render(class, class->Sprites[i]);
      }
      else {
	/* 回転拡大縮小あり */
	RenderZoomRot(class, class->Sprites[i]);
      }
    }
  }
}

void TGameScreen_UpdateScreen(TGameScreen *class)
{
  /* -- 画面更新 */
  TGameScreen_DispScreen(class);

  /* -- 開発デバッグフォント */
#ifdef DEBUG
  TDebugDisp(class);
#endif
}

void TGameScreen_SwapScreen(TGameScreen *class)
{
  /* --- スクリーンアップデート */
  SDL_UpdateRect(class->Screen, 0, 0, class->Width, class->Height);
  /* --- 全クリア */
  SDL_FillRect(class->Screen, 0, 0xff000040);
}


/* ---------------------------------------- */
/* --- テクスチャをロードする               */

void TGameScreen_LoadTexture(TGameScreen *class,
			     int num,
			     char *filename)
{
  TGameTexture_Load(class->Texture, num, filename, class->Screen, TRUE);
}

void TGameScreen_LoadTexturePure(TGameScreen *class,
				 int num,
				 char *filename)
{
  TGameTexture_Load(class->Texture, num, filename, class->Screen, FALSE);
}


/* ---------------------------------------- */
/* --- スプライトを渡す                     */
TGameSprite *TGameScreen_GetSprite(TGameScreen *class, int id)
{
  if (id < 0) return(0);
  if (id > SPRITEMAX) return(0);

  return(class->Sprites[id]);
}


/* ---------------------------------------- */
/* --- テクスチャを渡す                     */
SDL_Surface *TGameScreen_GetTexture(TGameScreen *class, int id)
{
  return(TGameTexture_GetTexture(class->Texture, id));
}


/* ---------------------------------------- */
/* --- スプライトの表示(画面転送)           */
void Render(TGameScreen *class, TGameSprite *spr)
{
  unsigned char  org_alpha;
  SDL_Rect  rect1, rect2;
  int  r;

  /* --- 表示スイッチ */
  if (spr->DispSw == FALSE) return;
  if (spr->Texture == NULL) return;

  /* --- 表示範囲 */
  rect1.w = spr->w;
  rect1.h = spr->h;
  rect2.w = spr->w;
  rect2.h = spr->h;
  rect1.x = spr->tx;
  rect1.y = spr->ty;
  rect2.x = spr->x;
  rect2.y = spr->y;
  org_alpha = spr->Texture->format->alpha;
  if (org_alpha != spr->alpha) {
    if (spr->alpha < 255) {
      SDL_SetAlpha(spr->Texture, SDL_SRCALPHA, spr->alpha);
    }
    else {
      SDL_SetAlpha(spr->Texture, 0, 0);
    }
  }
  r = SDL_BlitSurface(spr->Texture, &rect1, class->Screen, &rect2);
  if (org_alpha != spr->alpha) {
    SDL_SetAlpha(spr->Texture, SDL_SRCALPHA, org_alpha);
  }
}


/* ---------------------------------------- */
/* --- スプライトの表示(回転拡大縮小あり) */
void RenderZoomRot(TGameScreen *class, TGameSprite *spr)
{
  SDL_Surface  *after;
  SDL_Rect  rect1, rect2;
  int  pp_x, pp_y;
  int  pp_dw_x, pp_dh_x, pp_dw_y, pp_dh_y;
  int  fw, fh, rzoomx, rzoomy;
  int  max_w, max_h, max_w_int, max_h_int;
  int  step_x, step_y;
  int  rot;
  int  loop_x, loop_y;
  int  ix, iy;
  unsigned long  *dstpix, *srcpix;
    

  /* --- 表示スイッチ */
  if (spr->DispSw == FALSE) return;
  if (spr->Texture == NULL) return;

  /* --- 回転拡大前処理 */
  rot = (int)spr->rotation_z;
  rot %= 360;
  /* ピクセルステップ */
  rzoomx = (1024*1024) / (int)(spr->zoomx * 1024);
  rzoomy = (1024*1024) / (int)(spr->zoomy * 1024);
  pp_dw_x = (rzoomx * sprite_cos[rot]) / 1024;
  pp_dh_x = (rzoomx * sprite_sin[rot]) / 1024;
  pp_dw_y = -(rzoomy * sprite_sin[rot]) / 1024;
  pp_dh_y = (rzoomy * sprite_cos[rot]) / 1024;
  /* 回転後の画像の大きさ */
  if (rot != 0) {
    max_w = (spr->w * (int)(spr->zoomx * 1448));
    max_h = (spr->h * (int)(spr->zoomy * 1448));
  }
  else {
    max_w = spr->w * (int)(spr->zoomx * 1024);
    max_h = spr->h * (int)(spr->zoomy * 1024);
  }
  if (max_w > max_h) max_h = max_w;
  if (max_h > max_w) max_w = max_h;
  max_w_int = max_w >> 10;
  max_h_int = max_h >> 10;
  after = SDL_CreateRGBSurface(SDL_SWSURFACE,
			       max_w_int, max_h_int, 32,
			       DRmask, DGmask, DBmask, DAmask);
  SDL_FillRect(after, 0, 0x00000000);
  /* 開始点 */
  fw = -(max_w_int) * rzoomx / 2;
  fh = -(max_h_int) * rzoomy / 2;
  pp_x = ((fw * sprite_cos[rot]) - (fh * sprite_sin[rot])) / 1024;
  pp_y = ((fw * sprite_sin[rot]) + (fh * sprite_cos[rot])) / 1024;
  pp_x += (spr->w >> 1) * 1024;
  pp_y += (spr->h >> 1) * 1024;

  /* --- 回転転送処理 */
  dstpix = after->pixels;
  for(loop_y=0; loop_y<max_h_int; loop_y++) {
    step_x = pp_x;
    step_y = pp_y;
    for(loop_x=0; loop_x<max_w_int; loop_x++) {
      if ((step_x >= 0) && (step_x < (spr->w << 10)) &&
	  (step_y >= 0) && (step_y < (spr->h << 10))) {
	ix = step_x >> 10;
	iy = step_y >> 10;
	srcpix = (unsigned long *)spr->Texture->pixels;
	srcpix += (spr->tx + ix + ((spr->ty + iy) * spr->Texture->w));
	*dstpix = *srcpix;
      }
      else {
	*dstpix = 0;
      }
      step_x += pp_dw_x;
      step_y += pp_dh_x;
      dstpix += 1;
    }
    pp_x += pp_dw_y;
    pp_y += pp_dh_y;
  }

  /* --- スクリーンに描画 */
  rect1.w = after->w;
  rect1.h = after->h;
  rect2.w = after->w;
  rect2.h = after->h;
  rect1.x = 0;
  rect1.y = 0;
  rect2.x = spr->x + (spr->w / 2) - (after->w / 2);
  rect2.y = spr->y + (spr->h / 2) - (after->h / 2);
  SDL_BlitSurface(after, &rect1, class->Screen, &rect2);
  /* あとしまつ */
  SDL_FreeSurface(after);
}
