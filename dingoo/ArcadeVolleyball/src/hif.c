/*
Copyright 2010 Harteex Productions
License: Creative Commons Attribution-Noncommercial-No Derivative Works 3.0 Unported
http://creativecommons.org/licenses/by-nc-nd/3.0/
*/
#include "hif.h"

gfx_font* gfx_font_load_hif_from_buffer(uint8_t* buffer)
{
	gfx_font* tempFont = malloc(sizeof(gfx_font));

	if (tempFont == NULL)
		return NULL;

	tempFont->texture = gfx_tex_load_hif_from_buffer(buffer);

	if (tempFont->texture == NULL)
	{
		free(tempFont);
		return NULL;
	}

	tempFont->colorKey = COLOR_MAGENTA;
	tempFont->colorize = false;

	return tempFont;
}

gfx_texture* gfx_tex_load_hif_from_buffer(uint8_t* buffer)
{
	if (buffer == NULL)
	{
		return NULL;
	}

	char     hif_ident[3];
	uint8_t  hif_version;
	uint8_t  hif_flags;
	uint8_t  hif_number_of_colors;
	uint16_t hif_width;
	uint16_t hif_height;

	hif_ident[0] = buffer[0];
	hif_ident[1] = buffer[1];
	hif_ident[2] = buffer[2];
	hif_version = buffer[3];
	hif_flags = buffer[4];
	hif_number_of_colors = buffer[5];
	hif_width = buffer[6] + (buffer[7] << 8);
	hif_height = buffer[8] + (buffer[9] << 8);

	uint32_t bufIndex = 10;

	bool twobyte_mode = false;
	uint8_t bits_used_palette = 0;
	uint8_t mask_pixels = 0;
	uint8_t mask_color = 0;

	// Check identification
	if (hif_ident[0] != 'H' || hif_ident[1] != 'I' || hif_ident[2] != 'F')
	{
		return NULL;
	}

	// Check version
	if (hif_version > 1)
	{
		return NULL;
	}

	// Twobyte mode check
	if ((hif_flags & 0x80) > 0)
	{
		twobyte_mode = true;
	}

	if (!twobyte_mode)
	{
		bits_used_palette = (hif_flags & 0x70) >> 4;
		if (bits_used_palette <= 0)
		{
			return NULL;
		}

		uint32_t i;
		for (i = 0; i < 8-bits_used_palette; i++)
		{
			mask_pixels = mask_pixels | (0x01 << i);
		}
		mask_color = ~ mask_pixels;
	}

	gfx_texture* tempTexture = (gfx_texture*)malloc(sizeof(gfx_texture) + (hif_width * hif_height * 2));
	if (tempTexture == NULL)
	{
		return NULL;
	}
	tempTexture->address = (void*)((uintptr_t)tempTexture + sizeof(gfx_texture));
	tempTexture->width = hif_width;
	tempTexture->height = hif_height;

	gfx_color* palette = (gfx_color*)malloc((hif_number_of_colors + 1) * sizeof(gfx_color));
	if (palette == NULL)
	{
		free(tempTexture);
		return NULL;
	}

	uint32_t i;
	uint8_t tempColor[3];
	for (i = 0; i < hif_number_of_colors + 1; i++)
	{
		tempColor[2] = buffer[bufIndex + 0];
		tempColor[1] = buffer[bufIndex + 1];
		tempColor[0] = buffer[bufIndex + 2];
		bufIndex += 4;

		palette[i] = gfx_color_rgb(tempColor[0], tempColor[1], tempColor[2]);
	}

	bool error = false;
	uint8_t curColorIndex;
	uint8_t pixelsInRow;
	uint32_t curPixel = 0;
	uint32_t totalPixels = hif_width * hif_height;
	uint16_t* tempTexPtr = tempTexture->address;
	while (true)
	{
		if (curPixel == totalPixels)
			break;

		if (twobyte_mode)
		{
			curColorIndex = buffer[bufIndex];
			pixelsInRow = buffer[bufIndex + 1];
			bufIndex += 2;
		}
		else
		{
			//Break up byte in color index and number of pixels
			curColorIndex = (buffer[bufIndex] & mask_color) >> (8-bits_used_palette);
			pixelsInRow = buffer[bufIndex] & mask_pixels;
			bufIndex++;
		}

		if (curPixel + pixelsInRow > totalPixels) // OVERFLOW
		{
			error = true;
			break;
		}
		else
		{
			for (i = 0; i < pixelsInRow; i++, curPixel++)
			{
				*tempTexPtr = palette[curColorIndex];
				tempTexPtr++;
			}
		}
	}

	free(palette);
	
	if (error)
	{
		free(tempTexture);
		return NULL;
	}

	return tempTexture;
}
