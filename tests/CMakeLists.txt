add_library( test_base INTERFACE )
target_include_directories( test_base
    INTERFACE
    
    )
target_link_libraries( test_base
    INTERFACE
    doctest::doctest
    sequoia_static
    )

# add tests
#TEST_TARGET(test_xxx test_xxx.cc test_base)
