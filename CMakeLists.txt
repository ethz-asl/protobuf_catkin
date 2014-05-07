cmake_minimum_required(VERSION 2.8.3)
project(protobuf_catkin)

find_package(catkin_simple REQUIRED)
catkin_simple()

include(ExternalProject)

ExternalProject_Add(protobuf
  SVN_REPOSITORY http://protobuf.googlecode.com/svn/trunk/
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND cd ../protobuf && ./autogen.sh && ./configure --with-pic --prefix=${CATKIN_DEVEL_PREFIX}
  BUILD_COMMAND cd ../protobuf && make -j8
  INSTALL_COMMAND cd ../protobuf && make install -j8
)

cs_add_library(${PROJECT_NAME}_dependency_tracker src/dependency_tracker.cc)
link_directories(${CATKIN_DEVEL_PREFIX}/lib)
add_dependencies(${PROJECT_NAME}_dependency_tracker protobuf)
target_link_libraries(${PROJECT_NAME}_dependency_tracker 
  ${CATKIN_DEVEL_PREFIX}/lib/libprotobuf-lite.so 
  ${CATKIN_DEVEL_PREFIX}/lib/libprotoc.so 
  ${CATKIN_DEVEL_PREFIX}/lib/libprotobuf.so)

cs_install()

cs_export(
  INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include
  CFG_EXTRAS protobuf-generate-cpp.cmake)
