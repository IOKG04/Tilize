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
#include <stdint.h>
#include <SDL2/SDL.h>
#include "rgb24.h"
#include "texture.h"
#include "tinycthread.h"

// gui state
static SDL_Window   *gui_window;
static SDL_Renderer *gui_renderer;
static SDL_Surface  *gui_surface;
static int           gui_width,
                     gui_height;
static mtx_t         render_mtx,
                     present_mtx;

// sets up gui to render an image of size {width, height} and multiply its size by scalar for showing
int gui_setup(int width, int height, int scalar){
    // initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "Failed to initialize SDL in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        return 1;
    }

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
    gui_surface = SDL_CreateRGBSurface(0, width, height, 32, 0xff0000, 0x00ff00, 0x0000ff, 0);
    if(!gui_surface){
        fprintf(stderr, "Failed to create gui_surface in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        return 1;
    }

    // set gui_width and gui_height
    gui_width = width;
    gui_height = height;

    // initialize mutexes
    if(mtx_init(&render_mtx, mtx_plain) != thrd_success){
        fprintf(stderr, "Failed to initialize render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }
    if(mtx_init(&present_mtx, mtx_plain) != thrd_success){
        fprintf(stderr, "Failed to initialize present_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    // present initial scene (black screen)
    uint32_t *pixels = (uint32_t *)gui_surface->pixels;
    for(int i = 0; i < width * height; ++i){
        pixels[i] = 0;
    }
    if(gui_present()){
        fprintf(stderr, "Failed to render black scene in %s, %s, %i\n", __FILE__, __func__, __LINE__);
    }

    return 0;
}
// frees everything gui uses
void gui_free(){
    mtx_destroy(&render_mtx);
    mtx_destroy(&present_mtx);
    if(gui_surface){
        SDL_UnlockSurface(gui_surface);
        SDL_FreeSurface(gui_surface);
    }
    if(gui_renderer) SDL_DestroyRenderer(gui_renderer);
    if(gui_window) SDL_DestroyWindow(gui_window);
    SDL_Quit();
}

// renders current visuals to the window
int gui_present(){
    int trylock_success = mtx_trylock(&present_mtx);
    if(trylock_success == thrd_busy) return 0;
    if(trylock_success != thrd_success){
        fprintf(stderr, "Failed to lock present_mtx for some reason that isn't it being busy in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    if(mtx_lock(&render_mtx) != thrd_success){
        fprintf(stderr, "Failed to lock render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        if(mtx_unlock(&present_mtx) != thrd_success){
            fprintf(stderr, "Failed to unlock present_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        }
        return 1;
    }
    SDL_UnlockSurface(gui_surface);
    // create texture to be rendered
    SDL_Texture *gui_texture = SDL_CreateTextureFromSurface(gui_renderer, gui_surface);
    if(!gui_texture){
        fprintf(stderr, "Failed to create gui_texture in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        if(mtx_unlock(&render_mtx) != thrd_success){
            fprintf(stderr, "Failed to unlock render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        }
        if(mtx_unlock(&present_mtx) != thrd_success){
            fprintf(stderr, "Failed to unlock present_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        }
        return 1;
    }
    SDL_LockSurface(gui_surface);
    if(mtx_unlock(&render_mtx) != thrd_success){
        fprintf(stderr, "Failed to unlock render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        if(mtx_unlock(&present_mtx) != thrd_success){
            fprintf(stderr, "Failed to unlock present_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        }
        SDL_DestroyTexture(gui_texture);
        return 1;
    }

    // render said texture to gui_renderer
    if(SDL_RenderCopy(gui_renderer, gui_texture, NULL, NULL)){
        fprintf(stderr, "Failed to render gui_texture to gui_renderer in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        SDL_DestroyTexture(gui_texture);
        SDL_LockSurface(gui_surface);
        return 1;
    }

    // present the new rendered thingy
    SDL_RenderPresent(gui_renderer);

    // cleanup and return
    SDL_DestroyTexture(gui_texture);
    if(mtx_unlock(&present_mtx) != thrd_success){
        fprintf(stderr, "Failed to unlock present_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }
    return 0;
}

// sets pixel at {x, y} of gui's internal buffer to color
int gui_set_px(int x, int y, rgb24_t color){
    if(mtx_lock(&render_mtx) != thrd_success){
        fprintf(stderr, "Failed to lock render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    // check if {x, y} in bounds
    if(x < 0 || x >= gui_width ||
       y < 0 || y >= gui_height){
        return 1;
    }

    // set pixel
    SDL_LockSurface(gui_surface);
    ((uint32_t *)gui_surface->pixels)[x + y * gui_width] = (color.r << 16) |
                                                           (color.g <<  8) |
                                                           (color.b <<  0);
    SDL_UnlockSurface(gui_surface);

    if(mtx_unlock(&render_mtx) != thrd_success){
        fprintf(stderr, "Failed to lock render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }
    return 0;
}
// renders texture to gui's internal buffer at {x, y}
// returns amount of pixels not rendered
int gui_render_texture(int x, int y, const rgb24_texture_t *texture){
    if(mtx_lock(&render_mtx) != thrd_success){
        fprintf(stderr, "Failed to lock render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    int outp = 0;

    // render texture
    SDL_LockSurface(gui_surface);
    for(int tex_y = 0; tex_y < texture->height; ++tex_y){
        if(tex_y + y < 0 || tex_y + y >= gui_height){
            outp += texture->width;
            continue;
        }
        for(int tex_x = 0; tex_x < texture->width; ++tex_x){
            if(tex_x + x < 0 || tex_x + x >= gui_width){
                ++outp;
                continue;
            }
            const rgb24_t texture_px = texture->data[tex_x + tex_y * texture->width];
            ((uint32_t *)gui_surface->pixels)[(tex_x + x) + (tex_y + y) * gui_width] = (texture_px.r << 16) |
                                                                                       (texture_px.g <<  8) |
                                                                                       (texture_px.b <<  0);
        }
    }
    SDL_UnlockSurface(gui_surface);

    if(mtx_unlock(&render_mtx) != thrd_success){
        fprintf(stderr, "Failed to lock render_mtx in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }
    return outp;
}
