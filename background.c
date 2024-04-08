/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the background
 */

// --- Get background from Tiled
#include "tile_layers.c"

typedef struct _plane_t {
    u16 sprite;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
    u16 increment;
    u16 tmx[15][224];
    u16 position_x;
    u16 position_x_cpt1;
    u16 position_x_cpt2;
    u16 position_sprite;
    u16 position_tile_right;
    u16 position_tile_left;
} plane_t;

plane_t background = {
    .sprite = CROM_TILE_START_BACKGROUND,
    .palette = 1,
    .width = 32, // 32 * 16 = 512 pixels
    .height = 15,
    .x = 0,
    .y = 0,
    .increment = FIX_POINT(0,1),  // 1.0
    .tmx = {},
    .position_x = 0,
    .position_x_cpt1 = 0,
    .position_x_cpt2 = 0,
    .position_sprite = 0,
    .position_tile_right = 0,
    .position_tile_left = 0,
};

plane_t herbe = {
    .sprite = CROM_TILE_START_HERBE,
    .palette = 2,
    .width = 32, // 32 * 16 = 512 pixels
    .height = 15,
    .x = 0,
    .y = 0,
    .increment = FIX_POINT(0,1),  // 1.0
    .tmx = {},
    .position_x = 0,
    .position_x_cpt1 = 0,
    .position_x_cpt2 = 0,
    .position_sprite = 0,
    .position_tile_right = 0,
    .position_tile_left = 0, 
};
//.offset = FIX_POINT(16,0),

void setup_plane(plane_t *plane) {

    u16 tile_tmp;

    // The plane graphics are stored in the ROM as a sequence of
    // horizontal tiles. A sprite is a vertical sequence of tiles, so
    // its tiles are `plane->width` apart in the ROM
    for (u16 s=0; s<plane->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((plane->sprite+s)*64);
        for (u16 v=0; v<plane->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+plane->tmx[v][s]-1;
            *REG_VRAMRW = (plane->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    // sprite shape: position , max zoom
    *REG_VRAMADDR=ADDR_SCB2+plane->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(plane->y<<7)+plane->height;
    *REG_VRAMRW=(plane->x<<7);

    // --- On chaine l'ensemble des sprites
    for (u16 v=1; v<plane->width; v++) {
        *REG_VRAMADDR=ADDR_SCB2+plane->sprite+v;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6; // sticky
    }
}

void init_plane_tmx_background(plane_t *plane){
    for(u16 j = 0; j < 15; j++) {
        for(u16 i = 0; i < 224; i++) {
            //plane->tmx[j][i] = tmx_background[j][i];
            plane->tmx[j][i] = tmx_sol[j][i];
        }
    }
}

void init_plane_tmx_herbe(plane_t *plane){
    for(u16 j = 0; j < 15; j++) {
        for(u16 i = 0; i < 224; i++) {
            plane->tmx[j][i] = tmx_herbe[j][i];
        }
    }
}

void update_plane_left(plane_t *plane) {

    // Update tiles
    *REG_VRAMMOD=1;
    *REG_VRAMADDR=ADDR_SCB1+((plane->sprite+background.position_sprite)*64);
    for (u16 v=0; v<plane->height; v++) {
        *REG_VRAMRW = CROM_TILE_OFFSET+plane->tmx[v][background.position_tile_left-1]-1;
        if ( plane->tmx[v][background.position_tile_left-1]<401){
            *REG_VRAMRW = (1<<8);
        }
        else if ( plane->tmx[v][background.position_tile_left-1]<417){
            *REG_VRAMRW = (2<<8);
        }
        else if ( plane->tmx[v][background.position_tile_left-1]<427){
            *REG_VRAMRW = (3<<8);
        }
        else {
            *REG_VRAMRW = (4<<8);
        }
    }

    *REG_VRAMMOD=1;
    *REG_VRAMADDR=ADDR_SCB4+plane->sprite;
    *REG_VRAMRW=plane->x<<7;
}

void update_plane_right(plane_t *plane) {

    // Update tiles
    *REG_VRAMMOD=1;
    *REG_VRAMADDR=ADDR_SCB1+((plane->sprite+background.position_sprite)*64);
    for (u16 v=0; v<plane->height; v++) {
        *REG_VRAMRW = CROM_TILE_OFFSET+plane->tmx[v][background.position_tile_right-1]-1;
        if ( plane->tmx[v][background.position_tile_right-1]<401){
            *REG_VRAMRW = (1<<8);
        }
        else if ( plane->tmx[v][background.position_tile_right-1]<417){
            *REG_VRAMRW = (2<<8);
        }
        else if ( plane->tmx[v][background.position_tile_right-1]<427){
            *REG_VRAMRW = (3<<8);
        }
        else {
            *REG_VRAMRW = (4<<8);
        }
    }

    *REG_VRAMMOD=1;
    *REG_VRAMADDR=ADDR_SCB4+plane->sprite;
    *REG_VRAMRW=plane->x<<7;
}

void set_sprite_and_tile(plane_t *plane){
    u16 tmp_position_x = plane->position_x>>4;
    plane->position_sprite = tmp_position_x+31-(((tmp_position_x+30)>>5)*32)-1;
    plane->position_tile_right = (plane->position_x>>4)+31;
    plane->position_tile_left = (plane->position_x>>4)-1;
}

void move_plane_right(plane_t *plane){
    plane->x-=plane->increment;
    plane->position_x+=plane->increment;
    plane->position_x_cpt1++;
    if ( plane->position_x_cpt1 == 16 ){
        plane->position_x_cpt1 = 0;
        set_sprite_and_tile(plane);
    }
}

void move_plane_left(plane_t *plane){
    plane->x+=plane->increment;
    plane->position_x-=plane->increment;
    plane->position_x_cpt2++;
    if ( plane->position_x_cpt2 == 16 ){
        plane->position_x_cpt2 = 0;
        set_sprite_and_tile(plane);
    }
}

void move_planes_right(){
    move_plane_right(&background);
    move_plane_right(&herbe);
    update_plane_right(&background);
    update_plane_right(&herbe);
}

void move_planes_left(){
    move_plane_left(&background);
    move_plane_left(&herbe);
    update_plane_left(&background);
    update_plane_left(&herbe);
}
