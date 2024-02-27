set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}")

find_package(FLEX)
find_package(BISON)

set(LEXER_OUT "${PARSER_DIR}/lexer.c")
set(PARSER_OUT "${PARSER_DIR}/parser.c")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(FLAGS "--debug")
endif()

FLEX_TARGET(LEXER lexer.l "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/scanner.h" COMPILE_FLAGS "${FLAGS}")
BISON_TARGET(PARSER parser.y "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parser.h" COMPILE_FLAGS "${FLAGS}")
ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

add_executable(generator "${LEXER_OUT}" "${PARSER_OUT}")
target_include_directories(generator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

set(GENERATE_OUT "${CMAKE_BINARY_DIR}/bin")

set_target_properties(generator PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${GENERATE_OUT}"
)

set(RULES_NAME "alec.rules.hpp")
set(RULES_FILE "${CMAKE_BINARY_DIR}/${RULES_NAME}")

configure_file(alec.rules.hpp ${RULES_FILE} COPYONLY)

add_custom_command(
    OUTPUT ${GENERATE_OUT}/alec.hpp
    COMMAND generator ${RULES_FILE} > ${GENERATE_OUT}/alec.hpp
    DEPENDS generator ${RULES_NAME}
    COMMENT "Generating include file"
)

add_library(alec INTERFACE ${GENERATE_OUT}/alec.hpp)
target_include_directories(alec INTERFACE ${GENERATE_OUT})

set_target_properties(alec PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER ${GENERATE_OUT}/alec.hpp
)

install(TARGETS alec
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
