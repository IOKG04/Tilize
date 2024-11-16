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

#include "load_png.h"

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "texture.h"

// loads texture from png_path
int load_png(rgb24_texture_t *texture, const char *png_path){
    // initialize sdl_image
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        fprintf(stderr, "Failed to initialize SDL_image in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, IMG_GetError());
        return 1;
    }

    // load unconverted image
    SDL_Surface *unconverted = IMG_Load(png_path);
    if(!unconverted){
        fprintf(stderr, "Failed to load %s in %s, %s, %i:\n%s\n", png_path, __FILE__, __func__, __LINE__, IMG_GetError());
        IMG_Quit();
        return 1;
    }

    // convert image to format
    SDL_Surface *converted = SDL_ConvertSurfaceFormat(unconverted, SDL_PIXELFORMAT_RGBA8888, 0);
    if(!converted){
        fprintf(stderr, "Failed to convert unconverted to create converted in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, SDL_GetError());
        SDL_FreeSurface(unconverted);
        IMG_Quit();
        return 1;
    }

    // create texture
    if(rgb24_texture_create(texture, converted->w, converted->h)){
        fprintf(stderr, "Failed to create texture in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        SDL_FreeSurface(converted);
        SDL_FreeSurface(unconverted);
        IMG_Quit();
        return 1;
    }

    // load data into texture
    SDL_LockSurface(converted);
    uint32_t *pixels = (uint32_t *)converted->pixels;
    for(int i = 0; i < converted->w * converted->h; ++i){
        texture->data[i].r = (pixels[i] >> 24) & 0xff;
        texture->data[i].g = (pixels[i] >> 16) & 0xff;
        texture->data[i].b = (pixels[i] >>  8) & 0xff;
    }
    SDL_UnlockSurface(converted);

    // clean and return
    SDL_FreeSurface(converted);
    SDL_FreeSurface(unconverted);
    IMG_Quit();
    return 0;
}
