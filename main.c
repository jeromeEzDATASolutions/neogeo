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

// --- Define TILE_START foreach sprite group
#define CROM_TILE_START_NUAGE 1 // 8
#define CROM_TILE_START_BACKGROUND 9 // 32
#define CROM_TILE_START_ARTHUR 41 // 2
#define CROM_TILE_START_HERBE 43 // 32

// --- States for Arthur
#define ARTHUR_SUR_LE_SOL 0
#define ARTHUR_SUR_ECHELLE 1
#define ARTHUR_SAUTE 2

// --- Positions for Arthur
#define ARTHUR_DEBOUT 0
#define ARTHUR_CROUCHING 1

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
#include "nuage.c"

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

    // --- Display nuage
    nuage_setup(&nuage);

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
            snprintf(str, 10, "L %4d", bios_p1current); ng_text(2, 3, 0, str);
        }

        if (bios_p1current == 6 || bios_p1current == 2 || bios_p1current == 10 ){
            // --- Arthur se baisse
            arthur.position=ARTHUR_CROUCHING;
            arthur_accroupi(&arthur);
        }
        else if (bios_p1current & CNT_LEFT ) {

            arthur.frames++;

            if ( background.position_x > 0 ){

                for ( u16 i=0;i<GAME_SPEED;i++){
                    move_planes_left();
                    nuage_move_left(&nuage);
                    if ( background.position_x == 200 ){
                        // Display nuage
                        nuage.height=GNG_NUAGE_TMX_HEIGHT;
                        nuage_setup(&nuage);
                    }
                }

                // Moves Arthur on left
                arthur.position=ARTHUR_DEBOUT;
                arthur.position_x--;
                arthur_walk_left(&arthur);
            }
            else {
                // --- Arthur fait dur surplace
                arthur_walk_left(&arthur);
            }
        }
        else if (bios_p1current & CNT_RIGHT && background.position_x < 3260 ) {

            arthur.frames++;

            // Move all
            for ( u16 i=0;i<GAME_SPEED;i++){
                move_planes_right();
                nuage_move_right(&nuage);
                if ( background.position_x == 200 ){
                    // Hide nuage
                    nuage_hide(&nuage);
                }
            }

            // Moves Arthur on right
            arthur.position=ARTHUR_DEBOUT;
            arthur.position_x++;
            arthur_walk_right(&arthur);
        }
        else if ( bios_p1current & CNT_UP ){
            
        }
        else {
            // Position neutre de Arthur
            arthur.position=ARTHUR_DEBOUT;
            arthur_stop_walk(&arthur);
        }

        // if arthur mort, on fait disparaitre le niveau dans un fondu avec la palette
        // fadeInPalette(palettes1, 2);

        ng_wait_vblank();
    }

    return 0;
}