cmake_minimum_required(VERSION 3.1.0)
project(Enigma
    DESCRIPTION "Pet project game engine"
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

###################################
### Build and setup the library ###
###################################

add_library(libEnigma STATIC "")

# Find dependencies:
#  - Vulkan
#  - Range-v3

find_package(Vulkan REQUIRED)
find_package(range-v3 CONFIG REQUIRED)

target_include_directories(libEnigma PRIVATE Vulkan::Vulkan)

target_link_libraries(libEnigma
	PRIVATE Vulkan::Vulkan
	        range-v3 range-v3-meta range-v3::meta range-v3-concepts
)

#
# Setup compiler preprocessor defintions
#

target_compile_definitions(libEnigma PRIVATE
    $<$<PLATFORM_ID:Windows>:VK_USE_PLATFORM_WIN32_KHR>
)

# Set the list of source files
set(ENIGMA_SOURCE_INTERFACE
    "src/EnumUtil.cxx"
    
    "src/System/OperatingSystem.cxx"
    "src/Maths/Geometry.cxx" 
)
set(ENIGMA_SOURCE_IMPLEMENTATION
)

# Set the compiler flags
if(MSVC)
    set(ENIGMA_COMPILER_FLAGS_WARNING          "/W4")                                            # Warning level set to 4
    set(ENIGMA_COMPILER_FLAGS_INSTRUCTION_SET  "/arch:AVX512")                                   # Enables the AVX512 instruction set extensions support
    set(ENIGMA_COMPILER_FLAGS_MISC             "/permissive-" "/await" "/MP" "/Zc:preprocessor") # Broad range of features
    set(ENIGMA_COMPILER_FLAGS_CONCEPTS         "/experimental:concepts")                         # Enables support for C++20 concepts
    set(ENIGMA_COMPILER_FLAGS_MODULE           "/experimental:module")                           # Enables support for C++20 modules
    set(ENIGMA_COMPILER_FLAGS_MODULE_SEARCH    "/module:search" ${CMAKE_CURRENT_BINARY_DIR})     # Specifies the search path for precompiled modules
    set(ENIGMA_COMPILER_FLAGS_MODULE_INTERFACE "/module:interface")                              # Specifies that a file in a module interface (regardless of file extension)
    set(ENIGMA_COMPILER_FLAGS_DEBUG            "/JMC" "/Zi")                                     # Enables "Just my code" debugging and "Edit and Continue" support
else()
    set(ENIGMA_COMPILER_FLAGS_WARNING         )
    set(ENIGMA_COMPILER_FLAGS_INSTRUCTION_SET )
    set(ENIGMA_COMPILER_FLAGS_EXTRA           )
    set(ENIGMA_COMPILER_FLAGS_MODULE          )
    set(ENIGMA_COMPILER_FLAGS_MODULE_SEARCH   )
    set(ENIGMA_COMPILER_FLAGS_MODULE_INTERFACE)
    set(ENIGMA_COMPILER_FLAGS_DEBUG           )
    set(ENIGMA_COMPILER_FLAGS_MISC            )
endif()
# Sets the core set of Enigma Compiler Flags
set(ENIGMA_COMPILER_FLAGS
    ${ENIGMA_COMPILER_FLAGS_WARNING}
    ${ENIGMA_COMPILER_FLAGS_INSTRUCTION_SET}
    ${ENIGMA_COMPILER_FLAGS_EXTRA}
    ${ENIGMA_COMPILER_FLAGS_CONCEPTS}
    ${ENIGMA_COMPILER_FLAGS_MODULE}
    ${ENIGMA_COMPILER_FLAGS_MODULE_SEARCH}
    $<$<CONFIG:Debug>:${ENIGMA_COMPILER_FLAGS_DEBUG}>
    ${ENIGMA_COMPILER_FLAGS_MISC}
)

set(ENIGMA_PUBLIC_COMPILER_FLAGS  ${ENIGMA_COMPILER_FLAGS} ${ENIGMA_COMPILER_FLAGS_MODULE_INTERFACE}) # The flags required for the module interface units
set(ENIGMA_PRIVATE_COMPILER_FLAGS ${ENIGMA_COMPILER_FLAGS})											  # The flags required for the module implementation units

# Replaces the semi-colons with spaces
string (REPLACE ";" " " ENIGMA_COMPILER_FLAGS "${ENIGMA_COMPILER_FLAGS}")
string (REPLACE ";" " " ENIGMA_PUBLIC_COMPILER_FLAGS "${ENIGMA_PUBLIC_COMPILER_FLAGS}")
string (REPLACE ";" " " ENIGMA_PRIVATE_COMPILER_FLAGS "${ENIGMA_PRIVATE_COMPILER_FLAGS}")

set_property(SOURCE ${ENIGMA_SOURCE_INTERFACE}      PROPERTY COMPILE_FLAGS ${ENIGMA_PUBLIC_COMPILER_FLAGS})
set_property(SOURCE ${ENIGMA_SOURCE_IMPLEMENTATION} PROPERTY COMPILE_FLAGS ${ENIGMA_PRIVATE_COMPILER_FLAGS})

# Setup any preprocessor definitions
# First based on OS
# Then based on compiler

if(WIN32)
    add_compile_definitions(UNICODE)
endif()

if(WIN32)
    add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

target_sources(libEnigma
	PRIVATE ${ENIGMA_SOURCE_INTERFACE}
	        ${ENIGMA_SOURCE_IMPLEMENTATION}
)


#
# Build the examples
#

add_subdirectory(example/triangles)

#
# Build and run the tests
#

add_subdirectory(test)
