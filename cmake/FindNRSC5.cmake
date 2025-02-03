include(FindPackageHandleStandardArgs)

if(NRSC5_STATIC)
    set(NRSC5_LIBRARY_NAME nrsc5_static)
else()
    set(NRSC5_LIBRARY_NAME nrsc5)
endif()

message("${NRSC5_INCLUDE_DIR}")
message("${NRSC5_LIBRARIES}")

find_library(NRSC5_LIBRARIES
        NAMES ${NRSC5_LIBRARY_NAME}
        DOC "NRSC5 library location"
)

find_path(NRSC5_INCLUDE_DIR
        NAMES nrsc5.h
        PATH_SUFFIXES include
        DOC "NRSC5 include directory"
)

find_library(NRSC5_FAAD2
        NAMES libfaad_hdc.a libfaad_hdc.dll.a
        DOC "NRSC5 FAAD2 library location"
)

if (NOT RTLSDR_FOUND)
    find_package(RTLSDR REQUIRED)
endif ()

if (NOT FFTW_FLOAT_LIB_FOUND)
    find_package(FFTW REQUIRED COMPONENTS FLOAT_LIB)
endif ()

find_package_handle_standard_args(NRSC5 DEFAULT_MSG NRSC5_INCLUDE_DIR NRSC5_LIBRARIES NRSC5_FAAD2)

if (NRSC5_FOUND)
    add_library(nrsc5::faad2_hdc INTERFACE IMPORTED)
    set_target_properties(nrsc5::faad2_hdc PROPERTIES INTERFACE_LINK_LIBRARIES "${NRSC5_FAAD2}")

    add_library(nrsc5::nrsc5 INTERFACE IMPORTED)
    set_target_properties(nrsc5::nrsc5 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${NRSC5_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${NRSC5_LIBRARIES}"
    )

    target_link_libraries(nrsc5::nrsc5 INTERFACE
            FFTW::Float
            rtlsdr::rtlsdr
            nrsc5::faad2_hdc
    )

    if (WIN32)
        target_link_libraries(nrsc5::nrsc5 INTERFACE ws2_32)
    endif ()

    mark_as_advanced(
            NRSC5_LIBRARIES
            NRSC5_INCLUDE_DIR
            NRSC5_FAAD2
    )
endif ()
