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

#include "application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "atlas.h"
#include "configuration.h"
#include "gui.h"
#include "load_png.h"
#include "texture.h"

// data currently operated on
static int            num_colors;
static rgb24_t       *colors;
static rgb24_atlas_t  pattern_atlas;

// sets up application with the provided configs
int application_setup(const tilize_config_t *restrict tilize_config, const flag_config_t *restrict flag_config){
    // get final pattern path
    #define PP_LEN 256
    char pattern_path[PP_LEN] = {};
    strncpy(pattern_path, flag_config->config_path, PP_LEN);
    char *last_slash = strrchr(pattern_path, '/');
    if(last_slash == NULL){
        strncpy(pattern_path, tilize_config->pattern_path, PP_LEN);
    }
    else{
        ++last_slash;
        strncpy(last_slash, tilize_config->pattern_path, PP_LEN - (last_slash - pattern_path));
    }
    #undef PP_LEN

    // load pattern as texture
    rgb24_texture_t pattern_texture = {};
    if(load_png(&pattern_texture, pattern_path)){
        fprintf(stderr, "Failed to load pattern_texture in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    // split pattern_texture into pattern_atlas and clean
    if(rgb24_atlas_from_texture(&pattern_atlas, &pattern_texture, tilize_config->tile_width, tilize_config->tile_height)){
        fprintf(stderr, "Failed to split pattern_texture into pattern_atlas in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        rgb24_texture_destroy(&pattern_texture);
        return 1;
    }
    rgb24_texture_destroy(&pattern_texture);

    // copy colors
    num_colors = tilize_config->num_colors;
    colors = malloc(num_colors * sizeof(*colors));
    if(!colors){
        fprintf(stderr, "Failed to allocate colors in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        rgb24_atlas_destroy(&pattern_atlas);
        return 1;
    }
    for(int i = 0; i < num_colors; ++i){
        colors[i] = tilize_config->colors[i];
    }

    return 0;
}
// frees everything application uses
void application_free(){
    num_colors = 0;
    if(colors) free(colors);
    rgb24_atlas_destroy(&pattern_atlas);
}

// processes input
int application_process(const rgb24_texture_t *restrict input_texture){
    // split input_texture into input_atlas
    rgb24_atlas_t input_atlas = {};
    if(rgb24_atlas_from_texture(&input_atlas, input_texture, pattern_atlas.tile_width, pattern_atlas.tile_height)){
        fprintf(stderr, "Failed to split input_texture into input_atlas in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    // show previous image
    gui_render_texture(0, 0, input_texture);
    gui_present();

    // do the thing
    int ct_i = 0;
    while(ct_i < input_atlas.tile_amount_x * input_atlas.tile_amount_y){
        const int ct_x = ct_i % input_atlas.tile_amount_x,
                  ct_y = ct_i / input_atlas.tile_amount_x;

        // do the thing
        // TODO: make more readable and such
        const rgb24_t *current_tile = input_atlas.data[ct_i];
        unsigned long lowest_diff = ULONG_MAX;
        int lowest_pt, lowest_col1, lowest_col2;
        for(int pt_i = 0; pt_i < pattern_atlas.tile_amount_x * pattern_atlas.tile_amount_y; ++pt_i){
            const rgb24_t *pattern_tile = pattern_atlas.data[pt_i];
            for(int col1 = 0; col1 < num_colors; ++col1){
                for(int col2 = 0; col2 < num_colors; ++col2){
                    unsigned long difference = 0;
                    for(int i = 0; i < pattern_atlas.tile_width * pattern_atlas.tile_height; ++i){
                        rgb24_t col_cmp;
                        if(pattern_tile[i].r >= 0x80) col_cmp = colors[col1];
                        else                          col_cmp = colors[col2];
                        difference += abs(col_cmp.r - (int)current_tile[i].r);
                        difference += abs(col_cmp.g - (int)current_tile[i].g);
                        difference += abs(col_cmp.b - (int)current_tile[i].b);
                    }
                    if(difference < lowest_diff){
                        lowest_diff = difference;
                        lowest_pt   = pt_i;
                        lowest_col1 = col1;
                        lowest_col2 = col2;
                    }
                }
            }
        }

        // colorize best tile and render it to gui
        rgb24_texture_t best_pattern_colorized = {};
        if(rgb24_atlas_get_tile(&best_pattern_colorized, &pattern_atlas, lowest_pt % pattern_atlas.tile_amount_x, lowest_pt / pattern_atlas.tile_amount_x)){
            fprintf(stderr, "Failed to get best_pattern_colorized (lowest_pt = %i) from pattern_atlas in %s, %s, %i\n", lowest_pt, __FILE__, __func__, __LINE__);
            rgb24_atlas_destroy(&input_atlas);
            return 1;
        }
        for(int i = 0; i < best_pattern_colorized.width * best_pattern_colorized.height; ++i){
            if(best_pattern_colorized.data[i].r >= 0x80) best_pattern_colorized.data[i] = colors[lowest_col1];
            else                                         best_pattern_colorized.data[i] = colors[lowest_col2];
        }
        gui_render_texture(ct_x * input_atlas.tile_width, ct_y * input_atlas.tile_height, &best_pattern_colorized);
        gui_present();
        rgb24_texture_destroy(&best_pattern_colorized);

        ++ct_i;
    }

    // clean and return
    rgb24_atlas_destroy(&input_atlas);
    return 0;
}
