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

using json = nlohmann::json;

class RunningTest{
    json tests={};
public:
    RunningTest();
    void Add(int id,const std::string& name,const std::string& start,const std::string& dur);
    void Del(int id);
    std::string GetTests();
    std::string GetUnch();
};