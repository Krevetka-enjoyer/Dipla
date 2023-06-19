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
    void Add(const std::string& name,const std::string& start,const std::string& dur);
    void Del(const std::string& name);
    std::string GetTests();
};