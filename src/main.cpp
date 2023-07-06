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

CROW_ROUTE(app, "/teacher/<string>/tests").methods(crow::HTTPMethod::GET)//Получить все тесты
  ([&db,&a](const std::string& auth) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetTests());
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app, "/teacher/<string>/quest/<int>").methods(crow::HTTPMethod::GET)//Получить весь вопрос
  ([&db,&a](const std::string& auth,int id) {
    try
    {
      a.VerifyTeacher(auth);
      //std::cerr<<"ya\n";
      return crow::response(200,db.GetQuest(id));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/teacher/<string>/quests").methods(crow::HTTPMethod::POST)//Создать вопрос
  ([&db,&a](const crow::request& req,const std::string& auth) {
    try
    {
      json x = json::parse(req.body);
      //std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      db.InsertQuest(x);
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/teacher/<string>/quests").methods(crow::HTTPMethod::GET)//Получить все вопросы
  ([&db,&a](const std::string& auth) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetQuestList());
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/teacher/<string>/tests").methods(crow::HTTPMethod::POST) //Создать тест
  ([&db,&a](const crow::request& req,const std::string& auth) {
    try
    {
      json x = json::parse(req.body);
      //std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      db.InsertTest(x);
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/teacher/<string>/<int>").methods(crow::HTTPMethod::POST)//Запустить тест
  ([&db,&a,&tests](const crow::request& req,const std::string& auth,int test_id) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      tests.Add(test_id,x.at("name").get<std::string>(),x.at("start").get<std::string>(),x.at("finish").get<std::string>(),x.at("duration").get<std::string>());
      return crow::response(200,"yeah! Nice Test!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/teacher").methods(crow::HTTPMethod::POST)//Вход учителя
  ([&db,&a](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      //std::cerr<<x<<'\n';
      return crow::response(200,a.AuthTeacher(x.at("password").get<std::string>(),x.at("email").get<std::string>()));
    }
    catch(const std::runtime_error& e)
    {
      return crow::response(403,e.what());
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/students").methods(crow::HTTPMethod::POST)//Регистрация студента
  ([&db,&a](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      //std::cerr<<x<<'\n';
      a.AddStudent(x.at("surname").get<std::string>()+"_"+x.at("name").get<std::string>()+"_"+x.at("secondname").get<std::string>(),
                      x.at("password").get<std::string>(), x.at("email").get<std::string>(), x.at("group").get<std::string>());
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

//ОТТЕСТЕНО
CROW_ROUTE(app, "/student").methods(crow::HTTPMethod::POST) //Вход студента
  ([&db,&a](const crow::request& req) {
    try
    {
      json x = json::parse(req.body);
      //std::cerr<<x<<'\n';
      return crow::response(200,a.AuthStudent(x.at("password").get<std::string>(),x.at("email").get<std::string>()));
    }
    catch(const std::runtime_error& e)
    {
      return crow::response(403,e.what());
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/teacher/<string>/<int>/choose").methods(crow::HTTPMethod::GET)//Вопросы для проверки преподователю
  ([&db,&a](const std::string& auth,int test_id) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetChecking(test_id));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

CROW_ROUTE(app, "/student/<string>/tests").methods(crow::HTTPMethod::GET)//Получить запущеные тесты
  ([&db,&a,&tests](const std::string& auth) {
    try
    {
      a.VerifyStudent(auth);
      return crow::response(200,tests.GetTests());
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app, "/student/<string>/<int>").methods(crow::HTTPMethod::GET)//Когда студент начал тест
  ([&db,&a](const std::string& auth,int test_id) {
    try
    {
      a.VerifyStudent(auth);
      return crow::response(200,db.GetTest(test_id));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app, "/student/<string>/<int>/results").methods(crow::HTTPMethod::POST)//Когда студент завершил тест
  ([&db,&a](const crow::request& req,const std::string& auth,int test_id) {
    try
    {
      json x = json::parse(req.body);
      std::cerr<<x<<'\n';
      db.SetResult(test_id,a.VerifyStudent(auth),x.at("answers"),x.at("date_time"));
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app, "/teacher/<string>/student/<int>/choose/results").methods(crow::HTTPMethod::POST)//Завершение проверки и оценка студента
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
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/student/<string>/tests").methods(crow::HTTPMethod::GET)//Получить список тестов студента
  ([&db,&a](const std::string& auth,const std::string& email) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetStudTests(email));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/student/<string>/completed").methods(crow::HTTPMethod::GET)//Получить пройденые тесты студента
  ([&db,&a](const std::string& auth) {
    try
    {
      return crow::response(200,db.GetStudTests(a.VerifyStudent(auth)));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/student/<string>/<int>/results").methods(crow::HTTPMethod::GET)//Посмотреть результаты теста
  ([&db,&a](const std::string& auth,int test_id) {
    try
    {
      a.VerifyStudent(auth);
      return crow::response(200,db.GetTestReview(test_id));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/students").methods(crow::HTTPMethod::POST)//Смена группы студента
  ([&db,&a](const crow::request& req,const std::string& auth) {
    try
    {
      json x = json::parse(req.body);
      //std::cerr<<x<<'\n';
      a.VerifyTeacher(auth);
      db.ChangeGroup(x.at("group").get<int>(),x.at("email").get<std::string>());
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/student/<string>").methods(crow::HTTPMethod::DELETE)//Удаление студента
  ([&db,&a](const std::string& auth,const std::string& email) {
    try
    {
      a.VerifyTeacher(auth);
      db.DeleteStudent(email);
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/group/<int>").methods(crow::HTTPMethod::DELETE)//Удаление группы
  ([&db,&a](const std::string& auth,int group) {
    try
    {
      a.VerifyTeacher(auth);
      db.DeleteGroup(group);
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/groups").methods(crow::HTTPMethod::POST)//Добавление группы
  ([&db,&a](const crow::request& req,const std::string& auth) {
    try
    {
      json x = json::parse(req.body);
      //std::cerr<<x<<'\n';
      db.InsertGroup(x.at("name").get<std::string>(),a.VerifyTeacher(auth));
      return crow::response(200,"yeah! Nice Cock!");
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/student/<string>/<int>").methods(crow::HTTPMethod::GET)//Получить тест студента
  ([&db,&a](const std::string& auth,const std::string& email,int test_id) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetTestReview(test_id));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/group/<int>").methods(crow::HTTPMethod::GET)//Получить список студентов группы
  ([&db,&a](const std::string& auth,int group) {
    try
    {
      a.VerifyTeacher(auth);
      return crow::response(200,db.GetStudList(group));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/groups").methods(crow::HTTPMethod::GET)//Получить список групп
  ([&db,&a](const std::string& auth) {
    try
    {
      return crow::response(200,db.GetGroupList(a.VerifyTeacher(auth)));
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });
CROW_ROUTE(app,"/teacher/<string>/tests/check").methods(crow::HTTPMethod::GET)//Список непроверенных тестов
  ([&db,&a](const std::string& auth) {
    try
    {
      return crow::response(200,db.GetUncheckedTests());
    }
    catch(const std::exception& e)
    {
      std::cout<<e.what();
      return crow::response(400,e.what());
    }   
  });

//НЕТЕСТЕНО

  app.port(8080).multithreaded().run(); 
  return 0;
}
