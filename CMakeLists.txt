cmake_minimum_required(VERSION 3.28)
project(nrsc5_gui)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(SPDLOG_FMT_EXTERNAL ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(ENABLE_TESTS "Enable tests" OFF)

include(FetchContent)
include(ExternalProject)

# set(LINKER_FLAGS "-static-libgcc -static-libstdc++")

find_package(RTLSDR REQUIRED)
find_package(FFTW REQUIRED COMPONENTS FLOAT_LIB)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent PrintSupport Svg Sql OpenGL)
find_package(NRSC5 REQUIRED)
find_package(Volk)

qt_standard_project_setup()

message(STATUS "Qt version: ${Qt6_VERSION}")

if (NOT VOLK_FOUND)
#    message(STATUS "Libvolk couldn't be found. Building from source.")
#    FetchContent_Declare(volk
#            GIT_REPOSITORY https://github.com/gnuradio/volk.git
#            GIT_TAG v3.1.2
#            EXCLUDE_FROM_ALL
#    )
#    # FetchContent_MakeAvailable(volk)
endif ()

message(STATUS "Fetching dependencies...")
FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1
        EXCLUDE_FROM_ALL
)
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1
        EXCLUDE_FROM_ALL
)
FetchContent_Declare(PortAudio
        GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
        GIT_TAG master
        EXCLUDE_FROM_ALL
)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        EXCLUDE_FROM_ALL
)
FetchContent_Declare(PortSDR
        GIT_REPOSITORY https://github.com/thedachicken/PortSDR.git
        GIT_TAG master
        EXCLUDE_FROM_ALL
)

# Fetch content
FetchContent_MakeAvailable(fmt spdlog PortAudio json PortSDR)

# subdirectories
add_subdirectory(src)

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
