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
#include <stddef.h>
#include <string.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include "texture.h"

// returns whether or not str ends with match
static int strendswith(const char *restrict str, const char *restrict match);

// loads texture from png_path
int load_png(rgb24_texture_t *restrict texture, const char *png_path){
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
        stbi_image_free(img_data);
        return 1;
    }

    // copy data
    for(int i = 0; i < img_width * img_height; ++i){
        texture->data[i].r = img_data[i * 3 + 0];
        texture->data[i].g = img_data[i * 3 + 1];
        texture->data[i].b = img_data[i * 3 + 2];
    }

    // clean and return
    stbi_image_free(img_data);
    return 0;
}
// saves texture at png_path
int save_png(const char *png_path, const rgb24_texture_t *restrict texture){
    // get image format
    enum {IF_PNG, IF_JPG, IF_BMP} image_format;
    if(strendswith(png_path, ".png"))       image_format = IF_PNG;
    else if(strendswith(png_path, ".jpg") ||
            strendswith(png_path, ".jpeg")) image_format = IF_JPG;
    else if(strendswith(png_path, ".bmp"))  image_format = IF_BMP;
    else{
        fprintf(stderr, "Image format not recognized in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        fprintf(stderr, "If you are a user encountering this, please change the ending of you output file to one of\n");
        fprintf(stderr, " .png\n");
        fprintf(stderr, " .jpg\n");
        fprintf(stderr, " .bmp\n");
        return 1;
    }

    // copy data to format stb_image_write understands
    unsigned char *img_data = malloc(texture->width * texture->height * 3 * sizeof(unsigned char));
    if(!img_data){
        fprintf(stderr, "Failed to allocate img_data in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        return 1;
    }
    for(int i = 0; i < texture->width * texture->height; ++i){
        img_data[3 * i + 0] = texture->data[i].r;
        img_data[3 * i + 1] = texture->data[i].g;
        img_data[3 * i + 2] = texture->data[i].b;
    }

    // write data
    switch(image_format){
        case IF_PNG:
            if(!stbi_write_png(png_path, texture->width, texture->height, STBI_rgb, img_data, texture->width * 3)){
                fprintf(stderr, "Failed to save png in %s, %s, %i\n", __FILE__, __func__, __LINE__);
                free(img_data);
                return 1;
            }
            break;
        case IF_JPG:
            if(!stbi_write_jpg(png_path, texture->width, texture->height, STBI_rgb, img_data, 100)){
                fprintf(stderr, "Failed to save jpg in %s, %s, %i\n", __FILE__, __func__, __LINE__);
                free(img_data);
                return 1;
            }
            break;
        case IF_BMP:
            if(!stbi_write_bmp(png_path, texture->width, texture->height, STBI_rgb, img_data)){
                fprintf(stderr, "Failed to save bmp in %s, %s, %i\n", __FILE__, __func__, __LINE__);
                free(img_data);
                return 1;
            }
            break;
        default:
            fprintf(stderr, "Encountered unknown value for image_format (%i) in %s, %s, %i\n", image_format, __FILE__, __func__, __LINE__);
            free(img_data);
            return 1;
    }

    // clean and return
    free(img_data);
    return 0;
}

// returns whether or not str ends with match
static int strendswith(const char *restrict str, const char *restrict match){
    size_t str_len   = strlen(str),
           match_len = strlen(match);
    if(str_len < match_len) return 0;
    for(size_t i = 1; i <= match_len; ++i){
        if(match[match_len - i] != str[str_len - i]) return 0;
    }
    return 1;
}
