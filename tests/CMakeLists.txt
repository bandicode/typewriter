
enable_testing()

find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED)

file(GLOB_RECURSE TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(tests ${TEST_SRC_FILES})
add_dependencies(tests textedit)
target_include_directories(tests PUBLIC "../include")
target_link_libraries(tests textedit)
target_link_libraries(tests Qt5::Test)