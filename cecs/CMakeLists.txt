# CMakeList.txt : CMake project for ecs, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (ecs "main.c" "ecs.c" "ecs.h" "components.h" "component_list.h" "system.h" "entity.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ecs PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
