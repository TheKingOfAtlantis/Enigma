
enable_testing()

set(ENIGMA_SOURCE_TESTS
    "main.cpp"
    
    "test_enumUtil.cpp"
 )
add_executable(testEnigma ${ENIGMA_SOURCE_TESTS})

set_property(SOURCE ${ENIGMA_SOURCE_TESTS} PROPERTY COMPILE_FLAGS ${ENIGMA_COMPILER_FLAGS})

include(GoogleTest)
find_package(GTest CONFIG REQUIRED)

target_link_libraries(testEnigma 
    PRIVATE libEnigma
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
            range-v3 range-v3-meta range-v3::meta range-v3-concepts
)

gtest_discover_tests(testEnigma)
