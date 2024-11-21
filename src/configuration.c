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

#include "configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cJSON.h"
#include "print.h"

// json names for different things
// made using macros so no typos can occur
#define PATTERN_PATH "pattern_path"
#define TILE_WIDTH   "tile_width"
#define TILE_HEIGHT  "tile_height"
#define COLORS       "colors"
#define BCKG_COLOR   "background_color"
#define FORG_COLOR   "foreground_color"

// serializes a configuration into json
int tilize_config_serialize(char **serialized, const tilize_config_t *restrict config){
    int retcode = 0;

    // create root object
    cJSON *root = cJSON_CreateObject();
    if(!root){
        VERRPRINT(0, "Failed to create root");
        return 1;
    }

    // add pattern_path
    if(!cJSON_AddStringToObject(root, PATTERN_PATH, config->pattern_path)){
        VERRPRINT(0, "Failed to add pattern_path to root");
        retcode = 1;
        goto _clean_and_exit;
    }

    // add tile_width and tile_height
    if(!cJSON_AddNumberToObject(root, TILE_WIDTH, config->tile_width)){
        VERRPRINT(0, "Failed to add tile_width to root");
        retcode = 1;
        goto _clean_and_exit;
    }
    if(!cJSON_AddNumberToObject(root, TILE_HEIGHT, config->tile_height)){
        VERRPRINT(0, "Failed to add tile_height to root");
        retcode = 1;
        goto _clean_and_exit;
    }

    // num_colors implied from size of colors

    // add colors
    cJSON *colors = cJSON_AddArrayToObject(root, COLORS);
    if(!colors){
        VERRPRINT(0, "Failed to add colors to root");
        retcode = 1;
        goto _clean_and_exit;
    }
    for(int i = 0; i < config->num_colors; ++i){
        // add colors[i]
        char color_hex[7] = "";
        sprintf(color_hex, "%02x%02x%02x", config->colors[i].r, config->colors[i].g, config->colors[i].b);
        cJSON *ccolor = cJSON_CreateString(color_hex);
        if(!ccolor){
            VERRPRINTF(0, "Failed to create ccolor %i", i);
            retcode = 1;
            goto _clean_and_exit;
        }
        if(!cJSON_AddItemToArray(colors, ccolor)){
            VERRPRINTF(0, "Failed to add ccolor %i to colors", i);
            cJSON_Delete(ccolor);
            retcode = 1;
            goto _clean_and_exit;
        }
    }

    // add bckg_color and forg_color if != -1
    if(config->bckg_color != -1){
        cJSON *bckg_color = cJSON_AddNumberToObject(root, BCKG_COLOR, config->bckg_color);
        if(!bckg_color){
            VERRPRINT(0, "Failed to add bckg_color to root");
            retcode = 1;
            goto _clean_and_exit;
        }
    }
    if(config->forg_color != -1){
        cJSON *forg_color = cJSON_AddNumberToObject(root, FORG_COLOR, config->forg_color);
        if(!forg_color){
            VERRPRINT(0, "Failed to add forg_color to root");
            retcode = 1;
            goto _clean_and_exit;
        }
    }

    // print root
    *serialized = cJSON_Print(root);
    if(!serialized){
        VERRPRINT(0, "Failed to print root");
        retcode = 1;
        goto _clean_and_exit;
    }

    // clean and exit
    _clean_and_exit:;
    cJSON_Delete(root);
    return retcode;
}
// deserializes a configuration from json
int tilize_config_deserialize(tilize_config_t *restrict config, const char *restrict serialized){
    int retcode = 0;
    double temp;

    // parse serialized
    cJSON *root = cJSON_Parse(serialized);
    if(!root){
        if(get_verbosity() >= 0) fprintf(stderr, "Failed to parse serialized in %s, %s, %i:\n%256s\n", __FILE__, __func__, __LINE__, cJSON_GetErrorPtr());
        VPRINT(1, "If you are encountering this, the file you put after `-c` is not a tilize configuration file.\n");
        return 1;
    }

    // get pattern_path
    cJSON *pattern_path = cJSON_GetObjectItem(root, PATTERN_PATH);
    if(!pattern_path){
        VERRPRINT(0, "Failed to get pattern_path from root");
        VPRINT(1, "If you are encountering this, the file you put after `-c` is not a tilize configuration file.\n");
        retcode = 1;
        goto _clean_and_exit;
    }
    const char *whycantgetstringvaluejustreturnacopyonitsown = cJSON_GetStringValue(pattern_path);
    if(!whycantgetstringvaluejustreturnacopyonitsown){
        VERRPRINT(0, "Failed to get whycantgetstringvaluejustreturnacopyonitsown from pattern_path");
        retcode = 1;
        goto _clean_and_exit;
    }
    config->pattern_path = malloc(strlen(whycantgetstringvaluejustreturnacopyonitsown) + 1);
    if(!config->pattern_path){
        VERRPRINT(0, "Failed to allocate config->pattern_path");
        retcode = 1;
        goto _clean_and_exit;
    }
    strcpy(config->pattern_path, whycantgetstringvaluejustreturnacopyonitsown);

    // get tile_width and tile_height
    cJSON *tile_width = cJSON_GetObjectItem(root, TILE_WIDTH);
    if(!tile_width){
        VERRPRINT(0, "Failed to get tile_width from root");
        VPRINT(1, "If you are encountering this, the file you put after `-c` is not a tilize configuration file.\n");
        retcode = 1;
        goto _clean_and_exit;
    }
    temp = cJSON_GetNumberValue(tile_width);
    if(isnan(temp)){
        VERRPRINT(0, "Failed to get tile_width from tile_width");
        retcode = 1;
        goto _clean_and_exit;
    }
    config->tile_width = temp;
    cJSON *tile_height = cJSON_GetObjectItem(root, TILE_HEIGHT);
    if(!tile_height){
        VERRPRINT(0, "Failed to get tile_height from root");
        VPRINT(1, "If you are encountering this, the file you put after `-c` is not a tilize configuration file.\n");
        retcode = 1;
        goto _clean_and_exit;
    }
    temp = cJSON_GetNumberValue(tile_height);
    if(isnan(temp)){
        VERRPRINT(0, "Failed to get tile_height from tile_height");
        retcode = 1;
        goto _clean_and_exit;
    }
    config->tile_height = temp;

    // get colors and num_colors
    cJSON *colors = cJSON_GetObjectItem(root, COLORS);
    if(!colors){
        VERRPRINT(0, "Failed to get colors from root");
        VPRINT(1, "If you are encountering this, the file you put after `-c` is not a tilize configuration file.\n");
        retcode = 1;
        goto _clean_and_exit;
    }
    config->num_colors = cJSON_GetArraySize(colors);
    config->colors = malloc(config->num_colors * sizeof(*config->colors));
    if(!config->colors){
        VERRPRINT(0, "Failed to allocate config->colors");
        retcode = 1;
        goto _clean_and_exit;
    }
    cJSON *ccolor;
    int i = 0;
    cJSON_ArrayForEach(ccolor, colors){
        const char *color_hex = cJSON_GetStringValue(ccolor);
        if(!color_hex){
            VERRPRINTF(0, "Failed to get color_hex %i from ccolor", i);
            VPRINT(1, "If you are encountering this, the file you put after `-c` is not a tilize configuration file.\n");
            retcode = 1;
            goto _clean_and_exit;
        }
        unsigned long color = strtoul(color_hex, NULL, 16);
        config->colors[i].r = (color >> 16) & 0xff;
        config->colors[i].g = (color >>  8) & 0xff;
        config->colors[i].b = (color >>  0) & 0xff;
        ++i;
    }

    // get bckg_color and forg_color if they exist
    cJSON *bckg_color = cJSON_GetObjectItem(root, BCKG_COLOR);
    if(bckg_color) config->bckg_color = (int)cJSON_GetNumberValue(bckg_color);
    else           config->bckg_color = -1;
    cJSON *forg_color = cJSON_GetObjectItem(root, FORG_COLOR);
    if(forg_color) config->forg_color = (int)cJSON_GetNumberValue(forg_color);
    else           config->forg_color = -1;

    // clean and exit
    _clean_and_exit:;
    cJSON_Delete(root);
    return retcode;
}
