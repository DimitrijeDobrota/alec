cmake_minimum_required(VERSION 3.25.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    Alec
	VERSION 0.0.3
    DESCRIPTION "Abstraction Layer for Escape Codes"
	HOMEPAGE_URL https://git.dimitrijedobrota.com/alec.git
    LANGUAGES CXX
)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
add_subdirectory(demo)

