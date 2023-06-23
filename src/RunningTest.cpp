#include "RunningTest.h"

RunningTest::RunningTest()
{}

void RunningTest::Add(int id,const std::string& name,const std::string& start,const std::string& dur)
{
    std::string i=std::to_string(id);
    tests[i]["start"]=start;
    tests[i]["duration"]=dur;
    tests[i]["name"]=name;
}

void RunningTest::Del(int id)
{
    tests.erase(std::to_string(id));
}

std::string RunningTest::GetTests()
{
    json out;
    for (const auto& [key,val]:tests.items())
    {
        json value=val;
        value["id"]=key;
        out+=value;
    }
    return to_string(out);
}

std::string RunningTest::GetUnch()
{
    json out;
    for (const auto& [key,val]:tests.items())
    {
        json value;
        value["name"]=val.at("name").get<std::string>();
        value["id"]=key;
        out+=value;
    }
    return to_string(out);
}