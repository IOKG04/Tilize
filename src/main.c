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
#include <SDL2/SDL.h>
#include "atlas.h"
#include "gui.h"
#include "load_png.h"
#include "rgb24.h"
#include "texture.h"

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

    // initialize gui
    if(gui_setup(256, 256, 2)){
        fprintf(stderr, "Failed to initialize gui in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        goto _clean_and_exit;
    }

    // load input image
    rgb24_texture_t input_image = {};
    if(load_png(&input_image, argv[1])){
        goto _clean_and_exit;
    }

    // split up input image
    rgb24_atlas_t atlas = {};
    if(rgb24_atlas_from_texture(&atlas, &input_image, 256, 256)){
        goto _clean_and_exit;
    }

    // perform demo scene
    int c = 0;
    do{
        rgb24_texture_t ctile = {};
        rgb24_atlas_get_tile(&ctile, &atlas, c % atlas.tile_amount_x, c / atlas.tile_amount_x);
        gui_render_texture(0, 0, &ctile);
        rgb24_texture_destroy(&ctile);
        gui_present();
        c = (c + 1) % (atlas.tile_amount_x * atlas.tile_amount_y);
    }while(getchar() != 'q');

    // clean and exit
    _clean_and_exit:;
    rgb24_atlas_destroy(&atlas);
    rgb24_texture_destroy(&input_image);
    gui_free();
    SDL_Quit();
    return 0;
}
