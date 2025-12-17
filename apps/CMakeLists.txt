add_library( app_base INTERFACE )
target_include_directories( app_base
    INTERFACE
    
    )
target_link_libraries( app_base
    INTERFACE
    sequoia_static
    )

# add apps
APP_TARGET_WITH_STRIP( log_print log_print.cc app_base)
