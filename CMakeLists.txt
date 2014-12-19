cmake_minimum_required(VERSION 2.8.3)
project(protobuf_catkin)

find_package(catkin_simple REQUIRED)
catkin_simple()

include(ExternalProject)

file(MAKE_DIRECTORY ${CATKIN_DEVEL_PREFIX}/include)

ExternalProject_Add(protobuf_src
  URL https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz
  UPDATE_COMMAND ""
  PATCH_COMMAND cd ../protobuf_src/ && patch -N src/google/protobuf/extension_set.h ${CMAKE_CURRENT_SOURCE_DIR}/extension_set_h.patch && patch -N src/google/protobuf/stubs/common.h ${CMAKE_CURRENT_SOURCE_DIR}/common_h.patch
  CONFIGURE_COMMAND cd ../protobuf_src && ./autogen.sh && ./configure --with-pic --prefix=${CATKIN_DEVEL_PREFIX}
  BUILD_COMMAND cd ../protobuf_src && make -j8
  INSTALL_COMMAND cd ../protobuf_src && make install -j8
)

cs_add_library(${PROJECT_NAME} src/dependency_tracker.cc)
add_dependencies(${PROJECT_NAME} protobuf_src)
target_link_libraries(${PROJECT_NAME} 
  ${CATKIN_DEVEL_PREFIX}/lib/libprotobuf-lite${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${CATKIN_DEVEL_PREFIX}/lib/libprotoc${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${CATKIN_DEVEL_PREFIX}/lib/libprotobuf${CMAKE_SHARED_LIBRARY_SUFFIX})

cs_install()

cs_export(
  INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include
  CFG_EXTRAS protobuf-generate-cpp.cmake protobuf-extras.cmake)
