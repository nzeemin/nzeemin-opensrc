/* ---------------------------------------------------------- */
/*  input.h                                                   */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   input.h                                              */
/*     入力装置読みとり部分                               */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id:  */

/*------------------------------------------------------------- */
/** @file
    @brief		インプットマネージ
    @author		K.Kunikane (rerofumi)
    @since		Jul.29.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/


#ifndef INPUT_H
#define INPUT_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

#define  IN_Up       0x0001
#define  IN_Down     0x0002
#define  IN_Left     0x0004
#define  IN_Right    0x0008
#define  IN_Button1  0x0010
#define  IN_Button2  0x0020
#define  IN_Button3  0x0040
#define  IN_Button4  0x0080
#define  IN_Button5  0x0100
#define  IN_Button6  0x0200
#define  IN_Button7  0x0400
#define  IN_Button8  0x0800
#define  IN_Button9  0x1000
#define  IN_Button10 0x2000
#define  IN_Button11 0x4000
#define  IN_Button12 0x8000

/* --- ジョイスティック最大数 */
#define  JOY_NUM_MAX  2

/* --- ジョイスティックの遊び計数 */
#define  JOY_STICK_DITHER  20000

/* --- GP2X 用キーラベル */
#define  IN_GP_A        IN_Button1
#define  IN_GP_B        IN_Button2
#define  IN_GP_X        IN_Button3
#define  IN_GP_Y        IN_Button4
#define  IN_GP_L        IN_Button5
#define  IN_GP_R        IN_Button6
#define  IN_GP_START    IN_Button7
#define  IN_GP_SELECT   IN_Button8
#define  IN_GP_VOLUP    IN_Button9
#define  IN_GP_VOLDOWN  IN_Button10
#define  IN_GP_CLICK    IN_Button11
#define  IN_GP_UPLEFT     0x0010
#define  IN_GP_UPRIGHT    0x0020
#define  IN_GP_DOWNLEFT   0x0040
#define  IN_GP_DOWNRIGHT  0x0080

/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

void  InputInit(void);
void  InputFree(void);
void  InputPoll(void);
int   InputExit(void);
int   InputJoyKey(int side);
int   InputJoyKeyTriger(int side);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //INPUT.H

