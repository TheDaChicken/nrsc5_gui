FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(${PROJECT_NAME}-test
        Cache.cpp
        Table.cpp
        Radio.cpp
)

target_link_libraries(${PROJECT_NAME}-test
        PRIVATE
        gtest
        gtest_main
        gmock
        ${PROJECT_NAME}-lib
)

target_include_directories(${PROJECT_NAME}-test PRIVATE "${PROJECT_SOURCE_DIR}/src")
gtest_discover_tests(${PROJECT_NAME}-test)