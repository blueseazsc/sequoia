PROJECT_INIT_TARGET_VARIABLE(shared_library)

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
	)
target_compile_definitions(${TARGET_NAME} PUBLIC ${EXPORT_BASENAME}_SHARED_LIBS)
PROJECT_INSTALL(${TARGET_NAME} ${TARGETS_EXPORT_NAME})

PROJECT_INIT_TARGET_VARIABLE(static_library)

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
	)
PROJECT_INSTALL(${TARGET_NAME} ${TARGETS_EXPORT_NAME})

######################################################################################
##  							INSTALL  					##
######################################################################################
install(FILES ${TARGET_HEADER_FILES}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/library")

