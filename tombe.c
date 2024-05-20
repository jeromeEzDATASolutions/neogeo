/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the tombes
 */

typedef struct _tombe_t {
    u16 sprite;
    u16 tile_offset_x;
    u16 tile_offset_y;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
} tombe_t;

tombe_t tombes[3] = {
    {
        .sprite = CROM_TILE_START_TOMBE1,
        .tile_offset_x = 0, 
        .tile_offset_y = 0, 
        .palette = 10, 
        .width = 2,
        .height = 2,
        .x = 50,
        .y = 40,
    }, 
    {
        .sprite = CROM_TILE_START_TOMBE2,
        .tile_offset_x = 2, 
        .tile_offset_y = 0, 
        .palette = 10, 
        .width = 2,
        .height = 2,
        .x = 100,
        .y = 52,
    },
    {
        .sprite = CROM_TILE_START_TOMBE3,
        .tile_offset_x = 4, 
        .tile_offset_y = 0, 
        .palette = 10, 
        .width = 2,
        .height = 2,
        .x = 200,
        .y = 40,
    },
};

/**
 * Initialisation des tombes transmises en parametre via un array de structure
 */
void tombes_init(tombe_t p_tombes[]) {

    for(u16 k=0; k<3; k++){

        p_tombes[k].tile_offset_y = 0;

        for (u16 s=0; s<p_tombes[k].width; s++) {
            *REG_VRAMMOD=1;
            *REG_VRAMADDR=ADDR_SCB1+((p_tombes[k].sprite+s)*64);
            for (u16 v=0; v<p_tombes[k].height; v++) {
                *REG_VRAMRW = CROM_TILE_OFFSET+tmx_herbe[v+p_tombes[k].tile_offset_y][s+p_tombes[k].tile_offset_x]-1;
                *REG_VRAMRW = (p_tombes[k].palette<<8);
            }
        }

        *REG_VRAMMOD=0x200;
        *REG_VRAMADDR=ADDR_SCB2+p_tombes[k].sprite;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=(282+p_tombes[k].y<<7)+p_tombes[k].height;
        *REG_VRAMRW=(p_tombes[k].x<<7);

        // --- On chaine l'ensemble des sprites
        for (u16 v=1; v<p_tombes[k].width; v++) {
            *REG_VRAMADDR=ADDR_SCB2+p_tombes[k].sprite+v;
            *REG_VRAMRW=0xFFF;
            *REG_VRAMRW=1<<6; // sticky
        }
    }
}

void tombe_update(tombe_t *p_tombe){

    p_tombe->tile_offset_y = 6;

    for (u16 s=0; s<p_tombe->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((p_tombe->sprite+s)*64);
        for (u16 v=0; v<p_tombe->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+tmx_herbe[v+p_tombe->tile_offset_y][s+p_tombe->tile_offset_x]-1;
            *REG_VRAMRW = (p_tombe->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+p_tombe->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(282+p_tombe->y<<7)+p_tombe->height;
    *REG_VRAMRW=(p_tombe->x<<7);
}

void tombes_hide(tombe_t p_tombes[]){
    for(u16 i=0; i<3; i++){
        tombes[i].y = 260;
        tombe_update(&p_tombes[i]);
    }
}