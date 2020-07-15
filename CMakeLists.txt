cmake_minimum_required(VERSION 3.9)
project(typewriter)

set(CMAKE_CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

##################################################################
###### dependencies
##################################################################

add_subdirectory(dependencies)

##################################################################
###### typewriter
##################################################################

file(GLOB_RECURSE SRC_TYPEWRITER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HDR_TYPEWRITER_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/typewriter/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(typewriter STATIC ${HDR_TYPEWRITER_FILES} ${SRC_TYPEWRITER_FILES})
target_include_directories(typewriter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(typewriter PUBLIC -DTYPEWRITER_STATIC_LINKING)
target_link_libraries(typewriter unicode-header-only)

foreach(_source IN ITEMS ${HDR_TYPEWRITER_FILES} ${SRC_TYPEWRITER_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

##################################################################
###### Qt widget
##################################################################

add_subdirectory(widget)

##################################################################
###### Tests, tools & examples
##################################################################

add_subdirectory(tests)

set(TEXTEDIT_BUILD_EXAMPLES FALSE CACHE BOOL "Check if you want to build the examples")
if(TEXTEDIT_BUILD_EXAMPLES)
add_subdirectory(examples)
endif()
