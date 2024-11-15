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

#include "gui.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include "rgb24.h"

// gui state
static SDL_Window   *gui_window;
static SDL_Renderer *gui_renderer;
static SDL_Surface  *gui_surface;
static int           gui_width,
                     gui_height;

// just some stuff
static const _Bool __r_before_g = offsetof(rgb24_t, r) < offsetof(rgb24_t, g);

// sets up gui to render an image of size {width, height} and multiply its size by scalar for showing
int gui_setup(int width, int height, int scalar){
    // create window
    gui_window = SDL_CreateWindow("Tilize", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scalar, height * scalar, SDL_WINDOW_SHOWN);
    if(!gui_window){
        fprintf(stderr, "Failed to create gui_window in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        return 1;
    }

    // create renderer
    gui_renderer = SDL_CreateRenderer(gui_window, -1, 0);
    if(!gui_renderer){
        fprintf(stderr, "Failed to create gui_renderer in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        return 1;
    }

    // create surface
    if(__r_before_g) gui_surface = SDL_CreateRGBSurface(0, width, height, 24, 0x0000ff, 0x00ff00, 0xff0000, 0);
    else             gui_surface = SDL_CreateRGBSurface(0, width, height, 24, 0xff0000, 0x00ff00, 0x0000ff, 0);
    if(!gui_surface){
        fprintf(stderr, "Failed to create gui_surface in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        return 1;
    }

    // set gui_width and gui_height
    gui_width = width;
    gui_height = height;

    // present initial scene (black screen)
    rgb24_t *pixels = (rgb24_t *)gui_surface->pixels;
    for(int i = 0; i < width * height; ++i){
        pixels[i] = RGB24(0, 0, 0);
    }
    if(gui_present()){
        fprintf(stderr, "Failed to render black scene in %s, %s, %i\n", __FILE__, __func__, __LINE__);
    }

    return 0;
}
// frees everything gui uses
void gui_free(){
    if(gui_surface) SDL_FreeSurface(gui_surface);
    if(gui_renderer) SDL_DestroyRenderer(gui_renderer);
    if(gui_window) SDL_DestroyWindow(gui_window);
}

// renders current visuals to the window
int gui_present(){
    // create texture to be rendered
    SDL_Texture *gui_texture = SDL_CreateTextureFromSurface(gui_renderer, gui_surface);
    if(!gui_texture){
        fprintf(stderr, "Failed to create gui_texture in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        return 1;
    }

    // render said texture to gui_renderer
    if(SDL_RenderCopy(gui_renderer, gui_texture, NULL, NULL)){
        fprintf(stderr, "Failed to render gui_texture to gui_renderer in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        SDL_DestroyTexture(gui_texture);
        return 1;
    }

    // present the new rendered thingy
    SDL_RenderPresent(gui_renderer);

    // cleanup and return
    SDL_DestroyTexture(gui_texture);
    return 0;
}

// sets pixel at {x, y} of gui's internal buffer to color
int gui_set_px(int x, int y, rgb24_t color){
    // check if {x, y} in bounds
    if(x < 0 || x >= gui_width ||
       y < 0 || y >= gui_height){
        return 1;
    }

    // set pixel
    ((rgb24_t *)gui_surface->pixels)[x + y * gui_width] = color;

    return 0;
}
