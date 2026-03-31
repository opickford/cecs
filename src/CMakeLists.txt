find_package(chds 0.2 REQUIRED)

add_library (cecs STATIC 
	"ecs_internal.h"
	"ecs.c"
	"archetype_internal.h"
	"archetype.c"
	"view_internal.h"
	"view.c"
	"component_internal.h"

	# Headers for Visual Studio
	"../include/cecs/archetype.h"
	"../include/cecs/component.h"
	"../include/cecs/ecs.h"
	"../include/cecs/entity.h"
	"../include/cecs/view.h"
)

add_library(cecs::cecs ALIAS cecs)

target_link_libraries(cecs 
    PRIVATE
        chds::chds
)

target_include_directories(cecs 
	PUBLIC
		$<BUILD_INTERFACE:${CECS_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CECS_BINARY_DIR}/include>
		$<INSTALL_INTERFACE:include>

	PRIVATE
		$<BUILD_INTERFACE:${CECS_SOURCE_DIR}/include/cecs>
)

# TODO: Do we need.
set_target_properties(cecs PROPERTIES
    DEBUG_POSTFIX d
)
