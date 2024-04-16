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

#define CNT_BOTTOM 2

/// Start of sprite tiles
#define CROM_TILE_OFFSET 256

// --- Define TILE_START foreach sprite group
#define CROM_TILE_START_NUAGE 1 // 8
#define CROM_TILE_START_BACKGROUND 9 // 32
#define CROM_TILE_START_ARTHUR 41 // 2
#define CROM_TILE_START_HERBE 43 // 32
#define CROM_TILE_START_TONYO 75 // 7

// --- States for Arthur
#define ARTHUR_SUR_LE_SOL 0
#define ARTHUR_SUR_ECHELLE 1
#define ARTHUR_SAUTE_VERTICALEMENT 2
#define ARTHUR_SAUTE_HORIZONTALEMENT 3
#define ARTHUR_TOMBE 4

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
int palettes_background_herbe[] = {1,2};
int palette_nuage[] = {6};
int palette_background_herbe_nuage[] = {1,2,6};

// Plane and scrolling state
#include "palette.c"
#include "background.c"
#include "arthur.c"
#include "nuage.c"

static void scroll_left();
static void scroll_right();

int main(void) {

    char str[10];

    ng_cls();
    init_palette();

    // --- Set palette 6 to black
    //setPaletteToBlack(palette_nuage, 1);

    // --- Set palette 1 and 2 to black
    setPaletteToBlack(palettes_background_herbe, 2);

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

    // --- Display nuages
    fadeOutPalette(palette_nuage, 1);

    // --- Display background
    fadeOutPalette(palettes_background_herbe, 2);

    for(;;) {

        frames++;

        if ( frames == 1000 ){
            frames=0;
        }

        u8 u=(bios_p1current & CNT_UP);
        u8 d=(bios_p1current & CNT_DOWN);
        u8 l=(bios_p1current & CNT_LEFT);
        u8 r=(bios_p1current & CNT_RIGHT);
        u8 b1=(bios_p1current & 16);
        u8 b2=(bios_p1current & 32);

        if ( DEBUG ){
            //snprintf(str, 10, "PX %4d", background.position_x); ng_text(2, 3, 0, str);
            //snprintf(str, 10, "L %4d", bios_p1current); ng_text(2, 3, 0, str);
        }

       if (bios_p1current == 6 || bios_p1current == CNT_BOTTOM || bios_p1current == 10 ){
            // --- Arthur se baisse
            arthur_accroupi(&arthur);
        }
        else if (bios_p1current & CNT_LEFT ) {

            arthur.sens = 0;

            if ( arthur.state == ARTHUR_SUR_LE_SOL ){

                arthur.frames++;

                if ( background.position_x > 0 ){

                    if ( b2 ){
                        // Saut vers la gauche
                        arthur_jump_horizontal(&arthur);
                    }
                    else {
                        // Moves Arthur on left
                        if ( arthur_walk_left(&arthur) ){
                            scroll_left();
                        }
                    }
                }
                else {
                    // --- Arthur fait dur surplace
                    arthur_walk_left(&arthur);
                }
            }
        }
        else if (bios_p1current & CNT_RIGHT && background.position_x < 3260 ) {

            arthur.sens = 1;

            if ( arthur.state == ARTHUR_SUR_LE_SOL ){

                arthur.frames++;

                if ( b2 ){
                    // Saut vers la droite
                    arthur_jump_horizontal(&arthur);
                }
                else {
                    // Moves Arthur on right            
                    if ( arthur_walk_right(&arthur) ) {
                        scroll_right();
                    }
                }
            }
        }
        else if ( bios_p1current & CNT_UP ){
            /*arthur.y++;
            arthur.position_y++;
            arthur_update(&arthur);
            arthur_calcule_tiles(&arthur);*/
        }
        else {
            // Position neutre de Arthur
            arthur.position=ARTHUR_DEBOUT;
            arthur_stop_walk(&arthur);
        }

        // ---------------------------------------- //
        // --- Appui sur button B : on saute        //
        // ---------------------------------------- //
        if ( b2 && !l && !r ){
            arthur_jump_vertical(&arthur);
        }

        arthur_check_si_dans_le_vide(&arthur);

        // if arthur mort, on fait disparaitre le niveau dans un fondu avec la palette
        // fadeInPalette(palettes1, 2);

        if ( arthur.state == ARTHUR_SAUTE_VERTICALEMENT || arthur.state == ARTHUR_SAUTE_HORIZONTALEMENT ) {
            arthur_jump_update(&arthur);
            if ( arthur.state == ARTHUR_SAUTE_HORIZONTALEMENT ){
                if ( arthur.sens == 1 ){
                    if ( arthur.tile_right != 377 && arthur.tile_right != 357 && arthur.tile_right != 358 ){
                        arthur.position_x++;
                        scroll_right();
                    }
                }
                else {
                    if ( arthur.position_x > 144 ){
                        if ( arthur.tile_left != 377 && arthur.tile_left != 357 && arthur.tile_left != 358 ){
                            arthur.position_x--;
                            scroll_left();
                        }
                    }
                }
            }
            // arthur_walk_right(&arthur);
        }
        else if ( arthur.state == ARTHUR_TOMBE ) {
            arthur_tombe_update(&arthur);
        }

        snprintf(str, 30, "TX %3d", tmx_sol[arthur.tiley][arthur.tilex]); ng_text(2, 3, 0, str);
        //snprintf(str, 30, "DOWN %3d", arthur.saut_down); ng_text(2, 5, 0, str);
        //snprintf(str, 30, "POSY %3d", arthur.position_y); ng_text(2, 7, 0, str);
        //snprintf(str, 30, "ART %5d", tmx_sol[arthur.tiley+1][arthur.tilex]); ng_text(2, 3, 0, str);
        //snprintf(str, 30, "POSX %3d", arthur.position_x); ng_text(2, 5, 0, str);

        ng_wait_vblank();
    }

    return 0;
}

void scroll_left(){
    for ( u16 i=0;i<GAME_SPEED;i++){
        move_planes_left();
        nuage_move_left(&nuage);
        if ( background.position_x == 200 ){
            // Display nuage
            nuage.height=GNG_NUAGE_TMX_HEIGHT;
            nuage_setup(&nuage);
        }
    }
}

void scroll_right(){
    for ( u16 i=0;i<GAME_SPEED;i++){
        move_planes_right();
        nuage_move_right(&nuage);
        if ( background.position_x == 200 ){
            // Hide nuage
            nuage_hide(&nuage);
        }
    }
}