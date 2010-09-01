/* ---------------------------------------------------------- */
/*  grp_sptite.c                                              */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   grp_sprite.c                                         */
/*     スプライト管理構造体                               */
/*                                                        */
/*--------------------------------------------------------*/

/*------------------------------------------------------------- */
/** @file
    @brief		スプライト管理
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

#include "SDL.h"
#include "grp_sprite.h"


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
TGameSprite *TGameSprite_Create(void)
{
  TGameSprite *class;

  class = malloc(sizeof(TGameSprite));
  if (class == NULL) {
    return(0);
  }
  class->Texture = NULL;
  class->TextureId = -1;
  class->DispSw = FALSE;
  class->zoomx = 1.0;
  class->zoomy = 1.0;
  class->rotation_z = 0.0;

  return(class);
}


void TGameSprite_Destroy(TGameSprite *class)
{
  if (class) {
    free(class);
  }
}


/* ---------------------------------------- */
/* --- テクスチャーの登録                   */
/* ---------------------------------------- */
void TGameSprite_SetTextureDirect(TGameSprite *class,
				  int  texture_id,
				  SDL_Surface *bitmap)
{
  if (class == NULL) return;
  class->TextureId = texture_id;
  class->Texture = bitmap;
}
