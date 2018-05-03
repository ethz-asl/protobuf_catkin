#include <iostream>

#include <google/protobuf/stubs/common.h>

int main() {
  constexpr uint64_t kProtobufVersion = GOOGLE_PROTOBUF_VERSION;
  constexpr uint64_t kProtobufMajorVersion = kProtobufVersion / 1000000;
  constexpr uint64_t kProtobufMinorVersion =
      (kProtobufVersion - kProtobufMajorVersion * 1000000) / 1000;
  constexpr uint64_t kProtobufPatchVersion =
      (kProtobufVersion - kProtobufMajorVersion * 1000000 -
       kProtobufMinorVersion * 1000);
  std::cout << kProtobufMajorVersion << '.' << kProtobufMinorVersion << '.'
            << kProtobufPatchVersion << std::endl;
}
