cmake_minimum_required(VERSION 3.0.0)
project(testprotobuf VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(sockpp)
add_subdirectory(raylib)

file(GLOB protos
    "proto/gen/*.h"
    "proto/gen/*.cc"
    "tools/*.hpp"
    "tools/*.cpp"
    "tools/*.h"
    "tools/*.c"
)
file(GLOB serversrc
    "server/*.h"
    "server/*.cpp"
    "server/*.hpp"
    "server/*.c"
)
file(GLOB clientsrc
    "client/*.h"
    "client/*.cpp"
    "client/*.hpp"
    "client/*.c"
)
file(GLOB sharedsrc
    "shared/*.h"
    "shared/*.cpp"
    "shared/*.hpp"
    "shared/*.c"
)

add_library(minepackets ${protos})
target_include_directories(minepackets PRIVATE
    proto/gen
    sockpp/include
)
target_link_libraries(minepackets protobuf sockpp)

add_library(mineshared ${sharedsrc})
target_include_directories(mineshared PRIVATE
    proto/gen
)
target_link_libraries(mineshared minepackets)

add_executable(2dmine-server ${serversrc})
target_include_directories(2dmine-server PRIVATE
    sockpp/include
    proto/gen
    tools
    shared
)
target_link_libraries(2dmine-server minepackets sockpp mineshared)

add_executable(2dmine-client ${clientsrc})
target_include_directories(2dmine-client PRIVATE
    sockpp/include
    raylib/src
    raylib/src/external
    raylib/src/external/glfw/include
    raylib/src/external/glfw/include/glfw
    proto/gen
    tools
    shared
)
target_link_libraries(2dmine-client minepackets sockpp raylib mineshared)

# add_executable(testprotobuf ${basesrc} ${protos})


# target_link_libraries(testprotobuf protobuf sockpp raylib)
