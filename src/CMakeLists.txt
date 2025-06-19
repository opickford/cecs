add_library (cecs STATIC 
	"ecs.c" 
)

# Make users include with lib name prefix to avoid potential clashes.
cmake_path(SET publicIncludeDir NORMALIZE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
cmake_path(SET privateIncludeDir NORMALIZE ${CMAKE_CURRENT_SOURCE_DIR}/../include/cecs)

target_include_directories(cecs 
	PUBLIC ${publicIncludeDir}
	PRIVATE ${privateIncludeDir}
)