cmake_minimum_required(VERSION 3.28)
project(nrsc5_gui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to use")
set(SPDLOG_FMT_EXTERNAL ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(FMT_BUILT_IN "Use built-in fmt library" ON)
option(SPDLOG_BUILT_IN "Use built-in spdlog library" ON)
option(PORTAUDIO_BUILT_IN "Use built-in PortAudio library" ON)

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
endif()

find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(LIBUSB REQUIRED)

include(cmake/FetchImGui.cmake)

FetchContent_Declare(glfw_external
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glfw_external)

FetchContent_Declare(
        lunasvg_external
        GIT_REPOSITORY https://github.com/sammycage/lunasvg.git
        GIT_TAG master  # Specify the desired branch or tag
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

if (PORTAUDIO_BUILT_IN)
    FetchContent_Declare(portaudio_external
            GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
            GIT_TAG master
            EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(portaudio_external)
else ()
    find_package(PortAudio REQUIRED)
endif ()

add_subdirectory(src)
