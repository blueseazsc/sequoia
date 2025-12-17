add_library( test_base INTERFACE )
target_include_directories( test_base
    INTERFACE
    
    )
target_link_libraries( test_base
    INTERFACE
    sequoia_static
    spdlog::spdlog
	doctest::doctest
    Threads::Threads
    )

# add tests
TEST_TARGET(log_test log_test.cc test_base)
