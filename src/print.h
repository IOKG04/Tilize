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

#ifndef PRINT_H__
#define PRINT_H__

#include <stdio.h>

// print to stdout if verbosity is at least min_verb
#define VPRINT(min_verb, str)       if(get_verbosity() >= min_verb) printf(str)
#define VPRINTF(min_verb, str, ...) if(get_verbosity() >= min_verb) printf(str, __VA_ARGS__)

// print to stderr with line, function and file information if verbosity is at least min_verb and if NDEBUG is undefined or zero
#if NDEBUG
    #define VERRPRINT(min_verb, str)
    #define VERRPRINTF(min_verb, str, ...)
#else
    #define VERRPRINT(min_verb, str)       if(get_verbosity() >= min_verb) fprintf(stderr, str " in %s, %s, %i\n", __FILE__, __func__, __LINE__)
    #define VERRPRINTF(min_verb, str, ...) if(get_verbosity() >= min_verb) fprintf(stderr, str " in %s, %s, %i\n", __VA_ARGS__, __FILE__, __func__, __LINE__)
#endif

// gets current verbosity
int get_verbosity();
// sets verbosity
void set_verbosity(int v);

#endif
