cmake_minimum_required (VERSION 3.0)
project (UpdateDB)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_executable(
    UpdateDB
    main.cpp
    Connection.cpp
    Parser.cpp
    Stock.cpp
)

target_link_libraries(UpdateDB jsoncpp) #-ljsoncpp
target_link_libraries(UpdateDB mysqlclient) #-lmysqlclient
target_link_libraries(UpdateDB curl) #-lcurl
