#ifndef GET_THREADS_H__
#define GET_THREADS_H__

#ifndef GET_THREADS_SUPPORTED
    #if defined(_WIN32)
        #define GET_THREADS_SUPPORTED 1
    #elif defined(__unix__)
        #define GET_THREADS_SUPPORTED 1
    #else
        #define GET_THREADS_SUPPORTED 0
    #endif
#endif

// gets the amount of threads available
int get_thread_count();

#endif
