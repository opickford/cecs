add_executable(test_entity 
    "test_entity.c"
)

target_link_libraries(test_entity PRIVATE cecs)

add_test(NAME test_entity COMMAND test_entity)
