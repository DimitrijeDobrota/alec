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
