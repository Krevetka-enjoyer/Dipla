#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <pqxx/subtransaction>
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

#include "Disk.h"

using json = nlohmann::json;

class Postgres {
    pqxx::connection& con;
    void PrepareInserts(pqxx::connection& con) const;
    void PrepareSelects(pqxx::connection& con) const;
    void PrepareUpdates(pqxx::connection& con) const;
    void PrepareDelets(pqxx::connection& con) const;

    json GetImgs(pqxx::work& tx,int id) const;
    json QuestConsruct(pqxx::work& tx,int id,const std::string& text,const json& vars, const json& answer) const;
    std::string AddImg(const std::string& name,const std::string& img) const;
public:
    Postgres(pqxx::connection& c);

    std::string GetChecking(int test_id) const;
    std::string GetQuestList() const;
    std::string GetTest(int test_id) const;
    std::string GetQuest(int id) const;
    std::string GetTests() const;
    std::string GetStudTests(const std::string& email) const;
    std::string GetTestReview(int test_id) const;
    std::string GetStudList(int group) const;
    std::string GetGroupList(const std::string& email) const;

    void InsertGroup(const std::string& mail,const std::string& name) const;
    void InsertQuest(const json& quest) const;
    void InsertTest (const json& test) const;
    std::string SetResult (int test_id, const std::string& mail,const json& answers,const std::string date) const;
    
    void ChangeGroup(int id,const std::string& mail) const;
    void SetChecking (int test_id,const json& check) const;

    void DeleteStudent(const std::string& mail) const;
    void DeleteGroup(int id) const;
};