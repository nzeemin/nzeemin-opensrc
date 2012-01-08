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

#ifndef SPACE_LANDER_LANDER_H_
#define SPACE_LANDER_LANDER_H_

#include <SDL.h>
#include "misc.h"
#include "draw.h"

/**
 * Lander parameters.
 *
 * The lander is a 'vector' objects and looks like:
 * @verbatim
             __
            /  \
          *| 1  |* <- 6 
          __\__/__
         |___2____|
        /  /_3__\ \
       /           \ <--- 4
      --           --  <--- 5
   @endverbatim
 *
 * Where:
 *  - [1] Module for cabin crew: circle raduis depends on the Lander::size and 
 *        ScreenInfo::zoom. The centre of the circle is the lander rotation pt.
 *  - [2] Fuel tank (base): rectangle, connected to the module.
 *  - [3] Nozzle.
 *  - [4] Left and right landing legs (chassis).
 *  - [5] Left and right lading platrorms.
 *  - [6] Left and right navigation lights.
 *
 * The screen size of the module depends on the ScreenInfo::zoom, the real size
 * depends on the Lander::size.
 *
 *            
 * @note
 * Initially lander is defined head over heels and then rotated to the world 
 * space orientation (where Up looks to skies vs screen space where Up looks
 * to the floor). So when the bouding box is used - the left,bottom point will 
 * be box_rb, the right,bottom - box_lb and the same for the top.
 */
struct Lander 
{
    float size;           //!< Module-radius and main size factor.
    vec_t pos;            //!< Center of the Module, in world coords.
    Color color;          //!< Lander color.
    Color flame_color;    //!< Fuel flame color.
    float angle_deg;      //!< Orientation relative to the initial position
                          //!< in degrees.

    //
    // Points
    //

    // base
    vec_t base_lt; //!< base left-top
    vec_t base_lb; //!< base left-bottom
    vec_t base_rt; //!< base right-top
    vec_t base_rb; //!< base right-bottom

    // landing chassis
    vec_t left_leg_beg; //!< left landing gear begin
    vec_t left_leg_end; //!< left landing gear end
    vec_t right_leg_beg; //!< right landing gear begin
    vec_t right_leg_end; //!< right landing gear end

    // landing platform
    vec_t left_plat_beg; //!< left landing platform begin
    vec_t left_plat_end; //!< left landing platform end
    vec_t right_plat_beg; //!< right landing platform begin
    vec_t right_plat_end; //!< right landing platform end

    // nozzle
    vec_t nozzle_lt; //!< nozzle left-top
    vec_t nozzle_lb; //!< nozzle left-bottom
    vec_t nozzle_rt; //!< nozzle right-top
    vec_t nozzle_rb; //!< nozzle right-bottom
    
    // flame
    vec_t flame;     //!< flame direction

    // navigation lights
    vec_t left_nav_light;
    vec_t right_nav_light;

    // 
    // Collision bounding box
    //

    vec_t box_lt; //!< Left-top 
    vec_t box_lb; //!< Left-bottom
    vec_t box_rt; //!< Right-top
    vec_t box_rb; //!< Right-bottom

    //
    // Physics/Kinematics
    //

    float air_friction;  //!< air friction factor 
    float engine_power;  //!< engine power in Neutons
    vec_t vel;           //!< current velocity
    vec_t up;            //!< rotated up vector
    bool explode;        //!< explode mode

    //
    // Level
    //

    float altitude; //!< Altitude in world units.
    float fuel; //!< Remained fuel.

    // Score each landing:
    // soft: base + 2 * base * (lander.size / platform.size) + base * factor
    // hard: soft / 2 
    int score; //!< Score.
    int last_landing_score;

    // Misc
    Wave nav_light_wave;
};

/**
 * Initializes the lander.
 *
 * @param lander - Lander instance to be initialized.
 * @param xpos - Start X position in world coords.
 * @param ypos - Start Y position in world coords.
 * @param size - Lander size in world units.
 * @param color - Lander's color (when drawing).
 * @param flame_color - Lander's flame color (when drawing).
 */
void lander_init(Lander& lander, float xpos, float ypos, float size, 
                 const Color& color, const Color& flame_color);

/**
 * Updates lander points - i.e. performs moving and rotations. 
 * 
 * It recalculates the points based on updated Lander::pos and 
 * Lander::angle_deg.
 *
 * @param lander - Lander instance that points to be updated.
 * @param throttle - Throttle factor from 0 to 1: 0 engine off, 1 - full power.
 *                   Needed here for flame rendering.
 */
void lander_update_points(Lander& lander, float throttle);

/**
 * Update lander position and velocity based on the Kinematics formulas:
 *  air_friction_force = air_friction_factor * velocity
 *  acceleration = gravity + engine_power + air_friction_force
 *  velocity += acceleration * dt
 *  position += velocity * dt
 * 
 * @param lander - The lander instance to be updated.
 * @param throttle - Throttle factor from 0 to 1 (0 engine off, 1 full power).
 * @param dt - Elapsed time in seconds since last update.
 */
void lander_update_physics(Lander& lander, float throttle, float dt);

/**
 * Explodes the lander.
 *
 * @param lander - Lander to be exploded.
 */
void lander_explode(Lander& lander);

/**
 * Draws the lander.
 *
 * @param lander - Lander to be drawn.
 * @param alias - anti-aliasing mode: true on, false off. 
 */
void lander_draw(Lander& lander, bool alias);

#endif // SPACE_LANDER_LANDER_H_

