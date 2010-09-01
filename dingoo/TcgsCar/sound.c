/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   sound.pp                                             */
/*     �ȈՃT�E���h(SDL_mixer�g�p)                        */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: sound.pp,v 1.3 2002/08/10 03:05:14 rero2 Exp $ */

/*------------------------------------------------------------- */
/** @file
    @brief		�T�E���h�}�l�[�W��
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

/* ----- BGM �g���b�N */
Mix_Music  *SoundTrack;
char  BGMPool[BGMMAX][256];

/* ----- BGM �g���b�N */
Mix_Chunk  *SEPool[SEMAX];

/* ----- �T�E���h�L���t���O */
int  SoundEnable;

/* ----- �T�E���h�`�����l���̃g���b�N */
int  track;

/* -------------------------------------------------------------- */
/* --- �T�E���h                                                   */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- �T�E���h�̏����ƃ��X�g�ǂݍ���       */
/* ---------------------------------------- */
void  SoundInit(void)
{
  int  i, num, l;
  int  ret;
  FILE *FIN;
  char line[256];
  char name[256];

  track = 0;

  /* ----- SDL_mixer �̃I�[�v�� */
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

  /* ----- �z��N���A */
  for(i=0; i<BGMMAX; i++) {
    BGMPool[i][0] = 0;
  }
  for(i=0; i<SEMAX; i++) {
    SEPool[i] = 0;
  }

  /* ----- �T�E���h���X�g�ǂݍ��� */
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
/* --- �T�E���h�̉��                       */
/* ---------------------------------------- */
void  SoundFree(void)
{
  int  i;

  if (SoundEnable == FALSE) {
    return;
  }
  /* ----- �ێ��T�E���h�̉�� */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }
  /* ----- SE�`�����l���̒�~ */
  for(i=0; i<8; i++) {
    Mix_HaltChannel(i);
  }
  /* ----- SE chank �̉�� */
  for(i=0; i<SEMAX; i++) {
    if (SEPool[i] != 0) {
      Mix_FreeChunk(SEPool[i]);
      SEPool[i] = 0;
    }
  }
  /* ----- SDL_Mixer �̃N���[�Y */
  Mix_CloseAudio();
}


/* ---------------------------------------- */
/* --- BGM �̃��N�G�X�g                     */
/* ---------------------------------------- */
void  SoundMusic(int req)
{
  if (SoundEnable == FALSE) {
    return;
  }
  /* --- �͈̓`�F�b�N */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (BGMPool[req][0] == 0) return;
  /* ----- ����܂ł̉��t��~ */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }
  /* ----- BGM ���t�J�n */
  SoundTrack = Mix_LoadMUS(BGMPool[req]);
  if (SoundTrack == NULL) {
    SoundTrack = 0;
    return;
  }
  Mix_PlayMusic(SoundTrack, -1);
}

/* ---------------------------------------- */
/* --- �J��Ԃ��Ȃ� BGM �̃��N�G�X�g        */
/* ---------------------------------------- */
void  SoundMusicOneshot(int  req)
{
  if (SoundEnable == FALSE) {
    return;
  }
  /* --- �͈̓`�F�b�N */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (BGMPool[req][0] == 0) return;
  /* ----- ����܂ł̉��t��~ */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }
  /* ----- BGM ���t�J�n */
  SoundTrack = Mix_LoadMUS(BGMPool[req]);
  if (SoundTrack == NULL) {
    SoundTrack = 0;
    return;
  }
  Mix_PlayMusic(SoundTrack, 1);
}


/* ---------------------------------------- */
/* --- SE �̃��N�G�X�g                      */
/* ---------------------------------------- */
void  SoundSE(int req)
{
  if (SoundEnable == FALSE) return;
  /* --- �͈̓`�F�b�N */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (SEPool[req] == 0) return;
  /* ----- SE ���� */
  Mix_PlayChannel(track, SEPool[req], 0);
  track = (track + 1) % 8;
}

/* ---------------------------------------- */
/* --- BGM �g���b�N�̒�~                   */
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
/* --- SE �̑S��~                          */
/* ---------------------------------------- */
void  SoundSEStop(void)
{
  int i;

  if (SoundEnable == FALSE) {
    return;
  }
  /* --- */
  /* ----- SE�`�����l���̒�~ */
  for(i=0; i<8; i++) {
    Mix_HaltChannel(i);
  }
}


/* ---------------------------------------- */
/* --- Volume�l�̐ݒ�                      */
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


