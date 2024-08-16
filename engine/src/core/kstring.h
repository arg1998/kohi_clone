#pragma once

#include "defines.h"

KAPI char* string_duplicate(const char* str);

KAPI u64 string_length(const char* str);

// Case-sensitive string comparison. True if the same, otherwise false.
KAPI b8 strings_equal(const char* str0, const char* str1);