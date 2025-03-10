include(CMakeFindDependencyMacro)
find_dependency(cemplate)

if(cemplate_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/alecTargets.cmake")
endif()
