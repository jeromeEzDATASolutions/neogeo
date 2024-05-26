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

extern lance_t lances[3];