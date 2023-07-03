#include "RunningTest.h"

void RunningTest::Baran()
{
    while (!tests.empty())
    {
        std::lock_guard guard(AddMute);
        for (auto& ob:tests)
            if (ob.second.isStart && ob.second.finish>=second_clock::local_time())
                tests.erase(ob.first);
            else if (ob.second.start<=second_clock::local_time())
                ob.second.isStart=true;
    }
    isRunning=false;
} 

ptime RunningTest::StrToTime(const std::string& start) const
{
    std::tm tm = {};
    std::stringstream ss(start);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M");
    return ptime_from_tm(tm);
}

RunningTest::RunningTest()
{}

void RunningTest::Add(int id,const std::string& name,const std::string& start,const std::string& finish,const std::string& duration)
{
    std::lock_guard guard(AddMute);
    ptime start_t=StrToTime(start);
    ptime finish_t=StrToTime(finish);
    if (start_t>=second_clock::local_time())
        tests[id]={start_t,finish_t,name,duration,true};
    else
        tests[id]={start_t,finish_t,name,duration,false};
    if (!isRunning)
        Runner=std::thread(&RunningTest::Baran, this);
}

std::string RunningTest::GetTests()
{
    json out;
    for (const auto& ob:tests)
    {
        json value;
        value["id"]=ob.first;
        value["start"]=to_iso_extended_string(ob.second.start);
        value["duration"]=ob.second.duration;
        value["test_name"]=ob.second.name;
        out+=value;
    }
    return to_string(out);
}

RunningTest::~RunningTest()
{
    if (isRunning)
    {
        isRunning=false;
        Runner.join();
    }
}