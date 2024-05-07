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
    u16 height_origin; // Permet de conserver la valeur d'origine dans le cas où on cache le sprite
    s16 x;
    s16 y;
    s16 y_origin; // Permet de conserver la valeur d'origine dans le cas où on cache le sprite
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
    .start_x = 212, // 212
    .end_x = 325, // 325
    .display = 0, 
    .sens = 0, // Gauche par défaut
};

pont_t pont_origin = {
    .sprite = 0,
    .tile_offset_x = 0, 
    .tile_offset_y = 0, 
    .palette = 0, 
    .width = 0, 
    .height = 0, 
    .x = 0, 
    .y = 0, 
    .start_x = 0, 
    .end_x = 0, 
    .display = 0, 
    .sens = 0, 
};

void pont_save(pont_t *pont, pont_t *pont_origin){
    pont_origin->sprite = pont->sprite;
    pont_origin->tile_offset_x = pont->tile_offset_x;
    pont_origin->tile_offset_y = pont->tile_offset_y;
    pont_origin->palette = pont->palette;
    pont_origin->width = pont->width;
    pont_origin->height = pont->height;
    pont_origin->x = pont->x;
    pont_origin->y = pont->y;
    pont_origin->start_x = pont->start_x;
    pont_origin->end_x = pont->end_x;
    pont_origin->display = pont->display;
    pont_origin->sens = pont->sens;
}

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

    if ( p_pont->display == 1 ){

        if ( p_pont->sens == 0 ){
            if ( p_pont->x > p_pont->start_x ){
                p_pont->x--;
            }
            else if ( p_pont->x == p_pont->start_x ){
                p_pont->sens = 1;
                p_pont->x++;
            }
        }
        else if ( p_pont->sens == 1 ){
            if ( p_pont->x < p_pont->end_x ){
                p_pont->x++;
            }
            else if ( p_pont->x == p_pont->end_x ){
                p_pont->sens = 0;
                p_pont->x--;
            }
        }

        pont_update(p_pont);
    }
}

void pont_display(pont_t *p_pont){
    p_pont->display=1;
    p_pont->x = pont_origin.x;
    p_pont->height = pont_origin.height;
    pont_init(&pont);
}

void pont_reset_and_hide(pont_t *p_pont){
    p_pont->display = 0;
    p_pont->height = 0;
    p_pont->x = pont_origin.x;
    p_pont->sens = pont_origin.sens;
    p_pont->start_x = pont_origin.start_x;
    p_pont->end_x = pont_origin.end_x;
    pont_update(p_pont);
}

void pont_scroll_right(pont_t *p_pont){
    if ( p_pont->display == 1 ){
        p_pont->x--;
        p_pont->start_x--;
        p_pont->end_x--;
        pont_update(p_pont);
    }
}

void pont_scroll_left(pont_t *p_pont){
    if ( p_pont->display == 1 ){
        p_pont->x++;
        p_pont->start_x++;
        p_pont->end_x++;
        pont_update(p_pont);
    }
}
