PROJECT_INIT_TARGET_VARIABLE(model)

add_library( ${TARGET_NAME} OBJECT )

message("TARGET_HEADER_FILES: ${TARGET_HEADER_FILES}")
message("TARGET_SOURCE_FILES: ${TARGET_SOURCE_FILES}")
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
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
	)
target_link_libraries(${TARGET_NAME}
	PUBLIC
	odb::odb
	)

######################################################################################
set(TARGET_NAME_PREBUILD prebuild)
set(TARGET_NAME_STAMP generated.stamp)
set(SQL_PREFIX model)
set(SQL_DIR odb_sql)
set(GEN_DIR odb_gen)

message("TARGET_NAME_PREBUILD: ${TARGET_NAME_PREBUILD}")
message("TARGET_NAME_STAMP: ${TARGET_NAME_STAMP}")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME_STAMP}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${GEN_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${SQL_DIR}
    COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name "*.hxx" | sort | xargs odb -I/usr/local/include -d mysql --std c++20 --generate-query --generate-schema-only --at-once --input-name ${SQL_PREFIX} --generate-schema --changelog ${CMAKE_CURRENT_SOURCE_DIR}/changelog.xml -o ${CMAKE_CURRENT_BINARY_DIR}/${SQL_DIR}
    COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name "*.hxx" | sort | xargs odb -I/usr/local/include -d mysql --std c++20 --generate-query -o ${CMAKE_CURRENT_BINARY_DIR}/${GEN_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME_STAMP}
    COMMENT "Generating build prerequisites"
)

add_custom_target(${TARGET_NAME_PREBUILD} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME_STAMP})

add_dependencies(${TARGET_NAME} ${TARGET_NAME_PREBUILD})

target_include_directories( ${TARGET_NAME}
	PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${GEN_DIR}>"
	)

######################################################################################
##  							INSTALL  					##
######################################################################################
install(FILES ${TARGET_HEADER_FILES}
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${TARGET_NAME}")

PROJECT_INSTALL(${TARGET_NAME} ${TARGETS_EXPORT_NAME})
