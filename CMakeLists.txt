cmake_minimum_required(VERSION 3.9)
project(textedit)

set(CMAKE_CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

##################################################################
###### dependencies
##################################################################

add_subdirectory(dependencies)

##################################################################
###### textedit
##################################################################

file(GLOB_RECURSE SRC_TEXTEDIT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HDR_TEXTEDIT_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/textedit/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(textedit STATIC ${HDR_TEXTEDIT_FILES} ${SRC_TEXTEDIT_FILES})
target_include_directories(textedit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(textedit PUBLIC -DTEXTEDIT_STATIC_LINKING)
target_link_libraries(textedit unicode-header-only)
target_link_libraries(textedit Qt5::Core Qt5::Gui Qt5::Widgets)

foreach(_source IN ITEMS ${HDR_TEXTEDIT_FILES} ${SRC_TEXTEDIT_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

##################################################################
###### Tests, tools & examples
##################################################################

add_subdirectory(tests)

set(TEXTEDIT_BUILD_EXAMPLES FALSE CACHE BOOL "Check if you want to build the examples")
if(TEXTEDIT_BUILD_EXAMPLES)
add_subdirectory(examples)
endif()
