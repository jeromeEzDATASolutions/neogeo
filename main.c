/*
 * Copyright (c) 2020 Damien Ciabrini
 * This file is part of ngdevkit
 *
 * ngdevkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * ngdevkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Additional resources for sprites
 * https://wiki.neogeodev.org/index.php?title=Sprites
 */
#include <ngdevkit/neogeo.h>
#include <ngdevkit/ng-fix.h>
#include <ngdevkit/ng-video.h>
#include <stdio.h>

// Current state of player 1's controller
extern u8 bios_p1current;

/// Start of sprite tiles
#define CROM_TILE_OFFSET 256

/// Start of gradient tiles in BIOS ROM
#define SROM_GRADIENT_TILE_OFFSET 1280

// Fix point logic for slow scrolling
#define FIX_FRACTIONAL_BITS 3
#define FIX_POINT(a,f) ((a<<FIX_FRACTIONAL_BITS)+f)
#define FIX_ACCUM(x) (x>>FIX_FRACTIONAL_BITS)
#define FIX_ZERO (FIX_POINT(1,0)-1)

#define DEBUG 1
#define GAME_SPEED 1

u16 frames;

// Plane and scrolling state
#include "palette.c"
#include "background.c"
#include "arthur.c"

int main(void) {

    char str[10];

    ng_cls();
    init_palette();

    // --- Set palette 1 and 2 to black
    int palettes1[] = {1,2};
    setPaletteToBlack(palettes1, 2);

    // --- Load tmx from background
    init_plane_tmx_background(&background);
    init_plane_tmx_herbe(&herbe);
    
    // --- Display background (terre & herbe)
    setup_plane(&background);
    setup_plane(&herbe);

    // --- Calcul
    set_sprite_and_tile(&background);
    set_sprite_and_tile(&herbe);

    // --- Display sprite Arthur
    arthur_setup(&arthur);

    // --- Display background
    fadeOutPalette(palettes1, 2);

    for(;;) {

        frames++;

        if ( frames == 1000 ){
            frames=0;
        }

        if ( DEBUG ){
            //snprintf(str, 10, "PX %4d", background.position_x); ng_text(2, 3, 0, str);
            //snprintf(str, 10, "SP %4d", background.position_sprite); ng_text(2, 5, 0, str);
            //snprintf(str, 10, "PA %x", MMAP_PALBANK1[17]); ng_text(2, 7, 0, str);
            //snprintf(str, 10, "f %4d", frames); ng_text(2, 7, 0, str);
        }

        if (bios_p1current & CNT_LEFT ) {

            arthur.frames++;

            if ( background.position_x > 0 ){

                for ( u16 i=0;i<GAME_SPEED;i++){
                    move_planes_left();
                }

                // Moves Arthur on left
                arthur.position_x--;
                arthur_walk_left(&arthur);
            }
            else {
                arthur_walk_left(&arthur);
            }
        }
        else if (bios_p1current & CNT_RIGHT && background.position_x < 3260 ) {

            arthur.frames++;

            // Move background
            for ( u16 i=0;i<GAME_SPEED;i++){
                move_planes_right();
            }

            // Moves Arthur on right
            arthur.position_x++;
            arthur_walk_right(&arthur);
        }
        else {
            // Position neutre de Arthur
            arthur_stop_walk(&arthur);
        }

        ng_wait_vblank();
    }

    return 0;
}