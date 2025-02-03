include(FindPackageHandleStandardArgs)

find_library(LiquidDSP_LIBRARIES
        NAMES liquid libliquid
        PATH_SUFFIXES
        lib
)
find_path(LiquidDSP_INCLUDE_DIR
        liquid/liquid.h
        PATH_SUFFIXES
        include
)

find_package_handle_standard_args(LiquidDSP DEFAULT_MSG LiquidDSP_LIBRARIES LiquidDSP_INCLUDE_DIR)

if (LiquidDSP_FOUND)
    add_library(LiquidDSP::LiquidDSP INTERFACE IMPORTED)
    set_target_properties(LiquidDSP::LiquidDSP PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LiquidDSP_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LiquidDSP_LIBRARIES}"
    )
    mark_as_advanced(LiquidDSP_INCLUDE_DIR LiquidDSP_LIBRARIES)
endif ()
