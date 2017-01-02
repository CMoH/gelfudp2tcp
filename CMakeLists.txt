cmake_minimum_required(VERSION 2.6)
project(gelfudp2tcp)

find_package(Qt4 REQUIRED QtNetwork)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(gelfudp2tcp_SRCS 
    src/GelfChunkedMessage.cpp
    src/GelfChunkedMessageAssembler.cpp
    src/GelfTcpClient.cpp
    src/GelfUdpServer.cpp
    src/main.cpp)

qt4_automoc(${gelfudp2tcp_SRCS})
    
add_executable(gelfudp2tcp ${gelfudp2tcp_SRCS})
target_link_libraries(gelfudp2tcp Qt4::QtNetwork)
set_property(TARGET gelfudp2tcp PROPERTY CXX_STANDARD 11)

install(TARGETS gelfudp2tcp RUNTIME DESTINATION bin)
install(FILES gelfudp2tcp.cfg.sample DESTINATION etc/graylog)
