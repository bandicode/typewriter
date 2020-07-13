
enable_testing()

if(NOT DEFINED TYPEWRITER_DOWNLOADED_CATCH2 OR NOT TYPEWRITER_DOWNLOADED_CATCH2)

  message("Downloading Catch2")
  file(DOWNLOAD https://github.com/catchorg/Catch2/releases/download/v2.11.3/catch.hpp "${CMAKE_CURRENT_LIST_DIR}/catch.hpp" SHOW_PROGRESS)
  
  set(TYPEWRITER_DOWNLOADED_CATCH2 TRUE CACHE BOOL "Whether Catch2 was downloaded")

endif()

file(GLOB_RECURSE TYPEWRITER_TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(TEST_typewriter "catch.hpp" ${TYPEWRITER_TEST_SRC_FILES})
target_include_directories(TEST_typewriter PUBLIC "../include")
target_link_libraries(TEST_typewriter textedit)

file(COPY "catch.hpp" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
