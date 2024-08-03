#include "kstring.h"
#include "core/kmemory.h"

#include <string.h>

char* string_duplicate(const char* str) {
    u64 length = string_length(str) + 1;  // one more for the null terminator character --> \0
    char* copy_str = kallocate(length, MEMORY_TAG_STRING);
    kcopy_memory(copy_str, str, length);
    return copy_str;
}

u64 string_length(const char* str){
    return strlen(str);
}
