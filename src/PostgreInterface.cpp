#include "PostgreInterface.h"

std::string Postgres::AddImg(const std::string& img) const
{
    return img;
}

void Postgres::PrepareInserts(pqxx::connection& con) const
{
    //Teachers:
    con.prepare("InsTeacher","INSERT INTO _Teacher (_EMAIL,_FIO,_PASS) VALUES ($1, $2, $3)");
    con.prepare("InsGroup","INSERT INTO _GROUP (_NAME, _EMAIL_TEACHER) VALUES ($1, $2)");

    //Tests/Quests:
    con.prepare("InsTest","INSERT INTO _TEST (_NAME) VALUES ($1)");
    con.prepare("InsSetup","INSERT INTO _SETUP (_QUEST,_TEST,_NUMBER) VALUES ($1,$2,$3)");
    con.prepare("InsQuest","INSERT INTO _QUEST (_NAME,_TEXT,_VARIANTS,_ANSER) VALUES ($1,$2,$3,$4)");
    con.prepare("InsImg","INSERT INTO _IMG (_SRC,_TEXT) VALUES ($1,$2) RETURNING _ID");
    con.prepare("InsImgLink","INSERT INTO _IMG_QUEST (_QUEST,_IMG) VALUES ($1,$2)");

    //Students:
    con.prepare("InsStud","INSERT INTO _STUDENT (_EMAIL,_FIO,_PASS,_GROUP) VALUES ($1, $2, $3, $4)");
    con.prepare("InsSavedTest","INSERT INTO _SAVEDTEST (_NAME_TEST,_EMAIL_STUDENT) VALUES ($1,$2) RETURNING _ID");
}

void Postgres::PrepareSelects(pqxx::connection& con) const
{
    //Teachers:
    con.prepare("SelTeacher","SELECT _FIO, _PASS FROM _Teacher where _EMAIL = $1");

    //Tests:
    con.prepare("SelChecking","SELECT _EMAIL_STUDENT,_UNCHECKED_ANSERS,_ID FROM _SAVEDTEST WHERE _NAME_TEST=$1 AND _EMAIL_STUDENT=$2 AND _UNCHECKED_ANSERS IS NOT NULL");
    con.prepare("SelTestQuests",R"~(SELECT _QUEST._NAME, _TEXT, _VARIANTS, _ANSER FROM _QUEST 
                                    JOIN _SETUP ON _QUEST._NAME=_SETUP._QUEST
                                    JOIN _TEST ON _SETUP._TEST=_TEST._NAME
                                    JOIN _SAVEDTEST ON _TEST._NAME=_SAVEDTEST._NAME_TEST WHERE _SAVEDTEST._ID=$1)~");
    con.prepare("SelSavedTest","SELECT _ANSERS, _UNCHECKED_ANSERS FROM _SAVEDTEST WHERE _ID=$1");
    con.prepare("SelSavedAnsrs","SELECT _ANSERS,_UNCHECKED_ANSERS FROM _SAVEDTEST WHERE _ID=$1");
    con.prepare("SelEmptySavedTest","SELECT _ID FROM _SAVEDTEST WHERE _NAME_TEST=$1 AND _EMAIL_STUDENT=$2 AND _ANSERS IS NULL");

    //Quests:
    con.prepare("SelQuestList","SELECT _NAME FROM _QUEST");
    con.prepare("SelQuestImgs", R"~(SELECT _SRC,_IMG._TEXT FROM _IMG 
                                    JOIN _IMG_QUEST ON _IMG_QUEST._IMG=_IMG._ID
                                    JOIN _QUEST ON _IMG_QUEST._QUEST=_QUEST._NAME WHERE _QUEST._NAME=$1)~");
    con.prepare("SelQuests",R"~(SELECT _QUEST._NAME,_TEXT,_VARIANTS FROM _QUEST 
                                JOIN _SETUP ON _QUEST._NAME=_SETUP._QUEST
                                JOIN _TEST ON _SETUP._TEST=_TEST._NAME WHERE _TEST._NAME=$1 ORDER BY _SETUP._NUMBER)~");
    con.prepare("SelAnsr","SELECT _ANSER FROM _QUEST WHERE _NAME=$1");

    //Students:
    con.prepare("SelStud","SELECT _FIO, _PASS, _NUMBER, _GROUP FROM _STUDENT WHERE _EMAIL = $1");
    con.prepare("SelStudList","SELECT _FIO,_EMAIL FROM _STUDENT WHERE _GROUP=$1 ORDER BY _NUMBER");
    con.prepare("SelStudTests","SELECT _ID,_NAME_TEST FROM _SAVEDTEST WHERE _EMAIL_STUDENT=$1");
    con.prepare("SelQuest","SELECT _TEXT,_VARIANTS, _ANSER FROM _QUEST WHERE _NAME=$1");

}
    
void Postgres::PrepareUpdates(pqxx::connection& con) const
{
    con.prepare("ChangeGroup","UPDATE _STUDENT SET _GROUP = $1 WHERE _EMAIL=$2");
    con.prepare("UpdateChecking","UPDATE _SAVEDTEST SET _UNCHECKED_ANSERS=$1,_ANSERS=$2,_SCORE=$3 WHERE _ID=$4");
}

void Postgres::PrepareDelets(pqxx::connection& con) const
{
    con.prepare("DelGroup","DELETE FROM _GROUP WHERE _NAME = $1");
    con.prepare("DelStud","DELETE FROM _STUDENT WHERE _EMAIL = $1");
}

Postgres::Postgres (pqxx::connection& c):con(c){
    PrepareInserts(con);
    PrepareSelects(con);
    PrepareUpdates(con);
    PrepareDelets(con);
}

std::string Postgres::QuestConsruct(const std::string& name,const std::string& text,const std::string& vars) const
{
    json out;
    out["name"]=name;
    out["text"]=text;
    out["options"]=vars;
    pqxx::work tx(con);
    tx.exec_prepared("SelQuestImgs",name).for_each(
    [&out](std::string_view src,std::string_view txt) {
        json pics;
        pics["img"]=src;
        pics["caption"]=txt;
        out["pictures"].push_back(std::move(pics));
    });
    return to_string(out);
}

//Selects:
std::tuple<std::string,std::string,int,std::string> Postgres::GetStudent(const std::string& mail) const
{
    pqxx::work tx(con);
    return tx.exec_prepared("SelStud",mail).at(0).as<std::string,std::string,int,std::string>();
}

std::tuple<std::string,std::string> Postgres::GetTeacher(const std::string& mail) const
{
    pqxx::work tx(con);
    return tx.exec_prepared("SelTeacher",mail).at(0).as<std::string,std::string>();
}

std::string Postgres::GetChecking(const std::string& test_name) const
{
    json out;
    pqxx::work tx(con);
    tx.exec_prepared("SelChecking",test_name).
    for_each([this, &out, &test_name,&tx](std::string_view email,std::string_view ansers,int id)
    {
        json stud;
        stud["id"]=id;
        stud["email"]=email;
        json anrs;
        for (const auto& a:json::parse(ansers))
        {
            json quest;
            auto tup=tx.exec_prepared("GetNumedQuest",test_name,a.at("number").get<std::string>()).at(0).as<std::string,std::string,std::string>();
            quest["quest"]=QuestConsruct(std::get<0>(tup),std::get<1>(tup),std::get<2>(tup));
            quest["anser"]=to_string(a.at("anser"));
            anrs.push_back(std::move(quest));
        }
        stud["ansers"]=std::move(anrs);
        out.push_back(std::move(stud));
    });
    return to_string(out);
}

std::string Postgres::GetQuestList() const
{
    pqxx::work tx(con);
    json out;
    tx.exec_prepared("SelQuestList").for_each(
    [&out](std::string_view name) {
        out+=name;
    });
    return to_string(out);
}

std::string Postgres::GetTest(const std::string& test_name) const
{
    pqxx::work tx(con);
    json out;
    tx.exec_prepared("SelQuests",test_name).
    for_each([this,&test_name,&out](std::string name,std::string text,std::string vars)
    {
        out+=json::parse(QuestConsruct(name,text,vars));
    });
    return to_string(out);
}

//Inserts:
void Postgres::InsertTeacher(const std::string& mail,const std::string& fio,const std::string& pass) const
{
    pqxx::work tx(con);
    tx.exec_prepared("InsTeacher",mail,fio,pass);
    tx.commit();
}

void Postgres::InsertGroup(const std::string& name,const std::string& mail) const
{
    pqxx::work tx(con);
    tx.exec_prepared("InsGroup",name,mail);
    tx.commit();
}

void Postgres::InsertQuest(const json& quest) const
{
    pqxx::work tx(con);
    json variants;
    json ansers;
    int i=0;
    for (const auto& el:quest.at("options"))
    {
        variants+=el.at("heading").get<std::string>();
        if ("true"==el.at("isTrue").get<std::string>())
            ansers+=i;
        ++i;
    }
    tx.exec_prepared("InsQuest",quest.at("name").get<std::string>(),quest.at("text").get<std::string>(),to_string(variants),to_string(ansers)); 
    for (const auto& el:quest.at("pictures"))
    {
        int id=tx.exec_prepared("InsImg",AddImg(el.at("img").get<std::string>()),el.at("caption").get<std::string>()).at(0)[0].as<int>();
        tx.exec_prepared("InsImgLink",quest.at("name").get<std::string>(),id);    
    }
    tx.commit();
    //
}

void Postgres::InsertTest (const json& test) const
{
    pqxx::work tx(con);
    tx.exec_prepared("InsTest",test.at("name").get<std::string>());
    int i=0;
    for (const auto& quest:test.at("value"))
        tx.exec_prepared("InsSetup",std::string(quest),test.at("name").get<std::string>(),i++);
    tx.commit();
    //
}

void Postgres::InsertStudent(const std::string& mail,const std::string& fio,const std::string& pass ,const std::string& group) const
{
    pqxx::work tx(con);
    tx.exec_prepared("InsStud",mail,fio,pass,group);
    tx.commit();
    //
}

std::string Postgres::SetResult (const std::string& test_name, const std::string& mail,const json& ansers) const
{
    pqxx::work tx(con);
    json unchecked;
    json checked;
    int i=0;
    int k=0;
    for (const auto& [key,val]:ansers.items())
    {
        auto anser=std::get<std::string>(tx.exec_prepared("SelAnsr",key).at(0).as<std::string>());
        if (anser!="")
        {
            json an;
            if (val==anser)
            {
                ++k;
                an["right"]="true";
            }
            else
                an["right"]="false";
            an["anser"]=json::parse(anser); 
            checked+= an;           
        }
        else
        {
            json un;
            un[std::to_string(i)]=val;
            unchecked+=un;
        }
        i++;
    }
    tx.exec_prepared("InsSavedTest",test_name,mail,to_string(unchecked),to_string(checked));
    //
    json out;
    out["total"]=i;
    out["right"]=k;
    tx.commit();
    return to_string(out);
}

//Deletes:
void Postgres::DeleteStudent(const std::string& mail) const
{
    pqxx::work tx(con);
    tx.exec_prepared("DelStud",mail);
    tx.commit();
    //
}

void Postgres::DeleteGroup(const std::string& name) const
{
    pqxx::work tx(con);
    tx.exec_prepared("DelGroup",name);
    tx.commit();
    //
}

//Updates:
void Postgres::ChangeGroup(const std::string& name,const std::string& mail) const
{
    pqxx::work tx(con);
    tx.exec_prepared("ChangeGroup",name,mail);
    tx.commit();
    //
}

void Postgres::SetChecking (int test_id,const json& check) const
{
    pqxx::work tx(con);
    auto [a,u]=tx.exec_prepared("SelSavedAnsrs",test_id,check.at("email").get<std::string>()).at(0).as<std::string,std::string>();
    json ansrs=json::parse(a);
    json unchecked=json::parse(u);
    int i=0;
    for(const auto& [key,val]:unchecked.items())
    {
        json ans;
        ans["anser"]=val;
        ans["right"]=check.at("estimation")[i++];
        ansrs.insert(ansrs.begin()+std::stoi(key),to_string(ans));
    }
    tx.exec_prepared("UpdateChecking",nullptr,to_string(ansrs),check.at("score").get<int>(),test_id);
    tx.commit();
    //
}

