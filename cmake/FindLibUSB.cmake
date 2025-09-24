include(FindPackageHandleStandardArgs)

find_package(PkgConfig)

if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_LIBUSB libusb-1.0)
endif ()

find_path(LIBUSB_INCLUDE_DIR
        NAMES libusb.h
        HINTS ${PC_LIBUSB_INCLUDEDIR}
        PATH_SUFFIXES include libusb include/libusb-1.0
)

find_library(LIBUSB_LIBRARIES
        NAMES libusb-1.0.so libusb-1.0.a
        HINTS ${PC_LIBUSB_LIBDIR} $ENV{LIBUSB_ROOT}/include/libusb-1.0
        PATH_SUFFIXES lib
        PATHS /usr/local/lib /usr/lib
)

find_package_handle_standard_args(LIBUSB DEFAULT_MSG LIBUSB_INCLUDE_DIR LIBUSB_LIBRARIES)

if (LIBUSB_FOUND)
    add_library(libusb::libusb INTERFACE IMPORTED)
    set_target_properties(libusb::libusb PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBUSB_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LIBUSB_LIBRARIES}"
    )
    mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARIES)
endif ()
