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

#include "atlas.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// creates a new atlas
// if total_width or total_height == -1, they are automatically added in
int rgb24_atlas_create(rgb24_atlas_t *atlas, int tile_width, int tile_height, int tile_amount_x, int tile_amount_y, int total_width, int total_height){
    // basic stuff
    if(total_width == -1) total_width = tile_amount_x * tile_width;
    if(total_height == -1) total_height = tile_amount_y * tile_height;
    atlas->tile_width = tile_width;
    atlas->tile_height = tile_height;
    atlas->tile_amount_x = tile_amount_x;
    atlas->tile_amount_y = tile_amount_y;
    atlas->total_width = total_width;
    atlas->total_height = total_height;

    // allocations :3
    atlas->data = malloc(tile_amount_x * tile_amount_y * sizeof(rgb24_t *));
    if(!atlas->data){
        fprintf(stderr, "Failed to allocate atlas->data in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }
    for(int i = 0; i < tile_amount_x * tile_amount_y; ++i){
        atlas->data[i] = malloc(tile_width * tile_height * sizeof(*atlas->data[i]));
        if(!atlas->data[i]){
            fprintf(stderr, "Failed to allocate atlas->data[i] in %s, %s, %i\n", __FILE__, __func__, __LINE__);
            return 1;
        }
    }

    return 0;
}
// destroyes atlas
void rgb24_atlas_destroy(rgb24_atlas_t *atlas){
    if(atlas->data){
        for(int i = 0; i < atlas->tile_amount_x * atlas->tile_amount_y; ++i){
            if(atlas->data[i]){
                free(atlas->data[i]);
            }
        }
        free(atlas->data);
        atlas->data = NULL;
    }
    atlas->tile_width = 0;
    atlas->tile_height = 0;
    atlas->tile_amount_x = 0;
    atlas->tile_amount_y = 0;
    atlas->total_width = 0;
    atlas->total_height = 0;
}

// converts atlas to a texture
int rgb24_texture_from_atlas(rgb24_texture_t *restrict texture, const rgb24_atlas_t *restrict atlas){
    // create texture
    if(rgb24_texture_create(texture, atlas->total_width, atlas->total_height)){
        fprintf(stderr, "Failed to initialize texture in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    // copy data
    for(int y1 = 0; y1 < atlas->tile_amount_y; ++y1){
        const int tile_y = y1 * atlas->tile_height;
        for(int x1 = 0; x1 < atlas->tile_amount_x; ++x1){
            const int tile_x = x1 * atlas->tile_width;
            for(int y2 = 0; y2 < atlas->tile_height; ++y2){
                if(y2 + tile_y < 0 || y2 + tile_y >= atlas->total_height) continue;
                for(int x2 = 0; x2 < atlas->tile_width; ++x2){
                    if(x2 + tile_x < 0 || x2 + tile_x >= atlas->total_width) continue;
                    texture->data[(x2 + tile_x) + (y2 + tile_y) * atlas->total_width] = atlas->data[x1 + y1 * atlas->tile_amount_x][x2 + y2 * atlas->tile_width];
                }
            }
        }
    }

    return 0;
}
// splits texture into atlas of {tile_width, tile_height} sized tiles
int rgb24_atlas_from_texture(rgb24_atlas_t *restrict atlas, const rgb24_texture_t *restrict texture, int tile_width, int tile_height){
    // create atlas
    const int tile_amount_x = (int)ceil(texture->width / (double)tile_width),
              tile_amount_y = (int)ceil(texture->height / (double)tile_height);
    if(rgb24_atlas_create(atlas, tile_width, tile_height, tile_amount_x, tile_amount_y, texture->width, texture->height)){
        fprintf(stderr, "Failed to create texture in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    // copy data
    for(int y1 = 0; y1 < tile_amount_y; ++y1){
        const int tile_y = y1 * tile_height;
        for(int x1 = 0; x1 < tile_amount_x; ++x1){
            const int tile_x = x1 * tile_width;
            for(int y2 = 0; y2 < tile_height; ++y2){
                if(y2 + tile_y < 0 || y2 + tile_y >= texture->height) continue;
                for(int x2 = 0; x2 < tile_width; ++x2){
                    if(x2 + tile_x < 0 || x2 + tile_x >= texture->width) continue;
                    atlas->data[x1 + y1 * atlas->tile_amount_x][x2 + y2 * atlas->tile_width] = texture->data[(x2 + tile_x) + (y2 + tile_y) * atlas->total_width];
                }
            }
        }
    }

    return 0;
}

// gets the tile at {x, y} in atlas as a texture
int rgb24_atlas_get_tile(rgb24_texture_t *restrict tile_texture, const rgb24_atlas_t *restrict atlas, int x, int y){
    // create tile_texture
    if(rgb24_texture_create(tile_texture, atlas->tile_width, atlas->tile_height)){
        fprintf(stderr, "Failed to create tile_texture in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    // copy data
    for(int i = 0; i < atlas->tile_width * atlas->tile_height; ++i){
        tile_texture->data[i] = atlas->data[x + y * atlas->tile_amount_x][i];
    }

    return 0;
}
