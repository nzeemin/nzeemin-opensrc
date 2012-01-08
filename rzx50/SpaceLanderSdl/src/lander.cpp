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

#include <SDL_gfxPrimitives.h>
#include <sstream>
#include <iostream>
#include "lander.h"
#include "level.h"

void lander_init(Lander& lander, float xpos, float ypos, float size, 
                 const Color& color, const Color& flame_color)
{
    lander.size = size;
    lander.pos.x = xpos;
    lander.pos.y = ypos;
    lander.color = color; 
    lander.flame_color = color;
    lander.angle_deg = GAME.lander_launch_angle;
    lander.vel.set(0, 0);
    lander.air_friction = GAME.lander_air_friction;
    lander.engine_power = GAME.lander_engine_power;

    lander.explode = false;

    // Make navigation lights blinking 10 times pre second.
    lander.nav_light_wave = Wave(10);

    // update rotated points
    lander_update_points(lander, 0);
}

void lander_update_points(Lander& lander, float throttle)
{
    // proportions
    float base_sz = ceil(lander.size/2);
    float base_amp = 0; 
    float base_offset = 0; 
    float leg_len = lander.size;
    float leg_amp = 0.5;
    float leg_sz = 0.5; 
    float nozzle_offset = 0.2; 
    float nozzle_len = floor(lander.size/1.8);
    float flame_len = lander.size * 6;
    float flame_amp = ceil(flame_len/2);

    vec_t base_lt; 
    vec_t base_lb;
    vec_t base_rt;
    vec_t base_rb; 
    vec_t left_leg_beg;
    vec_t left_leg_end; 
    vec_t right_leg_beg;
    vec_t right_leg_end;
    vec_t left_plat_beg;
    vec_t left_plat_end;
    vec_t right_plat_beg;
    vec_t right_plat_end; 
    vec_t nozzle_lt;
    vec_t nozzle_lb;
    vec_t nozzle_rt;
    vec_t nozzle_rb;
    vec_t flame;
    vec_t box_lt;
    vec_t box_rt;
    
    if (!lander.explode)
    {
        // base
        base_lt.set(lander.pos.x - lander.size - base_amp, lander.pos.y + lander.size - base_offset);
        base_lb.set(lander.pos.x - lander.size - base_amp, lander.pos.y + lander.size + base_sz - base_offset);
        base_rt.set(lander.pos.x + lander.size + base_amp, lander.pos.y + lander.size - base_offset);
        base_rb.set(lander.pos.x + lander.size + base_amp, lander.pos.y + lander.size + base_sz - base_offset);

        // left landing leg
        left_leg_beg.set(lander.pos.x - lander.size, lander.pos.y + lander.size + base_sz);
        left_leg_end.set(lander.pos.x - lander.size - leg_amp, lander.pos.y + lander.size + base_sz + leg_len);

        // right landing leg
        right_leg_beg.set(lander.pos.x + lander.size, lander.pos.y + lander.size + base_sz);
        right_leg_end.set(lander.pos.x + lander.size + leg_amp, lander.pos.y + lander.size + base_sz + leg_len);

        // left landing platform
        left_plat_beg.set(lander.pos.x - lander.size - leg_amp - leg_sz, lander.pos.y + lander.size + base_sz + leg_len);
        left_plat_end.set(lander.pos.x - lander.size - leg_amp + leg_sz, lander.pos.y + lander.size + base_sz + leg_len);

        // right landing platform
        right_plat_beg.set(lander.pos.x + lander.size + leg_amp - leg_sz, lander.pos.y + lander.size + base_sz + leg_len);
        right_plat_end.set(lander.pos.x + lander.size + leg_amp + leg_sz, lander.pos.y + lander.size + base_sz + leg_len);

        // nozzle
        nozzle_lt.set(lander.pos.x - lander.size + nozzle_offset, lander.pos.y + lander.size + base_sz);
        nozzle_lb.set(lander.pos.x - lander.size - nozzle_offset/2, lander.pos.y + lander.size + base_sz + nozzle_len);
        nozzle_rt.set(lander.pos.x + lander.size - nozzle_offset, lander.pos.y + lander.size + base_sz);
        nozzle_rb.set(lander.pos.x + lander.size + nozzle_offset/2, lander.pos.y + lander.size + base_sz + nozzle_len);

        // flame

        if (throttle == 1)
        {
            flame.set(lander.pos.x, throttle * flame_len + nozzle_lb.y + frand() * flame_amp);
        }
        else
        {
            flame.set(lander.pos.x, throttle * flame_len + nozzle_lb.y);
        }

        //
        // rotate points
        //

        lander.base_lt = vec_rotate(base_lt, lander.pos, lander.angle_deg);
        lander.base_lb = vec_rotate(base_lb, lander.pos, lander.angle_deg);
        lander.base_rt = vec_rotate(base_rt, lander.pos, lander.angle_deg);
        lander.base_rb = vec_rotate(base_rb, lander.pos, lander.angle_deg);

        lander.left_leg_beg = vec_rotate(left_leg_beg, lander.pos, lander.angle_deg);
        lander.left_leg_end = vec_rotate(left_leg_end, lander.pos, lander.angle_deg);
        lander.right_leg_beg = vec_rotate(right_leg_beg, lander.pos, lander.angle_deg);
        lander.right_leg_end = vec_rotate(right_leg_end, lander.pos, lander.angle_deg);

        lander.left_plat_beg = vec_rotate(left_plat_beg, lander.pos, lander.angle_deg);
        lander.left_plat_end = vec_rotate(left_plat_end, lander.pos, lander.angle_deg);
        lander.right_plat_beg = vec_rotate(right_plat_beg, lander.pos, lander.angle_deg);
        lander.right_plat_end = vec_rotate(right_plat_end, lander.pos, lander.angle_deg);

        lander.nozzle_lt = vec_rotate(nozzle_lt, lander.pos, lander.angle_deg);
        lander.nozzle_lb = vec_rotate(nozzle_lb, lander.pos, lander.angle_deg);
        lander.nozzle_rt = vec_rotate(nozzle_rt, lander.pos, lander.angle_deg);
        lander.nozzle_rb = vec_rotate(nozzle_rb, lander.pos, lander.angle_deg);

        lander.flame = vec_rotate(flame, lander.pos, lander.angle_deg);

        //
        // get bounds
        //
        box_lt.set(lander.pos.x - lander.size, lander.pos.y - lander.size);
        box_rt.set(lander.pos.x + lander.size, lander.pos.y - lander.size);
        lander.box_lt = vec_rotate(box_lt, lander.pos, lander.angle_deg);
        lander.box_rt = vec_rotate(box_rt, lander.pos, lander.angle_deg);
        lander.box_lb = lander.left_plat_beg;
        lander.box_rb = lander.right_plat_end;

        vec_t up(0,-1); //as far as we define lander lookig to bottom initially
        lander.up = vec_rotate(up, lander.angle_deg);

        vec_t left = vec_scale(vec_t(-1, 0), lander.size*1.5);
        vec_t right = vec_scale(vec_t(1, 0), lander.size*1.5);
        lander.left_nav_light = vec_sum(vec_rotate(left, lander.angle_deg), lander.pos);
        lander.right_nav_light = vec_sum(vec_rotate(right, lander.angle_deg), lander.pos);
    }
    else
    {
        lander.pos.x += GAME.lander_module_explode_xspeed;
        lander.pos.y += GAME.lander_module_explode_yspeed;
        
        lander.base_lt.x += GAME.lander_base_explode_xspeed;
        lander.base_lt.y += GAME.lander_base_explode_yspeed;
        lander.base_rt.x += GAME.lander_base_explode_xspeed;
        lander.base_rt.y += GAME.lander_base_explode_yspeed;
        lander.base_lb.x += GAME.lander_base_explode_xspeed;
        lander.base_lb.y += GAME.lander_base_explode_yspeed;
        lander.base_rb.x += GAME.lander_base_explode_xspeed;
        lander.base_rb.y += GAME.lander_base_explode_yspeed;

        // left landing leg
        lander.left_leg_beg.x += GAME.lander_left_leg_explode_xspeed;
        lander.left_leg_beg.y += GAME.lander_left_leg_explode_yspeed;
        lander.left_leg_end.x += GAME.lander_left_leg_explode_xspeed;
        lander.left_leg_end.y += GAME.lander_left_leg_explode_yspeed;
        lander.left_plat_beg.x += GAME.lander_left_leg_explode_xspeed;
        lander.left_plat_beg.y += GAME.lander_left_leg_explode_yspeed;
        lander.left_plat_end.x += GAME.lander_left_leg_explode_xspeed;
        lander.left_plat_end.y += GAME.lander_left_leg_explode_yspeed;

        // right landing leg
        lander.right_leg_beg.x += GAME.lander_right_leg_explode_xspeed;
        lander.right_leg_beg.y += GAME.lander_right_leg_explode_yspeed;
        lander.right_leg_end.x += GAME.lander_right_leg_explode_xspeed;
        lander.right_leg_end.y += GAME.lander_right_leg_explode_yspeed;
        lander.right_plat_beg.x += GAME.lander_right_leg_explode_xspeed;
        lander.right_plat_beg.y += GAME.lander_right_leg_explode_yspeed;
        lander.right_plat_end.x += GAME.lander_right_leg_explode_xspeed;
        lander.right_plat_end.y += GAME.lander_right_leg_explode_yspeed;

        // nozzle
        lander.nozzle_lt.x += GAME.lander_nozzle_explode_xspeed;
        lander.nozzle_lt.y += GAME.lander_nozzle_explode_yspeed;
        lander.nozzle_rt.x += GAME.lander_nozzle_explode_xspeed;
        lander.nozzle_rt.y += GAME.lander_nozzle_explode_yspeed;
        lander.nozzle_lb.x += GAME.lander_nozzle_explode_xspeed;
        lander.nozzle_lb.y += GAME.lander_nozzle_explode_yspeed;
        lander.nozzle_rb.x += GAME.lander_nozzle_explode_xspeed;
        lander.nozzle_rb.y += GAME.lander_nozzle_explode_yspeed;
        
        lander.flame.set(lander.nozzle_lb.x, lander.nozzle_lb.y);
    }
}

void lander_explode(Lander& lander)
{
    lander.explode = true;
    lander.nav_light_wave.reset();
}

void lander_draw(Lander& lander, bool alias)
{   
    // module - radius manually zoomed with SCREEN.zoom
    // if zoom < 1 set radius to 2, else zoom it with factor 1.5 of screen 
    // zoom
    float radius = lander.size;
    if (SCREEN.zoom < 1)
        radius = 2;
    else
        radius *= SCREEN.zoom * 1.5;
    draw_circle(lander.pos, radius, lander.color, alias);

    // base
    draw_line(lander.base_lt, lander.base_rt, lander.color, alias);
    draw_line(lander.base_rt, lander.base_rb, lander.color, alias);
    draw_line(lander.base_rb, lander.base_lb, lander.color, alias);
    draw_line(lander.base_lb, lander.base_lt, lander.color, alias);
    
    // landing legs
    // left
    draw_line(lander.left_leg_beg, 
              lander.left_leg_end, 
              lander.color, alias);
    // right              
    draw_line(lander.right_leg_beg, 
              lander.right_leg_end, 
              lander.color, alias);
    
    // landing platforms
    // left
    draw_line(lander.left_plat_beg, 
              lander.left_leg_end,
              lander.color, alias);
    // right
    draw_line(lander.right_plat_beg, 
              lander.right_leg_end,
              lander.color, alias);
    
    // nozzle
    draw_line(lander.nozzle_lt, lander.nozzle_lb, lander.color, alias);
    draw_line(lander.nozzle_lb, lander.nozzle_rb, lander.color, alias);
    draw_line(lander.nozzle_rb, lander.nozzle_rt, lander.color, alias);
    
    // flame
    draw_line(lander.nozzle_lb, lander.flame, lander.flame_color, alias);
    draw_line(lander.nozzle_rb, lander.flame, lander.flame_color, alias);

    // nav lights
    // No need to draw in zoom-out mode and when lander is exploded
    if (!(SCREEN.zoom < 1) && !lander.explode)
    {
        bool draw_left_light = true;
        bool draw_right_light = true;

        // If landed - draw both, else draw one of the light depends on the 
        // wave function.
        if (COLLISION != LEVEL_LANDED &&
            COLLISION != LEVEL_LANDED_HARD)
        {
            draw_left_light = (lander.nav_light_wave.value() > 0);
            draw_right_light = !draw_left_light;
        }

        // Draw left light
        if (draw_left_light)
        {
            draw_circle(lander.left_nav_light,  // Centre
                        1,                      // Radius
                        Color::RED,             // Color
                        false,                  // Anti-Aliasing
                        true,                   // World Space
                        true);                  // Fill
        }
        
        // Draw right light
        if (draw_right_light)
        {
            draw_circle(lander.right_nav_light, // Centr
                        1,                      // Radiu
                        Color::GREEN,           // Color
                        false,                  // Anti-Aliasing
                        true,                   // World Space
                        true);                  // Fill
        }
    }

    // Draw the bounding box
    if (GAME.game_show_debug_objects)
    {
        draw_line(lander.box_lt, lander.box_rt, Color::RED, alias);
        draw_line(lander.box_rt, lander.box_rb, Color::RED, alias);
        draw_line(lander.box_rb, lander.box_lb, Color::RED, alias);
        draw_line(lander.box_lb, lander.box_lt, Color::RED, alias);
    }
}

void lander_update_physics(Lander& lander, float throttle, float dt)
{ 
    lander.nav_light_wave.update(dt);

    vec_t ge_force(0, GAME.level_gravity);
    vec_t air_friction = vec_scale(lander.vel, -lander.air_friction); 
    vec_t engine_power = vec_scale(lander.up, throttle * lander.engine_power); 
    vec_t acc = vec_sum(ge_force, air_friction);
    acc = vec_sum(acc, engine_power);
    
    lander.pos = vec_sum(lander.pos, vec_scale(lander.vel, dt));
    lander.vel = vec_sum(lander.vel, vec_scale(acc, dt)); 

    // Check speed for limit
    if (lander.vel.x > GAME.game_speed_limit)
        lander.vel.x = GAME.game_speed_limit;
    else 
    if (lander.vel.x < -GAME.game_speed_limit)
        lander.vel.x = -GAME.game_speed_limit;

    if (lander.vel.y > GAME.game_speed_limit)
        lander.vel.y = GAME.game_speed_limit;
    else
    if (lander.vel.y < -GAME.game_speed_limit)
        lander.vel.y = -GAME.game_speed_limit;

    lander.fuel -= throttle * GAME.lander_fuel_speed * dt;
    if (lander.fuel < 0)
        lander.fuel = 0;
}
