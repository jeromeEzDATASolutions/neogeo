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
    u8 state;               // CACHEE
} lance_t;

lance_t lances[3] = {
    {
        .sprite = CROM_TILE_START_LANCE1,
        .tile_offset_x = 0, 
        .tile_offset_y = 4, 
        .palette = 7, 
        .width = 2,
        .height = 1,
        .x = 100,
        .y = 260,
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
        .y = 260,
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
        .y = 260,
        .sens = 1, 
        .state = ARTHUR_LANCE_STATE_CACHEE, 
    },
};

/**
 * Initialisation des lances transmises en parametre via un array de structure
 */
void lances_init(lance_t p_lances[]) {

    for(u16 k=0; k<3; k++){

        if ( arthur.sens == 0 ) {
            p_lances[k].tile_offset_y = 6;
        }
        else {
            p_lances[k].tile_offset_y = 4;
        }

        for (u16 s=0; s<p_lances[k].width; s++) {
            *REG_VRAMMOD=1;
            *REG_VRAMADDR=ADDR_SCB1+((p_lances[k].sprite+s)*64);
            for (u16 v=0; v<p_lances[k].height; v++) {
                *REG_VRAMRW = CROM_TILE_OFFSET+arthur.tmx[v+p_lances[k].tile_offset_y][s+p_lances[k].tile_offset_x]-1;
                *REG_VRAMRW = (p_lances[k].palette<<8);
            }
        }

        *REG_VRAMMOD=0x200;
        *REG_VRAMADDR=ADDR_SCB2+p_lances[k].sprite;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=(282+p_lances[k].y<<7)+p_lances[k].height;
        *REG_VRAMRW=(p_lances[k].x<<7);

        // --- On chaine l'ensemble des sprites
        for (u16 v=1; v<p_lances[k].width; v++) {
            *REG_VRAMADDR=ADDR_SCB2+p_lances[k].sprite+v;
            *REG_VRAMRW=0xFFF;
            *REG_VRAMRW=1<<6; // sticky
        }
    }
}

void lance_update(lance_t *p_lance){

    if ( arthur.sens == 0 ) {
        p_lance->tile_offset_y = 6;
    }
    else {
        p_lance->tile_offset_y = 4;
    }

    for (u16 s=0; s<p_lance->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((p_lance->sprite+s)*64);
        for (u16 v=0; v<p_lance->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+arthur.tmx[v+p_lance->tile_offset_y][s+p_lance->tile_offset_x]-1;
            *REG_VRAMRW = (p_lance->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+p_lance->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(282+p_lance->y<<7)+p_lance->height;
    *REG_VRAMRW=(p_lance->x<<7);
}

void lance_start(lance_t p_lances[], s16 arthur_x, s16 arthur_y) {

    // --- Arthur tire des lances sauf s'il est sur une échell
    if ( arthur.state != ARTHUR_SUR_ECHELLE ){

        u8 all_lances_cachees = 1;

        for (u16 s=0; s<3; s++) {
            if ( lances[s].state != ARTHUR_LANCE_STATE_CACHEE ){
                all_lances_cachees = 0;
            }
        }

        if ( all_lances_cachees == 1 || (lances[arthur.frame_lance].state == ARTHUR_LANCE_STATE_CACHEE && (frames%5)==0)) {

            if ( arthur.sens == 1 ){
                lances[arthur.frame_lance].x = arthur_x + 20;
                lances[arthur.frame_lance].y = arthur_y + 18;
            }
            else {
                lances[arthur.frame_lance].x = arthur_x - 20;
                lances[arthur.frame_lance].y = arthur_y + 18;
            }

            if ( arthur.position == ARTHUR_ACCROUPI ){
                lances[arthur.frame_lance].y = arthur_y + 8;
            }

            lances[arthur.frame_lance].state = ARTHUR_LANCE_STATE_LANCEE;
            lances[arthur.frame_lance].sens = arthur.sens;
            lance_update(&lances[arthur.frame_lance]);

            arthur.frame_lance = arthur.frame_lance+1;
            if ( arthur.frame_lance == 3 ){
                arthur.frame_lance = 0;
            }
        }
    }
}

void lances_hide(lance_t p_lances[]){
    for(u16 i=0; i<3; i++){
        lances[i].y = 260;
        lance_update(&lances[i]);
    }
}

void lances_gestion(lance_t p_lances[]){
    for(u16 i=0; i<3; i++){
        if ( lances[i].state == ARTHUR_LANCE_STATE_LANCEE ){

            if ( lances[i].sens == 1 ){
                lances[i].x += ARTHUR_LANCE_VITESSE;
            }
            else {
                lances[i].x -= ARTHUR_LANCE_VITESSE;
            }
            lance_update(&lances[i]);

            if ( lances[i].x >= 350 || lances[i].x+30 <= 0 ){
                lances[i].state = ARTHUR_LANCE_STATE_CACHEE;
                lance_update(&lances[i]);
            }
        }
    }
}