# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fPIE -pie -g")

add_library(${PROJECT_NAME}-lib STATIC

        images/ImageDecoder.cpp
        images/PNGDecoder.cpp
        images/PNGDecoder.h
        images/ImageBuffer.h
        images/SVGDecoder.cpp
        images/JPEGDecoder.cpp
        images/JPEGDecoder.h

        gui/widgets/IconButton.cpp
        gui/platform/sdl/SDLTexture.cpp
        gui/managers/AssetsManager.cpp
        gui/managers/AssetsManager.h
        gui/widgets/Navigation.cpp
        gui/widgets/Navigation.h
        gui/widgets/DualIconButton.cpp
        gui/widgets/DualIconButton.h
        gui/panels/DockStationPanel.cpp
        gui/panels/DockStationPanel.h
        gui/managers/ThemeManager.cpp
        gui/managers/ThemeManager.h
        gui/widgets/ListView.cpp
        gui/widgets/ListView.h
        gui/view/radio/RadioView.cpp
        gui/view/radio/RadioView.h
        gui/panels/DockFavoritesPanel.cpp
        gui/panels/DockFavoritesPanel.h
        gui/panels/DockAudioPanel.cpp
        gui/panels/DockAudioPanel.h
        gui/panels/DockTunePanel.cpp
        gui/panels/DockTunePanel.h
        gui/view/SettingsView.cpp
        gui/view/SettingsView.h
        gui/utils/Icon.h
        gui/utils/Cache.h
        gui/panels/DockInputPanel.cpp
        gui/panels/DockInputPanel.h
        gui/managers/EventsDispatcher.cpp
        gui/managers/EventsDispatcher.h
        gui/UISession.cpp
        gui/UISession.h

        utils/Log.h
        utils/Log.cpp

        sql/connection/Connection.cpp
        sql/connection/ConnectionPool.cpp
        sql/sqlite/sqlite3.c

        nrsc5/LotManager.cpp
        nrsc5/LotManager.h
        nrsc5/Station.cpp
        nrsc5/Station.h
        nrsc5/Processor.cpp
        nrsc5/Processor.h
        nrsc5/Decoder.cpp
        nrsc5/Decoder.h

        dsp/Firdes.cpp
        dsp/Window.cpp
        dsp/resampler/ArbResampler.h

        hybrid/HybridDecoder.cpp
        hybrid/HybridDecoder.h

        models/FavoriteList.cpp
        models/FavoriteList.h

        utils/ThreadPool.cpp
        utils/ThreadPool.h
        utils/RingBuffer.cpp
        utils/RingBuffer.h
        utils/Expected.hpp

        gui/managers/ImageManager.cpp
        gui/managers/ImageManager.h
        sql/DatabaseManager.cpp
        sql/DatabaseManager.h
        input/SDRInput.cpp
        input/SDRInput.h
        input/RadioInput.h
        sql/SchemaTable.cpp
        sql/SchemaTable.h
        sql/SettingsTable.cpp
        sql/SettingsTable.h
        sql/LotTable.cpp
        sql/LotTable.h

        hybrid/HybridState.h

        gui/platform/sdl/SDLAudioManager.cpp
        gui/platform/sdl/SDLAudioManager.h
        dsp/resampler/IFilterBlock.h
        dsp/resampler/IFilterStream.h

        gui/platform/sdl/SDLTextureManager.cpp
        gui/platform/sdl/SDLTextureManager.h
        gui/platform/sdl/SDLPlatformContext.cpp
        gui/platform/sdl/SDLPlatformContext.h
        external/LotService.cpp
        external/LotService.h

        gui/view/radio/RadioHomeView.cpp
        gui/view/radio/RadioHomeView.h
        gui/view/radio/RadioBackView.cpp
        gui/view/radio/RadioBackView.h
        gui/platform/sdl/SDLWindow.cpp
        gui/platform/sdl/SDLWindow.h
        gui/view/IView.h

        gui/MainWindow.cpp
        gui/MainWindow.h
        gui/Util.h
        gui/utils/TextRender.cpp
        gui/utils/TextRender.h
        gui/panels/SDRSettings.cpp
        gui/panels/SDRSettings.h
        hybrid/HybridMetadata.cpp
        hybrid/HybridMetadata.h
        gui/controllers/StationController.cpp
        gui/controllers/StationController.h
        gui/controllers/StationController.cpp
        gui/controllers/StationController.h
        gui/controllers/Events.h
        gui/HybridAudio.cpp
        gui/HybridAudio.h
        gui/controllers/FavoritesController.cpp
        gui/controllers/FavoritesController.h
        utils/Frame.h
        gui/platform/ITexture.h
        gui/controllers/SDRController.cpp
        gui/controllers/SDRController.h
        utils/ThreadSafeQueue.h
        hybrid/HybridDecoder.cpp
        hybrid/HybridDecoder.h
        hybrid/HybridReceiver.cpp
        hybrid/HybridReceiver.h

        gui/platform/IGPUContext.h

        gui/managers/SessionManager.cpp
        gui/managers/SessionManager.h
        hybrid/HybridSession.cpp
        hybrid/HybridSession.h
        gui/panels/DeviceList.cpp
        gui/panels/DeviceList.h
        input/SDRHost.cpp
        input/SDRHost.h
)

target_link_libraries(${PROJECT_NAME}-lib PUBLIC
        SDL3::SDL3
        imgui
        PNG::PNG
        lunasvg::lunasvg
        JPEG::JPEG
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        nrsc5::nrsc5
        Freetype::Freetype
        PortSDR
        Volk::volk
)

add_executable(${PROJECT_NAME}
        main.cpp
        Application.cpp
        Application.h
        AppState.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PROJECT_NAME}-lib
)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources/
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
)
add_dependencies(${PROJECT_NAME} copy_assets)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        RESOURCE "${OUTPUT_RESOURCE_FILES}"
)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RESOURCE DESTINATION ${PROJECT_RESOURCE_DIR}
)