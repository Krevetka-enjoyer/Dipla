#include "Student.h"

Student::Student (const Postgres& post):db(post),pub_key("Grib")
{
}

std::string Student::VerifyToken (const std::string& token)
{
    JwtWork j;
    return json::parse(j.VerifyToken(token,pub_key)).at("email").get<std::string>();
}


void Student::Auth (const crow::request& req,crow::response& resp)
{
    json x = json::parse(req.body);
    auto [fio,pass,number,group] =db.GetStudent(x.at("email").get<std::string>());
    if (x.at("password").get<std::string>()==pass)
    {
        map payload;
        payload["fio"]=fio;
        payload["pass"]=pass;
        payload["number"]=number;
        payload["group"]=group;
        payload["email"]=x.at("email").get<std::string>();
        JwtWork j;
        resp.write(j.CreateToken(payload,pub_key));
    }
    else
        resp.write(R"~({"ERROR":"Wrong email or password!"})~");
    resp.end()
}

void Student::CreateNew (const crow::request& req)
{
    json x = json::parse(req.body);
    std::string fio;
    fio+=x.at("surname").get<std::string>();
    fio+=x.at("name").get<std::string>();
    fio+=x.at("secondname").get<std::string>();
    db.InsertStudent(x.at("email").get<std::string>(),fio,x.at("password").get<std::string>(),x.at("group").get<std::string>())
}

void Student::StartTest(const crow::request& req,const std::string& test_name)
{
    json test;
    json x = json::parse(req.body);
    test["test_name"]=test_name;
    test["start"]=x.at("start").get<std::string>();
    test["duration"]=x.at("duration").get<std::string>();
    running_tests.push_back(test);
}