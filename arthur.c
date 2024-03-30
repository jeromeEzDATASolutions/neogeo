/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the background
 */

#define GNG_ARTHUR_TMX_WIDTH 24
#define GNG_ARTHUR_TMX_HEIGHT 4

typedef struct _arthur_t {
    u16 sprite;
    u16 tile_offset_x;
    u16 tile_offset_y;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
    u16 tmx[GNG_ARTHUR_TMX_HEIGHT][GNG_ARTHUR_TMX_WIDTH];
    u16 position_x;
    u16 sens; // 0 left or 1 right
} arthur_t;

arthur_t arthur = {
    .sprite = 33,
    .tile_offset_x = 0, // Décalage si besoin de la première colonne pour Arthur
    .tile_offset_y = 0, // Arthur à gauche : ligne 2
    .palette = 4, 
    .width = 2, // 32 * 16 = 512 pixels
    .height = 2,
    .x = 144,
    .y = 31,
    .tmx = {},
    .position_x = 144,
    .sens = 1,
};

void arthur_init_tmx(arthur_t *arthur){
    for(u16 j = 0; j < GNG_ARTHUR_TMX_HEIGHT; j++) {
        for(u16 i = 0; i < GNG_ARTHUR_TMX_WIDTH; i++) {
            arthur->tmx[j][i] = tmx_arthur1[j][i];
        }
    }
}

void arthur_setup(arthur_t *arthur) {

    char str[10];
    arthur_init_tmx(arthur);

    for (u16 s=0; s<arthur->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((arthur->sprite+s)*64);
        for (u16 v=0; v<arthur->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+arthur->tmx[v+arthur->tile_offset_y][s+arthur->tile_offset_x]-1;
            *REG_VRAMRW = (arthur->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    // sprite shape: position , max zoom
    *REG_VRAMADDR=ADDR_SCB2+arthur->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(arthur->y-208<<7)+arthur->height;
    *REG_VRAMRW=(arthur->x<<7);

    // --- On chaine l'ensemble des sprites
    for (u16 v=1; v<arthur->width; v++) {
        *REG_VRAMADDR=ADDR_SCB2+arthur->sprite+v;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6; // sticky
    }
}

void arthur_update(arthur_t *arthur){
    
    for (u16 s=0; s<arthur->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((arthur->sprite+s)*64);
        for (u16 v=0; v<arthur->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+arthur->tmx[v+arthur->tile_offset_y][s+arthur->tile_offset_x]-1;
            *REG_VRAMRW = (arthur->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+arthur->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(arthur->y-208<<7)+arthur->height;
    *REG_VRAMRW=(arthur->x<<7);
}

void arthur_walk_left(arthur_t *arthur){

    arthur->sens=0;
    arthur->tile_offset_y=2;

    if ( frames % 4 == 0 || arthur->tile_offset_x == 0 ) {
        arthur->tile_offset_x+=2;
        if ( arthur->tile_offset_x == 18 ){
            arthur->tile_offset_x=2;
        }
        arthur_update(arthur);
    }
}

void arthur_walk_right(arthur_t *arthur){

    arthur->sens=1;
    arthur->tile_offset_y=0;

    if ( frames % 4 == 0 || arthur->tile_offset_x == 0 ) {
        arthur->tile_offset_x+=2;
        if ( arthur->tile_offset_x == 18 ){
            arthur->tile_offset_x=2;
        }
        arthur_update(arthur);
    }
}

void arthur_stop_walk(arthur_t *arthur){
    
    if ( arthur->sens == 1 )
        arthur->tile_offset_y=0;
    else arthur->tile_offset_y=2;

    arthur->tile_offset_x=0;
    
    arthur_update(arthur);
}