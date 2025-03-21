project(backend)

set(SOURCE_FILES
        audio/stream/StreamParametersDirectionSpecific.cpp
        audio/stream/StreamParameters.cpp
        audio/stream/StreamOutputPush.cpp
        audio/stream/Stream.cpp
        audio/HostApi.cpp
        audio/Device.cpp
        audio/PortAudioCpp.cpp

        # fm/FMDemod.cpp
        # fm/FMStereo.cpp
        # fm/FMDeemph.cpp
        # fm/FMPilotPhaseLock.cpp

        dsp/QuadratureDemod.cpp
        # dsp/BandpassFilter.cpp
        dsp/IQBalance.cpp
        # dsp/Resampler.cpp
        # dsp/LowpassFilter.cpp
        dsp/FFTProcessor.cpp
        dsp/FirdecimQ15.cpp

        dsp/filter/Firdes.cpp
        dsp/filter/Window.cc
        dsp/filter/IirFilter.cpp

        nrsc5/Decoder.cpp
        nrsc5/Station.cpp

        utils/Log.cpp
        utils/StringUtils.cpp
        utils/RingBuffer.cpp
        utils/MessageQueue.cpp
        utils/DataBuffer.h
        utils/Error.h

        HybridRadio.cpp

        audio/stream/StreamBuffer.cpp
        audio/stream/StreamBuffer.h
)

add_library(nrsc5_lib OBJECT
        ${SOURCE_FILES}
)

target_link_libraries(nrsc5_lib PUBLIC
        Qt6::Core
        Qt6::Widgets

        nrsc5::nrsc5

        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json

        portaudio
        PortSDR

        FFTW::Float
)


set_target_properties(nrsc5_lib PROPERTIES
        AUTOMOC ON
)

target_include_directories(nrsc5_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(nrsc5_lib PROPERTIES LINK_FLAGS "${LINKER_FLAGS}")
