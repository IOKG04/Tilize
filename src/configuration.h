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

#ifndef CONFIGURATION_H__
#define CONFIGURATION_H__

#include "rgb24.h"

// a configuration for Tilize
typedef struct tilize_config_t{
    char    *pattern_path; // relative to the path of the configuration itself
    int      tile_width,
             tile_height;
    int      num_colors;
    rgb24_t *colors;
    int      bckg_color,   // if == -1, all colors can be used
             forg_color;   // same as above
} tilize_config_t;

#define TILIZE_CONFIG_NULL ((tilize_config_t){NULL, 0, 0, 0, NULL, 0, 0})

// a configuration generated at runtime as a result of flags
typedef struct flag_config_t{
    int   showgui;
    int   num_threads;
    char *config_path;          // path of tilize configuration used
    const char *file_outp_path; // path to file output
} flag_config_t;

#define FLAG_CONFIG_NULL ((flag_config_t){0, 1, NULL, NULL})

// serializes a configuration into json
int tilize_config_serialize(char **serialized, const tilize_config_t *restrict config);
// deserializes a configuration from json
int tilize_config_deserialize(tilize_config_t *restrict config, const char *restrict serialized);

#endif
