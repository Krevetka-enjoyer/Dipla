#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <nlohmann/json.hpp>

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <string>
#include <tuple>

#include "JwtWork.cpp"

using json = nlohmann::json;

class Auth{
    pqxx::connection& c;
    JwtWork j;
    std::string pub_key="Grib";
public:
    Auth(pqxx::connection& con,const std::string key);
    std::string VerifyTeacher (const std::string& token);
    std::string VerifyStudent (const std::string& token);
    
    void AddTeacher (const std::string fio,const std::string pass, const std::string email);
    void AddStudent (const std::string fio,const std::string pass, const std::string email, const std::string group);

    std::string AuthTeacher (const std::string pass, const std::string email);
    std::string AuthStudent (const std::string pass, const std::string email);
};