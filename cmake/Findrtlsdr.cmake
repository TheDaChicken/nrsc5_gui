include(FindPackageHandleStandardArgs)

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_rtlsdr rtlsdr QUIET)
endif ()

find_package(LIBUSB REQUIRED)

find_path(rtlsdr_INCLUDE_DIR
        rtl-sdr.h
        PATHS ${PC_rtlsdr_INCLUDEDIR}
        DOC "rtlsdr include directory"
)
find_library(rtlsdr_LIBRARIES
        NAMES librtlsdr_static librtlsdr rtlsdr
        PATHS ${PC_rtlsdr_LIBDIR}
        DOC "rtlsdr library location"
)

find_package_handle_standard_args(rtlsdr DEFAULT_MSG
        rtlsdr_INCLUDE_DIR rtlsdr_LIBRARIES
)

if (rtlsdr_FOUND AND NOT TARGET rtlsdr::rtlsdr)
    add_library(rtlsdr::rtlsdr INTERFACE IMPORTED)
    set_target_properties(rtlsdr::rtlsdr PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${rtlsdr_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${rtlsdr_LIBRARIES}"
    )
    target_link_libraries(rtlsdr::rtlsdr INTERFACE libusb::libusb)

    mark_as_advanced(
            rtlsdr_LIBRARIES
            rtlsdr_STATIC_LIBRARIES
            rtlsdr_INCLUDE_DIR
    )
endif ()