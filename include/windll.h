#ifndef WINDLL_H
#define WINDLL_H

// Windows DLL
#ifdef _WIN32
    #ifdef BUILDING_CORE_DLL
        // build DLL
        #define CORE_API __declspec(dllexport)
    #else
        // call DLL
        #define CORE_API __declspec(dllimport)
    #endif
#else
    // Linux or others
    #define CORE_API
#endif

#endif // WINDLL_H