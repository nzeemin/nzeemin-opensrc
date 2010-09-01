/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   sound.pp                                             */
/*     簡易サウンド(SDL_mixer使用)                        */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: sound.pp,v 1.3 2002/08/10 03:05:14 rero2 Exp $ */

/*------------------------------------------------------------- */
/** @file
    @brief		サウンドマネージャ
    @author		K.Kunikane (rerofumi)
    @since		Sep.04.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_audio.h"
#include "SDL_mixer.h"

#include  "sound.h"

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
/* local value                   */
/*-------------------------------*/

/* ----- BGM トラック */
Mix_Music  *SoundTrack;
char  BGMPool[BGMMAX][256];

/* ----- BGM トラック */
Mix_Chunk  *SEPool[SEMAX];

/* ----- サウンド有効フラグ */
int  SoundEnable;

/* ----- サウンドチャンネルのトラック */
int  track;

/* -------------------------------------------------------------- */
/* --- サウンド                                                   */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- サウンドの初期とリスト読み込み       */
/* ---------------------------------------- */
void  SoundInit(void)
{
  int  i, num, l;
  int  ret;
  FILE *FIN;
  char line[256];
  char name[256];

  track = 0;

  /* ----- SDL_mixer のオープン */
  SoundEnable = TRUE;
  ret = Mix_OpenAudio(44100, AUDIO_S16, 2, 1024);
  if (ret != 0) {
    SoundEnable = FALSE;
#ifdef DEBUG
    printf("SDL mixer open failed.\n");
#endif   
    return;
  }
  for(i=0; i<8; i++) {
    Mix_Volume(i, VOLUMEDEFAULT);
  }
  Mix_VolumeMusic(VOLUMEDEFAULT);

  /* ----- 配列クリア */
  for(i=0; i<BGMMAX; i++) {
    BGMPool[i][0] = 0;
  }
  for(i=0; i<SEMAX; i++) {
    SEPool[i] = 0;
  }

  /* ----- サウンドリスト読み込み */
  FIN = fopen(SOUND_LIST_FILE, "r");
  if (FIN == NULL) {
    SoundEnable = FALSE;
    return;
  }
  while(feof(FIN) == FALSE) {
    fgets(line, 255, FIN);
    if (strlen(line) < 6) continue;
    if (line[0] == ' ') continue;
    if (line[0] == '#') continue;
    /* --- BGM list */
    if (line[0] == 'M') {
      num = (int)((line[1] - '0') * 100 + (line[2] - '0') * 10 + (line[3] - '0'));
      strcpy(name, (line + 5));
      l = strlen(name);
      if ((name[l-1] == '\n') ||
	  (name[l-1] == '\r')) {
	name[l-1] = 0;
      }
      if ((name[l-2] == '\n') ||
	  (name[l-2] == '\r')) {
	name[l-2] = 0;
      }
      strcpy(BGMPool[num], name);
    }
    /* --- SE list */
    if (line[0] == 'S') {
      num = (int)((line[1] - '0') * 100 + (line[2] - '0') * 10 + (line[3] - '0'));
      strcpy(name, (line + 5));
      l = strlen(name);
      if ((name[l-1] == '\n') ||
	  (name[l-1] == '\r')) {
	name[l-1] = 0;
      }
      if ((name[l-2] == '\n') ||
	  (name[l-2] == '\r')) {
	name[l-2] = 0;
      }
      SEPool[num] = Mix_LoadWAV(name);
    }
  }
  fclose(FIN);
}


/* ---------------------------------------- */
/* --- サウンドの解放                       */
/* ---------------------------------------- */
void  SoundFree(void)
{
  int  i;

  if (SoundEnable == FALSE) {
    return;
  }
  /* ----- 保持サウンドの解放 */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }
  /* ----- SEチャンネルの停止 */
  for(i=0; i<8; i++) {
    Mix_HaltChannel(i);
  }
  /* ----- SE chank の解放 */
  for(i=0; i<SEMAX; i++) {
    if (SEPool[i] != 0) {
      Mix_FreeChunk(SEPool[i]);
      SEPool[i] = 0;
    }
  }
  /* ----- SDL_Mixer のクローズ */
  Mix_CloseAudio();
}


/* ---------------------------------------- */
/* --- BGM のリクエスト                     */
/* ---------------------------------------- */
void  SoundMusic(int req)
{
  if (SoundEnable == FALSE) {
    return;
  }
  /* --- 範囲チェック */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (BGMPool[req][0] == 0) return;
  /* ----- それまでの演奏停止 */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }
  /* ----- BGM 演奏開始 */
  SoundTrack = Mix_LoadMUS(BGMPool[req]);
  if (SoundTrack == NULL) {
    SoundTrack = 0;
    return;
  }
  Mix_PlayMusic(SoundTrack, -1);
}

/* ---------------------------------------- */
/* --- 繰り返さない BGM のリクエスト        */
/* ---------------------------------------- */
void  SoundMusicOneshot(int  req)
{
  if (SoundEnable == FALSE) {
    return;
  }
  /* --- 範囲チェック */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (BGMPool[req][0] == 0) return;
  /* ----- それまでの演奏停止 */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }
  /* ----- BGM 演奏開始 */
  SoundTrack = Mix_LoadMUS(BGMPool[req]);
  if (SoundTrack == NULL) {
    SoundTrack = 0;
    return;
  }
  Mix_PlayMusic(SoundTrack, 1);
}


/* ---------------------------------------- */
/* --- SE のリクエスト                      */
/* ---------------------------------------- */
void  SoundSE(int req)
{
  if (SoundEnable == FALSE) return;
  /* --- 範囲チェック */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (SEPool[req] == 0) return;
  /* ----- SE 発呼 */
  Mix_PlayChannel(track, SEPool[req], 0);
  track = (track + 1) % 8;
}

/* ---------------------------------------- */
/* --- BGM トラックの停止                   */
/* ---------------------------------------- */
void  SoundMusicStop(void)
{
  if (SoundEnable == FALSE) {
    return;
  }
  /* --- */
  Mix_HaltMusic();
}

/* ---------------------------------------- */
/* --- SE の全停止                          */
/* ---------------------------------------- */
void  SoundSEStop(void)
{
  int i;

  if (SoundEnable == FALSE) {
    return;
  }
  /* --- */
  /* ----- SEチャンネルの停止 */
  for(i=0; i<8; i++) {
    Mix_HaltChannel(i);
  }
}


/* ---------------------------------------- */
/* --- Volume値の設定                      */
/* ---------------------------------------- */
void  SoundVolume(int value)
{
  int i;

  if (value > MIX_MAX_VOLUME) {
    value = MIX_MAX_VOLUME;
  }
  for(i=0; i<8; i++) {
    Mix_Volume(i, value);
  }
  Mix_VolumeMusic(value);
}


