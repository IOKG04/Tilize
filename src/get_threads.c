#include "get_threads.h"

#include <stdio.h>
#if defined(_WIN32)
    #include <Windows.h>
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
        return 1;
    #endif
}
