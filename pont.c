/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the plateforme PONT
 */

typedef struct _pont_t {
    u16 sprite;
    u16 tile_offset_x;
    u16 tile_offset_y;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
    s16 level_position_x; // Position X for pont in level
    s16 start_x; // Ne peut pas aller avant 
    s16 end_x;
    u8 display;
    u8 sens;
} pont_t;

pont_t pont = {
    .sprite = CROM_TILE_START_PONT,
    .tile_offset_x = 0, 
    .tile_offset_y = 0, 
    .palette = 9, 
    .width = 2,
    .height = 1,
    .x = 300,
    .y = 12,
    .level_position_x = 300,
    .start_x = 256, 
    .end_x = 400, 
    .display = 0, 
    .sens = 0, // Gauche par défaut
};

/**
 * Initialisation des lances transmises en parametre via un array de structure
 */
void pont_init(pont_t *p_pont) {

    p_pont->sens = 0;

    for (u16 s=0; s<p_pont->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((p_pont->sprite+s)*64);
        for (u16 v=0; v<p_pont->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+tmx_herbe[v][s]-1;
            *REG_VRAMRW = (p_pont->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+p_pont->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(p_pont->y+288<<7)+p_pont->height;
    *REG_VRAMRW=(p_pont->x<<7);

    // --- On chaine l'ensemble des sprites
    for (u16 v=1; v<p_pont->width; v++) {
        *REG_VRAMADDR=ADDR_SCB2+p_pont->sprite+v;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6; // sticky
    }
}

void pont_update(pont_t *p_pont){

    for (u16 s=0; s<p_pont->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((p_pont->sprite+s)*64);
        for (u16 v=0; v<p_pont->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+tmx_herbe[v][s]-1;
            *REG_VRAMRW = (p_pont->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+p_pont->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(p_pont->y+288<<7)+p_pont->height;
    *REG_VRAMRW=(p_pont->x<<7);
}

void pont_move(pont_t *p_pont){

    if ( p_pont->sens == 0 ){
        if ( p_pont->level_position_x > p_pont->start_x ){
            p_pont->x--;
            p_pont->level_position_x--;
        }
        else if ( p_pont->level_position_x == p_pont->start_x ){
            p_pont->sens = 1;
            p_pont->x++;
            p_pont->level_position_x++;
        }
    }
    else if ( p_pont->sens == 1 ){
        if ( p_pont->level_position_x < p_pont->end_x ){
            p_pont->x++;
            p_pont->level_position_x++;
        }
        else if ( p_pont->level_position_x == p_pont->end_x ){
            p_pont->sens = 0;
            p_pont->x--;
            p_pont->level_position_x--;
        }
    }

    pont_update(p_pont);
}

void pont_scroll_right(pont_t *p_pont){

    p_pont->x--;
    p_pont->level_position_x--;

    pont_update(p_pont);
}

void pont_scroll_left(pont_t *p_pont){

    p_pont->x++;
    p_pont->level_position_x++;

    pont_update(p_pont);
}
