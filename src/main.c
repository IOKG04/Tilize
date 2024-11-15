/************************************************\
| MIT License                                    |
|                                                |
| Copyright (c) 2024 rue04                       |
|                                                |
| Permission is hereby granted, free of charge,  |
| to any person obtaining a copy of this         |
| software and associated documentation files    |
| (the "Software"), to deal in the Software      |
| without restriction, including without         |
| limitation the rights to use, copy, modify,    |
| merge, publish, distribute, sublicense, and/or |
| sell copies of the Software, and to permit     |
| persons to whom the Software is furnished to   |
| do so, subject to the following conditions:    |
|                                                |
| The above copyright notice and this permission |
| notice shall be included in all copies or      |
| substantial portions of the Software.          |
|                                                |
| THE SOFTWARE IS PROVIDED "AS IS", WITHOUT      |
| WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,      |
| INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF |
| MERCHANTABILITY, FITNESS FOR A PARTICULAR      |
| PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL |
| THE AUTHORS OR COPYRIGHT HILDERS BE LIABLE FOR |
| ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER |
| IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   |
| ARISING FROM, OUT OF OR IN CONNECTION WITH THE |
| SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   |
| SOFTWARE.                                      |
\************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "atlas.h"
#include "gui.h"
#include "load_png.h"
#include "rgb24.h"
#include "texture.h"

#define TILE_SIZE 8

int main(int argc, char **argv){
    if(argc < 2){
        // TODO: update usage message as stuff gets added
        printf("Too few arguments\n\n");
        printf("Usage:\n");
        printf(" %s [.png]\tShows [.png]\n", argv[0]);
        return 1;
    }

    // initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "Failed to initialize SDL in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        goto _clean_and_exit;
    }

    // load and split input image
    rgb24_texture_t input_image = {};
    if(load_png(&input_image, argv[1])) goto _clean_and_exit;
    rgb24_atlas_t input_atlas = {};
    if(rgb24_atlas_from_texture(&input_atlas, &input_image, TILE_SIZE, TILE_SIZE)) goto _clean_and_exit;

    // initialize gui
    if(gui_setup(input_image.width, input_image.height, 1)){
        fprintf(stderr, "Failed to initialize gui in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        goto _clean_and_exit;
    }

    // load tiles
    rgb24_texture_t tiles_texture = {};
    if(load_png(&tiles_texture, "resources/extiles_8x8.png")) goto _clean_and_exit;
    rgb24_atlas_t tiles_atlas = {};
    if(rgb24_atlas_from_texture(&tiles_atlas, &tiles_texture, TILE_SIZE, TILE_SIZE)) goto _clean_and_exit;
    rgb24_texture_t *tiles = calloc(tiles_atlas.tile_amount_x * tiles_atlas.tile_amount_y, sizeof(*tiles));
    if(!tiles) goto _clean_and_exit;
    for(int i = 0; i < tiles_atlas.tile_amount_x * tiles_atlas.tile_amount_y; ++i){
        if(rgb24_atlas_get_tile(&tiles[i], &tiles_atlas, i % tiles_atlas.tile_amount_x, i / tiles_atlas.tile_amount_x)) goto _clean_and_exit;
    }

    // do the thing
    Uint64 thing_start = SDL_GetTicks64();
    gui_render_texture(0, 0, &input_image);
    gui_present();
    for(int i = 0; i < input_atlas.tile_amount_x * input_atlas.tile_amount_y; ++i){
        const int inp_x = i % input_atlas.tile_amount_x,
                  inp_y = i / input_atlas.tile_amount_x;
        rgb24_texture_t current_tile = {};
        if(rgb24_atlas_get_tile(&current_tile, &input_atlas, inp_x, inp_y)) goto _clean_and_exit;
        int min_diff_index = 0,
            min_diff       = INT_MAX;
        for(int j = 0; j < tiles_atlas.tile_amount_x * tiles_atlas.tile_amount_y; ++j){
            int diff = 0;
            for(int k = 0; k < TILE_SIZE * TILE_SIZE; ++k){
                diff += abs(current_tile.data[k].r - (int)tiles[j].data[k].r);
                diff += abs(current_tile.data[k].g - (int)tiles[j].data[k].g);
                diff += abs(current_tile.data[k].b - (int)tiles[j].data[k].b);
            }
            if(diff < min_diff){
                min_diff = diff;
                min_diff_index = j;
            }
        }
        rgb24_texture_destroy(&current_tile);
        gui_render_texture(inp_x * TILE_SIZE, inp_y * TILE_SIZE, &tiles[min_diff_index]);
        gui_present();
    }
    Uint64 thing_end = SDL_GetTicks64();
    printf("That took %llu ms to complete\n", (long long unsigned)(thing_end - thing_start));

    // wait to exit
    while(getchar() != '\n');

    // clean and exit
    _clean_and_exit:;
    if(tiles){
        for(int i = 0; i < tiles_atlas.tile_amount_x * tiles_atlas.tile_amount_y; ++i){
            rgb24_texture_destroy(&tiles[i]);
        }
        free(tiles);
    }
    rgb24_atlas_destroy(&tiles_atlas);
    rgb24_texture_destroy(&tiles_texture);
    rgb24_atlas_destroy(&input_atlas);
    rgb24_texture_destroy(&input_image);
    gui_free();
    SDL_Quit();
    return 0;
}
