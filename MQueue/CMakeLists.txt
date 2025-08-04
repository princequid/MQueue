cmake_minimum_required(VERSION 3.10)
project(MQueue CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(broker client network utils)
set(SOURCES
        main.cpp
        broker/Broker.cpp
        broker/TopicManager.cpp
        broker/Persistence.cpp
        network/SocketServer.cpp
        client/Publisher.cpp
        client/Subscriber.cpp
)
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(WINSOCK_LIBRARIES Ws2_32)
endif()
add_executable(broker
        main.cpp
        broker/Broker.cpp
        broker/TopicManager.cpp
        broker/Persistence.cpp
        network/SocketServer.cpp
)
add_executable(publisher client/Publisher.cpp)
add_executable(subscriber client/Subscriber.cpp)
if(WIN32)
    target_link_libraries(broker ${WINSOCK_LIBRARIES})
    target_link_libraries(publisher ${WINSOCK_LIBRARIES})
    target_link_libraries(subscriber ${WINSOCK_LIBRARIES})
endif()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/MessageStore)