set(LIBRARY_NAME "${PROJECT_NAME}-lib")
set(EXECUTABLE_PROJECT ${PROJECT_NAME})

set(SOURCE_FILES
        audio/stream/StreamParametersDirectionSpecific.cpp
        audio/stream/StreamParameters.cpp
        audio/stream/StreamOutputPush.cpp
        audio/stream/StreamBuffer.cpp
        audio/stream/Stream.cpp
        audio/HostApi.cpp
        audio/Device.cpp
        audio/PortAudioCpp.cpp

        pages/LoadingPage.cpp
        pages/RadioPage.cpp
        pages/SettingsPage.cpp

        settings/TunerPage.cpp
        settings/AboutPage.cpp

        sql/Connection.cpp
        sql/ConnectionPool.cpp
        sql/Database.cpp
        sql/sqlite/sqlite3.c

        nrsc5/LotManager.cpp
        nrsc5/Decoder.cpp
        nrsc5/Station.cpp

        threads/GuiSyncThread.cpp

        images/StationImageProvider.cpp
        images/ImageProviderPriorityManager.cpp
        images/providers/LotImageProvider.cpp

        models/FavoriteModel.cpp
        models/TunerDevicesModel.cpp

        delegates/ChannelDelegate.cpp
        delegates/BasicTextDelegate.cpp

        controllers/StationInfoManager.cpp

        display/StationInfoPanel.cpp
        display/RadioInfoDisplay.cpp
        display/RadioInfoDisplay.ui

        views/RadioControlPanelView.cpp
        views/RadioMainView.cpp
        views/SettingsView.cpp

        dsp/ArbResampler.cpp

        widgets/dualview/DualViewContainer.h
        widgets/dualview/DualViewWidget.cpp
        widgets/dualview/DualViewLayout.cpp
        widgets/navigation/NavigationHeader.cpp
        widgets/navigation/NavigationWidget.cpp
        widgets/AnimatedStackedWidget.cpp
        widgets/StationFramePanel.cpp
        widgets/favorites/FavoritesList.cpp
        widgets/favorites/FavoritesHeader.cpp
        widgets/ListViewHeader.cpp
        widgets/LineHeader.cpp
        widgets/text/ImageText.cpp
        widgets/text/ImageTextLabel.cpp
        widgets/text/ImageTextEdit.cpp
        widgets/text/TextSlider.cpp
        widgets/progress/ProgressCircleText.cpp
        widgets/CapsLabelAlign.cpp
        widgets/RoundButton.cpp
        widgets/sdr/TuneWidget.cpp
        widgets/ClickableQLabel.cpp
        widgets/sdr/GainSettings.cpp

        themes/ThemeManager.cpp

        utils/StylingText.cpp
        utils/Image.cpp
        utils/Log.cpp
        utils/StringUtils.cpp
        utils/RingBuffer.cpp
        utils/MessageQueue.cpp

        windows/MainWindow.cpp
        windows/MainWindow.ui

        Application.cpp
        controllers/HybridRadio.cpp
        HybridTuner.cpp
        dsp/Firdes.cpp
        dsp/window.cpp
)

qt_add_library(${LIBRARY_NAME} OBJECT ${SOURCE_FILES})
target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Svg
        Qt6::OpenGL
        Qt6::Concurrent
        Qt6::PrintSupport

        nrsc5::nrsc5
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json

        portaudio
        PortSDR

        Volk::volk
        FFTW::Float
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_executable(${EXECUTABLE_PROJECT} Main.cpp ${CMAKE_SOURCE_DIR}/resources/Resources.qrc)
target_link_libraries(${EXECUTABLE_PROJECT} PRIVATE ${LIBRARY_NAME})
target_include_directories(${EXECUTABLE_PROJECT} PRIVATE ${CMAKE_BINARY_DIR}/autogen/)
set_target_properties(${EXECUTABLE_PROJECT} PROPERTIES
        AUTOUIC ON
        AUTORCC ON
        AUTOMOC ON
)
set_target_properties(${EXECUTABLE_PROJECT} PROPERTIES LINK_FLAGS "${LINKER_FLAGS}")
set_target_properties(${EXECUTABLE_PROJECT} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)
add_dependencies(${EXECUTABLE_PROJECT} LibVolkExternal)

install(TARGETS ${EXECUTABLE_PROJECT}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (APPLE)
    set(executable_path "$<TARGET_FILE_NAME:${EXECUTABLE_PROJECT}>.APPLICATION")
else ()
    set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${EXECUTABLE_PROJECT}>")
endif ()

set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_${EXECUTABLE_PROJECT}.cmake")

file(GENERATE OUTPUT ${deploy_script} CONTENT "
include(\"${QT_DEPLOY_SUPPORT}\")
qt_deploy_runtime_dependencies(
    EXECUTABLE \"${executable_path}\"
    PLUGINS_DIR bin
    NO_TRANSLATIONS
)"
)

install(SCRIPT ${deploy_script})