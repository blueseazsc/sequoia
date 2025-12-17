set(EXPORT_BASENAME SEQUOIA)
set(PLATFORM_CONFIG_BASENAME SEQUOIA)
set(SYSTEM_CONFIG_BASENAME SEQUOIA)
set(VERSION_BASENAME SEQUOIA)

ADD_PLUGIN_LIBRARIES( sequoia shared_library static_library utils)

add_subdirectory(../external/project/cmake/env env)
add_subdirectory(library)
add_subdirectory(utils)
