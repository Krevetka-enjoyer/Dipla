#include <iostream>
#include "crow.h"
#include "crow/middlewares/cors.h"
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <fstream>

#include "PostgreInterface.h"
#include "RunningTest.h"
#include "Auth.h"

using json = nlohmann::json;

int main() {
  pqxx::connection c("host=localhost port=5432 user=iogurt password=Duraley195ASD dbname=dipla connect_timeout=10");
  Postgres db(c);
  RunningTest tests;
  Auth a(c,"Grib");
  crow::App<crow::CORSHandler> app;
  //auto& cors = app.get_middleware<crow::CORSHandler>();

  CROW_ROUTE(app, "/student").methods(crow::HTTPMethod::POST)
  ([&db,&a](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      return crow::response(200,a.AuthStudent(x.at("password").get<std::string>(),x.at("email").get<std::string>()));
    }
    catch(const std::runtime_error& e)
    {
      return crow::response(403,e.what());
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/tests").methods(crow::HTTPMethod::GET)
  ([&db,&a](const std::string& auth) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetTests());
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/quest/<string>").methods(crow::HTTPMethod::GET)
  ([&db,&a](const std::string& auth,const std::string& name) {
    try
    {
      a.VerifyTeacher(auth);
      std::cerr<<name<<'\n';
      return crow::response(200,db.GetQuest(name));
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/quests").methods(crow::HTTPMethod::POST)
  ([&db,&a](const crow::request& req,const std::string& auth) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      db.InsertQuest(x);
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/quests").methods(crow::HTTPMethod::GET)
  ([&db,&a](const std::string& auth) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetQuestList());
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/tests").methods(crow::HTTPMethod::POST)
  ([&db,&a](const crow::request& req,const std::string& auth) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      db.InsertTest(x);
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/<string>").methods(crow::HTTPMethod::POST)
  ([&db,&a,&tests](const crow::request& req,const std::string& auth,const std::string& test_name) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      tests.Add(test_name,x.at("start").get<std::string>(),x.at("duration").get<std::string>());
      return crow::response(200,"yeah! Nice Test!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/<string>/choose").methods(crow::HTTPMethod::GET)
  ([&db,&a](const std::string& auth,const std::string& test_name) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetChecking(test_name));
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher").methods(crow::HTTPMethod::POST)
  ([&db,&a](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      return crow::response(200,a.AuthTeacher(x.at("password").get<std::string>(),x.at("email").get<std::string>()));
    }
    catch(const std::runtime_error& e)
    {
      return crow::response(403,e.what());
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/students").methods(crow::HTTPMethod::POST)
  ([&db,&a](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      a.AddStudent(x.at("surname").get<std::string>()+"_"+x.at("name").get<std::string>()+"_"+x.at("secondname").get<std::string>(),
                      x.at("password").get<std::string>(), x.at("email").get<std::string>(), x.at("group").get<std::string>());
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/student/<string>/<string>").methods(crow::HTTPMethod::GET)
  ([&db,&a](const std::string& auth,const std::string& test_name) {
    try
    {
      a.VerifyStudent(auth);
      return crow::response(200,db.GetTest(test_name));
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/student/<string>/tests").methods(crow::HTTPMethod::GET)
  ([&db,&a,&tests](const std::string& auth) {
    try
    {
      a.VerifyStudent(auth);
      return crow::response(200,tests.GetTests());
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/student/<string>/<string>/results").methods(crow::HTTPMethod::POST)
  ([&db,&a](const crow::request& req,const std::string& auth,const std::string& test_name) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      return crow::response(200,db.SetResult(test_name,a.VerifyStudent(auth),x));
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  CROW_ROUTE(app, "/teacher/<string>/student/<int>/choose/results").methods(crow::HTTPMethod::POST)
  ([&db,&a](const crow::request& req,const std::string& auth,const int test_id) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      db.SetChecking(test_id,x);
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      return crow::response(400,e.what());
    }   
  });

  app.port(8080).multithreaded().run(); 
  return 0;
}
