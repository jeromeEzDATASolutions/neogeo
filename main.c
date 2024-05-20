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
#define CROM_TILE_START_PONT 43 // 2
#define CROM_TILE_START_HERBE 45 // 32
#define CROM_TILE_START_LANCE1 77 // 2
#define CROM_TILE_START_LANCE2 79 // 2
#define CROM_TILE_START_LANCE3 81 // 2
#define CROM_TILE_START_MAP 1 // 32
#define CROM_TILE_START_TOMBE1 83 // 2
#define CROM_TILE_START_TOMBE2 85 // 2
#define CROM_TILE_START_TOMBE3 87 // 2

// --- States for Arthur
#define ARTHUR_SUR_LE_SOL 0
#define ARTHUR_SUR_ECHELLE 1
#define ARTHUR_SAUTE_VERTICALEMENT 2
#define ARTHUR_SAUTE_HORIZONTALEMENT 3
#define ARTHUR_TOMBE 4
#define ARTHUR_SUR_PLATEFORME 5

// --- Positions for Arthur
#define ARTHUR_DEBOUT 0
#define ARTHUR_CROUCHING 1
#define ARTHUR_ACCROUPI 2

// --- Tuile dure sur lesquelles on peut marcher
#define SOLDUR1 375
#define SOLDUR2 357
#define SOLDUR3 377
#define SOLDUR4 378
#define MURDURLEFT 377
#define MURDURRIGHT 378
#define MURDURLEFTRIGHT 357

// --- Tiles decor
#define TILE_ECHELLE 397
#define TILE_ECHELLE_END 398

/// Start of gradient tiles in BIOS ROM
#define SROM_GRADIENT_TILE_OFFSET 1280

// Fix point logic for slow scrolling
#define FIX_FRACTIONAL_BITS 3
#define FIX_POINT(a,f) ((a<<FIX_FRACTIONAL_BITS)+f)
#define FIX_ACCUM(x) (x>>FIX_FRACTIONAL_BITS)
#define FIX_ZERO (FIX_POINT(1,0)-1)

#define DEBUG 0
#define GAME_SPEED 1
#define GAME_LEVEL_START 10  // 0 map, 10 level 1, 20 level 2

#define GNG_START_ARTHUR_POSISTION_X 144

u16 frames;
int palettes_map[] = {8};
int palettes_background_herbe_eau_nuage[] = {1,2,3,6};
int palette_nuage[] = {6};
int palette_background_herbe_nuage[] = {1,2,6};

static void scroll_left();
static void scroll_right();
static void scroll_top();

// Plane and scrolling state
#include "palette.c"
#include "background.c"
#include "pont.c"
#include "arthur.c"
#include "nuage.c"
#include "lance.c"
#include "map.c" // Intro avant d'afficher le Niveau 1
#include "tombe.c"

u16 gng_niveau = GAME_LEVEL_START;

void pause(u8 nombre_secondes){
    volatile int i;
    for(i = 0; i < (25000*nombre_secondes); i++) {
        // Pause approximative de 1 secondes
    }
}

int main(void) {

    char str[10];

    ng_cls();
    init_palette();

    // --- On conserve dans un coin les infos de Arthur
    arthur_origin.palette = arthur.palette;

    // Backup origin structure for reset
    background_save(&background, &background_origin);
    herbe_save(&herbe, &herbe_origin);
    pont_save(&pont, &pont_origin);

    for(;;) {

        if ( gng_niveau == 0 ){

            // Clear all sprites
            for (u16 s=1; s<380; s++) {
                *REG_VRAMMOD=1;
                *REG_VRAMADDR=ADDR_SCB1+(s*64);
                for (u16 v=0; v<15; v++) {
                    *REG_VRAMRW = CROM_TILE_OFFSET+background.tmx[0][0];
                    *REG_VRAMRW = (1<<8);
                }
            }

            // Display map
            setPaletteToBlack(palettes_map, 1);
            map_setup(&map);
            fadeOutPalette(palettes_map, 1);
            
            // Pause de 1 seconde
            pause(1);

            // Passage au niveau 1
            gng_niveau = 1;
        }
        else if ( gng_niveau == 1 ){

            // Scroll Map
            for(u16 i = 0; i < 90; i++) {
                map_move_left(&map);
                ng_wait_vblank();
            }

            // --- Pause de 2 secondes
            pause(2);

            // Hide map with palette
            setPaletteToBlack(palettes_map, 1);

            // Passage au niveau 10 qui est le premier niveau
            gng_niveau = 10;
        }
        else if ( gng_niveau == 10 ){

            // --- Set palette 6 to black
            //setPaletteToBlack(palette_nuage, 1);

            // --- Set palette 1 and 2 to black
            setPaletteToBlack(palettes_background_herbe_eau_nuage, 4);

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

            // --- Display sprites Lances
            lances_init(lances);

            // --- Display sprites Tombes
            tombes_init(tombes);
            tombe_update(&tombes[0]);

            // --- Display background
            fadeOutPalette(palettes_background_herbe_eau_nuage, 4);

            // --- Display nuages
            fadeOutPalette(palette_nuage, 1);

            gng_niveau = 11;
        }

        if ( gng_niveau == 11 ){

            //snprintf(str, 10, "ArX %4d", arthur.x); ng_text(2, 3, 0, str);
            //snprintf(str, 10, "PtX %4d", pont.x); ng_text(2, 5, 0, str);

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
                //snprintf(str, 10, "POS %4d", arthur.position); ng_text(2, 3, 0, str);
                //snprintf(str, 10, "PX %4d", background.position_x); ng_text(2, 3, 0, str);
                //snprintf(str, 10, "L %4d", bios_p1current); ng_text(2, 3, 0, str);
            }

            lances_gestion(lances);

            // --------------------------------------------------
            // --- ARTHUR BOTTOM
            // --------------------------------------------------
            if (bios_p1current & CNT_LEFT ) {

                arthur.sens = 0;

                if ( arthur.state == ARTHUR_SUR_LE_SOL || arthur.state == ARTHUR_SUR_PLATEFORME){

                    arthur.frames++;

                    if ( b2 ){
                        // Saut vers la gauche
                        arthur_jump_horizontal(&arthur);
                    }
                    else arthur_walk_left(&arthur);

                    /*if ( background.position_x > 0 ){

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
                        // --- Arthur fait du surplace
                        arthur_walk_left(&arthur);
                    }*/
                }
            }
            else if (bios_p1current & CNT_RIGHT && background.position_x < 3260 ) {

                arthur.sens = 1;

                if ( arthur.state == ARTHUR_SUR_LE_SOL || arthur.state == ARTHUR_SUR_PLATEFORME){

                    arthur.frames++;

                    if ( b2 ){
                        // Saut vers la droite
                        arthur_jump_horizontal(&arthur);
                    }
                    else {
                        arthur_walk_right(&arthur);
                    }
                }
            }
            else if (bios_p1current == 6 || bios_p1current == CNT_BOTTOM || bios_p1current == 10){
                u16 retour_descend_echelle = arthur_descend_echelle(&arthur);
            }
            else if ( bios_p1current & CNT_UP ){

                if ( arthur.state == ARTHUR_SUR_LE_SOL || arthur.state == ARTHUR_SUR_ECHELLE ) {

                    u16 arthur_tile1 = 0;
                    u16 arthur_tile2 = 0;

                    if ( arthur.sens == 1 ){
                        arthur_tile1 = tmx_sol[arthur.tiley][((arthur.absolute_bottom_left_x+6)>>4)];
                        arthur_tile2 = tmx_sol[arthur.tiley][((arthur.absolute_bottom_right_x-6)>>4)];
                    }
                    else {
                        arthur_tile1 = tmx_sol[arthur.tiley][((arthur.absolute_bottom_right_x-6)>>4)];
                        arthur_tile2 = tmx_sol[arthur.tiley][((arthur.absolute_bottom_left_x+6)>>4)];
                    }

                    // --- Arthur peut monter à l'échelle : tile TILE_ECHELLE
                    if ( (arthur_tile1 == TILE_ECHELLE && arthur_tile2 == TILE_ECHELLE) || arthur_tile1 == SOLDUR1 ){
                        arthur.y++;
                        arthur.position_y++;
                        arthur.yf = arthur.y*8;
                        arthur.state = ARTHUR_SUR_ECHELLE;
                        arthur.frame_echelle++;
                        arthur_sur_echelle(&arthur); // --- Display sprite Arthur sur echelle
                        arthur_calcule_tiles(&arthur);
                        arthur.frame_echelle_end=0;
                    }
                    else if ( arthur_tile1 == TILE_ECHELLE_END || arthur_tile2 == TILE_ECHELLE_END ){
                        arthur.y++;
                        arthur.position_y++;
                        arthur.yf = arthur.y*8;
                        arthur.state = ARTHUR_SUR_ECHELLE;
                        arthur.frame_echelle++;
                        arthur_sur_echelle_last_etape(&arthur); // --- Display last etape from sprite Arthur sur echelle
                        arthur_calcule_tiles(&arthur);
                    }
                    else {
                        arthur.state = ARTHUR_SUR_LE_SOL;
                        arthur.position=ARTHUR_DEBOUT;
                        arthur_stop_walk(&arthur);
                        arthur.display_y = -2;
                        arthur.frame_echelle_end=0;
                    }

                    /*
                    arthur.y++;
                    arthur.position_y++;
                    arthur_update(&arthur);
                    */
                }
            }
            else if ( b1 ) {
                lance_start(lances, arthur.x, arthur.y);
            }
            else if ( arthur.state == ARTHUR_SUR_LE_SOL ){
                // Position neutre de Arthur
                arthur.position=ARTHUR_DEBOUT;
                arthur_stop_walk(&arthur);
            }

            // ---------------------------------------- //
            // --- Appui sur button A : on tire         //
            // ---------------------------------------- //
            if ( b1 ) {
                lance_start(lances, arthur.x, arthur.y);
            }

            // ---------------------------------------- //
            // --- Appui sur button B : on saute        //
            // ---------------------------------------- //
            if ( b2 && !l && !r ){
                arthur_jump_vertical(&arthur);
            }

            // if arthur mort, on fait disparaitre le niveau dans un fondu avec la palette
            // fadeInPalette(palettes1, 2);

            if ( arthur.state == ARTHUR_SAUTE_VERTICALEMENT || arthur.state == ARTHUR_SAUTE_HORIZONTALEMENT ) {
                arthur_jump_update(&arthur, &pont);
                if ( arthur.state == ARTHUR_SAUTE_HORIZONTALEMENT ){
                    if ( arthur.sens == 1 ){
                        //if ( arthur.tile_right != SOLDUR3 && arthur.tile_right != SOLDUR2 && arthur.tile_right != 358 ){
                            arthur_update_posision_x_right(&arthur);
                            scroll_right();
                        //}
                    }
                    else {
                        if ( arthur.position_x > 144 ){
                            if ( arthur.tile_left != SOLDUR3 && arthur.tile_left != SOLDUR2 && arthur.tile_left != 358 ){
                                arthur_update_posision_x_left(&arthur);
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

            // ------------------------------------------
            // Premier PONT
            // ------------------------------------------
            if ( arthur.position_x == 156 && pont.display == 0 ){
                pont_display(&pont);
            }
            if ( arthur.sens == 1 && arthur.position_x == (156+374) ){
                pont_reset_and_hide(&pont);
            }
            else if ( arthur.sens == 0 && arthur.position_x == (156+254) ){
                if ( pont.display == 0 ){
                    pont.x=-30;
                    pont.sens=1;
                    pont.start_x=-44;
                    pont.end_x=74;
                    pont_display_left(&pont);
                }
            }

            // ------------------------------------------
            // Deuxieme PONT
            // ------------------------------------------
            if ( arthur.position_x == 1484 && pont.display == 0 ){
                pont_display(&pont);
            }
            if ( arthur.sens == 1 && arthur.position_x == (1484+374) ){
                pont_reset_and_hide(&pont);
            }
            else if ( arthur.sens == 0 && arthur.position_x == (1484+254) ){
                pont.x=-30;
                pont.sens=1;
                pont.start_x=-44;
                pont.end_x=74;
                pont_display_left(&pont);
            }

            // ------------------------------------------
            // Gestion du PONT
            // ------------------------------------------
            pont_move(&pont);

            if ( arthur.state == ARTHUR_SUR_PLATEFORME ){
                // Arthur est sur une plateforme, il est donc porté par la plateforme
                if ( !l && !r ){
                    arthur_stop_walk(&arthur);
                }
                if ( pont.sens == 1 ){
                    arthur_update_posision_x_right(&arthur);
                    scroll_right();
                }
                else if ( pont.sens == 0 ){
                    arthur_update_posision_x_left(&arthur);
                    scroll_left();
                }
            }

            // On checke la position Y de Arthur pour arreter le jeu
            if ( arthur.y < -30 ){

                pause(2);

                gng_niveau = GAME_LEVEL_START;

                map.x = 30;

                // --- Reset lances
                lances[0].y=260;
                lances[1].y=260;
                lances[2].y=260;
                lances_init(lances);

                // --- Reset all
                arthur_reset(&arthur, &arthur_origin);
                background_reset(&background, &background_origin);
                herbe_reset(&herbe, &herbe_origin);
                pont_reset_and_hide(&pont);
                nuage.x = 60;
                nuage.y = 140;
            }

            //snprintf(str, 10, "POSY %3d", arthur.position_y); ng_text(2, 9, 0, str);
        }

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
        pont_scroll_left(&pont);
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
        pont_scroll_right(&pont);
    }
}