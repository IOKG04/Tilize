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
#include <string.h>
#include <limits.h>
#if defined(_WIN32)
    #if defined(__MINGW32__)
        #include <windows.h> // for whatever reason its with a non capital 'W' with mingw, so ig ill say that extra :333
    #else
        #include <Windows.h>
    #endif
#elif defined(__unix__)
    #include <time.h>
#endif
#include "application.h"
#include "atlas.h"
#include "get_threads.h"
#include "gui.h"
#include "load_png.h"
#include "print.h"
#include "rgb24.h"
#include "texture.h"

typedef long long unsigned ms_t;

static const char *help_msg = "Usage:\n"
                              " Tilize [[options]] [file]  | Tilizes [file] with [options]\n"
                              " Tilize help                | Show this message\n"
                              "\n"
                              "Options:\n"
                              " -o [file]                  | Save result to [file]\n"
                              " -c [file]                  | Use [file] as configuration\n"
                          #if GET_THREADS_SUPPORTED
                              " -j[=number]                | Use multiple threads ([number] if provided, otherwise maximum amount available)\n"
                          #else
                              " -j=[number]                | Use [number] threads\n"
                          #endif
                          #if GUI_SUPPORTED
                              " -q                         | Run without GUI\n"
                          #endif
                              " -v[=number]                | Print more (or less) information\n"
                              "                            | -1          : Print only interactive prompts\n"
                              "                            | 0           : Print errors\n"
                              "                            | 1 (default) : Print errors and warnings\n"
                              "                            | 2 (`-v`)    : Print errors, warnings and subprocess times\n"
                              " -y                         | Automatically answer `yes` to all questions directed at the user\n"
                              "\n"
                              "If the same option is provided multiple times, the last one is used.\n"
                              "\n"
                              "Compiled on " __DATE__ "\n"
                              "\n";

// returns whether or not an option with the name opt_name was provided, if so puts its index into index
static int option_provided(int argc, const char **argv, const char *restrict opt_name, int *restrict index);
// makes a copy of src and returns it
static char *strdup_exceptmyversionsobettercauseitisntc23exclusive(const char *restrict src);
// gets current time in ms
static ms_t current_ms(void);

int main(int argc, const char **argv){
    int return_code = EXIT_SUCCESS;
    tilize_config_t tilize_config = TILIZE_CONFIG_NULL;
    flag_config_t   flag_config   = FLAG_CONFIG_NULL;
    int option_index;
    int auto_answer_y = 0;
    ms_t flag_start_ms        = 0,
         load_input_start_ms  = 0,
         #if GUI_SUPPORTED
             gui_start_ms     = 0,
         #endif
         application_start_ms = 0,
         deinit_start_ms      = 0,
         tilize_end_ms        = 0;

    // check if the user used too few arguments or if help is requested
    if(argc < 2 || option_provided(argc, argv, "help", NULL) || option_provided(argc, argv, "--help", NULL) || option_provided(argc, argv, "-h", NULL)){
        printf("%s", help_msg);
        return EXIT_SUCCESS;
    }

    // -v option, verbosity
    if(option_provided(argc, argv, "-v", &option_index)){
        // option provided
        if(strlen(argv[option_index]) > 2 && argv[option_index][2] == '='){
            // -v= option
            int v = atoi(&argv[option_index][3]);
            set_verbosity(v);
        }
        else{
            // -v option
            set_verbosity(2);
        }
    }

    // flag_start_ms (tilize_start_ms), only after `-v` flag so it doesnt print if `-v=-1` is specified
    if(get_verbosity() >= 2){
        flag_start_ms = current_ms();
    }

    // -y option, auto answer yes
    if(option_provided(argc, argv, "-y", &option_index)) auto_answer_y = 1;
    else                                                 auto_answer_y = 0;

    // -c option, configuration
    if(option_provided(argc, argv, "-c", &option_index)){
        // config file provided
        if(argc <= option_index + 1){
            VPRINT(1, "Cannot try opening config_file because `-c` was given as the last argument");
            return EXIT_FAILURE;
        }
        FILE *config_file = fopen(argv[option_index + 1], "r");
        if(!config_file){
            VERRPRINT(0, "Failed to open config_file");
            return EXIT_FAILURE;
        }
        if(fseek(config_file, 0, SEEK_END)){
            VERRPRINT(0, "Failed to seek to end of config_file");
            fclose(config_file);
            return EXIT_FAILURE;
        }
        long config_file_size = ftell(config_file);
        if(config_file_size == -1L){
            VERRPRINT(0, "Failed to tell position in config_file");
            fclose(config_file);
            return EXIT_FAILURE;
        }
        rewind(config_file);
        char *config_text = calloc(config_file_size + 16, sizeof(char));
        if(!config_text){
            VERRPRINT(0, "Failed to allocate config_text");
            fclose(config_file);
            return EXIT_FAILURE;
        }
        fread(config_text, sizeof(char), config_file_size + 8, config_file);
        fclose(config_file);
        if(tilize_config_deserialize(&tilize_config, config_text)){
            VERRPRINT(0, "Failed to deserialize config");
            free(config_text);
            return EXIT_FAILURE;
        }
        free(config_text);

        flag_config.config_path = strdup_exceptmyversionsobettercauseitisntc23exclusive(argv[option_index + 1]);
        if(!flag_config.config_path){
            VERRPRINT(0, "Failed to duplicate `-c` path to flag_config.config_path");
            return EXIT_FAILURE;
        }
    }
    else{
        // config file not provided or failed to load
        flag_config.config_path = "resources/this_shouldnt_be_real.json";

        tilize_config.pattern_path = "simpletiles_4x4.png";
        tilize_config.tile_width   = 4;
        tilize_config.tile_height  = 4;
        tilize_config.num_colors   = 2;
        tilize_config.colors       = malloc(2 * sizeof(*tilize_config.colors));
        if(!tilize_config.colors){
            VERRPRINT(0, "Failed to allocate colors");
            return EXIT_FAILURE;
        }
        tilize_config.colors[0]    = RGB24(0x00,0x00,0x00);
        tilize_config.colors[1]    = RGB24(0xff,0xff,0xff);
        tilize_config.bckg_color   = -1;
        tilize_config.forg_color   = -1;
    }

    // -j option, thread count
    if(option_provided(argc, argv, "-j", &option_index)){
        // option provided
        if(strlen(argv[option_index]) > 2 && argv[option_index][2] == '='){
            // -j= option, specified thread count
            flag_config.num_threads = atoi(&argv[option_index][3]);
            if(flag_config.num_threads <= 0){
                VPRINT(1, "Cannot run with non positive amount of threads. Please use a positive number for `-j`\n");
                return EXIT_FAILURE;
            }
        }
        else{
            // -j option, get thread count
            #if GET_THREADS_SUPPORTED
                flag_config.num_threads = get_thread_count();
                if(flag_config.num_threads < 0){
                    VERRPRINT(0, "Failed to get thread count");
                    return EXIT_FAILURE;
                }
            #else
                VPRINT(1, "Your system does not currently support the `-j` option. Please use `-j=` instead.\n");
                return EXIT_FAILURE;
            #endif
        }
    }
    else{
        // option not provided
        flag_config.num_threads = 1;
    }

    #if GUI_SUPPORTED
        // -q option, disable GUI
        if(option_provided(argc, argv, "-q", &option_index)){
            // option provided
            flag_config.showgui = 0;
        }
        else{
            // option not provided
            flag_config.showgui = 1;
        }
    #else
        flag_config.showgui = 0;
    #endif

    // -o option, output file
    if(option_provided(argc, argv, "-o", &option_index)){
        // option provided
        if(argc <= option_index + 1){
            VPRINT(1, "Cannot try writing to output file because `-o` was given as the last argument\n");
            return EXIT_FAILURE;
        }
        flag_config.file_outp_path = argv[option_index + 1];
        if(!auto_answer_y){
            FILE *outp_file_test = fopen(flag_config.file_outp_path, "r");
            if(outp_file_test){
                fclose(outp_file_test);
                printf("Warning: %s already exists. Overwrite (y / N)?\n", flag_config.file_outp_path);
                char yN = getchar();
                if(!(yN == 'y' || yN == 'Y')){
                    VPRINTF(1, "Not overwriting %s, exiting\n", flag_config.file_outp_path);
                    return EXIT_SUCCESS;
                }
                while(getchar() != '\n');
            }
        }
    }
    else{
        // option not provided
        flag_config.file_outp_path = NULL;
        if(!auto_answer_y && !flag_config.showgui){
            printf("Warning: Running without any form of output. Proceed (y / N)?\n");
            char yN = getchar();
            if(!(yN == 'y' || yN == 'Y')){
                VPRINT(1, "Not running without an output, exiting\n");
                return EXIT_SUCCESS;
            }
            while(getchar() != '\n');
        }
    }

    // get load_input_start_ms
    if(get_verbosity() >= 2){
        load_input_start_ms = current_ms();
        VPRINTF(2, "Finished parsing flags in %llu ms\n", (long long unsigned)(load_input_start_ms - flag_start_ms));
    }

    // load input image
    rgb24_texture_t input_image = RGB24_TEXTURE_NULL;
    if(load_png(&input_image, argv[argc - 1])){
        VERRPRINT(0, "Failed to load input_image");
        return_code = EXIT_FAILURE;
        goto _clean_and_exit;
    }

    #if GUI_SUPPORTED
        // get gui_start_ms
        if(get_verbosity() >= 2){
            gui_start_ms = current_ms();
            VPRINTF(2, "Finished loading input image in %llu ms\n", (long long unsigned)(gui_start_ms - load_input_start_ms ));
        }

        if(!flag_config.showgui) goto _no_init_gui;
        // initialize gui
        if(gui_setup(input_image.width, input_image.height, 1)){
            VERRPRINT(0, "Failed to initialize gui");
            return_code = EXIT_FAILURE;
            goto _clean_and_exit;
        }
        _no_init_gui:;
    #endif

    // get application_start_ms
    if(get_verbosity() >= 2){
        application_start_ms = current_ms();
        #if GUI_SUPPORTED
            VPRINTF(2, "Finished starting GUI in %llu ms\n", (long long unsigned)(application_start_ms - gui_start_ms));
        #else
            VPRINTF(2, "Finished loading input image in %llu ms\n", (long long unsigned)(application_start_ms - load_input_start_ms));
        #endif
    }

    // do the thing
    if(application_setup(&tilize_config, &flag_config)){
        VERRPRINT(0, "Failed to setup application");
        return_code = EXIT_FAILURE;
        goto _clean_and_exit;
    }
    if(application_process(&input_image)){
        VERRPRINT(0, "Failed to process input_image");
        return_code = EXIT_FAILURE;
        goto _clean_and_exit;
    }
    application_free();

    // get deinit_start_ms
    if(get_verbosity() >= 2){
        deinit_start_ms = current_ms();
        VPRINTF(2, "Finished application in %llu ms\n", (long long unsigned)(deinit_start_ms - application_start_ms));
    }

    #if GUI_SUPPORTED
        // wait to exit
        printf("Finished Tilizing, press enter to exit\n");
        while(flag_config.showgui && getchar() != '\n');
    #endif

    // clean and exit
    _clean_and_exit:;
    #if GUI_SUPPORTED
        if(flag_config.showgui) gui_free();
    #endif
    rgb24_texture_destroy(&input_image);
    if(return_code == EXIT_SUCCESS && get_verbosity() >= 2){
        tilize_end_ms = current_ms();
        VPRINTF(2, "Finished deinitialization in %llu ms\n", (long long unsigned)(tilize_end_ms - deinit_start_ms));
        VPRINTF(2, "Finished Tilize in %llu ms\n", (long long unsigned)(tilize_end_ms - flag_start_ms));
    }
    return return_code;
}

// returns whether or not an option with the name opt_name was provided, if so puts its index into index
static int option_provided(int argc, const char **argv, const char *restrict opt_name, int *restrict index){
    int opt_len = strlen(opt_name);
    for(int i = argc - 1; i > 0; --i){
        for(int j = 0; j < opt_len; ++j){
            if(argv[i][j] == 0) goto _next_argument;
            if(argv[i][j] != opt_name[j]) goto _next_argument;
        }
        if(index) *index = i;
        return 1;
        _next_argument:;
    }
    return 0;
}
// makes a copy of src and returns it
static char *strdup_exceptmyversionsobettercauseitisntc23exclusive(const char *restrict src){
    size_t src_len = strlen(src) + 1;
    char *outp = malloc(src_len);
    if(!outp){
        VERRPRINT(0, "Failed to allocate outp");
        return NULL;
    }
    #if defined(__MINGW32__)
        strcpy(outp, src); // i [beep]ing hate you, mingw-gcc
    #else
        strncpy(outp, src, src_len);
    #endif
    return outp;
}
// gets current time in ms
static ms_t current_ms(void){
    #if defined(_WIN32)
        return GetTickCount();
    #elif defined(__unix__)
        struct timespec ts;
        if(!timespec_get(&ts, TIME_UTC)){
            VERRPRINT(0, "Failed to get current time");
            VPRINT(1, "Please ignore any timing information given after this\n");
            return 0;
        }
        return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    #else
        fprintf(stderr, "Warning: current_ms() not supported at compiletime\n");
        return 0;
    #endif
}
