
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(typewriter-widget STATIC ${HDR_FILES} ${SRC_FILES})

target_link_libraries(typewriter-widget typewriter)
target_link_libraries(typewriter-widget Qt5::Core Qt5::Gui Qt5::Widgets)
