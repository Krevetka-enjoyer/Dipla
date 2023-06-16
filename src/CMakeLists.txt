add_executable(${PROJECT} "main.cpp")

add_library(JwtWork JwtWork.cpp)
target_link_libraries(JwtWork cpp-jwt::cpp-jwt)

add_library(Postgres PostgreInterface.cpp PostgreInterface.h)
target_link_libraries(Postgres nlohmann_json::nlohmann_json pqxx)  

target_link_libraries(${PROJECT} Postgres JwtWork nlohmann_json::nlohmann_json Crow::Crow pqxx) 

