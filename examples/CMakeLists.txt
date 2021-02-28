
file(GLOB_RECURSE TYPEWRITER_EXAMPLES_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE TYPEWRITER_EXAMPLES_HRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(typewriter-examples ${TYPEWRITER_EXAMPLES_HRC_FILES} ${TYPEWRITER_EXAMPLES_SRC_FILES})
target_link_libraries(typewriter-examples typewriter-qt)

set_target_properties(typewriter-examples PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(typewriter-examples PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_include_directories(typewriter-examples PUBLIC ".")
