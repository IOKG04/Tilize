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

#include "texture.h"

#include <stdlib.h>
#include "print.h"

// creates a new texture
int rgb24_texture_create(rgb24_texture_t *texture, int width, int height){
    texture->width = width;
    texture->height = height;
    texture->data = malloc(width * height * sizeof(*texture->data));
    if(!texture->data){
        VERRPRINT(0, "Failed to allocate texture->data");
        return 1;
    }
    return 0;
}
// destroyes texture
void rgb24_texture_destroy(rgb24_texture_t *texture){
    texture->width = 0;
    texture->height = 0;
    if(texture->data){
        free(texture->data);
        texture->data = NULL;
    }
}

// replaces texture with a new one of size {new_width, new_height}, such that the original is in the top left and everything else gets filled with black
int rgb24_texture_resize(rgb24_texture_t *texture, int new_width, int new_height){
    rgb24_texture_t prev = *texture;

    // create new texture
    if(rgb24_texture_create(texture, new_width, new_height)){
        VERRPRINT(0, "Failed to create new texture");
        *texture = prev;
        return 1;
    }

    // copy data
    for(int y = 0; y < new_height; ++y){
        for(int x = 0; x < new_width; ++x){
            if(x < prev.width && y < prev.height) texture->data[x + y * new_width] = prev.data[x + y * prev.width];
            else if(y < prev.height)              texture->data[x + y * new_width] = prev.data[(prev.width - 1) + y * prev.width];
            else if(x < prev.width)               texture->data[x + y * new_width] = prev.data[x + (prev.height - 1) * prev.width];
            else                                  texture->data[x + y * new_width] = prev.data[(prev.width - 1) + (prev.height - 1) * prev.width];
        }
    }

    // clean and return
    rgb24_texture_destroy(&prev);
    return 0;
}
