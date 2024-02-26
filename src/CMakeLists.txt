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

set_target_properties(generator PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_library(alec INTERFACE)
target_include_directories(alec INTERFACE .)

set_target_properties(alec PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER alec.hpp
)

install(TARGETS alec
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
