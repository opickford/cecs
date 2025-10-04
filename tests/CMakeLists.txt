add_executable(tests 
    "main.c"
    
    "test_entity.h"
    )

target_link_libraries(tests PRIVATE cecs)
