#include "common.h"

char* get_base_filename(const char *full_path) {
    static char base_name[256];
    
    const char *last_slash = strrchr(full_path, PATH_SEPARATOR);
#ifdef _WIN32
    const char *last_backslash = strrchr(full_path, PATH_SEPARATOR);
    if (last_backslash && (!last_slash || last_backslash > last_slash)) {
        last_slash = last_backslash;
    }
#endif
    
    const char *filename = last_slash ? last_slash + 1 : full_path;
    
    strncpy(base_name, filename, sizeof(base_name) - 1);
    base_name[sizeof(base_name) - 1] = '\0';
    
    char *dot = strrchr(base_name, '.');
    if (dot) {
        *dot = '\0';
    }
    
    return base_name;
}