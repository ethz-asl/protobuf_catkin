cmake_minimum_required(VERSION 2.8.5)
project(protobuf_catkin)

find_package(catkin_simple REQUIRED)
catkin_simple()

include(ExternalProject)

file(MAKE_DIRECTORY ${CATKIN_DEVEL_PREFIX}/include)

set(PROTOBUF_MINIMUM_VERSION 2.6.1)

set(USE_SYSTEM_PROTOBUF "AUTO" CACHE INTERNAL "Whether to use the system version of protobuf.")

if(USE_SYSTEM_PROTOBUF STREQUAL "AUTO")
  find_package(Protobuf QUIET)
  set(USE_SYSTEM_PROTOBUF "OFF")
  if(PROTOBUF_FOUND)
    message(STATUS "PROTOBUF_LITE_LIBRARY: ${PROTOBUF_LITE_LIBRARY}")
    message(STATUS "PROTOBUF_PROTOC_LIBRARY: ${PROTOBUF_PROTOC_LIBRARY}")
    message(STATUS "PROTOBUF_LIBRARY: ${PROTOBUF_LIBRARY}")

    # Compile a little program to figure out the installed protobuf version.
    # Note: with cmake >= 3.6, FindProtobuf should set a variable to indicate
    # the protobuf version.
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -std=c++11 -lprotobuf
              -o get_protobuf_version
              ${CMAKE_SOURCE_DIR}/get_protobuf_version.cc
      WORKING_DIRECTORY .
      RESULT_VARIABLE CXX_RESULT)

    if(${CXX_RESULT} EQUAL 0)
      execute_process(
        COMMAND "${CMAKE_BINARY_DIR}/get_protobuf_version"
        WORKING_DIRECTORY .
        RESULT_VARIABLE PROTOBUF_VERSION_RETURN_VALUE
        OUTPUT_VARIABLE PROTOBUF_INSTALLED_VERSION)
      if(${PROTOBUF_VERSION_RETURN_VALUE} EQUAL 0)
        if((${PROTOBUF_INSTALLED_VERSION} VERSION_GREATER
                ${PROTOBUF_MINIMUM_VERSION}) OR
            (${PROTOBUF_INSTALLED_VERSION} VERSION_EQUAL
                ${PROTOBUF_MINIMUM_VERSION}))
          message(STATUS "Installed protobuf version is "
                         "${PROTOBUF_INSTALLED_VERSION}, which is newer or "
                         "equal to the required minimum version "
                         "${PROTOBUF_MINIMUM_VERSION}.")
          set(USE_SYSTEM_PROTOBUF "ON")
        endif()
      else()
        message(WARNING "Failed to determine protobuf version -- will fall "
                        "back to install custom version.")
      endif()
    else()
      message(WARNING "Couldn't compile protobuf test program -- will install "
                      "custom version.")
    endif()
  else()
    message(STATUS "No protobuf library found -- will install custom versoin.")
  endif()
endif()

if(USE_SYSTEM_PROTOBUF)
  message(STATUS "protobuf_catkin is configured to USE system protobuf.")
  find_package(Protobuf REQUIRED)

  # Copy protobuf compiler to catkin workspace so that both system and
  # downloaded library will have the same path to the compiler.
  if(EXISTS ${PROTOBUF_PROTOC_EXECUTABLE})
    message(STATUS "Found protobuf compiler under: "
                   "${PROTOBUF_PROTOC_EXECUTABLE}")
    file(COPY ${PROTOBUF_PROTOC_EXECUTABLE}
         DESTINATION "${CATKIN_DEVEL_PREFIX}/bin")
  else()
    message(FATAL_ERROR "protobuf_catkin is set to use system protobuf if new "
                        "enough, however, protoc is not installed. Please "
                        "install it with\n\tsudo apt install protobuf-compiler")
  endif()
else()
  message(STATUS "protobuf_catkin will download and compile protobuf library.")
  ExternalProject_Add(protobuf_src
    URL https://github.com/google/protobuf/releases/download/v${PROTOBUF_MINIMUM_VERSION}/protobuf-${PROTOBUF_MINIMUM_VERSION}.tar.gz
    UPDATE_COMMAND ""
    PATCH_COMMAND patch -p0 < ${CMAKE_SOURCE_DIR}/unused_parameter_warnings.patch &&
                  patch -p0 < ${CMAKE_SOURCE_DIR}/unused_parameter_warnings_wire_format.patch
    CONFIGURE_COMMAND cd ../protobuf_src && ./autogen.sh && ./configure --with-pic --prefix=${CATKIN_DEVEL_PREFIX}
    BUILD_COMMAND cd ../protobuf_src && make -j8 && cd python && python setup.py build --build-purelib build
    INSTALL_COMMAND cd ../protobuf_src && make install -j8 &&
                    cd python && python setup.py install --root ${CATKIN_DEVEL_PREFIX} --install-lib ${CATKIN_GLOBAL_PYTHON_DESTINATION} &&
                    cp build/google/__init__.py ${CATKIN_DEVEL_PREFIX}/${CATKIN_GLOBAL_PYTHON_DESTINATION}/google
  )

  set(PROTOBUF_LITE_LIBRARY
      "${CATKIN_DEVEL_PREFIX}/lib/libprotobuf-lite${CMAKE_SHARED_LIBRARY_SUFFIX}")
  set(PROTOBUF_PROTOC_LIBRARY
      "${CATKIN_DEVEL_PREFIX}/lib/libprotoc${CMAKE_SHARED_LIBRARY_SUFFIX}")
  set(PROTOBUF_LIBRARY
      "${CATKIN_DEVEL_PREFIX}/lib/libprotobuf${CMAKE_SHARED_LIBRARY_SUFFIX}")

  cs_add_library(${PROJECT_NAME} src/dependency_tracker.cc)
  add_dependencies(${PROJECT_NAME} protobuf_src)
  target_link_libraries(${PROJECT_NAME}
                        ${PROTOBUF_LITE_LIBRARY}
                        ${PROTOBUF_PROTOC_LIBRARY}
                        ${PROTOBUF_LIBRARY})
endif()

message(STATUS "PROTOBUF_LITE_LIBRARY: ${PROTOBUF_LITE_LIBRARY}")
message(STATUS "PROTOBUF_PROTOC_LIBRARY: ${PROTOBUF_PROTOC_LIBRARY}")
message(STATUS "PROTOBUF_LIBRARY: ${PROTOBUF_LIBRARY}")
cs_install()
cs_export(
  INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include
  CFG_EXTRAS protobuf-generate-cpp.cmake protobuf-extras.cmake)
