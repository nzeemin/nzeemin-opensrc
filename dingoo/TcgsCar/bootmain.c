/**
 *  --- SDL puzzle project 'tailtale'.
 *
 */

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef __GP2X__
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "SDL.h"
#include "SDL_main.h"
#include "SDL_audio.h"
#include "SDL_mixer.h"

#include "bootmain.h"
#include "debug.h"
#include "input.h"
#include "sound.h"
#include "grp_screen.h"

#include "gamemain.h"

TGameScreen  *scr;
TGameMain  *gamemain;

#ifdef __GP2X__
static volatile unsigned int *memregs32;
static volatile unsigned short *memregs16;
static volatile unsigned char *memregs8;
int wiz_device;
#endif

/* --------------------------------------------- */
/* --- システムタイマー                        --- */
/* --------------------------------------------- */

void  init_timer()
{
#ifdef __GP2X__
  wiz_device = open("/dev/mem", O_RDWR);
  memregs32 = (volatile unsigned int *)mmap(0,
					    0x20000,
					    PROT_READ|PROT_WRITE,
					    MAP_SHARED,
					    wiz_device,
					    0xC0000000);
  memregs16 = (volatile unsigned short *)memregs32;
  memregs8 = (volatile unsigned char *)memregs32;
#endif
}

void  close_timer()
{
#ifdef __GP2X__
  close(wiz_device);
#endif
}

unsigned long  get_timer()
{
  return clock() / (CLOCKS_PER_SEC / 1000);
}

void timer_delay(unsigned long time)
{
#ifndef __MACH__
  //usleep(time*1000);
  SDL_Delay(time);
#endif
}

void vsync_wait()
{
#ifdef __GP2X__
  while((memregs16[0x308C>>1] & (1 << 10)) == 0);
  memregs16[0x308C>>1] |= (1 << 10);
#endif
}

#ifdef _WIN32
#undef main  //HACK for error LNK1561: entry point must be defined
#endif

/* --------------------------------------------- */
/* --- メインルーチン                          --- */
/* --------------------------------------------- */
int  main(int argc, char *argv[])
{
  int  i;
  /* --- メインルーチンループフラグ */
  int  endflag;
  /* --- タイムカウント */
  int  SystemTime;
  int  BeforeTiming;
  int  NowTiming;
  int  WorkTime;
  int  DispTime;
  int  FrameCounter;
  int  FrameSkip;

  WorkTime = 0;
  FrameSkip = 0;
  NowTiming = 0;

  /* ----- SDL 初期化 */
  i = SDL_Init(SDL_INIT_VIDEO |
	       SDL_INIT_AUDIO);
  if (i != 0) {
    /* --- SDLが初期化できなかった */
#ifdef DEBUG
    printf("SDL initialize error.\n");
#endif
    exit(-1);
  }

  /* --- 終了フラグ */
  endflag = 0;

  /* ----- ハードウェア初期化 */
  init_timer();
  InputInit();
  SoundInit();
  scr = TGameScreen_Create(SCREEN_WIDTH, SCREEN_HEIGHT,
			   SCREEN_DEPTH);

  TGameScreen_SetWMName(scr, WindowName);
  SystemTime = get_timer();
  BeforeTiming = SystemTime;
  DispTime = 0;
  FrameCounter = 0;
  srand(1);

  /* ----- ゲーム本体初期化 */
  gamemain = TGameMain_Create(scr);

  /* ---------------------------------------- */
  /* ----- メインルーチン                     */
  /* ---------------------------------------- */
  while(endflag == 0) {
    /* --- ゲーム本体ループ */
    for(i=0; i<=FrameSkip; i++) {
      FrameCounter = FrameCounter + 1;
      InputPoll();
      /* ------------------------------- */
      /* ----- ゲームメインへ */
      if ((TGameMain_Poll(gamemain, FrameCounter) == FALSE) ||
	  (InputExit() != 0)) {
	BeforeTiming = get_timer();
	break;
      }
    }

    /* ----- デバッグ用監視 */
    DEBUGPRINT("%d", FrameCounter);
    DEBUGPRINT("Work Time : W%02d/D%02d/%d", WorkTime, DispTime, (1000/FRAME_RATE));
    DEBUGPRINT("Frame Skip : %d", FrameSkip);
    DEBUGPRINT("Key Input : %x", InputJoyKey(0));
    DEBUGPRINT("Delay : %d", NowTiming);

    WorkTime = get_timer() - BeforeTiming;

    /* --- フレーム終了、スクリーン描画 */
    TGameScreen_UpdateScreen(scr);

    /* --- フレームタイマー */
    DispTime = get_timer() - BeforeTiming;
#ifdef __GP2X__
    vsync_wait();
    /* swap screen */
    TGameScreen_SwapScreen(scr);
#else
    /* swap screen */
    TGameScreen_SwapScreen(scr);
    NowTiming = (1000 / FRAME_RATE) - (DispTime % (1000 / FRAME_RATE));
    if ((NowTiming > 0) && (NowTiming <= (1000 / FRAME_RATE))) {
      timer_delay(NowTiming);
    }
    FrameSkip = DispTime / (1000 / FRAME_RATE);
    if (FrameSkip > FRAME_SKIP_MAX) {
      FrameSkip = FRAME_SKIP_MAX;
    }
#endif
    endflag = InputExit() || TGameMain_GetState(gamemain);
    BeforeTiming = get_timer();


    /* ---------------------------------------- */
    /* ----- メインループココまで               */
  }

  /* --- ゲームメインループ終了 */
  TGameMain_Destroy(gamemain);
  /* --- スクリーン解放 */
  TGameScreen_Destroy(scr);
  /* --- 終了、SDL 後処理 */
  SoundFree();
  InputFree();
  close_timer();
#ifdef __GP2X__
  chdir("/usr/gp2x");
  execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#else
  SDL_Quit();
#endif

  return(0);
}

