cmake_minimum_required(VERSION 2.6)
project(gelfudp2tcp)

find_package(Qt4 REQUIRED QtNetwork)
find_package(ZLIB REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QT_INCLUDES})
include_directories(${ZLIB_INCLUDE_DIRS})

set(gelfudp2tcp_SRCS 
    src/Decompress.cpp
    src/GelfChunkedMessage.cpp
    src/GelfChunkedMessageAssembler.cpp
    src/GelfTcpClient.cpp
    src/GelfUdpServer.cpp
    src/main.cpp)

qt4_automoc(${gelfudp2tcp_SRCS})
    
add_executable(gelfudp2tcp ${gelfudp2tcp_SRCS})
target_link_libraries(gelfudp2tcp Qt4::QtNetwork ${ZLIB_LIBRARIES})
set_property(TARGET gelfudp2tcp PROPERTY CXX_STANDARD 11)

install(TARGETS gelfudp2tcp RUNTIME DESTINATION bin)
install(FILES gelfudp2tcp.cfg.sample DESTINATION etc/graylog)
