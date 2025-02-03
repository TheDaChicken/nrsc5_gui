cmake_minimum_required(VERSION 3.22)
project(testing)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)

add_executable(Tests
        # Database.cpp
        NRSC5.cpp
        HybridTest.cpp
)

target_link_libraries(Tests
        PRIVATE
        nrsc5_lib
        gtest
        gtest_main
        gmock
)
gtest_discover_tests(Tests)

