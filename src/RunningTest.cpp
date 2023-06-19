#include "RunningTest.h"

RunningTest::RunningTest()
{}

void RunningTest::Add(const std::string& name,const std::string& start,const std::string& dur)
{
    tests[name]["start"]=start;
    tests[name]["duration"]=dur;
}

void RunningTest::Del(const std::string& name)
{
    tests.erase(name);
}

std::string RunningTest::GetTests()
{
    json out;
    for (const auto& [key,val]:tests.items())
    {
        json value=val;
        value["test_name"]=key;
        out+=value;
    }
    return to_string(out);
}