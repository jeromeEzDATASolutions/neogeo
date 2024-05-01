/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the background
 */
#define ARTHUR_LANCE_VITESSE 5
#define ARTHUR_LANCE_STATE_CACHEE 1
#define ARTHUR_LANCE_STATE_LANCEE 2

typedef struct _lance_t {
    u16 sprite;
    u16 tile_offset_x;
    u16 tile_offset_y;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
    u16 sens;               // 0 left or 1 right
    u8 state;               // CACHEE - 
} lance_t;

lance_t lance = {
    .sprite = CROM_TILE_START_LANCE1,
    .tile_offset_x = 0, 
    .tile_offset_y = 4, 
    .palette = 7, 
    .width = 2,
    .height = 1,
    .x = 100,
    .y = 0,
    .sens = 1, 
    .state = ARTHUR_LANCE_STATE_CACHEE, 
};

lance_t lances[3] = {
    {
        .sprite = CROM_TILE_START_LANCE1,
        .tile_offset_x = 0, 
        .tile_offset_y = 4, 
        .palette = 7, 
        .width = 2,
        .height = 1,
        .x = 100,
        .y = 0,
        .sens = 1, 
        .state = ARTHUR_LANCE_STATE_CACHEE, 
    }, 
    {
        .sprite = CROM_TILE_START_LANCE2,
        .tile_offset_x = 0, 
        .tile_offset_y = 4, 
        .palette = 7, 
        .width = 2,
        .height = 1,
        .x = 100,
        .y = 0,
        .sens = 1, 
        .state = ARTHUR_LANCE_STATE_CACHEE, 
    },
    {
        .sprite = CROM_TILE_START_LANCE3,
        .tile_offset_x = 0, 
        .tile_offset_y = 4, 
        .palette = 7, 
        .width = 2,
        .height = 1,
        .x = 100,
        .y = 0,
        .sens = 1, 
        .state = ARTHUR_LANCE_STATE_CACHEE, 
    },
};

void lance_init_position(lance_t *lance) {
    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+lance->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(282+lance->y<<7)+lance->height;
    *REG_VRAMRW=(lance->x<<7);
}

void lance_init(lance_t *lance) {

    if ( arthur.sens == 0 ) {
        lance->tile_offset_y = 6;
    }
    else {
        lance->tile_offset_y = 4;
    }

    for (u16 s=0; s<lance->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((lance->sprite+s)*64);
        for (u16 v=0; v<lance->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+arthur.tmx[v+lance->tile_offset_y][s+lance->tile_offset_x]-1;
            *REG_VRAMRW = (lance->palette<<8);
        }
    }

    lance_init_position(lance);

    // --- On chaine l'ensemble des sprites
    for (u16 v=1; v<lance->width; v++) {
        *REG_VRAMADDR=ADDR_SCB2+lance->sprite+v;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6; // sticky
    }
}

void lance_update(lance_t *lance){

    if ( arthur.sens == 0 ) {
        lance->tile_offset_y = 6;
    }
    else {
        lance->tile_offset_y = 4;
    }

    for (u16 s=0; s<lance->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((lance->sprite+s)*64);
        for (u16 v=0; v<lance->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+arthur.tmx[v+lance->tile_offset_y][s+lance->tile_offset_x]-1;
            *REG_VRAMRW = (lance->palette<<8);
        }
    }

    lance_init_position(lance);
}

void lance_start(s16 arthur_x, s16 arthur_y) {
    if ( lance.state == ARTHUR_LANCE_STATE_CACHEE ) {
        if ( arthur.sens == 1 ){
            lance.x = arthur_x + 20;
            lance.y = arthur_y + 18;
        }
        else {
            lance.x = arthur_x - 20;
            lance.y = arthur_y + 18;
        }

        if ( arthur.position == ARTHUR_ACCROUPI ){
            lance.y = arthur_y + 8;
        }

        lance.state = ARTHUR_LANCE_STATE_LANCEE;
        lance.sens = arthur.sens;
        lance_update(&lance);
    }
}

void lance_gestion(){
    if ( lance.state == ARTHUR_LANCE_STATE_LANCEE ){

        if ( lance.sens == 1 ){
            lance.x += ARTHUR_LANCE_VITESSE;
        }
        else {
            lance.x -= ARTHUR_LANCE_VITESSE;
        }
        lance_update(&lance);

        if ( lance.x >= 350 || lance.x+30 <= 0 ){
            lance.state = ARTHUR_LANCE_STATE_CACHEE;
            lance_update(&lance);
        }
    }
}
