
set(CMAKE_AUTORCC TRUE)

file(GLOB_RECURSE TEXTEDIT_EXAMPLES_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE TEXTEDIT_EXAMPLES_HRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(examples ${TEXTEDIT_EXAMPLES_HRC_FILES} ${TEXTEDIT_EXAMPLES_SRC_FILES})
add_dependencies(examples textedit)

target_link_libraries(examples textedit)

set_target_properties(examples PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(examples PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_include_directories(examples PUBLIC ".")
