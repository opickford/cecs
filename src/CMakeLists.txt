add_library (cecs STATIC 
	"ecs.c" 

	# Headers for Visual Studio
	"../include/cecs/archetype.h"
	"../include/cecs/component.h"
	"../include/cecs/ecs.h"
	"../include/cecs/entity.h"
	"../include/cecs/view.h"
)

add_library(cecs::cecs ALIAS cecs)

target_include_directories(cecs 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
		$<INSTALL_INTERFACE:include>

	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/cecs>
)

# TODO: Do we need.
set_target_properties(cecs PROPERTIES
    DEBUG_POSTFIX d
)
