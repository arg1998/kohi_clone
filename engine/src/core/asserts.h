#pragma once

#include "defines.h"

// we can disable assertions by commenting the line bellow
#define KASSERTIONS_ENABLED




#ifdef KASSERTIONS_ENABLED
    #if _MSC_VER
        // for MSVC
        #include <intrin.h>
        #define debugBreak() __debugbreak()
    #else
        // for GCC
        #define debugBreak() __builtin_trap()
    #endif


    KAPI  void report_assertion_failure(const char *expression, const char *message, const char *file, i32 line);



    #define KASSERT(expr){                                                      \
        if (expr){                                                              \
        } else {                                                                \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);            \
            debugBreak();                                                       \
        }                                                                       \
    }

    #define KASSERT_MSG(expr, message){                                         \
        if (expr){                                                              \
        } else {                                                                \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);       \
            debugBreak();                                                       \
        }                                                                       \
    }

    #ifdef _DEBUG
        #define KASSERT_DEBUG(expr){                                            \
            if (expr){                                                          \
            } else {                                                            \
                report_assertion_failure(#expr, "", __FILE__, __LINE__);        \
                debugBreak();                                                   \
            }                                                                   \
        }
    #else
        #define KASSERT_DEBUG(expr) 
    #endif

#else
    // if assertions are disabled
    #define KASSERT(expr)
    #define KASSERT_MSG(expr, message)
    #define KASSERT_DEBUG(expr)
#endif