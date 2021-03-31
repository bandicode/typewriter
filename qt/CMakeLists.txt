
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets Quick Qml REQUIRED)

file(GLOB_RECURSE HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/typewriter/qt/*.h)
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE WIDGET_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/widget/*.cpp)
file(GLOB_RECURSE QML_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/*.cpp)

if(TYPEWRITER_BUILD_QT_WIDGET)
  list(APPEND SRC_FILES ${WIDGET_SRC_FILES})
endif()

if(TYPEWRITER_BUILD_QT_QML)
  list(APPEND SRC_FILES ${QML_SRC_FILES})
endif()

if(NOT DEFINED CACHE{TYPEWRITER_BUILD_QT_QML_LIB_STATIC})
  set(TYPEWRITER_BUILD_QT_QML_LIB_STATIC OFF CACHE BOOL "whether to build typewriter-qt as a static library")
endif()

if (TYPEWRITER_BUILD_QT_QML_LIB_STATIC)
  add_library(typewriter-qt STATIC ${HDR_FILES} ${SRC_FILES})
  target_compile_definitions(typewriter-qt PUBLIC -DTYPEWRITER_QT_BUILD_STATIC)
else()
  add_library(typewriter-qt SHARED ${HDR_FILES} ${SRC_FILES})
  target_compile_definitions(typewriter-qt PRIVATE -DTYPEWRITER_QT_BUILD_SHARED)
  set_target_properties(typewriter-qt PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(typewriter-qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

target_include_directories(typewriter-qt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(typewriter-qt typewriter)
target_link_libraries(typewriter-qt Qt5::Core Qt5::Gui)

if(TYPEWRITER_BUILD_QT_WIDGET)
  target_link_libraries(typewriter-qt Qt5::Widgets)
endif()

if(TYPEWRITER_BUILD_QT_QML)
  target_link_libraries(typewriter-qt Qt5::Quick Qt5::Qml)
endif()
