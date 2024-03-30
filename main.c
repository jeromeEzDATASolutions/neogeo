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
#include "background.c"
#include "arthur.c"

const u16 clut[][16]= {
    /// first 16 colors palette for the fix tiles
    {0x8000, 0x0fff, 0x0333, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    // --- sprite palettes
    #include "sprites/back.pal"
    #include "sprites/herbe.pal"
    #include "sprites/flotte.pal"
    #include "sprites/arthur1.pal"
    #include "sprites/arthur2.pal"
};

const u16 palette_backup[][16]= {
    /// first 16 colors palette for the fix tiles
    {0x8000, 0x0fff, 0x0333, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    // --- sprite palettes
    #include "sprites/back.pal"
    #include "sprites/herbe.pal"
    #include "sprites/flotte.pal"
    #include "sprites/arthur1.pal"
    #include "sprites/arthur2.pal"
};

void init_palette() {
    // --- Initialize the two palettes in the first palette bank
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
    //*((volatile u16*)0x401ffe)=0x18bf;
    *((volatile u16*)0x0000)=0x0000;
}

void setPaletteToBlack(int palettes[], int nb_elem) {
    ng_wait_vblank();
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 c=16*palettes[p], l=0; l<16; c++, l++) {
            MMAP_PALBANK1[c] = 0x0000;
        }
    }
}

void fadeOutPalette(int palettes[], int nb_elem) {

    char str[20];
    u8 end = 0;
    u16 palette = 0;
    u16 colors_target[nb_elem][16][3];
    u16 colors_ok[nb_elem][16];
    u16 caching[16];

    // --- Init color OK
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 i=0; i<16; i++) {
            colors_ok[p][i] = 0;
        }
    }

    // --- Caching all colors from MMAP_PALBANK1
    u16 colors_palbank[nb_elem][16][3];
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 c=16*palettes[p], l=0; l<16; c++, l++) {
            colors_palbank[p][l][0] = (MMAP_PALBANK1[c] >> 8) & 0xF;
            colors_palbank[p][l][1] = (MMAP_PALBANK1[c] >> 4) & 0xF;
            colors_palbank[p][l][2] = MMAP_PALBANK1[c] & 0xF;
        }
    }

    // --- Caching colors destination
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 i=0; i<16; i++) {
            colors_target[p][i][0] = (palette_backup[palettes[p]][i] >> 8) & 0xF;
            colors_target[p][i][1] = (palette_backup[palettes[p]][i] >> 4) & 0xF;
            colors_target[p][i][2] = palette_backup[palettes[p]][i] & 0xF;
        }
    }

    while ( end == 0 ) {

        for (u16 p=0; p<nb_elem; p++) {

            for (u16 c=16*palettes[p], l=0; l < 16; c++, l++) {

                if ( colors_palbank[p][l][0] < colors_target[p][l][0] ) {
                    colors_palbank[p][l][0]++;
                }

                if ( colors_palbank[p][l][1] < colors_target[p][l][1] ) {
                    colors_palbank[p][l][1]++;
                }

                if ( colors_palbank[p][l][2] < colors_target[p][l][2] ) {
                    colors_palbank[p][l][2]++;
                }

                caching[l] = (colors_palbank[p][l][0] << 8) | (colors_palbank[p][l][1] << 4) | colors_palbank[p][l][2];

                if ( colors_palbank[p][l][0] >= colors_target[p][l][0] && colors_palbank[p][l][1] >= colors_target[p][l][1] && colors_palbank[p][l][2] >= colors_target[p][l][2] ){
                    colors_ok[p][l] = 1;
                }
            }

            ng_wait_vblank();

            for (u16 c=16*palettes[p], l=0; l < 16; c++, l++) {
                MMAP_PALBANK1[c] = caching[l];
            }
        }

        // --- On checke que tout le tab est rempli afin d'arreter le fondu
        end = 1;
        for (u16 p=0; p<nb_elem; p++) {
            for (u16 i=0; i<16; i++) {
                if ( colors_ok[p][i] == 0 ) {
                    end=0;
                }
            }
        }
    }
}

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
            frames = 0;
        }

        if ( DEBUG ){
            //snprintf(str, 10, "PX %4d", background.position_x); ng_text(2, 3, 0, str);
            //snprintf(str, 10, "SP %4d", background.position_sprite); ng_text(2, 5, 0, str);
            //snprintf(str, 10, "PA %x", MMAP_PALBANK1[17]); ng_text(2, 7, 0, str);
            //snprintf(str, 10, "f %4d", frames); ng_text(2, 7, 0, str);
        }

        if (bios_p1current & CNT_LEFT ) {

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

            // Move background
            for ( u16 i=0;i<GAME_SPEED;i++){
                move_planes_right();
            }

            // Moves Arthur on right
            arthur.position_x++;
            arthur_walk_right(&arthur);

            if ( 1==2 ) {
                setPaletteToBlack(palettes1, 2);
                fadeOutPalette(palettes1, 2);
            }
        }
        else {
            // Position neutre de Arthur
            arthur_stop_walk(&arthur);
        }

        ng_wait_vblank();
    }

    return 0;
}