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
###### Qt 
##################################################################

set(TYPEWRITER_BUILD_QT_WIDGET OFF CACHE BOOL "whether to build the 'typewriter' Qt widget")
set(TYPEWRITER_BUILD_QT_QML OFF CACHE BOOL "whether to build the 'typewriter' Qt QML")

if (TYPEWRITER_BUILD_QT_WIDGET OR TYPEWRITER_BUILD_QT_QML)
  add_subdirectory(qt)
endif()

##################################################################
###### Tests, tools & examples
##################################################################

set(TYPEWRITER_BUILD_TESTS ON CACHE BOOL "whether to build the 'typewriter' tests")

if (TYPEWRITER_BUILD_TESTS)
  add_subdirectory(tests)
endif()

set(TYPEWRITER_BUILD_EXAMPLES FALSE CACHE BOOL "Check if you want to build the 'typewriter' examples")
if(TYPEWRITER_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
