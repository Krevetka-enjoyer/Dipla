#include "crow.h"
#include "PostgreInterface.h"
#include <nlohmann/json.hpp>

#include <stdexcept>
#include <map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <string>
#include <tuple>

#include "JwtWork.cpp"

using json = nlohmann::json;
typedef std::map<std::string,std::string> map;
typedef std::unordered_set<std::string> set;
class Teacher {

    Postgres db;
    std::string pub_key;

public:
    Teacher (const Postgres& post);

    std::string VerifyToken (const std::string& token);

    void Auth (const crow::request& req,crow::response& resp);
    void CreateNew (const crow::request& req);
    void AddGroup (const crow::request& req,const std::string& mail);
    void DeleteStudent (const crow::request& req);
    void DeleteGroup (const crow::request& req);
    void ChangeGroup (const crow::request& req);
    void GetChecking (crow::response& resp,const std::string& test_name);
    void AddQuest (const crow::request& req);
    void AddTest (const crow::request& req);
    void GetQuests(crow::response& resp);
    //void GetQuest(const crow::request& req,crow::response& resp);
    //void GetCompleteTest(crow::response& resp,const std::string& stud_email,const std::string& test_id);
    //void GetTests(crow::response& resp,const std::string& stud_email);
    //void GetStudList(crow::response& resp,const std::string& group);
};