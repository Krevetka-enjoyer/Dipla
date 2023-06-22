add_executable(${PROJECT} "main.cpp")

add_library(Help Help.cpp)
target_link_libraries(Help CURL::libcurl)

add_library(Disk Disk.cpp Disk.h)
target_link_libraries(Disk Help CURL::libcurl nlohmann_json::nlohmann_json)

add_library(Tests RunningTest.cpp RunningTest.h)
target_link_libraries(Tests nlohmann_json::nlohmann_json)

add_library(JwtWork JwtWork.cpp)
target_link_libraries(JwtWork cpp-jwt::cpp-jwt)

add_library(Auth Auth.cpp Auth.h)
target_link_libraries(Auth JwtWork nlohmann_json::nlohmann_json pqxx)

add_library(Postgres PostgreInterface.cpp PostgreInterface.h)
target_link_libraries(Postgres Disk nlohmann_json::nlohmann_json pqxx)  

target_link_libraries(${PROJECT} Postgres Auth Tests nlohmann_json::nlohmann_json Crow::Crow pqxx) 

