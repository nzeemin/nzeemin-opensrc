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

#ifndef SPACE_LANDER_MISC_H_
#define SPACE_LANDER_MISC_H_

#include <stdlib.h>
#include <cmath>
#include <limits>
#include <string>

/**
 * Zoom and scroll information.
 */
struct ScreenInfo
{
    int xscroll;
    int yscroll;
    float zoom;
};

/**
 * Mess of game settings.
 */
struct Game
{
    // Screen information
    int screen_width;
    int screen_height;
    int fps;

    // Landscape and stars options
    int level_map_width;  //!< Landscape width in world units. 
    int level_map_height; //!< Landscape height in world units.
    int level_max_stars;  //!< Total amount of stars.

    // Landing information
    // 
    // For landing it is required:
    //
    // 1) Lander should intersects with a horizontal area and the minimum allowed
    //    intersection % should exceeds level_minimum_intersection_with_landing_area.
    // 2) Lander orientation differenece relative to up should be less than 
    //    level_allowed_landing_roll.
    // 3) Lander landing speed should be less than:
    //   - level_allowed_soft_landing_xspeed, level_allowed_soft_landing_yspeed
    //     for soft landing.
    //   - level_allowed_hard_landing_xspeed, level_allowed_hard_landing_yspeed
    //     for hard landing.
    int level_allowed_landing_roll;
    int level_allowed_soft_landing_xspeed;
    int level_allowed_soft_landing_yspeed;
    int level_allowed_hard_landing_xspeed;
    int level_allowed_hard_landing_yspeed;
    int level_minimum_intersection_with_landing_area;
    float level_landing_size_factor; //!< Landing area assumes good if it is 
                                     //   wider than: 
                                     //   level_landing_size_factor*lander.size
                                     
    // Level & Lander physical properties
    float level_gravity;        //!< Gravity force, should be less than 0.
    float lander_air_friction;  //!< Lander air friction factor.
    float lander_engine_power;  //!< Lander engine power should be greater than
                                //   abs(level_gravity) to compensate it.

    // Lander explode parameters
    float lander_module_explode_xspeed;    
    float lander_module_explode_yspeed;    
    float lander_base_explode_xspeed;      
    float lander_base_explode_yspeed;
    float lander_left_leg_explode_xspeed;
    float lander_left_leg_explode_yspeed;
    float lander_right_leg_explode_xspeed;
    float lander_right_leg_explode_yspeed; 
    float lander_nozzle_explode_xspeed;
    float lander_nozzle_explode_yspeed;

    // Lander parameters
    float lander_rot_step;      //!< Lander rotation step in degrees
    float lander_throttle_step; //!< Lander throttle step, should be less than 1
    float lander_fuel_speed;    //!< Lander fuel speed unit/seconds when throttle = 1 
    float lander_fuel_capacity; //!< Lander fuel capacity in units
    float lander_launch_xpos;   //!< Lander launch X position 
    float lander_launch_ypos;   //!< Lander launch Y position
    float lander_launch_xspeed; //!< Lander launch X speed
    float lander_launch_yspeed; //!< Lander launch Y speed
    float lander_launch_angle;  //!< Lander launch angle in degrees 
    float lander_size;          //!< Lander size in world space units

    // Game properties
    int game_scroll_ytrigger;    //!< Y scroll trigger in pixels (from top and bottom)
    int game_scroll_xtrigger;    //!< X scroll trigger in pixels (from left and right)
    int game_show_debug_objects; //!< Set to true to show debug objects
    int game_speed_limit;        //!< Speed limit in world units/second

    // Lavel builder properties that used for building level from array.
    float level_builder_max_height;
    float level_builder_min_height;
};

/**
 * Global variables not a good idea in general, but it is the most handy
 * way to share some data across all modules without passing annyoing stuff
 * many times to different procedures.
 */
extern int        COLLISION;
extern Game       GAME;
extern ScreenInfo SCREEN;

/**
 * Useful function to check if two values are equal.
 * In case of float/double it is not correct to use == for this.
 */
template <typename T>
bool equal(T left, T right)
{
    return std::abs(left - right) <= std::numeric_limits<T>::epsilon();
}


/** 
 * Simple 2D vector.
 * 
 * Very simple basic functionality of vector, like adding, subtracting,
 * scaling and rotating.
 */
struct vec_t
{
    float x;
    float y;

    explicit vec_t(float xcoord = 0, float ycoord = 0)
        :   x(xcoord),
            y(ycoord) {}

    void set(float xcoord, float ycoord) 
    {
        x = xcoord;
        y = ycoord;
    }
};

/**
 * Degrees to radians
 */
inline float deg_to_rad(float deg)
{
    const float PI = 3.14159265f;
    return deg * PI / 180;
}

/**
 * Difference of two vectors v1-v2.
 */
inline vec_t vec_diff(const vec_t & v1, const vec_t & v2)
{
    return vec_t(v2.x - v1.x, v2.y - v1.y);
}

/**
 * Summ of two vectors v1+v2.
 */
inline vec_t vec_sum(const vec_t & v1, const vec_t & v2)
{
    return vec_t(v1.x + v2.x, v1.y + v2.y);
}

/**
 * Scale vector
 */
inline vec_t vec_scale(const vec_t& v, float factor)
{
    return vec_t(factor * v.x, factor * v.y);
}

/**
 * Rotate vector around (0,0).
 */
inline vec_t vec_rotate(const vec_t& vec, float ang_deg)
{
    vec_t res;
    float ang_rad = deg_to_rad(ang_deg);
    float _sin = sinf(ang_rad);
    float _cos = cosf(ang_rad);
    res.x = _sin * vec.y + _cos * vec.x;
    res.y = _cos * vec.y - _sin * vec.x;
    return res;
}

/**
 * Rotate vector around center. 
 */
inline vec_t vec_rotate(const vec_t& vec, const vec_t& center, float ang_deg)
{
    vec_t dir_rot = vec_rotate(vec_diff(vec, center), ang_deg);
    return vec_sum(center, dir_rot);
}

/**
 * Generates random float from 0 to 1.
 */
inline float frand()
{
    return (float)rand() / RAND_MAX;
}

/**
 * Converts current time to string.
 *
 * Format: YYYY_MM_DD_HHMMSS
 *
 * @param timestamp - Output string with converted timestamp.
 */
void util_current_time_to_string(std::string& str);

/**
 * This functions checks if file exists.
 *
 * @param filename - Checking file name.
 *
 * @note
 * The best Unix way to check it is stat() system call.
 * The best cross-platrorm way is boost::filesystem::exist.
 *
 * The way I am doing it - open the file and check the result using 
 * C++ std::ifstream.
 *
 * vs stat: this actually opens the file, but it is cross-platform.
 * vs boost: boost is cross-platform too and better solution, but I 
 * want to avoid this dependancy at the moment.
 */
bool util_check_filename(const std::string& filename);

/**
 * The wave: f(t) = sin(f*t - p), where:
 *  - f - frequency;
 *  - p - phase.
 *
 * @par
 * The wave might be used for blink lights, when it above 0 - light is on,
 * when it under 0 - light off, etc.
 *
 * @note 
 * Space-Lander welcomes the first C++ class! :)
 */
class Wave 
{
public:

    /**
     * Setup wave to initial position.
     *
     * @param frequency - Wave frequency.
     * @param phase - Wave phase.
     */ 
    explicit Wave(float frequency = 1, float phase = 0)
        :   frequency_(frequency),
            phase_(phase),
            time_(0)
    {
        update(0);
    }

    /**
     * Reset the wave to initial position.
     */
    void reset()
    {
        time_ = 0;
        update(0);
    }

    /**
     * Gets the last wave value.
     */
    float value() const 
    {
        return last_value_;
    }

    /**
     * Update the wave.
     * 
     * @param dt - Time in seconds, elapsed since last update.
     */
    void update(float dt)
    {
        time_ += dt;
        last_value_ = std::sin(frequency_ * time_ - phase_);
    }
    
private:

    float frequency_;  //!< Frequency in Hz
    float phase_;      //!< Phase 
    float time_;       //!< Total elapsed time since last reset/construction
    float last_value_; //!< The recent wave value
};

#endif //SPACE_LANDER_MISC_H_
