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

#include "level.h"
#include <sstream>
#include <cassert>
#include <iostream>
#include <SDL/SDL_gfxPrimitives.h>
#include "level-data.h"

//
// level builder
//

//! Gets level at <index> in range from GAME.level_builder_min_height to
//! GAME.level_builder_max_height.
static float level_builder_get_height(int index)
{
    if (index < 0 && index >= kLevelArraySize)
        throw std::runtime_error("out of range");

    float height_diff = kLevelArrayDataMax - kLevelArrayDataMin; 
    float factor = (float)(level_array_data[index] - kLevelArrayDataMin)/height_diff;
    return GAME.level_builder_min_height + factor * (GAME.level_builder_max_height -
                                                     GAME.level_builder_min_height);
}

// Calculates x coordinate based on index in level_array_data.
static float level_builder_get_x(int index)
{
   return GAME.level_map_width * (float)index / (kLevelArraySize-1);
}

// Builds list of points in world space units from pre-defined 
// level_array_data.
// Optimizes it a little by replacing all horizontal lines by two points, so
// all the horizontal line will conform to one segment and can be used
// as landing areas (if size is enough to fit the lander).
static void level_builder_build_level(Level& level, std::vector<vec_t>& points)
{
    points.clear();
    int platform_start_index = -1;
    int platform_end_index = -1;
    for(int i=0; i<kLevelArraySize; ++i)
    {
        // Check platform start
        if (platform_start_index == -1 &&
           i < kLevelArraySize-1 &&
           equal(level_array_data[i], level_array_data[i+1]))
        {
            platform_start_index = i;
            float x = level_builder_get_x(platform_start_index); 
            float y = level_builder_get_height(platform_start_index);
            vec_t p(x, y);
            points.push_back(p);        
        }

        // Check platform end
        if (platform_start_index != -1 && 
           !equal(level_array_data[i], 
                  level_array_data[platform_start_index]))
        {
            platform_end_index = i - 1;
            float x = level_builder_get_x(platform_end_index); 
            float y = level_builder_get_height(platform_end_index);        
            vec_t p(x, y);
            points.push_back(p); 
            
            platform_start_index = -1;
            platform_end_index = -1;
        }
        
        // Not a platrofm
        if (platform_start_index == -1)
        {
            float x = level_builder_get_x(i);
            float y = level_builder_get_height(i);
            vec_t p(x, y);
            points.push_back(p);
        }
    }
    if (!points.empty())
    {
        points.back().x = GAME.level_map_width;
        points.front().y = points.back().y;
    }        
}

void level_generate_best_landing_areas(Level& level, const Lander& lander, 
                                       size_t num_areas)
{
    // Reset first
    for (size_t i = 0; i < level.segments.size(); ++i)
        level.segments[i].landing_score_factor = 0;

    // Pass num_areas = 0 to only reset preveously prepared areas
    if (!num_areas)
        return;

    if (num_areas > level.segments.size())
        num_areas = level.segments.size();

    float factor = lander.size * GAME.level_landing_size_factor;

    for (size_t i = 0; i < num_areas; ++i)
    {
        size_t index = rand() % level.segments.size();
        for (size_t j = index; j < level.segments.size(); ++j)
        {
            Segment& area = level.segments[j];
            if (!area.good_for_landing)
            {
                continue;
            }
            else
            {
                float width = area.width();

                if (width <= factor * 1.5) 
                    area.landing_score_factor = 5;
                else 
                if (width <= factor * 2)
                    area.landing_score_factor = 3;
                else
                    area.landing_score_factor = 2;
                break;
            }
        }
    }
}

//
// End of level builder
//

bool __no_zooming_hack = false;

float level_map_to_screen_x(float xunits)
{
    // 1 unit = 1 pixel
    if (__no_zooming_hack)
        return xunits + SCREEN.xscroll;
    else
        return SCREEN.zoom * (xunits + SCREEN.xscroll);
}

float level_map_to_screen_y(float yunits)
{
    // 1 unit = 1 pixel
    if (__no_zooming_hack)
        return GAME.screen_height - yunits + SCREEN.yscroll;
    else
        return (GAME.screen_height - yunits + SCREEN.yscroll) * SCREEN.zoom;
}


static void level_search_segment(const Level& level, const vec_t& pos)
{
    int cur = level.current_segment;
    if (cur < 0)                                   
    {
        // whole segment 
        for (int i=0; i<(int)level.segments.size(); ++i)
        {
            if (level.segments[i].within(pos.x))
            {
                level.current_segment = i;
                return;
            }
        }
    }
    else 
    if (pos.x <= level.segments[cur].begin.x)
    {
        // to left from current
        for (int i=cur; i>=0; --i)
        {
            if (level.segments[i].within(pos.x))
            {
                level.current_segment = i;
                return;
            }
        }
    }   
    else
    if (pos.x > level.segments[cur].begin.x)
    {
        // to right from current
        for (int i=cur; i<(int)level.segments.size(); ++i)
        {
            if (level.segments[i].within(pos.x))
            {
                level.current_segment = i;
                return;
            }
        }
    }   

    // assign there level.current_segment to -1, if nothing was found -
    // when lander is out of area
    level.current_segment = -1;
}

float level_get_altitude(const Level& level, const vec_t& pos)
{
    int x = (int)pos.x % GAME.level_map_width;
    if (x < 0)
        x += GAME.level_map_width;
    vec_t screen_pos(x, pos.y); 
    level_search_segment(level, screen_pos);
    int i = level.current_segment;
    if (level.current_segment >= 0)
    {
        return (pos.y - (x * level.segments[i].k + level.segments[i].b));
    }   
    return 1;
}

void level_init(Level& level, const Lander& lander, const Color& color)
{ 
    // Generate landscape
    GAME.level_map_height = kLevelArrayDataMax - kLevelArrayDataMin;
    std::vector<vec_t> points;
    level_builder_build_level(level, points);

    // We need at least two points to build the landscape.
    if (points.size() < 2)
        throw std::runtime_error("level_init not enough data");

    // Init landscape segments 
    for (int i=0; i<(int)points.size()-1; ++i)
    {
        level.segments.push_back(Segment(points[i], points[i+1]));
        Segment& segment = level.segments.back();
         
        // Check if the segment is good for landing
        // Level consists of segments which are linear curves defined as
        // f(x) = k * x + b. So for landing areas should be horizontal (k = 0).
        if (segment.horizontal() && 
            segment.width() >= lander.size * GAME.level_landing_size_factor)
        {
            segment.good_for_landing = true;
        }
        else
        {
            segment.good_for_landing = false;
        }
    }
        
    // init stars
    float alt, k;
    level.stars.resize(GAME.level_max_stars);
    for (int i=0; i<(int)level.stars.size(); ++i)
    {
        level.stars[i].x = rand() % GAME.level_map_width;
        level.stars[i].y = 0;
        // as far as 'bottom in bottom' so level 0 altitude is supposed
        // to be under the ground level, that is why we get it as negative
        // this method (when pos = 0) allows us to get ground level
        // relative 0
        alt = -level_get_altitude(level, level.stars[i]);
        k = frand(); 
        level.stars[i].y = (1-k)*(alt+10) + k * (alt+10+GAME.level_map_height * 2); 
    }

    level.current_segment = -1;
    level_search_segment(level, lander.pos);

    level.color = color;
}

//! Draws areas which are good for landing as RED and also draws text 
//! with score factor (best landing areas).
//!
//! @param level - The level.
//! @param x0 - Segment's pre-calculated screen space begin.x coord.
//! @param x1 - Segment's pre-calculated screen space end.x coord.
//! @param index - Index of the checking segment.
//! @param alias - Anti-aliasing flag, true - on, false - off.
//!
//! @return 
//! true - the segment is good for landing and drawn RED, so no need
//! to be drawn again.
static bool level_draw_landing_area(const Level& level, 
                                    int x0, int x1,
                                    int index, 
                                    bool alias)
{
    assert(index >= 0);
    assert(index < (int)level.segments.size());

    const Segment& area = level.segments[index];

    if (area.good_for_landing) 
    {
        std::ostringstream oss;
        oss << "x" << (int)area.landing_score_factor;
        size_t len = oss.str().length();

        // The screen coords of the area, xx0 needs to center the most left
        // area: if it's good for landing and intersects with the beginning of
        // the screen.
        int xx0 = x0;
        if (equal(xx0, 0))
            xx0 = x1 - level.segments[index].width() * SCREEN.zoom;
        int y = level_map_to_screen_y(level.segments[index].begin.y);
        
        if (area.landing_score_factor > 0)
        {
            draw_text(oss.str(),                    // Text
                      vec_t(xx0+(x1-xx0)/2, y+5),   // Position
                      Color::RED,                   // Color
                      false,                        // World space
                      true,                         // Shift to center x
                      false);                       // Shift to center y
        }                      
        draw_line(vec_t(x0, y), vec_t(x1, y), Color::RED, alias, false);

        return true;
    }

    return false;
}

void level_draw(const Level& level, bool alias)
{
    float x0, y0, x1, y1;

    // Push current segment, needs to be restored at the end, as calling of
    // level_get_altitude() modifies current_segment.
    int current_segment = level.current_segment;

    // Find and draw the first segment that intersects left edge of the
    // screen.
    vec_t beg(-SCREEN.xscroll, 0);
    beg.y = -level_get_altitude(level, beg); 
    int x = (int)beg.x % GAME.level_map_width;
    if (x < 0) 
        x += GAME.level_map_width;
    assert(level.current_segment >= 0);

    float len = level.segments[level.current_segment].end.x - x;
    x0 = 0;
    x1 = len * SCREEN.zoom;
    y0 = level_map_to_screen_y(beg.y);
    y1 = level_map_to_screen_y(level.segments[level.current_segment].end.y);
    
    if (!level_draw_landing_area(level, x0, x1, level.current_segment, alias))
        draw_line(vec_t(x0, y0), vec_t(x1, y1), level.color, alias, false);

    // Draw rest segments till the end of the screen.
    for(int i = level.current_segment+1; ; ++i)
    {
        int c = i % level.segments.size();
        x0 = x1;
        float begin = level_map_to_screen_x(level.segments[c].begin.x);
        float end = level_map_to_screen_x(level.segments[c].end.x);
        x1 = x0 + (end - begin);

        y0 = level_map_to_screen_y(level.segments[c].begin.y);
        y1 = level_map_to_screen_y(level.segments[c].end.y);

        // Outside the screen?
        if (x0 > GAME.screen_width)
            break;

        if (!level_draw_landing_area(level, x0, x1, c, alias))
        {
            draw_line(vec_t(x0, y0), // Start point
                      vec_t(x1, y1), // End point
                      level.color,   // Color
                      alias,         // Anti-aliasing
                      false);        // World space
        }
    }

    // Revert the current interval back.
    if (current_segment > 1)
        level.current_segment = current_segment; 

    // Draw stars
    for (int i=0; i<(int)level.stars.size(); ++i)
    {
        x0 = level_map_to_screen_x(level.stars[i].x);
        y0 = level_map_to_screen_y(level.stars[i].y);
        x0 = (int)x0 % (int)(GAME.level_map_width * SCREEN.zoom);
        if (x0 < 0) 
            x0 += GAME.level_map_width * SCREEN.zoom;

        // Make it continious when moving up
        y0 = (int)y0 % GAME.screen_height;

        draw_point(vec_t(x0, y0),  // Position
                   Color::WHITE,   // Color
                   false);         // World space
    }
}

// Called when lander intersects with the landscape and checks if it's landed
// or not. 
// For landing it is required:
//
// 1) Lander should intersects with a horizontal area and the minimum allowed
//    intersection % should exceeds GAME.level_minimum_intersection_with_landing_area.
// 2) Lander orientation differenece relative to up should be less than 
//    GAME.level_allowed_landing_roll.
// 3) Lander landing speed should be less than:
//   - GAME.level_allowed_soft_landing_xspeed, GAME.level_allowed_soft_landing_yspeed
//     for soft landing.
//   - GAME.level_allowed_hard_landing_xspeed, GAME.level_allowed_hard_landing_yspeed
//     for hard landing.
static int level_check_landing(const Level& level, Lander& lander)
{
    int i = level.current_segment;
    if (i < 0)
        throw std::runtime_error("level_check_landing invalid segment index");

    const int score_base = 100;
    int score_landing_soft = score_base;
    
    // Check landing collisions
    bool landed_ok = false;
    for (size_t i = 0; i < level.segments.size(); ++i)
    {
        const Segment& area = level.segments[i];

        // Allow lander to be able to land to any horizontal area.
        if (!area.horizontal())
            continue;
        
        // Left and right are really swapped, because initially lander was 
        // defined as head over heels.
        float left = (int)lander.box_rb.x % GAME.level_map_width;
        float right = (int)lander.box_lb.x % GAME.level_map_width;
        // To let the lander landing in left direction.
        if (left < 0)
            left += GAME.level_map_width;
        if (right < 0)
            right += GAME.level_map_width;

        float start = level.segments[i].begin.x;
        float end = level.segments[i].end.x;
        float width = end - start;

        if ((left >= start && left < end) ||
            (right >= start && right < end))
        {
            float isect = width;
            if (left > start)
                isect -= (left - start);
            if (right < end)
                isect -= (end - right);
            float isect_percent = isect * 100 / (right - left);

            if (isect_percent > 
                GAME.level_minimum_intersection_with_landing_area)
            {
                // Update the score
                score_landing_soft += 2 * score_base * 
                                      (float)lander.size / area.width();
                score_landing_soft += score_base * 
                                      area.landing_score_factor;
                landed_ok = true;
                break;
            }
        }
    }

    // Lander does not intersects properly with the landscape, let it crash.
    if (!landed_ok)
        return LEVEL_CRASHED;

    //
    // Lander intersects properly with one of landing areas, so the next step
    // is to check allowances - landing speed and orientation.
    //


    // Check landing speed and lander orientation
    if (std::abs((int)lander.angle_deg)%360 <= GAME.level_allowed_landing_roll)
    {
        if (std::abs(lander.vel.x) < GAME.level_allowed_soft_landing_xspeed &&
            std::abs(lander.vel.y) < GAME.level_allowed_soft_landing_yspeed)
        {
            lander.last_landing_score = score_landing_soft;
            lander.score += lander.last_landing_score;
            return LEVEL_LANDED;
        }
        else
        if (std::abs(lander.vel.x) < GAME.level_allowed_hard_landing_xspeed &&
            std::abs(lander.vel.y) < GAME.level_allowed_hard_landing_yspeed)
        {
            lander.last_landing_score = score_landing_soft / 2;
            lander.score += lander.last_landing_score;
            return LEVEL_LANDED_HARD;
        }
        else
            return LEVEL_CRASHED;
    }
    else
    {
        return LEVEL_CRASHED;
    }
}

int level_check_collisions(const Level& level, Lander& lander)
{
    // for all 4 points of lander's bounding box
    
    lander.altitude = level_get_altitude(level, lander.box_lt);
    if (lander.altitude < 0)
        return level_check_landing(level, lander);

    lander.altitude = std::min(lander.altitude, level_get_altitude(level, lander.box_lb));
    if (lander.altitude < 0)
        return level_check_landing(level, lander);

    lander.altitude = std::min(lander.altitude, level_get_altitude(level, lander.box_rt));
    if (lander.altitude < 0)
        return level_check_landing(level, lander);

    lander.altitude = std::min(lander.altitude, level_get_altitude(level, lander.box_rb));
    if (lander.altitude < 0)
        return level_check_landing(level, lander);

    return LEVEL_IN_FLIGHT;
}
