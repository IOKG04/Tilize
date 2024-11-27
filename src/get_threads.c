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

#include "get_threads.h"

#include <stdio.h>
#if defined(_WIN32)
    #if defined(__MINGW32__)
        #include <windows.h> // for whatever reason its with a non capital 'W' with mingw, so ig ill say that extra :333
    #else
        #include <Windows.h>
    #endif
#elif defined(__unix__)
    #include <unistd.h>
#endif

// gets the amount of threads available
int get_thread_count(){
    #if GET_THREADS_SUPPORTED
        #if defined(_WIN32)
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            return (int)sysinfo.dwNumberOfProcessors;
        #elif defined(__unix__) && defined(_SC_NPROCESSORS_ONLN)
            return (int)sysconf(_SC_NPROCESSORS_ONLN);
        #else
            fprintf(stderr, "Warning: get_thread_count() not supported by environment\n");
            return 1;
        #endif
    #else
        fprintf(stderr, "Warning: get_thread_count() not supported at compiletime\n");
        return 0;
    #endif
}
