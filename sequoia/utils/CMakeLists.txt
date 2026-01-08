PROJECT_INIT_TARGET_VARIABLE_RECURSE(utils)

add_library( ${TARGET_NAME} OBJECT )

target_sources( ${TARGET_NAME}
	PRIVATE
	"$<BUILD_INTERFACE:${TARGET_HEADER_FILES}>"
	"$<BUILD_INTERFACE:${TARGET_SOURCE_FILES}>"
	)

target_include_directories( ${TARGET_NAME}
	PUBLIC
	"$<BUILD_INTERFACE:${TARGET_INCLUDE_PATH}>"
	"$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
	)

target_link_libraries(${TARGET_NAME}
	PUBLIC
	iconv
	spdlog::spdlog
	${CMAKE_DL_LIBS}
	Threads::Threads
	)

if (UTIL_TRACE_ENABLE)
target_compile_definitions(${TARGET_NAME}
	PUBLIC
	UTIL_TRACE_ENABLE=1
	)
target_link_libraries(${TARGET_NAME}
	PUBLIC
	Tracy::TracyClient
	)
endif()

######################################################################################
##  							INSTALL  					##
######################################################################################
install(FILES ${TARGET_HEADER_FILES}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${TARGET_NAME}")

PROJECT_INSTALL(${TARGET_NAME} ${TARGETS_EXPORT_NAME})
