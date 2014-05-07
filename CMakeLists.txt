cmake_minimum_required(VERSION 2.8.3)
project(protobuf_catkin)

find_package(catkin_simple REQUIRED)
catkin_simple()

include(ExternalProject)

ExternalProject_Add(protobuf_src
  SVN_REPOSITORY http://protobuf.googlecode.com/svn/trunk/
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND cd ../protobuf_src && ./autogen.sh && ./configure --with-pic --prefix=${CATKIN_DEVEL_PREFIX}
  BUILD_COMMAND cd ../protobuf_src && make -j8
  INSTALL_COMMAND cd ../protobuf_src && make install -j8
)

# TODO(dymczykm, tcies) would be cool to get rid of this everywhere
link_directories(${CATKIN_DEVEL_PREFIX}/lib)

cs_add_library(${PROJECT_NAME}_dependency_tracker src/dependency_tracker.cc)
add_dependencies(${PROJECT_NAME}_dependency_tracker protobuf_src)
target_link_libraries(${PROJECT_NAME}_dependency_tracker 
  protobuf-lite 
  protoc
  protobuf)

cs_install()

cs_export(
  INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include
  CFG_EXTRAS protobuf-generate-cpp.cmake)
