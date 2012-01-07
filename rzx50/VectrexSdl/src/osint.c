
#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_gfxPrimitives.h"

#include "osint.h"
#include "vecx.h"
#include "e8910.h"

#define EMU_TIMER 30 /* the emulators heart beats at 20 milliseconds */

static SDL_Surface *screen = NULL;
static SDL_Surface *overlay = NULL;

static int  rotatexy;
static long screenx;
static long screeny;
static long scl_factor;
static long offx;
static long offy;

// SDL audio stuff
SDL_AudioSpec reqSpec;
SDL_AudioSpec givenSpec;
SDL_AudioSpec *usedSpec;

#ifndef WIN32 
  #define stricmp strcasecmp 
#endif

void osint_render(void){
	int v;

    SDL_FillRect(screen, NULL, 0);

	for(v = 0; v < vector_draw_cnt; v++){
		Uint8 c = vectors_draw[v].color * 256 / VECTREX_COLORS;
        if (rotatexy)
		    aalineRGBA(screen,
				    (Sint16)(screenx - offx - vectors_draw[v].y0 / scl_factor),
				    (Sint16)(offy + vectors_draw[v].x0 / scl_factor),
				    (Sint16)(screenx - offx - vectors_draw[v].y1 / scl_factor),
				    (Sint16)(offy + vectors_draw[v].x1 / scl_factor),
				    c, c, c, 0xff);
        else
		    aalineRGBA(screen,
				    (Sint16)(offx + vectors_draw[v].x0 / scl_factor),
				    (Sint16)(offy + vectors_draw[v].y0 / scl_factor),
				    (Sint16)(offx + vectors_draw[v].x1 / scl_factor),
				    (Sint16)(offy + vectors_draw[v].y1 / scl_factor),
				    c, c, c, 0xff);
	}

    // Draw overlay image
    if (overlay != NULL)
    {
        SDL_Rect src, dst;
        dst.x = src.x = 0;  dst.y = src.y = 0;
        dst.w = src.w = (Uint16)screenx;  dst.h = src.h = (Uint16)screeny;
        SDL_BlitSurface(overlay, &src, screen, &dst);
    }

	SDL_Flip(screen);
}

static char *romfilename = "ROM.VECX";
static char *cartfilename = NULL;

static void init(){
    FILE *f;
	if(!(f = fopen(romfilename, "rb"))){
		perror(romfilename);
		exit(EXIT_FAILURE);
	}
	if(fread(rom, 1, sizeof (rom), f) != sizeof (rom)){
		printf("Invalid rom length\n");
		exit(EXIT_FAILURE);
	}
	fclose(f);

	memset(cart, 0, sizeof (cart));
	if(cartfilename){
        char buffer[256];
        char* period;
		FILE *f;

		if(!(f = fopen(cartfilename, "rb"))){
			perror(cartfilename);
			exit(EXIT_FAILURE);
		}
		fread(cart, 1, sizeof (cart), f);
		fclose(f);

        // Prepare name for overlay image: same as cartridge name but with ".png" extension
        strcpy(buffer, cartfilename);
        period = strrchr(buffer, '.');
        strcpy(period, ".png");

        // Seek for overlay image, load if found
        overlay = NULL;
		if(f = fopen(buffer, "rb")){
    		fclose(f);

            overlay = IMG_Load(buffer);
        }
	}
}

void resize(int width, int height, int rotate){
	long sclx, scly;

	screenx = width;
	screeny = height;
	screen = SDL_SetVideoMode(screenx, screeny, 0, 0);
    rotatexy = rotate;

    sclx = (rotate ? ALG_MAX_Y : ALG_MAX_X) / screen->w;
	scly = (rotate ? ALG_MAX_X : ALG_MAX_Y) / screen->h;

	scl_factor = sclx > scly ? sclx : scly;

	offx = (screenx - (rotate ? ALG_MAX_Y : ALG_MAX_X) / scl_factor) / 2;
	offy = (screeny - (rotate ? ALG_MAX_X : ALG_MAX_Y) / scl_factor) / 2;
}

static void readevents(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				exit(0);
				break;
			//case SDL_VIDEORESIZE:
			//	resize(e.resize.w, e.resize.h);
			//	break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym){
					case SDLK_ESCAPE:   // SELECT button on Dingoo
                    case SDLK_PAUSE:    // POWERUP button on Dingoo
						exit(0);
					case SDLK_a:
                    case SDLK_LSHIFT:   // Y button on Dingoo
						snd_regs[14] &= ~0x01;
						break;
					case SDLK_s:
                    case SDLK_SPACE:    // X button on Dingoo
						snd_regs[14] &= ~0x02;
						break;
					case SDLK_d:
                    case SDLK_LALT:     // B button on Dingoo
						snd_regs[14] &= ~0x04;
						break;
					case SDLK_f:
                    case SDLK_LCTRL:    // A button on Dingoo
						snd_regs[14] &= ~0x08;
						break;
					case SDLK_LEFT:
                        if (rotatexy) alg_jch1 = 0x00; else alg_jch0 = 0x00;
                        break;
					case SDLK_RIGHT:
                        if (rotatexy) alg_jch1 = 0xff; else alg_jch0 = 0xff;
                        break;
                    case SDLK_UP:
                        if (rotatexy) alg_jch0 = 0x00; else alg_jch1 = 0xff;
                        break;
                    case SDLK_DOWN:
                        if (rotatexy) alg_jch0 = 0xff; else alg_jch1 = 0x00;
                        break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch(e.key.keysym.sym){
					case SDLK_a:
                    case SDLK_LSHIFT:   // Y button on Dingoo
						snd_regs[14] |= 0x01;
						break;
					case SDLK_s:
                    case SDLK_SPACE:    // X button on Dingoo
						snd_regs[14] |= 0x02;
						break;
					case SDLK_d:
                    case SDLK_LALT:     // B button on Dingoo
						snd_regs[14] |= 0x04;
						break;
					case SDLK_f:
                    case SDLK_LCTRL:    // A button on Dingoo
						snd_regs[14] |= 0x08;
						break;
					case SDLK_LEFT:
                        if (rotatexy) alg_jch1 = 0x80; else alg_jch0 = 0x80;
                        break;
					case SDLK_RIGHT:
                        if (rotatexy) alg_jch1 = 0x80; else alg_jch0 = 0x80;
                        break;
                    case SDLK_UP:
                        if (rotatexy) alg_jch0 = 0x80; else alg_jch1 = 0x80;
                        break;
                    case SDLK_DOWN:
                        if (rotatexy) alg_jch0 = 0x80; else alg_jch1 = 0x80;
                        break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
}

void osint_emuloop(){
	Uint32 next_time = SDL_GetTicks() + EMU_TIMER;
	vecx_reset();
	for(;;){
		vecx_emu((VECTREX_MHZ / 1000) * EMU_TIMER, 0);
		readevents();

		{
			Uint32 now = SDL_GetTicks();
			if(now < next_time)
				SDL_Delay(next_time - now);
			else
				next_time = now;
			next_time += EMU_TIMER;
		}
	}
}

#ifdef _WIN32
#undef main  //HACK for VC error LNK1561: entry point must be defined
#endif

int main(int argc, char *argv[]){
#ifndef _WIN32
    SDL_putenv("DINGOO_IGNORE_OS_EVENTS=1");  //HACK to fix "push long time on X" problem
#endif
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
	    fprintf(stderr, "Failed to initialize video: %s\n", SDL_GetError());
	    exit(-1);
    }
    atexit(SDL_Quit);

#ifdef _WIN32
    SDL_putenv("SDL_VIDEO_WINDOW_POS=300,200");
    SDL_WM_SetCaption("Vectrex SDL", "Vectrex SDL");

    resize(272, 480, 0);
#else
    SDL_ShowCursor(0);

    resize(480, 272, 1);
#endif

#ifdef _WIN32
    if (argc > 1) cartfilename = argv[1];
#else
    if (stricmp(argv[0], "ROM.VECX") != 0)  // If user selected ROM.VECX cartridge then don't use any cartridge
    {
	    cartfilename = argv[0];  // In SIM files argv[0] contains the target to be loaded.
    }
#endif

	init();
    e8910_init_sound();

	osint_emuloop();

    e8910_done_sound();
    SDL_Quit();

	return 0;
}
