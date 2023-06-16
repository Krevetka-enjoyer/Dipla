#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string_view>
#include <map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <string>
#include <tuple>

using json = nlohmann::json;

class Postgres {
    pqxx::connection& con;
    void PrepareInserts(pqxx::connection& con) const;
    void PrepareSelects(pqxx::connection& con) const;
    void PrepareUpdates(pqxx::connection& con) const;
    void PrepareDelets(pqxx::connection& con) const;
public:
    std::string AddImg(const std::string& img) const;
    Postgres(pqxx::connection& c);

    std::string QuestConsruct(const std::string& name,const std::string& text,const std::string& vars) const;

    std::tuple<std::string,std::string,int,std::string> GetStudent(const std::string& mail) const;
    std::tuple<std::string,std::string> GetTeacher(const std::string& mail) const;
    std::string GetChecking(const std::string& test_name) const;
    std::string GetQuestList() const;
    std::string GetTest(const std::string& test_name) const;

    void InsertTeacher(const std::string& mail,const std::string& fio,const std::string& pass) const;
    void InsertGroup(const std::string& mail,const std::string& name) const;
    void InsertQuest(const json& quest) const;
    void InsertTest (const json& test) const;
    void InsertStudent(const std::string& mail,const std::string& fio,const std::string& pass ,const std::string& group) const;
    std::string SetResult (const std::string& test_name, const std::string& mail,const json& ansers) const;
    
    void ChangeGroup(const std::string& name,const std::string& mail) const;
    void SetChecking (int test_id,const json& check) const;

    void DeleteStudent(const std::string& mail) const;
    void DeleteGroup(const std::string& name) const;
};