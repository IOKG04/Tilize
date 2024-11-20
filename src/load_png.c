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
#include "stb_image.h"
#include "texture.h"

// loads texture from png_path
int load_png(rgb24_texture_t *texture, const char *png_path){
    // load image
    int img_width, img_height;
    unsigned char *img_data = stbi_load(png_path, &img_width, &img_height, NULL, STBI_rgb);
    if(!img_data){
        fprintf(stderr, "Failed to load image in %s, %s, %i:\n%s\n", __FILE__, __func__, __LINE__, stbi_failure_reason());
        return 1;
    }

    // create texture
    if(rgb24_texture_create(texture, img_width, img_height)){
        fprintf(stderr, "Failed to create texture in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        free(img_data);
        return 1;
    }

    // copy data
    for(int i = 0; i < img_width * img_height; ++i){
        texture->data[i].r = img_data[i * 3 + 0];
        texture->data[i].g = img_data[i * 3 + 1];
        texture->data[i].b = img_data[i * 3 + 2];
    }

    // clean and return
    free(img_data);
    return 0;
}
