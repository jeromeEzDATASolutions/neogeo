/*
 * Copyright (c) 2020 Jérôme BRACONNIER
 * Structure & functions for palettes
 */

const u16 clut[][16]= {
    /// first 16 colors palette for the fix tiles
    {0x8000, 0x0fff, 0x0333, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    // --- sprite palettes
    #include "sprites/back.pal"
    #include "sprites/herbe.pal"
    #include "sprites/flotte.pal"
    #include "sprites/arthur1.pal"
    #include "sprites/arthur2.pal"
    #include "sprites/nuage.pal"
    #include "sprites/lance.pal"
};

const u16 palette_backup[][16]= {
    /// first 16 colors palette for the fix tiles
    {0x8000, 0x0fff, 0x0333, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    // --- sprite palettes
    #include "sprites/back.pal"
    #include "sprites/herbe.pal"
    #include "sprites/flotte.pal"
    #include "sprites/arthur1.pal"
    #include "sprites/arthur2.pal"
    #include "sprites/nuage.pal"
    #include "sprites/lance.pal"
};

const u16 paletes_from_tile[][2] = {
    {1, 1}, 
    {401, 2}, 
    {417, 3}, 
    {427, 4}, 
    {547, 5}, 
    {667, 6},
    {709, 7},
};

void init_palette() {
    // --- Initialize the two palettes in the first palette bank
    u16 *p=(u16*)clut;
    for (u16 i=0;i<sizeof(clut)/2; i++) {
        MMAP_PALBANK1[i]=p[i];
    }
    //*((volatile u16*)0x401ffe)=0x18bf;
    *((volatile u16*)0x0000)=0x0000;
}

void setPaletteToBlack(int palettes[], int nb_elem) {
    ng_wait_vblank();
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 c=16*palettes[p], l=0; l<16; c++, l++) {
            MMAP_PALBANK1[c] = 0x0000;
        }
    }
}

// -----------------------------
// --- Palette to black
// -----------------------------
void fadeInPalette(int palettes[], int nb_elem) {

    u8 end = 0;
    u16 palette = 0;
    u16 colors_ok[nb_elem][16];
    u16 caching[16];

    // --- Init color OK
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 i=0; i<16; i++) {
            colors_ok[p][i] = 0;
        }
    }

    // --- Caching all colors from MMAP_PALBANK1
    u16 colors_palbank[nb_elem][16][3];
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 c=16*palettes[p], l=0; l<16; c++, l++) {
            colors_palbank[p][l][0] = (MMAP_PALBANK1[c] >> 8) & 0xF;
            colors_palbank[p][l][1] = (MMAP_PALBANK1[c] >> 4) & 0xF;
            colors_palbank[p][l][2] = MMAP_PALBANK1[c] & 0xF;
        }
    }

    while ( end == 0 ) {

        for (u16 p=0; p<nb_elem; p++) {

            for (u16 c=16*palettes[p], l=0; l < 16; c++, l++) {

                if ( colors_palbank[p][l][0] > 0 ) {
                    colors_palbank[p][l][0]--;
                }

                if ( colors_palbank[p][l][1] > 0 ) {
                    colors_palbank[p][l][1]--;
                }

                if ( colors_palbank[p][l][2] > 0 ) {
                    colors_palbank[p][l][2]--;
                }

                caching[l] = (colors_palbank[p][l][0] << 8) | (colors_palbank[p][l][1] << 4) | colors_palbank[p][l][2];

                if ( colors_palbank[p][l][0] == 0 && colors_palbank[p][l][1] == 0 && colors_palbank[p][l][2] == 0 ){
                    colors_ok[p][l] = 1;
                }
            }

            ng_wait_vblank();

            for (u16 c=16*palettes[p], l=0; l < 16; c++, l++) {
                MMAP_PALBANK1[c] = caching[l];
            }
        }

        // --- On checke que tout le tab est rempli afin d'arreter le fondu
        end = 1;
        for (u16 p=0; p<nb_elem; p++) {
            for (u16 i=0; i<16; i++) {
                if ( colors_ok[p][i] == 0 ) {
                    end=0;
                }
            }
        }
    }
}

void fadeOutPalette(int palettes[], int nb_elem) {

    char str[20];
    u8 end = 0;
    u16 palette = 0;
    u16 colors_target[nb_elem][16][3];
    u16 colors_ok[nb_elem][16];
    u16 caching[16];

    // --- Init color OK
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 i=0; i<16; i++) {
            colors_ok[p][i] = 0;
        }
    }

    // --- Caching all colors from MMAP_PALBANK1
    u16 colors_palbank[nb_elem][16][3];
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 c=16*palettes[p], l=0; l<16; c++, l++) {
            colors_palbank[p][l][0] = (MMAP_PALBANK1[c] >> 8) & 0xF;
            colors_palbank[p][l][1] = (MMAP_PALBANK1[c] >> 4) & 0xF;
            colors_palbank[p][l][2] = MMAP_PALBANK1[c] & 0xF;
        }
    }

    // --- Caching colors destination
    for (u16 p=0; p<nb_elem; p++) {
        for (u16 i=0; i<16; i++) {
            colors_target[p][i][0] = (palette_backup[palettes[p]][i] >> 8) & 0xF;
            colors_target[p][i][1] = (palette_backup[palettes[p]][i] >> 4) & 0xF;
            colors_target[p][i][2] = palette_backup[palettes[p]][i] & 0xF;
        }
    }

    while ( end == 0 ) {

        for (u16 p=0; p<nb_elem; p++) {

            for (u16 c=16*palettes[p], l=0; l < 16; c++, l++) {

                if ( colors_palbank[p][l][0] < colors_target[p][l][0] ) {
                    colors_palbank[p][l][0]++;
                }

                if ( colors_palbank[p][l][1] < colors_target[p][l][1] ) {
                    colors_palbank[p][l][1]++;
                }

                if ( colors_palbank[p][l][2] < colors_target[p][l][2] ) {
                    colors_palbank[p][l][2]++;
                }

                caching[l] = (colors_palbank[p][l][0] << 8) | (colors_palbank[p][l][1] << 4) | colors_palbank[p][l][2];

                if ( colors_palbank[p][l][0] >= colors_target[p][l][0] && colors_palbank[p][l][1] >= colors_target[p][l][1] && colors_palbank[p][l][2] >= colors_target[p][l][2] ){
                    colors_ok[p][l] = 1;
                }
            }

            ng_wait_vblank();

            for (u16 c=16*palettes[p], l=0; l < 16; c++, l++) {
                MMAP_PALBANK1[c] = caching[l];
            }
        }

        // --- On checke que tout le tab est rempli afin d'arreter le fondu
        end = 1;
        for (u16 p=0; p<nb_elem; p++) {
            for (u16 i=0; i<16; i++) {
                if ( colors_ok[p][i] == 0 ) {
                    end=0;
                }
            }
        }
    }
}