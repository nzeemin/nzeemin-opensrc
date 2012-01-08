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

#ifndef SPACE_LANDER_LEVEL_H_
#define SPACE_LANDER_LEVEL_H_

#include <cstdlib>
#include <vector>
#include <map>
#include <stdexcept>
#include <SDL.h>
#include "misc.h"
#include "lander.h"
#include "draw.h"

/**
 * Collision result.
 */
enum 
{   
    LEVEL_IN_FLIGHT,
    LEVEL_LANDED,
    LEVEL_LANDED_HARD,
    LEVEL_CRASHED
};

/**
 * Landscape segment.
 *
 * It contains begin and end points and also k and b factors, which describe
 * this segmens as a linear curve: f(x) = k * x + b.
 */ 
struct Segment 
{
    float k;                    //!< f(x) = k * x + b 
    float b;                    //!< f(x) = k * x + b
    vec_t begin;                //!< Segment start point.
    vec_t end;                  //!< Segment end point.
    bool  good_for_landing;     //!< True if segment is good for landing.
    float landing_score_factor; //!< Landing score factor (bonus).

    /**
     * Constructs the segment.
     * 
     * @param i_begin - Segment start point.
     * @param i_end - Segment end point.
     *
     * @note
     * Segment must not be vertical. 
     */
    Segment(const vec_t& i_begin, const vec_t& i_end)
        :   begin(i_begin), 
            end(i_end), 
            good_for_landing(false), 
            landing_score_factor(0)
    {
        // Check if it's vertical. 
        if (equal(begin.x, end.x))
            throw std::runtime_error("Segment must not be vertical");

        k = (end.y - begin.y) / (end.x - begin.x); 
        b = begin.y - k * begin.x;
    }

    /**
     * Returns true if the segment is a  horizontal line.
     */
    bool horizontal() const 
    {
        return equal(k, 0.0f);
    }

    /**
     * Returns the segment width in world units, from begin to end by x.
     */
    float width() const 
    {
        return end.x - begin.x;
    }

    /**
     * Checks if the point lies within the segment by x.
     *
     * @param point - Checking point.
     * 
     * @return 
     * true - point within segment, false - outside.
     */
    bool within(const float& point) const 
    {
        if (point >= begin.x && point < end.x)
            return true;
        else
            return false;
    }
};

/**
 * Level consist of segments that defines landscape.
 *
 * @note
 * All is defined in world units here.
 */
struct Level 
{
    std::vector<Segment> segments; //!< List of segments.
    mutable int current_segment;   //!< Current segment; 
                                   //   mutable because it's cached value.
    std::vector<vec_t> stars;      //!< List of stars.
    Color color;                   //!< Landscape color.
};

/**
 * Performs level initialization.
 *
 * @param level - The level.
 * @param lander - Lander instance, needed for initialization areas for
 *                 landing, that depends on the lander's size.
 * @param color - Landscape color.
 *
 * @note
 * This function must be called first, before any other in this module.
 */
void level_init(Level& level, const Lander& lander, const Color& color);

/**
 * Calculates altitude for given position.
 *
 * @param level - The level.
 * @param pos - Position in world space, y-component is not matter here.
 *
 * @return
 * Altitude in world units.
 */
float level_get_altitude(const Level& level, const vec_t& pos);

/**
 * Prepares several areas which are better for landing (score bonuses).
 * Normally it should be called before each lander launch.
 * 
 * @param level - The level. 
 * @param lander - Lander instance: it generates best areas for this lander.
 * @param num_areas - Total bonus landing areas (prepares up to num_areas).
 */
void level_generate_best_landing_areas(Level& level, const Lander& lander, 
                                       size_t num_areas);

/**
 * Draws the landscape and stars in space.
 * Normally it should be called once per frame.
 *
 * @param level - The level.
 * @param alias - Anti-aliasing flag: true - on, false - off.
 */
void level_draw(const Level& level, bool alias);

/**
 * Checks collision and returns one of the collision statuses. 
 *
 * @param level - The level.
 * @param lander - Lander instance: it checks collision for this lander.
 *
 * @return
 * One of the collision statuses: LEVEL_IN_FLIGHT, LEVEL_LANDED, and so on.
 *
 * @note
 * Modifies Lander::altitude.
 */
int level_check_collisions(const Level& level, Lander& lander);

/**
 * Maps world space X coordinate to the screen space, handles zoom and 
 * scrolling.
 * 
 * @param xunits - X in the world space.
 *
 * @return
 * X in the screen space.
 */
float level_map_to_screen_x(float xunits);

/**
 * Maps world space Y coordinate to the screen space, handles zoom and.
 * scrolling. 
 */
float level_map_to_screen_y(float yunits);

#endif // SPACE_LANDER_LEVEL_H_
