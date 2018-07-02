cmake_minimum_required(VERSION 2.8.3)
project(protobuf_catkin)

find_package(catkin_simple REQUIRED)
catkin_simple()

include(ExternalProject)

file(MAKE_DIRECTORY ${CATKIN_DEVEL_PREFIX}/include)

ExternalProject_Add(protobuf_src
  URL https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz
  UPDATE_COMMAND ""
  PATCH_COMMAND patch -p0 < ${CMAKE_SOURCE_DIR}/unused_parameter_warnings.patch &&
                patch -p0 < ${CMAKE_SOURCE_DIR}/unused_parameter_warnings_wire_format.patch
  CONFIGURE_COMMAND cd ../protobuf_src && ./autogen.sh && ./configure --with-pic --prefix=${CATKIN_DEVEL_PREFIX}
  BUILD_COMMAND cd ../protobuf_src && make -j8 && cd python && python setup.py build --build-purelib build
  INSTALL_COMMAND cd ../protobuf_src && make install -j8 &&
                  cd python && python setup.py install --root ${CATKIN_DEVEL_PREFIX} --install-lib ${CATKIN_GLOBAL_PYTHON_DESTINATION} &&
                  cp build/google/__init__.py ${CATKIN_DEVEL_PREFIX}/${CATKIN_GLOBAL_PYTHON_DESTINATION}/google
)

install(DIRECTORY ${CATKIN_DEVEL_PREFIX}/include/google/protobuf
        DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
        FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CATKIN_DEVEL_PREFIX}/lib/
        DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        FILES_MATCHING PATTERN "libproto*")
install(FILES ${CATKIN_DEVEL_PREFIX}/bin/protoc
        DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
cs_export(INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include
          LIBRARIES protobuf protobuf-lite protoc
          CFG_EXTRAS protobuf-generate-cpp.cmake)
