#include "Auth.h"
Auth::Auth(pqxx::connection& con,const std::string key):c(con),pub_key(key)
{
    c.prepare("InsTeacher","INSERT INTO _Teacher (_EMAIL,_FIO,_PASS) VALUES ($1, $2, $3)");
    c.prepare("InsStud","INSERT INTO _STUDENT (_EMAIL,_FIO,_PASS,_GROUP) VALUES ($1, $2, $3, $4)");
    c.prepare("SelTeacher","SELECT _FIO, _PASS FROM _Teacher where _EMAIL = $1");
    c.prepare("SelStud","SELECT _FIO, _PASS, _NUMBER, _GROUP FROM _STUDENT WHERE _EMAIL = $1");
}

std::string Auth::VerifyTeacher (const std::string& token)
{
    pqxx::work tx(c);
    json tok=json::parse(j.VerifyToken(token,pub_key));
    if (tx.exec_prepared("SelTeacher",tok.at("email").get<std::string>()).empty())
        throw std::runtime_error("Authentifycation failed!");
    return tok.at("email").get<std::string>();
}

std::string Auth::VerifyStudent (const std::string& token)
{
    pqxx::work tx(c);
    json tok=json::parse(j.VerifyToken(token,pub_key));
    if (tx.exec_prepared("SelStud",tok.at("email").get<std::string>()).empty())
        throw std::runtime_error("Authentifycation failed!");
    return tok.at("email").get<std::string>();
}
    
void Auth::AddTeacher (const std::string fio,const std::string pass, const std::string email)
{
    pqxx::work tx(c);
    tx.exec_prepared("InsTeacher",email,fio,pass);
    tx.commit();
}

void Auth::AddStudent (const std::string fio,const std::string pass, const std::string email, const std::string group)
{
    pqxx::work tx(c);
    tx.exec_prepared("InsStud",email,fio,pass,group);
    tx.commit();
}

std::string Auth::AuthTeacher (const std::string pass, const std::string email)
{
    pqxx::work tx(c);
    auto [fio,password] = tx.exec_prepared("SelTeacher",email).at(0).as<std::string,std::string>();
    if (password==pass)
    {
        map payload;
        payload["fio"]=fio;
        payload["pass"]=pass;
        payload["email"]=email;
        return j.CreateToken(payload,pub_key);
    }
    else
        throw std::runtime_error("Hey man! Missing Brains?");
}

std::string Auth::AuthStudent (const std::string pass, const std::string email)
{
    pqxx::work tx(c);
    auto [fio,password,number,group] = tx.exec_prepared("SelStud",email).at(0).as<std::string,std::string,int,std::string>();
    if (password==pass)
    {
        map payload;
        payload["fio"]=fio;
        payload["pass"]=pass;
        payload["number"]=number;
        payload["group"]=group;
        payload["email"]=email;
        return j.CreateToken(payload,pub_key);
    }
    else
        throw std::runtime_error("Hey man! Missing Brains?");
}