include(FindPackageHandleStandardArgs)

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_RTLSDR rtlsdr QUIET)
endif ()

if (NOT LIBUSB_FOUND)
    find_package(LIBUSB REQUIRED)
endif ()

find_path(RTLSDR_INCLUDE_DIR
        rtl-sdr.h
        PATHS ${PC_RTLSDR_INCLUDEDIR}
        DOC "RTLSDR include directory"
)
find_library(RTLSDR_LIBRARIES
        NAMES librtlsdr_static librtlsdr rtlsdr
        PATHS ${PC_RTLSDR_LIBDIR}
        DOC "RTLSDR library location"
)

find_package_handle_standard_args(RTLSDR DEFAULT_MSG
        RTLSDR_INCLUDE_DIR RTLSDR_LIBRARIES
)

if (RTLSDR_FOUND)
    add_library(rtlsdr::rtlsdr INTERFACE IMPORTED)
    set_target_properties(rtlsdr::rtlsdr PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${RTLSDR_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${RTLSDR_LIBRARIES}"
    )
    target_link_libraries(rtlsdr::rtlsdr INTERFACE libusb::libusb)

    mark_as_advanced(
            RTLSDR_LIBRARIES
            RTLSDR_STATIC_LIBRARIES
            RTLSDR_INCLUDE_DIR
    )
endif ()
