/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the background
 */

#define GNG_NUAGE_TMX_WIDTH 8
#define GNG_NUAGE_TMX_HEIGHT 3

typedef struct _nuage_t {
    u16 sprite;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
    u16 tmx[GNG_NUAGE_TMX_HEIGHT][GNG_NUAGE_TMX_WIDTH];
} nuage_t;

nuage_t nuage = {
    .sprite = CROM_TILE_START_NUAGE,
    .palette = 6, 
    .width = GNG_NUAGE_TMX_WIDTH,
    .height = GNG_NUAGE_TMX_HEIGHT,
    .x = 60,
    .y = 140,
    .tmx = {},
};

void nuage_init_tmx(nuage_t *nuage){
    for(u16 j = 0; j < GNG_NUAGE_TMX_HEIGHT; j++) {
        for(u16 i = 0; i < GNG_NUAGE_TMX_WIDTH; i++) {
            nuage->tmx[j][i] = tmx_nuage[j][i];
        }
    }
}

void nuage_setup(nuage_t *nuage) {

    nuage_init_tmx(nuage);

    for (u16 s=0; s<nuage->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((nuage->sprite+s)*64);
        for (u16 v=0; v<nuage->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+nuage->tmx[v][s]-1;
            *REG_VRAMRW = (nuage->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    // sprite shape: position , max zoom
    *REG_VRAMADDR=ADDR_SCB2+nuage->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(nuage->y-208<<7)+nuage->height;
    *REG_VRAMRW=(nuage->x<<7);

    // --- On chaine l'ensemble des sprites
    for (u16 v=1; v<nuage->width; v++) {
        *REG_VRAMADDR=ADDR_SCB2+nuage->sprite+v;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6; // sticky
    }
}

void nuage_update(nuage_t *nuage){

    for (u16 s=0; s<nuage->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((nuage->sprite+s)*64);
        for (u16 v=0; v<nuage->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+nuage->tmx[v][s]-1;
            *REG_VRAMRW = (nuage->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+nuage->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(nuage->y-208<<7)+nuage->height;
    *REG_VRAMRW=(nuage->x<<7);
}

void nuage_move_left(nuage_t *nuage){
    nuage->x++;
    nuage_update(nuage);
}

void nuage_move_right(nuage_t *nuage){
    nuage->x--;
    nuage_update(nuage);
}

void nuage_hide(nuage_t *nuage){
    nuage->height=0;
    nuage_setup(nuage);
}