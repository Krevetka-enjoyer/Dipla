#include "Teacher.h"

Teacher::Teacher (const Postgres& post):db(post),pub_key("Grib"){
}


std::string Teacher::VerifyToken (const std::string& token)
{
    JwtWork j;
    return json::parse(j.VerifyToken(token,pub_key)).at("email").get<std::string>();
}

void Teacher::Auth (const crow::request& req,crow::response& resp)
{
    json x = json::parse(req.body);
    auto [fio,pass] = db.GetTeacher(x.at("email").get<std::string>());
    if (x.at("password").get<std::string>()==pass)
    {
        map payload;
        payload["fio"]=fio;
        payload["pass"]=pass;
        payload["email"]=x.at("email").get<std::string>();
        JwtWork j;
        resp.write(j.CreateToken(payload,pub_key));
    }
    else
        resp.write(R"~({"ERROR":"Wrong email or password!"})~");
    resp.end();
}

void Teacher::CreateNew (const crow::request& req)
{
    json x = json::parse(req.body);
    std::string fio;
    fio+=x.at("surname").get<std::string>();
    fio+=x.at("name").get<std::string>();
    fio+=x.at("secondname").get<std::string>();
    db.InsertTeacher(x.at("email").get<std::string>(),fio,x.at("password").get<std::string>());
}

void Teacher::AddGroup (const crow::request& req,const std::string& mail)
{
    json x = json::parse(req.body);
    db.InsertGroup(x.at("group_name").get<std::string>(),mail);
}

void Teacher::DeleteStudent (const crow::request& req)
{
    json x = json::parse(req.body);
    db.DeleteStudent(x.at("email").get<std::string>());
}

void Teacher::DeleteGroup (const crow::request& req)
{
    json x = json::parse(req.body);
    db.DeleteGroup(x.at("group").get<std::string>());
}

void Teacher::ChangeGroup (const crow::request& req)
{
    json x = json::parse(req.body);
    db.ChangeGroup(x.at("group").get<std::string>(),x.at("email").get<std::string>());
}

void Teacher::GetChecking (crow::response& resp,const std::string& test_name)
{
    resp.write(db.GetChecking(test_name));
    resp.end();
}

void Teacher::AddQuest (const crow::request& req)
{
    json request=json::parse(req.body);
    db.InsertQuest(request.at("name").get<std::string>(),request.at("text").get<std::string>(),to_string(variants),to_string(ansers),request.at("pictures"));
}

void Teacher::AddTest (const crow::request& req)
{
    json request=json::parse(req.body);
    db.InsertTest(request.at("name").get<std::string>(),request.at("value"));
}

void Teacher::GetQuests(crow::response& resp)
{
    resp.write(db.GetQuestList());
    resp.end();
}

// void Teacher::GetQuest(const crow::request& req,crow::response& resp)
// {
//     resp=QuestConsruct(tx.exec_prepared("GetNumedQuest",test_name,a.at("number").get<std::string>()).at(0).as<std::string,std::string,std::string>());
// }


