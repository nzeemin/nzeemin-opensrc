/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintainer  Rerorero@fumi.  */
/*      C-SDL convert     2nd Maintainer  rerofumi.       */
/*                                                        */
/*   bootmain.h                                           */
/*     大元のメインルーチンとその起動処理                 */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: bootmain.pp,v 1.6 2002/08/10 03:05:14 rero2 Exp $ */

#ifndef BOOTMAIN_H
#define BOOTMAIN_H

/* ------------------------------------------------ */
/* --- define                                       */
/* ------------------------------------------------ */

/* ----- 画面サイズ設定値 */

#ifdef  __PSP_SCREEN__
#define  SCREEN_WIDTH   480
#define  SCREEN_HEIGHT  272
#define  SCREEN_DEPTH   32
#else
#define  SCREEN_WIDTH   320
#define  SCREEN_HEIGHT  240
#ifdef __GP2X__
#define  SCREEN_DEPTH   16
#else
#define  SCREEN_DEPTH   16
#endif
#endif

/* ----- フレームレートの設定 */
#define  FRAME_RATE     60

/* ----- フレームスキップの最大値 */
#define FRAME_SKIP_MAX  2

/* ----- window title */
char   *WindowName = "The Cheap Game Show - CAR";


#endif //BOOTMAIN_H



