
#define _CRT_SECURE_NO_WARNINGS  // For VC warning C4996: 'sprintf': This function or variable may be unsafe.

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "osint.h"
#include "vecx.h"

#define EMU_TIMER 30 /* the emulators heart beats at 20 milliseconds */

static SDL_Surface *screen = NULL;

static int  rotatexy;
static long screenx;
static long screeny;
static long scl_factor;
static long offx;
static long offy;

void osint_render(void){
	int v;

    SDL_FillRect(screen, NULL, 0);

	for(v = 0; v < vector_draw_cnt; v++){
		Uint8 c = vectors_draw[v].color * 256 / VECTREX_COLORS;
        if (rotatexy)
		    aalineRGBA(screen,
				    screenx - offx - vectors_draw[v].y0 / scl_factor,
				    offy + vectors_draw[v].x0 / scl_factor,
				    screenx - offx - vectors_draw[v].y1 / scl_factor,
				    offy + vectors_draw[v].x1 / scl_factor,
				    c, c, c, 0xff);
        else
		    aalineRGBA(screen,
				    offx + vectors_draw[v].x0 / scl_factor,
				    offy + vectors_draw[v].y0 / scl_factor,
				    offx + vectors_draw[v].x1 / scl_factor,
				    offy + vectors_draw[v].y1 / scl_factor,
				    c, c, c, 0xff);
	}
	SDL_Flip(screen);
}

static char *romfilename = "rom.dat";
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
		FILE *f;
		if(!(f = fopen(cartfilename, "rb"))){
			perror(cartfilename);
			exit(EXIT_FAILURE);
		}
		fread(cart, 1, sizeof (cart), f);
		fclose(f);
	}
}

void resize(int width, int height, int rotate){
	long sclx, scly;

	screenx = width;
	screeny = height;
	screen = SDL_SetVideoMode(screenx, screeny, 0, SDL_SWSURFACE | SDL_RESIZABLE);
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
					case SDLK_ESCAPE:
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
					//case SDLK_LEFT:
                    case SDLK_UP:
						alg_jch0 = 0x00;
						break;
					//case SDLK_RIGHT:
                    case SDLK_DOWN:
						alg_jch0 = 0xff;
						break;
					//case SDLK_UP:
                    case SDLK_RIGHT:
						alg_jch1 = 0xff;
						break;
					//case SDLK_DOWN:
                    case SDLK_LEFT:
						alg_jch1 = 0x00;
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
					//case SDLK_LEFT:
                    case SDLK_UP:
						alg_jch0 = 0x80;
						break;
					//case SDLK_RIGHT:
                    case SDLK_DOWN:
						alg_jch0 = 0x80;
						break;
					//case SDLK_UP:
                    case SDLK_RIGHT:
						alg_jch1 = 0x80;
						break;
					//case SDLK_DOWN:
                    case SDLK_LEFT:
						alg_jch1 = 0x80;
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
	SDL_Init(SDL_INIT_VIDEO);

	//resize(330*3/2, 410*3/2);
    resize(320, 240, 1);

	if(argc > 1)
		cartfilename = argv[1];

	init();

	osint_emuloop();

	return 0;
}

