#include "JwtWork.cpp"
#include "crow.h"
#include "PostgreInterface.h"
#include <nlohmann/json.hpp>

#include <stdexcept>
#include <map>
#include <string>
#include <chrono>
#include <string>
#include <tuple>

using json = nlohmann::json;

typedef std::map<std::string,std::string> map;

class Student
{
    std::string pub_key;
    Postgres db;
    json running_tests;
public:

    Student (const Postgres& post);

    std::string VerifyToken (const std::string& token);

    void StartTest(const crow::request& req,crow::response& resp, const std::string& test_name);
    void SetChecking (const crow::request& req,crow::response& resp,const std::string& test_id);
    void SetResults(const crow::request& req,crow::response& resp,const std::string& email,const std::string& test_name);
    void GetTest(crow::response& resp,const std::string& email,const std::string& test_name);
    void Auth (const crow::request& req,crow::response& resp);
    void CreateNew (const crow::request& req,crow::response& resp);
};