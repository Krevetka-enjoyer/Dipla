#include <nlohmann/json.hpp>

#include <sstream>
#include <mutex>
#include <thread>

#include <iostream>
#include <string>
#include <unordered_map>

#include "boost/date_time/posix_time/posix_time.hpp" 

using namespace boost::posix_time;
using json = nlohmann::json;

struct Object{
    ptime start;
    ptime finish;
    std::string name;
    std::string duration;
    bool isStart;
};

typedef std::unordered_map<int,Object> map;

class RunningTest{
    std::mutex AddMute;
    bool isRunning;
    std::thread Runner;
    map tests;
    ptime StrToTime(const std::string& start) const;
    void Baran();

public:
    RunningTest();
    void Add(int id,const std::string& name,const std::string& start,const std::string& finish,const std::string& duration);
    std::string GetTests();
    ~RunningTest();
};