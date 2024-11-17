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
#include "application.h"
#include "atlas.h"
#include "gui.h"
#include "load_png.h"
#include "rgb24.h"
#include "texture.h"

static const tilize_config_t default_tilize_config = {"extiles_24x24.png", 24, 24, 8, (rgb24_t[8]){RGB24(0x00,0x00,0x00),
                                                                                                   RGB24(0x00,0x00,0xff),
                                                                                                   RGB24(0x00,0xff,0x00),
                                                                                                   RGB24(0x00,0xff,0xff),
                                                                                                   RGB24(0xff,0x00,0x00),
                                                                                                   RGB24(0xff,0x00,0xff),
                                                                                                   RGB24(0xff,0xff,0x00),
                                                                                                   RGB24(0xff,0xff,0xff)}};
static const flag_config_t   default_flag_config   = {4, "resources/exconfig.json"};

int main(int argc, char **argv){
    if(argc < 2){
        // TODO: update usage message as stuff gets added
        printf("Too few arguments\n\n");
        printf("Usage:\n");
        printf(" %s [.png]\tProcesses [.png]\n", argv[0]);
        return 1;
    }

    // initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "Failed to initialize SDL in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        goto _clean_and_exit;
    }

    // load input image
    rgb24_texture_t input_image = {};
    if(load_png(&input_image, argv[1])){
        fprintf(stderr, "Failed to load input_image in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        goto _clean_and_exit;
    }

    // resize input image so it fits
    const int ideal_width  = (int)ceil(input_image.width / (double)default_tilize_config.tile_width) * default_tilize_config.tile_width,
              ideal_height = (int)ceil(input_image.height / (double)default_tilize_config.tile_height) * default_tilize_config.tile_height;
    if(input_image.width != ideal_width || input_image.height != ideal_height){
        fprintf(stderr, "Resizing input_image from {%i, %i} to {%i, %i}", input_image.width, input_image.height, ideal_width, ideal_height);
        if(rgb24_texture_resize(&input_image, ideal_width, ideal_height)){
            fprintf(stderr, "Failed to resize input_image in %s, %s, %i\n", __FILE__, __func__, __LINE__);
            goto _clean_and_exit;
        }
        fprintf(stderr, " / Done\n");
    }

    // initialize gui
    if(gui_setup(input_image.width, input_image.height, 1)){
        fprintf(stderr, "Failed to initialize gui in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        goto _clean_and_exit;
    }

    // do the thing
    if(application_setup(&default_tilize_config, &default_flag_config)){
        fprintf(stderr, "Failed to setup application in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        goto _clean_and_exit;
    }
    Uint64 process_start = SDL_GetTicks64();
    if(application_process(&input_image)){
        fprintf(stderr, "Failed to process input_image in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        goto _clean_and_exit;
    }
    Uint64 process_end   = SDL_GetTicks64();
    printf("Finished in %llu ms\n", (long long unsigned)(process_end - process_start));
    application_free();

    // wait to exit
    while(getchar() != '\n');

    // clean and exit
    _clean_and_exit:;
    gui_free();
    rgb24_texture_destroy(&input_image);
    SDL_Quit();
    return 0;
}
