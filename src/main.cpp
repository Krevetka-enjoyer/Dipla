#include <iostream>
#include "crow.h"
#include "crow/middlewares/cors.h"
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <fstream>

#include "PostgreInterface.h"
#include "JwtWork.cpp"

using json = nlohmann::json;
std::string pub_key="Grib";
std::string VerifyToken (const std::string& token)
{
    JwtWork j;
    return json::parse(j.VerifyToken(token,pub_key)).at("email").get<std::string>();
}

int main() {
  pqxx::connection c("host=localhost port=5432 user=iogurt password=Duraley195ASD dbname=dipla connect_timeout=10");
  Postgres db(c);
  json running_tests;
  crow::App<crow::CORSHandler> app;
  //auto& cors = app.get_middleware<crow::CORSHandler>();
  CROW_ROUTE(app, "/student").methods(crow::HTTPMethod::POST)
  ([&db](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      std::cerr << "Stud!";
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
        return crow::response(200,j.CreateToken(payload,pub_key));
      }
      else
        return crow::response(403,"Hey man! Missing Brains?");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });



  CROW_ROUTE(app, "/teacher/<string>/quests").methods(crow::HTTPMethod::POST)
  ([&db](const crow::request& req,const std::string& auth) {
    try
    {
      VerifyToken(auth);
      db.InsertQuest(json::parse(req.body));
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/quests").methods(crow::HTTPMethod::GET)
  ([&db](const std::string& auth) {
    try
    {
      VerifyToken(auth);
      return crow::response(200,db.GetQuestList());
    }
    catch(const std::exception& e)
    {
      std::cerr<<e.what();
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/tests").methods(crow::HTTPMethod::POST)
  ([&db](const crow::request& req,const std::string& auth) {
    try
    {
      VerifyToken(auth);
      db.InsertTest(json::parse(req.body));
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/<string>").methods(crow::HTTPMethod::POST)
  ([&db,&running_tests](const crow::request& req,const std::string& auth,const std::string& test_name) {
    try
    {
      VerifyToken(auth);//
      json test;
      json x = json::parse(req.body);
      test["test_name"]=test_name;
      test["start"]=x.at("start").get<std::string>();
      test["duration"]=x.at("duration").get<std::string>();
      running_tests.push_back(test);
      return crow::response(200,"yeah! Welcome to family Boy!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/<string>/choose").methods(crow::HTTPMethod::GET)
  ([&db](const std::string& auth,const std::string& test_name) {
    try
    {
      VerifyToken(auth);
      return crow::response(200,db.GetChecking(test_name));
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher").methods(crow::HTTPMethod::POST)
  ([&db](const crow::request& req) {
    try
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
        return crow::response(200,j.CreateToken(payload,pub_key));
      }
      else
        return crow::response(403,"Hey man! Missing Brains?");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/students").methods(crow::HTTPMethod::POST)
  ([&db](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      std::string fio;
      fio+=x.at("surname").get<std::string>();
      fio+=x.at("name").get<std::string>();
      fio+=x.at("secondname").get<std::string>();
      db.InsertStudent(x.at("email").get<std::string>(),fio,x.at("password").get<std::string>(),x.at("group").get<std::string>());
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/student/<string>/<string>").methods(crow::HTTPMethod::GET)
  ([&db](const std::string& auth,const std::string& test_name) {
    try
    {
      VerifyToken(auth);
      return crow::response(200,db.GetTest(test_name));
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/student/<string>/<string>/results").methods(crow::HTTPMethod::POST)
  ([&db](const crow::request& req,const std::string& auth,const std::string& test_name) {
    try
    {
      return crow::response(200,db.SetResult(test_name,VerifyToken(auth),json::parse(req.body)));
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/student/<int>/choose/results").methods(crow::HTTPMethod::POST)
  ([&db](const crow::request& req,const std::string& auth,const int test_id) {
    try
    {
      VerifyToken(auth);
      db.SetChecking(test_id,json::parse(req.body));
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });
  //crow::logger::setLogLevel(crow::LogLevel::Debug);
  app.port(8080).multithreaded().run(); 
  return 0;
}
