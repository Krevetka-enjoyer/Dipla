cmake_minimum_required(VERSION 3.12)

set(PROJECT Dipla)
project(${PROJECT} C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wextra -pedantic -pthread -lboost_system")
set(JSON_Install OFF CACHE INTERNAL "")

find_package(Crow)
include_directories(third_party/json-develop
		     third_party/libpqxx
		     third_party/cpp-jwt
                    third_party/)
add_subdirectory(third_party/json-develop)
add_subdirectory(third_party/cpp-jwt)
add_subdirectory(third_party/libpqxx)
add_subdirectory(src)
