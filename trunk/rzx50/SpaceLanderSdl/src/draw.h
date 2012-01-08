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

#ifndef SPACE_LANDER_DRAW_H_
#define SPACE_LANDER_DRAW_H_

#include <stdint.h>
#include <string>
#include "misc.h"

//
// Drawing
// 

/**
 * Simple color structure that provides color in RGBA space.
 */
struct Color
{
    union
    {
        struct
        {   
            uint8_t r, g, b, a;
        } rgba8;
        uint32_t rgba32;
    };

    explicit Color(uint32_t color = 0xFFFFFFFF) 
        : rgba32(color) {}

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
    {
        rgba8.r = r;
        rgba8.g = g;
        rgba8.b = b;
        rgba8.a = a;
    }

    // Clear colors
    static const Color WHITE;
    static const Color BLACK;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
};

/**
 * Draw a line. 
 * 
 * @param begin - Start point of the line in world or screen coords.
 * @param end - End point of the line in world or screen coords.
 * @param color - Line color. 
 * @param alias - Anti-aliasing flag: true on, false off.
 * @param world - true: begin and end are defined in world space; 
 *                false: begin and end are defined in screen space.
 */
void draw_line(const vec_t& begin, const vec_t& end, const Color& color,
               bool alias = true, bool world = true);

/**
 * Draw a circle.
 *
 * @param center - Position of circle's centre in world or screen coords.
 * @param radus - Circle's radius in world or screen units.
 * @param color - Circle's color.
 * @param alias - Anti-aliasing flag: true on, false off.
 * @param world - true: center and radius are defined in world space; 
 *                false: center and radius are defined in screen space.
 * @param fill - true if circle need to be filled by color.
 */
void draw_circle(const vec_t& center, float radius, const Color& color,
                 bool alias = true, bool world = true, bool fill = false);


/**
 * Draw a point (pixel actually).
 *
 * @param position - Point's position.
 * @param color - Point's color.
 * @param world - true: position defined in world space;
 *                false: in screen space.
 */
void draw_point(const vec_t& position, const Color& color, bool world = true);

/** 
 * Draw a text.
 * 
 * Draw text in certain position. 
 *
 * @param text - The text.
 * @param pos - Text position in world or screen coords.
 * @param color - Text color.
 * @param world - true: text position is defined in world coords;
 *                flase: text position is defined in screen coords.
 * @param center_x - shift text to make position as center by x.
 * @param center_y - shift text to make position as center by y.
 * 
 */
void draw_text(const std::string& text, const vec_t& pos, const Color& color,
               bool world = false, bool center_x = true, bool center_y = true); 

#endif //SPACE_LANDER_DRAW_H_
