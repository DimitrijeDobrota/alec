cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    alec
    VERSION 0.1.11
    DESCRIPTION "Abstraction Layer for Escape Codes"
    HOMEPAGE_URL "git://git.dimitrijedobrota.com/alec.git"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Build library ----

set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}/source")
file(MAKE_DIRECTORY ${PARSER_DIR})

find_package(FLEX)
find_package(BISON)

set(LEXER_OUT "${PARSER_DIR}/lexer.cpp")
set(PARSER_OUT "${PARSER_DIR}/parser.cpp")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(FLAGS "--debug")
endif()

FLEX_TARGET(LEXER source/lexer.l "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/scanner.hpp" COMPILE_FLAGS "${FLAGS}")
BISON_TARGET(PARSER source/parser.y "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parser.hpp" COMPILE_FLAGS "${FLAGS}")
ADD_FLEX_BISON_DEPENDENCY(LEXER PARSER)

set_source_files_properties(${LEXER_OUT} PROPERTIES SKIP_LINTING ON)
set_source_files_properties(${PARSER_OUT} PROPERTIES SKIP_LINTING ON)

add_executable(generator "${LEXER_OUT}" "${PARSER_OUT}" source/generator.cpp)
target_include_directories(generator PRIVATE source ${PARSER_DIR})
target_compile_features(generator PUBLIC cxx_std_20)

set(GENERATE_OUT_BIN "${PROJECT_BINARY_DIR}/bin")
set(GENERATE_OUT_INCLUDE "${PROJECT_BINARY_DIR}/include/alec")
file(MAKE_DIRECTORY ${GENERATE_OUT_BIN} ${GENERATE_OUT_INCLUDE})

set_target_properties(generator PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${GENERATE_OUT_BIN}"
)

set(RULES_NAME "alec.rules.hpp")
set(RULES_FILE "${GENERATE_OUT_INCLUDE}/${RULES_NAME}")
configure_file(source/alec.rules.hpp ${RULES_FILE} COPYONLY)

add_custom_command(
    OUTPUT ${GENERATE_OUT_INCLUDE}/alec.hpp
    COMMAND generator ${RULES_FILE} > ${GENERATE_OUT_INCLUDE}/alec.hpp
    DEPENDS generator source/${RULES_NAME}
    COMMENT "Generating include file"
)

# ---- Declare library ----

add_library(alec_alec INTERFACE ${GENERATE_OUT_INCLUDE}/alec.hpp)
add_library(alec::alec ALIAS alec_alec)

set_property(
    TARGET alec_alec PROPERTY
    EXPORT_NAME alec
)

target_include_directories(
    alec_alec ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
)

target_compile_features(alec_alec INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${alec_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()


# ---- Developer mode ----

if(NOT alec_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of alec"
  )
endif()

include(cmake/dev-mode.cmake)
