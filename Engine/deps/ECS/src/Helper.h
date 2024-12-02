#pragma once

// Microsoft Visual C++
#if defined(_MSC_VER)
    #define INLINE __forceinline
    
// GCC, Clang, Intel
#elif defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define INLINE __attribute__((always_inline)) inline
    
// Standard C++17 and newer
#elif defined(__cplusplus) && __cplusplus >= 201703L
    #define INLINE inline
    
// Fallback for other compilers
#else
    #define INLINE static inline
#endif