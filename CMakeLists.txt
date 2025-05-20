cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    alec
    VERSION 0.2.0
    DESCRIPTION "Abstraction Layer for Escape Codes"
    HOMEPAGE_URL "git://git.dimitrijedobrota.com/alec.git"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(alec_alec INTERFACE)
add_library(alec::alec ALIAS alec_alec)

find_package(based 0.1.2 CONFIG REQUIRED)
target_link_libraries(alec_alec INTERFACE based::based)

set_property(
    TARGET alec_alec PROPERTY
    EXPORT_NAME alec
)

target_include_directories(
    alec_alec ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
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
