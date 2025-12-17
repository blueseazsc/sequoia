add_library( app_base INTERFACE )
target_include_directories( app_base
    INTERFACE
    
    )
target_link_libraries( app_base
    INTERFACE
    sequoia_static
    )

# add apps
#APP_TARGET(app_xxx app_xxx.cc app_base)
