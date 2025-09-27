cmake_minimum_required(VERSION 3.28)
project(nrsc5_gui VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to use")
set(SPDLOG_FMT_EXTERNAL ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

option(FMT_BUILT_IN "Use built-in fmt library" ON)
option(SPDLOG_BUILT_IN "Use built-in spdlog library" ON)

include(FetchContent)
include(ExternalProject)

if (WIN32)
    set(PROJECT_RESOURCE_DIR "bin/resources")
elseif (APPLE)
    # For macOS, resources are typically bundled within the app package
    # This is ignored for Apple platforms
    set(PROJECT_RESOURCE_DIR "")
elseif (LINUX)
    set(PROJECT_RESOURCE_DIR "share/${PROJECT_NAME}")
elseif ()
    # Unsupported platform
    message(FATAL_ERROR "Unsupported platform for resource installation.")
endif ()

find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(LibUSB REQUIRED)
find_package(NRSC5 REQUIRED)
find_package(JPEG REQUIRED)
find_package(Freetype REQUIRED)

include(cmake/FetchImGui.cmake)

FetchContent_Declare(sdl_external
        GIT_REPOSITORY https://github.com/TheDaChicken/SDL.git
        GIT_TAG preferred-theme
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(sdl_external)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        lunasvg_external
        GIT_REPOSITORY https://github.com/sammycage/lunasvg.git
        GIT_TAG master  # Specify the desired branch or tag
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(lunasvg_external)

if (FMT_BUILT_IN)
    FetchContent_Declare(fmt_external
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 10.2.1
            EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(fmt_external)
else ()
    find_package(fmt 10.2.1 REQUIRED)
endif ()

if (SPDLOG_BUILT_IN)
    FetchContent_Declare(spdlog_external
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.14.1
            EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(spdlog_external)
else ()
    find_package(spdlog 1.14.1 REQUIRED)
endif ()

FetchContent_Declare(portsdr_external
        GIT_REPOSITORY https://github.com/thedachicken/portsdr.git
        GIT_TAG master
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(portsdr_external)

if (NOT VOLK_FOUND)
    message(STATUS "Libvolk couldn't be found. Building from source.")
    set(LIBVOLK_PREFIX "${CMAKE_BINARY_DIR}/libvolk/")
    set(LIBVOLK_INCLUDE_DIR "${LIBVOLK_PREFIX}/include/")
    if (WIN32)
        set(LIBVOLK_LIBRARIES "${LIBVOLK_PREFIX}/lib/libvolk.dll.a")
    else ()
        set(LIBVOLK_LIBRARIES "${LIBVOLK_PREFIX}/lib/libvolk.so")
    endif ()

    ExternalProject_Add(LibVolkExternal
            GIT_REPOSITORY https://github.com/gnuradio/volk.git
            GIT_TAG v3.2.0
            PREFIX ${LIBVOLK_PREFIX}
            UPDATE_COMMAND ""
            BUILD_BYPRODUCTS ${LIBVOLK_LIBRARIES}
            CMAKE_ARGS
            ${COMMON_CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX:STRING=${LIBVOLK_PREFIX}
    )
    file(MAKE_DIRECTORY ${LIBVOLK_INCLUDE_DIR})

    add_library(Volk::volk INTERFACE IMPORTED GLOBAL)
    set_target_properties(Volk::volk PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBVOLK_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LIBVOLK_LIBRARIES}"
    )
    add_dependencies(Volk::volk LibVolkExternal)
endif ()

add_subdirectory(src)
add_subdirectory(tests)