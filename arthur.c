/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for the background
 */

#define GNG_ARTHUR_TMX_WIDTH 34
#define GNG_ARTHUR_TMX_HEIGHT 7

#define FIXED_POINT 8
#define INT_TO_FIXED(x) ((x) << FIXED_POINT)
#define FIXED_TO_INT(x) ((x) >> FIXED_POINT)
#define ADD_FIXED(x, y) ((x) + (y))
#define SUB_FIXED(x, y) ((x) - (y))
#define MUL_FIXED(x, y) (((x) * (y)) >> FIXED_POINT)
#define DIV_FIXED(x, y) (((x) << FIXED_POINT) / (y))
#define JUMP_VELOCITY INT_TO_FIXED(-4)
#define GRAVITY INT_TO_FIXED(1)

#define GNG_ARTHUR_TILES_ACCROUPI 22
#define GNG_ARTHUR_TILES_ECHELLE 24
#define GNG_ARTHUR_TILES_ECHELLE_END 26
#define GNG_ARTHUR_TILES_CUL 32

static void arthur_init_tmx();
static int arthur_walk_right();
static void arthur_stop_walk();
static void arthur_jump_vertical();
static void arthur_jump_horizontal();
static void arthur_jump_update();
static void arthur_check_si_dans_le_vide();
static void arthur_calcule_tiles();
static void arthur_tombe();
static void arthur_sur_echelle_last_etape();
static void arthur_sur_echelle();
static void arthur_tombe_update();
static void arthur_update_posision_x_left();
static void arthur_update_posision_x_right();
static int arthur_can_go_to_right();

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
    u16 display_y;          // y to display arthur on the screen. It's juste for the display
    u16 sens;               // 0 left or 1 right
    u16 frames;
    u16 state;              // ARTHUR_SUR_LE_SOL - ARTHUR_SUR_ECHELLE - ARTHUR_SAUTE
    u16 position;           // ARTHUR_DEBOUT - ARTHUR_CROUCHING
    u16 tilex;
    u16 tiley;
    int velocity;
    u16 tile_bottom;
    u16 tile_right;
    u16 tile_left;
    s16 yf;
    u8 saut_up;             // Phase montante du saut d'Arthur
    u8 saut_down;           // Phase descendante du saut d'Arthur
    u16 frame_echelle;
    u16 frame_echelle_end;
    u16 frame_lance;
    u16 absolute_bottom_left_x;
    u16 absolute_bottom_right_x;
} arthur_t;

arthur_t arthur = {
    .sprite = CROM_TILE_START_ARTHUR,
    .tile_offset_x = 0, // Décalage si besoin de la première colonne pour Arthur
    .tile_offset_y = 0, // Arthur à gauche : ligne 2
    .palette = 4, 
    .width = 2, // 32 * 16 = 512 pixels
    .height = 2,
    .x = GNG_START_ARTHUR_POSISTION_X,
    .y = 32,
    .tmx = {},
    .position_x = GNG_START_ARTHUR_POSISTION_X,
    .position_y = 32,
    .display_y = -2,
    .sens = 1,
    .frames = 0, 
    .state = ARTHUR_SUR_LE_SOL,
    .tilex = 0, 
    .tiley = 0, 
    .velocity = 0,
    .tile_bottom = 0,
    .tile_right = 0,
    .tile_left = 0,
    .yf = 32*8,
    .saut_up = 0,
    .saut_down = 0, 
    .frame_echelle = 0, 
    .frame_echelle_end = 0, 
    .frame_lance = 0, 
    .absolute_bottom_left_x = GNG_START_ARTHUR_POSISTION_X, 
    .absolute_bottom_right_x = GNG_START_ARTHUR_POSISTION_X + (2*16), 
};

arthur_t arthur_origin = {
    .sprite = CROM_TILE_START_ARTHUR,
    .tile_offset_x = 0, // Décalage si besoin de la première colonne pour Arthur
    .tile_offset_y = 0, // Arthur à gauche : ligne 2
    .palette = 4, 
    .width = 2, // 32 * 16 = 512 pixels
    .height = 2,
    .x = GNG_START_ARTHUR_POSISTION_X,
    .y = 32,
    .tmx = {},
    .position_x = GNG_START_ARTHUR_POSISTION_X,
    .position_y = 32,
    .display_y = -2,
    .sens = 1,
    .frames = 0, 
    .state = ARTHUR_SUR_LE_SOL,
    .tilex = 0, 
    .tiley = 0, 
    .velocity = 0,
    .tile_bottom = 0,
    .tile_right = 0,
    .tile_left = 0,
    .yf = 32*8,
    .saut_up = 0,
    .saut_down = 0, 
    .frame_echelle = 0, 
    .frame_echelle_end = 0, 
    .frame_lance = 0, 
    .absolute_bottom_left_x = GNG_START_ARTHUR_POSISTION_X, 
    .absolute_bottom_right_x = GNG_START_ARTHUR_POSISTION_X + (2*16), 
};

void arthur_reset(arthur_t *arthur, arthur_t *arthur_origin){
    arthur->sprite = arthur_origin->sprite;
    arthur->tile_offset_x = arthur_origin->tile_offset_x;
    arthur->tile_offset_y = arthur_origin->tile_offset_y;
    arthur->palette = arthur_origin->palette;
    arthur->width = arthur_origin->width;
    arthur->height = arthur_origin->height;
    arthur->x = arthur_origin->x;
    arthur->y = arthur_origin->y;
    arthur->position_x = arthur_origin->position_x;
    arthur->position_y = arthur_origin->position_y;
    arthur->display_y = arthur_origin->display_y;
    arthur->sens = arthur_origin->sens;
    arthur->frames = arthur_origin->frames;
    arthur->state = arthur_origin->state;
    arthur->tilex = arthur_origin->tilex;
    arthur->tiley = arthur_origin->tiley;
    arthur->velocity = arthur_origin->velocity;
    arthur->tile_bottom = arthur_origin->tile_bottom;
    arthur->tile_right = arthur_origin->tile_right;
    arthur->tile_left = arthur_origin->tile_left;
    arthur->yf = arthur_origin->yf;
    arthur->saut_up = arthur_origin->saut_up;
    arthur->saut_down = arthur_origin->saut_down;
    arthur->frame_echelle = arthur_origin->frame_echelle;
    arthur->frame_echelle_end = arthur_origin->frame_echelle_end;
    arthur->frame_lance = arthur_origin->frame_lance;
    arthur->absolute_bottom_left_x = arthur_origin->absolute_bottom_left_x;
    arthur->absolute_bottom_right_x = arthur_origin->absolute_bottom_right_x;
}

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
    *REG_VRAMRW=(arthur->y-208+arthur->display_y<<7)+arthur->height;
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
    *REG_VRAMRW=(arthur->y-208+arthur->display_y<<7)+arthur->height;
    *REG_VRAMRW=(arthur->x<<7);
}

int arthur_walk_left(arthur_t *arthur){

    char str[10];
    arthur->sens=0;
    arthur->tile_offset_y=2;

    if ( arthur->tile_offset_x > 18 ){
        arthur->tile_offset_x=0;
    }

    if ( ( arthur->state == ARTHUR_SUR_LE_SOL || arthur->state == ARTHUR_SUR_PLATEFORME ) && ( arthur->frames == 4 || arthur->tile_offset_x == 0 )) {
        arthur->frames = 0;
        arthur->tile_offset_x+=2;
        if ( arthur->tile_offset_x == 18 ){
            arthur->tile_offset_x=2;
        }
        arthur_update(arthur);
    }

    // --- On checke si Arthur peut aller à droite
    u8 tmp_choix_arthur = arthur_can_go_to_left(arthur);
    if ( tmp_choix_arthur == 1 ){
        // Arthur peut marcher à droite
        arthur->position=ARTHUR_DEBOUT;
        arthur_update_posision_x_left(arthur);
        return 1;
    }
    else if ( tmp_choix_arthur == 2 ){
        snprintf(str, 20, "TOMBE %4d", arthur->state); ng_text(2, 9, 0, str);
        arthur_tombe(arthur);
        return 0;
    }
    
    /*
    // --- On determine la tile sur laquelle est Arthur
    arthur_calcule_tiles(arthur);

    // fadeInPalette(palette_background_herbe_nuage, 3);

    if ( arthur->state == ARTHUR_SUR_PLATEFORME ){
        // --- On doit checker si Arthur est toujours sur la plateforme
        if ( arthur->x>=pont.x-12 ){
            arthur_update_posision_x_left(arthur);
            return 1;
        }
        else {
            // Il tombe
            arthur->state = ARTHUR_TOMBE;
            arthur_tombe_update(&arthur);
        }
    }
    else if ( tmx_sol[arthur->tiley+1][arthur->tilex] != 0 && tmx_sol[arthur->tiley+1][arthur->tilex] != TILE_ECHELLE ){

        if ( arthur->tile_left == 377 || arthur->tile_left == 357 || arthur->tile_left == 358 ) {
        }
        else {
            arthur->position=ARTHUR_DEBOUT;
            if ( arthur->position_x > 144 ){
                arthur_update_posision_x_left(arthur);
            }
            return 1;
        }
    }
    else {
        arthur_tombe(arthur);
    }
    */

    return 0;
}

int arthur_walk_right(arthur_t *arthur){

    char str[10];
    arthur->sens=1;
    arthur->tile_offset_y=0;

    if ( arthur->tile_offset_x > 18 ){
        arthur->tile_offset_x=0;
    }

    // --- Animation d'Arthur qui marche à droite
    if ( ( arthur->state == ARTHUR_SUR_LE_SOL || arthur->state == ARTHUR_SUR_PLATEFORME ) && ( arthur->frames == 4 || arthur->tile_offset_x == 0 ) ) {
        arthur->frames = 0;
        arthur->tile_offset_x+=2;
        if ( arthur->tile_offset_x == 18 ){
            arthur->tile_offset_x=2;
        }
        arthur_update(arthur);
    }

    // --- On checke si Arthur peut aller à droite
    u8 tmp_choix_arthur = arthur_can_go_to_right(arthur);
    if ( tmp_choix_arthur == 1 ){
        // Arthur peut marcher à droite
        arthur->position=ARTHUR_DEBOUT;
        arthur_update_posision_x_right(arthur);
        return 1;
    }
    else if ( tmp_choix_arthur == 2 ){
        snprintf(str, 20, "TOMBE %4d", arthur->state); ng_text(2, 9, 0, str);
        arthur_tombe(arthur);
        return 0;
    }

    /*
    if ( arthur->state == ARTHUR_SUR_ECHELLE ){
    }

    if ( arthur->state == ARTHUR_SUR_PLATEFORME ){
        if ( arthur->x<=pont.x+16 ){
            arthur_update_posision_x_right(arthur);
            return 1;
        }
        else {
            // Il tombe
            arthur->state = ARTHUR_TOMBE;
            arthur_tombe_update(&arthur);
        }
    }
    else if ( tmx_sol[arthur->tiley+1][arthur->tilex] != 0 && tmx_sol[arthur->tiley+1][arthur->tilex] != TILE_ECHELLE ){
        // Arthur ne peut marcher que sur le sol : tile 401
        if ( arthur->tile_right == 377 || arthur->tile_right == 357 || arthur->tile_right == 358 ) {
        }
        else {
            // Arthur peut marcher à droite
            arthur->position=ARTHUR_DEBOUT;
            return 1;
        }
    }
    else {
        //arthur_tombe(arthur);
    }
    */

    return 0;
}

void arthur_update_posision_x_left(arthur_t *arthur){
    arthur->position_x--;
    arthur->absolute_bottom_left_x--;
    arthur->absolute_bottom_right_x--;
}

void arthur_update_posision_x_right(arthur_t *arthur){
    arthur->position_x++;
    arthur->absolute_bottom_left_x++;
    arthur->absolute_bottom_right_x++;
}

void arthur_stop_walk(arthur_t *arthur){
    arthur->tile_offset_x=0;
    if ( arthur->sens == 1 )
        arthur->tile_offset_y=0;
    else arthur->tile_offset_y=2;
    arthur_update(arthur);
}

void arthur_set_position(arthur_t *arthur, u16 tile_top_left){
    arthur->tile_offset_x = tile_top_left;
}

void arthur_accroupi(arthur_t *arthur){

    if ( bios_p1current == 6 ){
        arthur->sens = 0;
    }
    else if ( bios_p1current == 10 ){
        arthur->sens = 1;
    }
    
    arthur_set_position(arthur, GNG_ARTHUR_TILES_ACCROUPI);

    arthur->tile_offset_y=2;
    if ( arthur->sens == 1 )
        arthur->tile_offset_y=0;

    arthur->position = ARTHUR_ACCROUPI;
    
    arthur_update(arthur);
}

int arthur_descend_echelle(arthur_t *arthur){

    if ( arthur->state == ARTHUR_SUR_LE_SOL ){
        // --- On checke si la tuile sous Arthur est une fin d'echelle
        if ( arthur->tile_bottom == 398 ){

            // On commence par la derniere etape
            // TODO
            arthur->y++;
            arthur->position_y++;
            arthur->yf = arthur->y*8;
            arthur->state = ARTHUR_SUR_ECHELLE;
            arthur->frame_echelle++;
            arthur_sur_echelle_last_etape(arthur); // --- Display last etape from sprite Arthur sur echelle
            arthur_calcule_tiles(arthur);

        }
        else {
            // --- Arthur se baisse
            arthur_accroupi(arthur);
        }
    }
    else if ( arthur->state == ARTHUR_SUR_ECHELLE ){
        arthur->y--;
        arthur->position_y--;
        arthur->yf = arthur->y*8;
        //arthur->state = ARTHUR_SUR_ECHELLE;
        arthur->frame_echelle++;
        arthur_sur_echelle(arthur); // --- Display sprite Arthur sur echelle
        arthur_calcule_tiles(arthur);
        arthur->frame_echelle_end=0;

        if ( tmx_sol[arthur->tiley][arthur->tilex] == 375 ){
            arthur->state = ARTHUR_SUR_LE_SOL;
            arthur->y = (15-(arthur->tiley))*16;
            arthur->position_y = (15-(arthur->tiley))*16;
            arthur->yf = arthur->y*8;
        }
    }

    return 1;
}

void arthur_sur_echelle(arthur_t *arthur){

    arthur_set_position(arthur, GNG_ARTHUR_TILES_ECHELLE);

    // On alterne le sprite d'arthur qui monte à l'échelle en jouant sur Y
    if ( arthur->frame_echelle == 8 ){
        if ( arthur->tile_offset_y == 0 ){
            arthur->tile_offset_y=2;
        }
        else {
            arthur->tile_offset_y=0;
        }
        arthur->frame_echelle = 0;
    }

    arthur_update(arthur);
}

void arthur_sur_echelle_last_etape(arthur_t *arthur){

    char str[10];

    arthur_set_position(arthur, GNG_ARTHUR_TILES_ECHELLE_END);

    arthur->frame_echelle_end++;
    if ( arthur->frame_echelle_end >= 3 && arthur->frame_echelle_end <=12 ){
        arthur->display_y--;
    }
    else if ( arthur->frame_echelle_end > 12 ){
        if ( arthur->frame_echelle_end == 12 ){
            arthur->display_y = 0;
        }
        else if ( arthur->frame_echelle_end == 13 ){
            arthur->display_y = -1;
        }
        else if ( arthur->frame_echelle_end == 14 ){
            arthur->display_y = -2;
        }
        else if ( arthur->frame_echelle_end == 15 ){
            arthur->display_y = -3;
        }
        arthur_set_position(arthur, GNG_ARTHUR_TILES_CUL);
    }

    // On alterne le sprite d'arthur qui monte à l'échelle en jouant sur Y
    if ( arthur->frame_echelle == 8 ){
        if ( arthur->tile_offset_y == 0 ){
            arthur->tile_offset_y=2;
        }
        else {
            arthur->tile_offset_y=0;
        }
        arthur->frame_echelle = 0;
    }

    arthur_update(arthur);

    //snprintf(str, 30, "End %3d", arthur->frame_echelle_end); ng_text(2, 3, 0, str);
}

void arthur_check_si_dans_le_vide(arthur_t *arthur){
    char str1[10];
}

void arthur_jump_vertical(arthur_t *arthur){
    if ( arthur->state == ARTHUR_SUR_LE_SOL || arthur->state == ARTHUR_SUR_PLATEFORME ){
        arthur->state = ARTHUR_SAUTE_VERTICALEMENT;
        arthur->velocity = 35; // 35
    }
}

void arthur_jump_horizontal(arthur_t *arthur){
    if ( arthur->state == ARTHUR_SUR_LE_SOL || arthur->state == ARTHUR_SUR_PLATEFORME ){
        arthur->state = ARTHUR_SAUTE_HORIZONTALEMENT;
        arthur->velocity = 35; // 35
    }
}

void arthur_jump_update(arthur_t *arthur, pont_t *pont){

    char str[10];

    // --- On affiche la tile pour le saut vertical
    if ( arthur->state == ARTHUR_SAUTE_VERTICALEMENT ){
        arthur->tile_offset_x=18;
        if ( arthur->sens == 1 )
            arthur->tile_offset_y=0;
        else arthur->tile_offset_y=2;
    }

    if ( arthur->state == ARTHUR_SAUTE_HORIZONTALEMENT ){
        arthur->tile_offset_x=20;
        if ( arthur->sens == 1 )
            arthur->tile_offset_y=0;
        else arthur->tile_offset_y=2;
    }

    if ( arthur->state == ARTHUR_SAUTE_VERTICALEMENT || arthur->state == ARTHUR_SAUTE_HORIZONTALEMENT) {

        if ( arthur->yf < arthur->yf + arthur->velocity ){
            arthur->saut_up = 1;
            arthur->saut_down = 0;
        }
        else {
            arthur->saut_up = 0;
            arthur->saut_down = 1;
        }

        arthur->velocity-=2;
        arthur->yf+=arthur->velocity;
        arthur->y = arthur->yf/8;
        arthur->position_y = arthur->y;

        // --- On determine la tile sur laquelle est Arthur
        arthur_calcule_tiles(arthur);

        // --- arthur->y == 31 
        if ( arthur->saut_down && ( arthur->tile_bottom == 267 || arthur->tile_bottom == 375 || arthur->tile_bottom == 377 || arthur->tile_bottom == 378 || arthur->tile_bottom == 357 || arthur->tile_bottom == 80 || arthur->tile_bottom == TILE_ECHELLE_END ) ){
            arthur->velocity = 35; // 35
            arthur->state = ARTHUR_SUR_LE_SOL;
            arthur->saut_up = 0;
            arthur->saut_down = 0;
            arthur->y = (15-(arthur->tiley+1))*16;
            arthur->position_y = (15-(arthur->tiley+1))*16;
            arthur->yf = arthur->y*8;
        }

        if ( pont->display == 1 ){
            if ( arthur->y < 32 && arthur->x >= pont->x && arthur->x <= (pont->x)+32 ){
                arthur->velocity = 35; // 35
                arthur->state = ARTHUR_SUR_PLATEFORME;
                arthur->saut_up = 0;
                arthur->saut_down = 0;
                arthur->y = pont->y+16;
                arthur->position_y = pont->y+16;
                arthur->yf = arthur->y*8;
            }
        }

        arthur_update(arthur);
    }
}

int arthur_can_go_to_left(arthur_t *arthur){

    char str[10];
    arthur->tiley = 15-((arthur->position_y>>4)+1);
    u16 tile = tmx_sol[arthur->tiley+1][(arthur->absolute_bottom_right_x-10)>>4];

    if ( arthur->sens == 0 ){
        if ( tile == 375 || tile == 357 ){
            return 1; 
        }
        else if ( tile == 0 ){
            return 2;
        }
    }

    return 0; 
}

int arthur_can_go_to_right(arthur_t *arthur){

    char str[10];
    arthur->tiley = 15-((arthur->position_y>>4)+1);
    u16 tile = tmx_sol[arthur->tiley+1][(arthur->absolute_bottom_left_x+10)>>4];

    if ( arthur->sens == 1 ){
        if ( tile == 375 || tile == 357 ){
            return 1; 
        }
        else if ( tile == 0 ){
            return 2;
        }
    }

    return 0; 
}

void arthur_calcule_tiles(arthur_t *arthur){

    char str[10];

    u16 arthur_sprite_x_left = arthur->position_x;
    u16 arthur_sprite_x_right = arthur->position_x+27;

    arthur->tilex = ((arthur->absolute_bottom_left_x)>>4);
    arthur->tiley = 15-((arthur->position_y>>4)+1);

    // --- On determine la tile à droite d'Arthur
    arthur->tile_right = tmx_sol[arthur->tiley][(arthur->absolute_bottom_left_x+32)>>4];

    // --- On determine la tile à gauche d'Arthur
    arthur->tile_left = tmx_sol[arthur->tiley][(arthur_sprite_x_left+4)>>4];

    // --- On determine la tile sous Arthur
    arthur->tile_bottom = tmx_sol[arthur->tiley+1][arthur->tilex];
}

void arthur_tombe(arthur_t *arthur){
    if ( arthur->state == ARTHUR_SUR_LE_SOL ){
        arthur->state = ARTHUR_TOMBE;
    }
}

void arthur_tombe_update(arthur_t *arthur){

    char str[10];
    snprintf(str, 20, "STATE %4d", arthur->state); ng_text(2, 7, 0, str);

    if ( arthur->state == ARTHUR_TOMBE ){
        if ( 1==2 && (arthur->tile_bottom == 267 || arthur->tile_bottom == 375 || arthur->tile_bottom == 377 || arthur->tile_bottom == 378 || arthur->tile_bottom == 357 || arthur->tile_bottom == 80 )) {
            arthur->state = ARTHUR_SUR_LE_SOL;
            arthur->y = (15-(arthur->tiley+1))*16;
            arthur->position_y = (15-(arthur->tiley+1))*16;
            arthur->yf = arthur->y*8;
        }
        else{
            if ( arthur->position_y > 0 ){
                arthur->y-=10;
                arthur->position_y-=10;
                arthur_calcule_tiles(arthur);
                arthur_update(arthur);
            }
        }
    }
}