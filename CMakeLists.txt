cmake_minimum_required(VERSION 3.12)

set(PROJECT Dipla)
project(${PROJECT} C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -pedantic -pthread -lboost_system")
set(JSON_Install OFF CACHE INTERNAL "")

find_package(Crow)
find_package( CURL REQUIRED )
find_package(Boost 1.71.0 REQUIRED COMPONENTS date_time) 

include_directories(third_party/json
		     third_party/libpqxx
		     third_party/cpp-jwt
                    third_party/)
add_subdirectory(third_party/json)
add_subdirectory(third_party/cpp-jwt)
add_subdirectory(third_party/libpqxx)
add_subdirectory(src)
