FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)

add_executable(${PROJECT_NAME}-test
        NRSC5.cpp
        Database.cpp
        ResamplerTest.cpp
)

target_link_libraries(${PROJECT_NAME}-test
        PRIVATE
        ${PROJECT_NAME}-lib
        gtest
        gtest_main
        gmock
)

target_include_directories(${PROJECT_NAME}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")
gtest_discover_tests(${PROJECT_NAME}-test)
