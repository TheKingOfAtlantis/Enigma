
enable_testing()

set(ENIGMA_SOURCE_TESTS
    "main.cpp"
)

add_executable(testEnigma ${ENIGMA_SOURCE_TESTS})

include(GoogleTest)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(testEnigma 
    PRIVATE libEnigma
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)

gtest_discover_tests(testEnigma)
