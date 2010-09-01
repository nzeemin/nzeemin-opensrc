/* ---------------------------------------------------------- */
/*  grp_sprite.h                                              */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   grp_sprite.h                                         */
/*     スプライト管理構造体                               */
/*                                                        */
/*--------------------------------------------------------*/

#ifndef GRP_SPRITE_H
#define GRP_SPRITE_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include "SDL.h"

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

typedef struct {
  int  TextureId;
  /* - public */
  int  DispSw;
  int  x, y;
  int  w, h;
  int  tx, ty;
  float  zoomx, zoomy;
  float  rotation_z;
  unsigned char  alpha;
  SDL_Surface  *Texture;
} TGameSprite, *PTGameSprite;

/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

TGameSprite *TGameSprite_Create(void);
void TGameSprite_Destroy(TGameSprite *class);

void TGameSprite_SetTextureDirect(TGameSprite *class,
				  int  texture_id,
				  SDL_Surface *bitmap);


#endif //GRP_SPRITE_H
