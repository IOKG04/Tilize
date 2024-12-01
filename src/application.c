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

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdatomic.h>
#include "atlas.h"
#include "configuration.h"
#include "gui.h"
#include "load_png.h"
#include "print.h"
#include "texture.h"
#include "tinycthread.h"
#if GUI_SUPPORTED
    #include <SDL2/SDL.h>
#endif

// try to present to gui every ... itterations of the loop (to speed up stuff obv)
#define PRESENT_ITERATIONS 16

// all data needed by a thread running the application
struct app_thread_data{
    int ct_min, ct_max; // minimum and maximum (exclusive) tile indecies computed by thread
    rgb24_atlas_t *input_atlas;
    atomic_int *running;
    #if GUI_SUPPORTED
        int check_sdl;
    #endif
};

// data currently operated on
static int            num_threads;
static int            num_colors;
static rgb24_t       *colors;
static rgb24_atlas_t  pattern_atlas;
static int            col1_min,
                      col1_max,
                      col2_min,
                      col2_max;
static const char    *output_file;
#if GUI_SUPPORTED
    static int        show_gui;
#endif

// performs the loop that does the thing
static int process_loop(void *input_data_void);
// gets difference between a pattern with specific colors and an rgb24_t array
static unsigned long get_difference(const rgb24_t *restrict data, int pattern_index, int col1, int col2);

// sets up application with the provided configs
int application_setup(const tilize_config_t *restrict tilize_config, const flag_config_t *restrict flag_config){
    // load pattern texture
    rgb24_texture_t pattern_texture = RGB24_TEXTURE_NULL;
    if(!(flag_config->config_path == NULL || tilize_config->pattern_path == NULL)){
        // path provided
        // get final pattern path
        #define PP_LEN 255
        char pattern_path[PP_LEN + 1] = "";
        strncpy(pattern_path, flag_config->config_path, PP_LEN);
        char *last_slash = strrchr(pattern_path, '/');
        if(last_slash == NULL) last_slash = strrchr(pattern_path, '\\'); // retry with '\'
        if(last_slash == NULL){
            strncpy(pattern_path, tilize_config->pattern_path, PP_LEN);
        }
        else{
            ++last_slash;
            strncpy(last_slash, tilize_config->pattern_path, PP_LEN - (last_slash - pattern_path));
        }
        #undef PP_LEN

        // load pattern as texture
        if(load_png(&pattern_texture, pattern_path)){
            VERRPRINT(0, "Failed to load pattern_texture");
            return 1;
        }
    }
    else{
        // path not provided, generating at runtime
        // is based on default tilize config defined in `main.c` in `main()`
        if(rgb24_texture_create(&pattern_texture, 8, 4)){
            VERRPRINT(0, "Failed to create pattern_texture");
            return 1;
        }
        for(int i = 0; i < 4 * 8; ++i){ // lets just hope the compiler takes care of optimizing this
            const int x = i % 8,
                      y = i / 8;
            if(x <= 1 || (x >= 4 && y <= 1)) pattern_texture.data[i] = RGB24(0xff, 0xff, 0xff);
            else                             pattern_texture.data[i] = RGB24(0x00, 0x00, 0x00);
        }
    }

    // split pattern_texture into pattern_atlas and clean
    if(rgb24_atlas_from_texture(&pattern_atlas, &pattern_texture, tilize_config->tile_width, tilize_config->tile_height)){
        VERRPRINT(0, "Failed to split pattern_texture into pattern_atlas");
        rgb24_texture_destroy(&pattern_texture);
        return 1;
    }
    rgb24_texture_destroy(&pattern_texture);

    // copy colors
    num_colors = tilize_config->num_colors;
    colors = malloc(num_colors * sizeof(*colors));
    if(!colors){
        VERRPRINT(0, "Failed to allocate colors");
        rgb24_atlas_destroy(&pattern_atlas);
        return 1;
    }
    for(int i = 0; i < num_colors; ++i){
        colors[i] = tilize_config->colors[i];
    }

    // get bckg_color and forg_color
    if(tilize_config->bckg_color != -1){
        col2_min = tilize_config->bckg_color;
        col2_max = tilize_config->bckg_color + 1;
    }
    else{
        col2_min = 0;
        col2_max = num_colors;
    }
    if(tilize_config->forg_color != -1){
        col1_min = tilize_config->forg_color;
        col1_max = tilize_config->forg_color + 1;
    }
    else{
        col1_min = 0;
        col1_max = num_colors;
    }

    // misc
    num_threads = flag_config->num_threads;
    if(num_threads < 1) num_threads = 1;
    output_file = flag_config->file_outp_path;
    #if GUI_SUPPORTED
        show_gui = flag_config->showgui;
    #endif

    return 0;
}
// frees everything application uses
void application_free(void){
    num_colors = 0;
    if(colors) free(colors);
    rgb24_atlas_destroy(&pattern_atlas);
}

// processes input
int application_process(const rgb24_texture_t *restrict input_texture){
    // split input_texture into input_atlas
    rgb24_atlas_t input_atlas = RGB24_ATLAS_NULL;
    if(rgb24_atlas_from_texture(&input_atlas, input_texture, pattern_atlas.tile_width, pattern_atlas.tile_height)){
        VERRPRINT(0, "Failed to split input_texture into input_atlas");
        return 1;
    }

    #if GUI_SUPPORTED
        if(!show_gui) goto _post_show_prev;
        // show previous image
        gui_render_texture(0, 0, input_texture);
        gui_present();
        _post_show_prev:;
    #endif

    // do the thing
    atomic_int running;
    atomic_store(&running, 1);
    thrd_t                 *process_threads = NULL;
    struct app_thread_data *thread_data     = NULL;
    if(num_threads > 1){
        process_threads = malloc((num_threads - 1) * sizeof(*process_threads));
        if(!process_threads){
            VERRPRINT(0, "Failed to allocate process_threads");
            goto _main_process;
        }
        thread_data = malloc((num_threads - 1) * sizeof(*thread_data));
        if(!thread_data){
            VERRPRINT(0, "Failed to allocate thread_data");
            goto _main_process;
        }
        for(int i = 0; i < num_threads - 1; ++i){
            thread_data[i].ct_min        = (input_atlas.tile_amount_x * input_atlas.tile_amount_y) * (i + 1) / num_threads;
            thread_data[i].ct_max        = (input_atlas.tile_amount_x * input_atlas.tile_amount_y) * (i + 2) / num_threads;
            thread_data[i].input_atlas   = &input_atlas;
            thread_data[i].running       = &running;
            #if GUI_SUPPORTED
                thread_data[i].check_sdl = (i == 2) ? 1 : 0;
            #endif
            if(thrd_create(&process_threads[i], &process_loop, &thread_data[i]) != thrd_success){
                VERRPRINTF(0, "Failed to initialize process_threads[%i]", i);
                goto _main_process;
            }
        }
    }
    _main_process:;
    struct app_thread_data main_thrd_data;
    main_thrd_data.ct_min        = 0;
    main_thrd_data.ct_max        = input_atlas.tile_amount_x * input_atlas.tile_amount_y / num_threads;
    main_thrd_data.input_atlas   = &input_atlas;
    main_thrd_data.running       = &running;
    #if GUI_SUPPORTED
        main_thrd_data.check_sdl = (num_threads > 1) ? 0 : 1;
    #endif
    int total_ret_code = 0;
    total_ret_code |= process_loop(&main_thrd_data);
    if(num_threads > 1){
        if(process_threads){
            for(int i = 0; i < num_threads - 1; ++i){
                int ret_code;
                thrd_join(process_threads[i], &ret_code);
                total_ret_code |= ret_code;
            }
            free(process_threads);
        }
        if(thread_data) free(thread_data);
    }
    #if GUI_SUPPORTED
        if(show_gui) gui_present();
    #endif

    if(total_ret_code & 1){
        VERRPRINT(0, "Failed to complete all threads");
        rgb24_atlas_destroy(&input_atlas);
        return 1;
    }
    else if(total_ret_code & 2){
        VPRINT(1, "Cancelled Tilizing\n");
        rgb24_atlas_destroy(&input_atlas);
        return 0;
    }

    // output to file
    if(output_file){
        rgb24_texture_t output_texture = RGB24_TEXTURE_NULL;
        if(rgb24_texture_from_atlas(&output_texture, &input_atlas)){
            VERRPRINT(0, "Failed to generate output_texture from input_atlas");
            rgb24_atlas_destroy(&input_atlas);
            return 1;
        }
        if(save_png(output_file, &output_texture)){
            VERRPRINTF(0, "Failed to save output_texture to %s", output_file);
            rgb24_texture_destroy(&output_texture);
            rgb24_atlas_destroy(&input_atlas);
            return 1;
        }
        rgb24_texture_destroy(&output_texture);
    }

    // clean and return
    rgb24_atlas_destroy(&input_atlas);
    return 0;
}

// performs the loop that does the thing
static int process_loop(void *input_data_void){
    // macros for convenience
    #define input_data  ((struct app_thread_data *)input_data_void)
    #define input_atlas (input_data->input_atlas)
    const int ct_min = input_data->ct_min,
              ct_max = input_data->ct_max;
    for(int ct_i = ct_min; ct_i < ct_max; ++ct_i){
        if(!atomic_load(input_data->running)) return 2; // exit if told to do so
        const int ct_x = ct_i % input_atlas->tile_amount_x,
                  ct_y = ct_i / input_atlas->tile_amount_x;

        // do the thing
        // TODO: make more readable and such
        const rgb24_t *current_tile = input_atlas->data[ct_i];
        unsigned long lowest_diff = ULONG_MAX;
        int lowest_pt   = 0,
            lowest_col1 = 0,
            lowest_col2 = 0;
        for(int pt_i = 0; pt_i < pattern_atlas.tile_amount_x * pattern_atlas.tile_amount_y; ++pt_i){
            for(int col1 = col1_min; col1 < col1_max; ++col1){
                for(int col2 = col2_min; col2 < col2_max; ++col2){
                    unsigned long difference = get_difference(current_tile, pt_i, col1, col2);
                    if(difference < lowest_diff){
                        lowest_diff = difference;
                        lowest_pt   = pt_i;
                        lowest_col1 = col1;
                        lowest_col2 = col2;
                    }
                }
            }
        }

        // colorize best tile
        rgb24_texture_t best_pattern_colorized = RGB24_TEXTURE_NULL;
        if(rgb24_atlas_get_tile(&best_pattern_colorized, &pattern_atlas, lowest_pt % pattern_atlas.tile_amount_x, lowest_pt / pattern_atlas.tile_amount_x)){
            VERRPRINTF(0, "Failed to get best_pattern_colorized (lowest_pt = %i) from pattern_atlas", lowest_pt);
            return 1;
        }
        for(int i = 0; i < best_pattern_colorized.width * best_pattern_colorized.height; ++i){
            if(best_pattern_colorized.data[i].r >= 0x80) best_pattern_colorized.data[i] = colors[lowest_col1];
            else                                         best_pattern_colorized.data[i] = colors[lowest_col2];
        }
        #if GUI_SUPPORTED
            // render best tile to gui
            if(show_gui){
                gui_render_texture(ct_x * input_atlas->tile_width, ct_y * input_atlas->tile_height, &best_pattern_colorized);
                // try presenting every ... tiles
                if(ct_i % PRESENT_ITERATIONS == 0) gui_present();
            }
            if(input_data->check_sdl){
                SDL_PumpEvents();
                SDL_Event e;
                while(SDL_PollEvent(&e)){
                    switch(e.type){
                        case SDL_QUIT:
                            atomic_store(input_data->running, 0);
                            break;
                        case SDL_KEYDOWN:
                            if(e.key.keysym.scancode == SDL_SCANCODE_Q ||
                               e.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
                                atomic_store(input_data->running, 0);
                            }
                            break;
                    }
                }
            }
        #endif
        // save best tile to input_atlas
        rgb24_atlas_set_tile(input_atlas, &best_pattern_colorized, ct_x, ct_y);
        // clean up
        rgb24_texture_destroy(&best_pattern_colorized);
    }
    return 0;
    #undef input_data
    #undef input_atlas
}
// gets difference between a pattern with specific colors and an rgb24_t array
static unsigned long get_difference(const rgb24_t *restrict current_tile, int pattern_index, int col1, int col2){
    unsigned long difference = 0;
    const rgb24_t *pattern_tile = pattern_atlas.data[pattern_index];
    for(int i = 0; i < pattern_atlas.tile_width * pattern_atlas.tile_height; ++i){
        rgb24_t col_cmp;
        if(pattern_tile[i].r >= 0x80) col_cmp = colors[col1];
        else                          col_cmp = colors[col2];
        difference += abs(col_cmp.r - (int)current_tile[i].r);
        difference += abs(col_cmp.g - (int)current_tile[i].g);
        difference += abs(col_cmp.b - (int)current_tile[i].b);
    }
    return difference;
}
