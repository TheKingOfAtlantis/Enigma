/**
 * @file
 * Alas the one thing C++20 modules cannot deal with is preprocessor 
 * macros as such we need this header file to provide them.
 */


#if defined(_WIN32)
#    define ENIGMA_WINDOWS 1
#elif defined(__ANDROID__)
#    define ENIGMA_ANDROID 1
#    define ENIGMA_LINUX   1
#    define ENIGMA_UNIX    1
#elif defined(__linux__)
#    define ENIGMA_LINUX 1
#    define ENIGMA_UNIX  1
#elif defined(__APPLE__) && defined(__MACH__)
#    define ENIGMA_APPLE 1
#    define ENIGMA_UNIX  1
#    if TARGET_OS_MAC == 1
#        define ENIGMA_MACOS 1
#    elif(TARGET_OS_IPHONE == 1) || (TARGET_TARGET_IPHONE_SIMULATOR == 1)
#        define ENIGMA_IOS 1
#    endif
#elif defined(__unix__) || defined(__unix) || defined(unix)
#    define ENIGMA_UNIX 1
#else
#    define ENIGMA_UNKNOWN 1
#endif


