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

#ifndef ATLAS_H__
#define ATLAS_H__

#include "rgb24.h"
#include "texture.h"

// a collection of textures
typedef struct rgb24_atlas_t{
    int       tile_width,
              tile_height,
              tile_amount_x,
              tile_amount_y,
              total_width,   // may differ from tile_width * tile_amount_x if image width isnt evenly divisible by tile_width
              total_height;  // same as above except vertical now
    rgb24_t **data;
} rgb24_atlas_t;

#define RGB24_ATLAS_NULL ((rgb24_atlas_t){0, 0, 0, 0, 0, 0, NULL})

// creates a new atlas
// if total_width or total_height == -1, they are automatically added in
int rgb24_atlas_create(rgb24_atlas_t *atlas, int tile_width, int tile_height, int tile_amount_x, int tile_amount_y, int total_width, int total_height);
// destroyes atlas
void rgb24_atlas_destroy(rgb24_atlas_t *atlas);

// converts atlas to a texture
int rgb24_texture_from_atlas(rgb24_texture_t *restrict texture, const rgb24_atlas_t *restrict atlas);
// splits texture into atlas of {tile_width, tile_height} sized tiles
int rgb24_atlas_from_texture(rgb24_atlas_t *restrict atlas, const rgb24_texture_t *restrict texture, int tile_width, int tile_height);

// gets the tile at {x, y} in atlas as a texture
int rgb24_atlas_get_tile(rgb24_texture_t *restrict tile_texture, const rgb24_atlas_t *restrict atlas, int x, int y);
// sets tile at {x, y} in atlas to tile_texture
int rgb24_atlas_set_tile(rgb24_atlas_t *restrict atlas, const rgb24_texture_t *restrict tile_texture, int x, int y);

#endif
