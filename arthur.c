/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the background
 */

#define GNG_ARTHUR_TMX_WIDTH 24
#define GNG_ARTHUR_TMX_HEIGHT 4

#define FIXED_POINT 8
#define INT_TO_FIXED(x) ((x) << FIXED_POINT)
#define FIXED_TO_INT(x) ((x) >> FIXED_POINT)
#define ADD_FIXED(x, y) ((x) + (y))
#define SUB_FIXED(x, y) ((x) - (y))
#define MUL_FIXED(x, y) (((x) * (y)) >> FIXED_POINT)
#define DIV_FIXED(x, y) (((x) << FIXED_POINT) / (y))
#define JUMP_VELOCITY INT_TO_FIXED(-4)
#define GRAVITY INT_TO_FIXED(1)

static void arthur_init_tmx();
static int arthur_walk_right();
static void arthur_stop_walk();
static void arthur_jump_vertical();
static void arthur_jump_update();
static void arthur_check_si_dans_le_vide();

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
    u16 position_y;
    u16 sens; // 0 left or 1 right
    u16 frames;
    u16 state; // ARTHUR_SUR_LE_SOL - ARTHUR_SUR_ECHELLE - ARTHUR_SAUTE
    u16 position; // ARTHUR_DEBOUT - ARTHUR_CROUCHING
    u16 tilex;
    u16 tiley;
    int velocity;
    u16 tile_bottom;
    s16 yf;
} arthur_t;

arthur_t arthur = {
    .sprite = CROM_TILE_START_ARTHUR,
    .tile_offset_x = 0, // Décalage si besoin de la première colonne pour Arthur
    .tile_offset_y = 0, // Arthur à gauche : ligne 2
    .palette = 4, 
    .width = 2, // 32 * 16 = 512 pixels
    .height = 2,
    .x = 144,
    .y = 31,
    .tmx = {},
    .position_x = 144,
    .position_y = 31,
    .sens = 1,
    .frames = 0, 
    .state = ARTHUR_SUR_LE_SOL,
    .tilex = 0, 
    .tiley = 0, 
    .velocity = 0,
    .tile_bottom = 0,
    .yf = 31*8,
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

    if ( arthur->tile_offset_x > 18 ){
        arthur->tile_offset_x=0;
    }

    if ( arthur->frames == 4 || arthur->tile_offset_x == 0 ) {
        arthur->frames = 0;
        arthur->tile_offset_x+=2;
        if ( arthur->tile_offset_x == 18 ){
            arthur->tile_offset_x=2;
        }
        arthur_update(arthur);
    }

    // --- On determine la tile sur laquelle est Arthur
    arthur->tilex = (arthur->position_x>>4)+1;
    arthur->tiley = 15-((arthur->position_y>>4)+2);
    arthur->tile_bottom = background.tmx[arthur->tiley][arthur->tilex];

    // Arthur ne peut marcher que sur le sol : tile 401
    //if ( background.tmx[13][arthur->tilex] == 0 ){
        
        // Arthur meurt - on le fait tomber

        // Ensuite on fait un fondu pour tout faire disparaitre
        // fadeInPalette(palette_background_herbe_nuage, 3);
    //}
}

void arthur_jump_vertical(arthur_t *arthur){

    if ( arthur->state != ARTHUR_SAUTE_VERTICALEMENT ){
        arthur->state = ARTHUR_SAUTE_VERTICALEMENT;
        arthur->velocity = 35;
    }

    arthur->tile_offset_x=18;
    if ( arthur->sens == 1 )
        arthur->tile_offset_y=0;
    else arthur->tile_offset_y=2;

    /*
    arthur_stop_walk(arthur);
    arthur->state = ARTHUR_SAUTE;
    arthur->y+=1;
    arthur->position_y+=1;
    */

    //arthur_update(arthur);
}

void arthur_jump_update(arthur_t *arthur){

    if ( arthur->y >= 31 || arthur->state == ARTHUR_SAUTE_VERTICALEMENT) {
        arthur->velocity-=2;
        arthur->yf+=arthur->velocity;
        arthur->y = arthur->yf/8;
        arthur_update(arthur);
    }

    if ( arthur->y == 31 ){
        arthur->velocity = 32;
        arthur->state=ARTHUR_SUR_LE_SOL;
    }

    //arthur->tilex = (arthur->position_x>>4)+1;
    //arthur->tiley = 15-((arthur->position_y>>4)+2);
    //arthur->tile_bottom = background.tmx[arthur->tiley+1][arthur->tilex];
}

int arthur_walk_right(arthur_t *arthur){

    char str[10];
    arthur->sens=1;
    arthur->tile_offset_y=0;

    if ( arthur->tile_offset_x > 18 ){
        arthur->tile_offset_x=0;
    }

    if ( arthur->frames == 4 || arthur->tile_offset_x == 0 ) {
        arthur->frames = 0;
        arthur->tile_offset_x+=2;
        if ( arthur->tile_offset_x == 18 ){
            arthur->tile_offset_x=2;
        }
        arthur_update(arthur);
    }

    // --- On determine la tile sur laquelle est Arthur
    arthur->tilex = (arthur->position_x>>4)+1;
    arthur->tiley = 15-((arthur->position_y>>4)+2);
    arthur->tile_bottom = background.tmx[arthur->tiley+1][arthur->tilex];

    // Arthur ne peut marcher que sur le sol : tile 401
    u16 tile_on_right = arthur->tilex+1;
    if ( background.tmx[arthur->tiley][tile_on_right] == 0 || 1 ){
        
        // Arthur peut marcher à droite
        arthur->position=ARTHUR_DEBOUT;
        arthur->position_x++;

        // jump(arthur);
        // update(arthur);

        // snprintf(str, 30, "Coor X-Y %3d", FIXED_TO_INT(arthur->y)); ng_text(2, 5, 0, str);

        // Ensuite on fait un fondu pour tout faire disparaitre
        // fadeInPalette(palette_background_herbe_nuage, 3);
        return 1;
    }

    return 0;
}

void arthur_stop_walk(arthur_t *arthur){
    arthur->tile_offset_x=0;
    if ( arthur->sens == 1 )
        arthur->tile_offset_y=0;
    else arthur->tile_offset_y=2;
    arthur_update(arthur);
}

void arthur_accroupi(arthur_t *arthur){

    if ( bios_p1current == 6 ){
        arthur->sens = 0;
    }
    else if ( bios_p1current == 10 ){
        arthur->sens = 1;
    }
    
    arthur->tile_offset_x=22;
    
    if ( arthur->sens == 1 )
        arthur->tile_offset_y=0;
    else arthur->tile_offset_y=2;
    
    arthur_update(arthur);
}

void arthur_check_si_dans_le_vide(arthur_t *arthur){
    char str1[10];
    //u16 nombre = FIX_POINT(10,50);
    //snprintf(str1, 30, "tilex %3d", arthur->tile_bottom); ng_text(2, 3, 0, str1);
}