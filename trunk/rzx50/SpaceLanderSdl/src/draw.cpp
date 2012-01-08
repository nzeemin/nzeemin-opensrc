//
// This source file is a part of Space Lander game.
// 
// Copyright (C) 2011-2012 by Dmitry Simakov <dosimakov@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "draw.h"
#include <SDL/SDL_gfxPrimitives.h>
#include "level.h"

const Color Color::WHITE(0xFFFFFFFF);
const Color Color::BLACK(0x000000FF);
const Color Color::RED(0xFF0000FF);
const Color Color::GREEN(0x00FF00FF);
const Color Color::BLUE(0x0000FFFF);

// The font is 8x8 in SDL_gfx.
// This needs for center a text against a position.
static const int FONT_SIZE_X = 8; 
static const int FONT_SIZE_Y = 8; 

void draw_line(const vec_t& begin, const vec_t& end, const Color& color,
               bool alias, bool world)
{
    if (alias)
    {
        aalineColor(SDL_GetVideoSurface(), 
                   world ? level_map_to_screen_x(begin.x) : begin.x, 
                   world ? level_map_to_screen_y(begin.y) : begin.y,
                   world ? level_map_to_screen_x(end.x) : end.x, 
                   world ? level_map_to_screen_y(end.y) : end.y, 
                   color.rgba32);
    }
    else
    {
        lineColor(SDL_GetVideoSurface(), 
                  world ? level_map_to_screen_x(begin.x) : begin.x, 
                  world ? level_map_to_screen_y(begin.y) : begin.y,
                  world ? level_map_to_screen_x(end.x) : end.x, 
                  world ? level_map_to_screen_y(end.y) : end.y, 
                  color.rgba32);
    }
}

void draw_circle(const vec_t& center, float radius, const Color& color,
                 bool alias, bool world, bool fill)
{
    if (fill)
    {
        filledCircleColor(SDL_GetVideoSurface(), 
                          world ? level_map_to_screen_x(center.x) : center.x,
                          world ? level_map_to_screen_y(center.y) : center.y, 
                          radius, color.rgba32);
    }
    else
    if (alias)
    {
        aacircleColor(SDL_GetVideoSurface(), 
                      world ? level_map_to_screen_x(center.x) : center.x, 
                      world ? level_map_to_screen_y(center.y) : center.y, 
                      radius, color.rgba32);
    }
    else
    {
        circleColor(SDL_GetVideoSurface(), 
                    world ? level_map_to_screen_x(center.x) : center.x, 
                    world ? level_map_to_screen_y(center.y) : center.y, 
                    radius, color.rgba32);
    }
}

void draw_point(const vec_t& position, const Color& color, bool world)
{
    pixelColor(SDL_GetVideoSurface(), 
               world ? level_map_to_screen_x(position.x) : position.x,
               world ? level_map_to_screen_y(position.y) : position.y,
               color.rgba32);
}

void draw_text(const std::string& text, const vec_t& pos, const Color& color,
               bool world, bool center_x, bool center_y) 
{
    if (text.empty())
        return;

    float text_len = text.length();
    float shift_x = center_x ? (-text_len * FONT_SIZE_X / 2) : 0;
    float shift_y = center_y ? (-text_len * FONT_SIZE_Y / 2) : 0;

    stringColor(SDL_GetVideoSurface(),
                (world ? level_map_to_screen_x(pos.x) : pos.x) + shift_x,
                (world ? level_map_to_screen_y(pos.y) : pos.y) + shift_y,
                text.c_str(), color.rgba32);
}
