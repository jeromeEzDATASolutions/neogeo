#define GNG_MAP_TMX_WIDTH 32
#define GNG_MAP_TMX_HEIGHT 15

typedef struct _map_t {
    u16 sprite;
    u16 palette;
    u16 width;
    u16 height;
    s16 x;
    s16 y;
    u16 tmx[GNG_MAP_TMX_HEIGHT][GNG_MAP_TMX_WIDTH];
} map_t;

map_t map = {
    .sprite = CROM_TILE_START_MAP,
    .palette = 8, 
    .width = GNG_MAP_TMX_WIDTH,
    .height = GNG_MAP_TMX_HEIGHT,
    .x = 30,
    .y = 30,
    .tmx = {},
};

void map_init_tmx(map_t *map){
    for(u16 j = 0; j < GNG_MAP_TMX_HEIGHT; j++) {
        for(u16 i = 0; i < GNG_MAP_TMX_WIDTH; i++) {
            map->tmx[j][i] = tmx_map[j][i];
        }
    }
}

void map_setup(map_t *map) {

    map_init_tmx(map);

    for (u16 s=0; s<map->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((map->sprite+s)*64);
        for (u16 v=0; v<map->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+map->tmx[v][s]-1;
            *REG_VRAMRW = (map->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    // sprite shape: position , max zoom
    *REG_VRAMADDR=ADDR_SCB2+map->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(map->y<<7)+map->height;
    *REG_VRAMRW=(map->x<<7);

    // --- On chaine l'ensemble des sprites
    for (u16 v=1; v<map->width; v++) {
        *REG_VRAMADDR=ADDR_SCB2+map->sprite+v;
        *REG_VRAMRW=0xFFF;
        *REG_VRAMRW=1<<6; // sticky
    }
}

void map_update(map_t *map){

    for (u16 s=0; s<map->width; s++) {
        *REG_VRAMMOD=1;
        *REG_VRAMADDR=ADDR_SCB1+((map->sprite+s)*64);
        for (u16 v=0; v<map->height; v++) {
            *REG_VRAMRW = CROM_TILE_OFFSET+map->tmx[v][s]-1;
            *REG_VRAMRW = (map->palette<<8);
        }
    }

    *REG_VRAMMOD=0x200;
    *REG_VRAMADDR=ADDR_SCB2+map->sprite;
    *REG_VRAMRW=0xFFF;
    *REG_VRAMRW=(map->y<<7)+map->height;
    *REG_VRAMRW=(map->x<<7);
}

void map_move_left(map_t *map){
    map->x--;
    map_update(map);
}

void map_move_right(map_t *map){
    map->x++;
    map_update(map);
}

void map_hide(map_t *map){
    map->height=0;
    map_setup(map);
}