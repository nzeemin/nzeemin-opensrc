/* ---------------------------------------------------------- */
/*  debug.h                                                   */
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
    @brief		デバッグチェックルーチン
    @author		K.Kunikane (rerofumi)
    @since		Jul.27.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/


#ifndef DEBUG_H
#define DEBUG_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include "SDL.h"

#include "grp_screen.h"

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

/* ----- メッセージの表示量 */
#define  MessageMax  20

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

#ifdef DEBUG
 #define DEBUGPRINT(FMT, ...) \
    snprintf(debug_line, 127, FMT, __VA_ARGS__); \
    TDebugPrint(debug_line);
#else
 #define DEBUGPRINT(FMT, ...) 
#endif

/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

extern char debug_line[128];

/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

void  TDebugInit(TGameScreen *screen,
		int Width, int Height, int Depth);
void  TDebugFree(void);
void  TDebugDisp(TGameScreen *screen);
void  TDebugPrint(char *mes);
void  IntToStr(int num, char *buf);
void  IntToHex(int num, int length, char *buf);

#endif //DEBUG_H
