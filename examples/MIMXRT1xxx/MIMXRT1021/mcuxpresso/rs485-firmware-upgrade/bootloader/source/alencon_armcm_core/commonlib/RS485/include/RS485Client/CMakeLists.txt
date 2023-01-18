project (RS485)

include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${PROJECT_SOURCE_DIR}/../include/RS485Client/)

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/../include/RS485Config.h.in"
  "${PROJECT_BINARY_DIR}/RS485Config.h"
)

add_executable(../../src/RS485Demo ../../src/RS485Demo/rs485demo.cxx)
add_library(../../src/RS485Client ../../src/RS485Client/RS485Client.c ../../src/RS485Client/RS485Packet.c ../../src/RS485Client/crc.c)