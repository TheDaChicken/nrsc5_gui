include(FindPackageHandleStandardArgs)

find_package(PkgConfig QUIET)

if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_LIBUSB libusb-1.0)
endif ()

find_path(LibUSB_INCLUDE_DIR
        NAMES libusb.h
        HINTS ${PC_LIBUSB_INCLUDEDIR}
        PATH_SUFFIXES include libusb include/libusb-1.0
)

find_library(LibUSB_LIBRARY
        NAMES libusb-1.0.so libusb-1.0.a
        HINTS ${PC_LIBUSB_LIBDIR} $ENV{LibUSB_ROOT}/include/libusb-1.0
        PATH_SUFFIXES lib
        PATHS /usr/local/lib /usr/lib
)

find_package_handle_standard_args(LibUSB DEFAULT_MSG LibUSB_INCLUDE_DIR LibUSB_LIBRARY)

if (LibUSB_FOUND AND NOT TARGET LibUSB::LibUSB)
    add_library(LibUSB::LibUSB IMPORTED UNKNOWN)
    set_target_properties(LibUSB::LibUSB PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LibUSB_INCLUDE_DIR}"
            IMPORTED_LOCATION "${LibUSB_LIBRARY}"
    )
    mark_as_advanced(LibUSB_INCLUDE_DIR LibUSB_LIBRARY)
endif ()
