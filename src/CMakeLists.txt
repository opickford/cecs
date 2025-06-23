add_library (cecs STATIC 
	"ecs.c" 
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
