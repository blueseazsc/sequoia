set(EXPORT_BASENAME SEQUOIA)
set(PLATFORM_CONFIG_BASENAME SEQUOIA)
set(SYSTEM_CONFIG_BASENAME SEQUOIA)
set(VERSION_BASENAME SEQUOIA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

ADD_PLUGIN_LIBRARIES( sequoia shared_library static_library helloworld)

add_subdirectory(../external/project/cmake/env env)
add_subdirectory(library)
add_subdirectory(helloworld)
