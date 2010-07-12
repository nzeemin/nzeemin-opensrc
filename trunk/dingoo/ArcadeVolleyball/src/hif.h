/*
Copyright 2010 Harteex Productions
License: Creative Commons Attribution-Noncommercial-No Derivative Works 3.0 Unported
http://creativecommons.org/licenses/by-nc-nd/3.0/
*/

#include <stdint.h>
#include <stdlib.h>

#include <sml/graphics.h>

#ifndef __hif_h__
#define __hif_h__

extern gfx_font* gfx_font_load_hif_from_buffer(uint8_t* buffer);
extern gfx_texture* gfx_tex_load_hif_from_buffer(uint8_t* buffer);

#endif
