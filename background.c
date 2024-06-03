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
    .increment = 1,  // FIX_POINT(0,1)
    .tmx = {},
    .position_x = 0,
    .position_x_cpt1 = 0,
    .position_x_cpt2 = 0,
    .position_sprite = 0,
    .position_tile_right = 0,
    .position_tile_left = 0,
};

plane_t background_origin = {
    .sprite = 0,
    .palette = 0,
    .width = 0, 
    .height = 0,
    .x = 0,
    .y = 0,
    .increment = 0, 
    .tmx = {},
    .position_x = 0,
    .position_x_cpt1 = 0,
    .position_x_cpt2 = 0,
    .position_sprite = 0,
    .position_tile_right = 0,
    .position_tile_left = 0,
};

void background_save(plane_t *background, plane_t *background_origin){
    background_origin->sprite = background->sprite;
    background_origin->palette = background->palette;
    background_origin->width = background->width;
    background_origin->height = background->height;
    background_origin->x = background->x;
    background_origin->y = background->y;
    background_origin->increment = background->increment;
    background_origin->position_x = background->position_x;
    background_origin->position_x_cpt1 = background->position_x_cpt1;
    background_origin->position_x_cpt2 = background->position_x_cpt2;
    background_origin->position_sprite = background->position_sprite;
    background_origin->position_tile_right = background->position_tile_right;
    background_origin->position_tile_left = background->position_tile_left;
}

void background_reset(plane_t *background, plane_t *background_origin){
    background->sprite = background_origin->sprite;
    background->palette = background_origin->palette;
    background->width = background_origin->width;
    background->height = background_origin->height;
    background->x = background_origin->x;
    background->y = background_origin->y;
    background->increment = background_origin->increment;
    background->position_x = background_origin->position_x;
    background->position_x_cpt1 = background_origin->position_x_cpt1;
    background->position_x_cpt2 = background_origin->position_x_cpt2;
    background->position_sprite = background_origin->position_sprite;
    background->position_tile_right = background_origin->position_tile_right;
    background->position_tile_left = background_origin->position_tile_left;
}

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

plane_t herbe_origin= {
    .sprite = 0, 
    .palette = 0,
    .width = 0,
    .height = 0,
    .x = 0,
    .y = 0,
    .increment = 0, 
    .tmx = {},
    .position_x = 0,
    .position_x_cpt1 = 0,
    .position_x_cpt2 = 0,
    .position_sprite = 0,
    .position_tile_right = 0,
    .position_tile_left = 0, 
};

void herbe_save(plane_t *herbe, plane_t *herbe_origin){
    herbe_origin->sprite = herbe->sprite;
    herbe_origin->palette = herbe->palette;
    herbe_origin->width = herbe->width;
    herbe_origin->height = herbe->height;
    herbe_origin->x = herbe->x;
    herbe_origin->y = herbe->y;
    herbe_origin->increment = herbe->increment;
    herbe_origin->position_x = herbe->position_x;
    herbe_origin->position_x_cpt1 = herbe->position_x_cpt1;
    herbe_origin->position_x_cpt2 = herbe->position_x_cpt2;
    herbe_origin->position_sprite = herbe->position_sprite;
    herbe_origin->position_tile_right = herbe->position_tile_right;
    herbe_origin->position_tile_left = herbe->position_tile_left;
}


void herbe_reset(plane_t *herbe, plane_t *herbe_origin){
    herbe->sprite = herbe_origin->sprite;
    herbe->palette = herbe_origin->palette;
    herbe->width = herbe_origin->width;
    herbe->height = herbe_origin->height;
    herbe->x = herbe_origin->x;
    herbe->y = herbe_origin->y;
    herbe->increment = herbe_origin->increment;
    herbe->position_x = herbe_origin->position_x;
    herbe->position_x_cpt1 = herbe_origin->position_x_cpt1;
    herbe->position_x_cpt2 = herbe_origin->position_x_cpt2;
    herbe->position_sprite = herbe_origin->position_sprite;
    herbe->position_tile_right = herbe_origin->position_tile_right;
    herbe->position_tile_left = herbe_origin->position_tile_left;
}

void setup_plane(plane_t *plane) {

    u16 tile_tmp;
    u16 palette_tmp = 1;

    // The plane graphics are stored in the ROM as a sequence of
    // horizontal tiles. A sprite is a vertical sequence of tiles, so
    // its tiles are `plane->width` apart in the ROM
    for (u16 s=0; s<plane->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((plane->sprite+s)*64);
        for (u16 v=0; v<plane->height; v++) {

            // Recherche de la bonne palette
            for(u16 k=0;k<sizeof(palettes_offset);k++){
                if ( plane->tmx[v][s] < palettes_offset[k][0]){
                    palette_tmp = palettes_offset[k][1];
                    break;
                }
            }

            *REG_VRAMRW = CROM_TILE_OFFSET+plane->tmx[v][s]-1;
            //*REG_VRAMRW = (plane->palette<<8);
            *REG_VRAMRW = (palette_tmp<<8);
        }
    }

    *REG_VRAMMOD=0x200;
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
            plane->tmx[j][i] = tmx_background[j][i];
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
    //plane->position_sprite = tmp_position_x+31-(((tmp_position_x+30)>>5)*32)-1;
    //plane->position_sprite = tmp_position_x+31-(((tmp_position_x+30)>>5)<<5)-1;
    plane->position_sprite = tmp_position_x+31-(((tmp_position_x+30)>>5)<<5)-1;
    plane->position_tile_right = (plane->position_x>>4)+31;
    plane->position_tile_left = (plane->position_x>>4)-1;
}

void move_plane_right(plane_t *plane){
    //plane->x-=plane->increment;
    //plane->position_x+=plane->increment;
    plane->x--;
    plane->position_x++;
    /*plane->position_x_cpt1++;
    if ( plane->position_x_cpt1 == 16 ){
        plane->position_x_cpt1 = 0;
        set_sprite_and_tile(plane);
    }*/
    set_sprite_and_tile(plane);
    update_plane_right(plane);
}

void move_plane_left(plane_t *plane){
    //plane->x+=plane->increment;
    plane->x++;
    //plane->position_x-=plane->increment;
    plane->position_x--;
    /*plane->position_x_cpt2++;
    if ( plane->position_x_cpt2 == 16 ){
        plane->position_x_cpt2 = 0;
        set_sprite_and_tile(plane);
    }*/
    set_sprite_and_tile(plane);
    update_plane_left(plane);
}

void move_planes_right(){
    move_plane_right(&background);
    move_plane_right(&herbe);
}

void move_planes_left(){
    move_plane_left(&background);
    move_plane_left(&herbe);
}
