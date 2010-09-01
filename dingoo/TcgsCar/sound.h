/* ---------------------------------------------------------- */
/*  sound.h                                                   */
/* ---------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   sound.h                                              */
/*     �����}�l�[�W��                                     */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id:  $ */


/*------------------------------------------------------------- */
/** @file
    @brief		�����}�l�[�W��
    @author		K.Kunikane (rerofumi)
    @since		Sep.04.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/


#ifndef SOUND_H
#define SOUND_H

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

/* --- �T�E���h�ꗗ�\�̃t�@�C���l�[�� */
#define   SOUND_LIST_FILE "sounddat.lst"

/* --- BGM �̊Ǘ��ő吔 */
#define BGMMAX  32

/* --- SE �̊Ǘ��ő吔 */
#define SEMAX   64

/* --- VOUME �̍ő�l */
#ifdef __GP2X__
#define VOLUMEDEFAULT  96
#else
#define VOLUMEDEFAULT  (MIX_MAX_VOLUME / 4)
#endif



/*-------------------------------*/
/* struct                        */
/*-------------------------------*/

/* ---------------------------------------------- */
/* --- extern                                  -- */
/* ---------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

void  SoundInit(void);
void  SoundFree(void);
void  SoundMusic(int req);
void  SoundMusicOneshot(int req);
void  SoundSE(int req);
void  SoundMusicStop(void);
void  SoundSEStop(void);
void  SoundVolume(int value);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //SOUND_H






