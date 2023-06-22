#include "PostgreInterface.h"

std::string Postgres::AddImg(const std::string& name,const std::string& img) const
{
    Disk d;
    return d.Send(name,img);
}

void Postgres::PrepareInserts(pqxx::connection& con) const
{
    //Teachers:
    con.prepare("InsGroup","INSERT INTO _GROUP (_NAME, _EMAIL_TEACHER) VALUES ($1, $2)");

    //Tests/Quests:
    con.prepare("InsTest","INSERT INTO _TEST (_NAME) VALUES ($1)");
    con.prepare("InsSetup","INSERT INTO _SETUP (_QUEST,_TEST,_NUMBER) VALUES ($1,$2,$3)");
    con.prepare("InsQuest","INSERT INTO _QUEST (_NAME,_TEXT,_VARIANTS,_answer) VALUES ($1,$2,$3,$4)");
    con.prepare("InsImg","INSERT INTO _IMG (_QUEST_NAME,_TEXT) VALUES ($1,$2) RETURNING _ID");

    //Students:
    con.prepare("InsSavedTest","INSERT INTO _SAVEDTEST (_NAME_TEST,_EMAIL_STUDENT,_answers,_UNCHECKED_answers,_DATE) VALUES ($1,$2,$3,$4,$5) RETURNING _ID");
}

void Postgres::PrepareSelects(pqxx::connection& con) const
{
    //Tests:
    con.prepare("SelChecking","SELECT _EMAIL_STUDENT,_UNCHECKED_answers,_ID FROM _SAVEDTEST WHERE _NAME_TEST=$1 AND _EMAIL_STUDENT=$2 AND _UNCHECKED_answers IS NOT NULL");
    con.prepare("SelSavedAnsrs","SELECT _answers,_UNCHECKED_answers FROM _SAVEDTEST WHERE _ID=$1");
    con.prepare("SelEmptySavedTest","SELECT _ID FROM _SAVEDTEST WHERE _NAME_TEST=$1 AND _EMAIL_STUDENT=$2 AND _answers IS NULL");
    con.prepare("SelTests", "SELECT _NAME FROM _TEST");
    con.prepare("SelStudTests", "SELECT _ID,_NAME_TEST,_DATE FROM _SAVEDTEST WHERE _EMAIL_STUDENT=$1");
    con.prepare("SelTest", "SELECT _NAME_TEST,_ANSWERS,_SCORE FROM _SAVEDTEST WHERE _ID=$1");

    //Quests:
    con.prepare("SelQuestList","SELECT _NAME FROM _QUEST");
    con.prepare("SelQuestImgs", R"~(SELECT _SRC,_IMG._TEXT FROM _IMG WHERE _QUEST_NAME=$1)~");
    con.prepare("SelQuests",R"~(SELECT _QUEST._NAME,_TEXT,_VARIANTS FROM _QUEST 
                                JOIN _SETUP ON _QUEST._NAME=_SETUP._QUEST
                                JOIN _TEST ON _SETUP._TEST=_TEST._NAME WHERE _TEST._NAME=$1 ORDER BY _SETUP._NUMBER)~");
    con.prepare("SelAnsr","SELECT _answer FROM _QUEST WHERE _NAME=$1");
    con.prepare("SelQuest","SELECT _TEXT,_VARIANTS, _answer FROM _QUEST WHERE _NAME=$1");

    //Students:
    con.prepare("SelStudList","SELECT _FIO,_EMAIL FROM _STUDENT WHERE _GROUP=$1 ORDER BY _NUMBER");

}
    
void Postgres::PrepareUpdates(pqxx::connection& con) const
{
    con.prepare("ChangeGroup","UPDATE _STUDENT SET _GROUP = $1 WHERE _EMAIL=$2");
    con.prepare("UpdateChecking","UPDATE _SAVEDTEST SET _UNCHECKED_answers=$1,_answers=$2,_SCORE=$3 WHERE _ID=$4");
    con.prepare("UpdateImg","UPDATE _IMG SET _SRC=$1 WHERE _ID=$2");
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

json Postgres::GetImgs(pqxx::work& tx,const std::string& name) const
{
    json out;
    tx.exec_prepared("SelQuestImgs",name).for_each(
    [&out](std::string_view src,std::string_view txt) {
        json pics;
        pics["img"]=src;
        pics["caption"]=txt;
        out.push_back(std::move(pics));
    });
    return out;
}

std::string Postgres::QuestConsruct(pqxx::work& tx,const std::string& name,const std::string& text,const json& vars, const json& answer={}) const
{
    json out;
    out["name"]=name;
    out["text"]=text;
    if (!answer.empty())
    {
        json options;
        int k=0;
        for (int i=0;i<vars.size();++i)
        {
            json opt;
            opt["heading"]=vars[i];
            if ((k<answer.size()) && (answer[k]==i))
            {
                opt["isTrue"]=true;
                ++k;
            }
            else
                opt["isTrue"]=false;
            options+=opt;
        }
        out["options"]=options;
    }
    else
        out["options"]={};
    out["pictures"]=GetImgs(tx,name);
    return to_string(out);
}

//Selects:
std::string Postgres::GetStudList(const std::string& group) const
{
    json out;
    pqxx::work tx(con);
    tx.exec_prepared("SelStudList").
    for_each([&out](std::string fio,std::string mail)
    {
        json stud;
        int pos = fio.find('_');
        stud["name"]=fio.substr(0,pos);
        int newpos = fio.find('_',++pos);
        stud["secondname"]=fio.substr(pos,newpos-pos);
        stud["surname"]=fio.substr(newpos+1);
        stud["email"]=mail;
    });
    return to_string(out);
}

std::string Postgres::GetTests() const
{
    json out;
    pqxx::work tx(con);
    tx.exec_prepared("SelTests").
    for_each([&out](std::string name)
    {
        out+=name;
    });
    return to_string(out);
}

std::string Postgres::GetChecking(const std::string& test_name) const
{
    json out;
    pqxx::work tx(con);
    tx.exec_prepared("SelChecking",test_name).
    for_each([this, &out, &test_name,&tx](std::string_view email,std::string_view answers,int id)
    {
        json stud;
        stud["id"]=id;
        stud["email"]=email;
        json anrs;
        for (const auto& a:json::parse(answers))
        {
            json quest;
            auto tup=tx.exec_prepared("GetNumedQuest",test_name,a.at("number").get<std::string>()).at(0).as<std::string,std::string,std::string>();
            quest["quest"]=QuestConsruct(tx,std::get<0>(tup),std::get<1>(tup),json::parse(std::get<2>(tup)));
            quest["answer"]=to_string(a.at("answer"));
            anrs.push_back(std::move(quest));
        }
        stud["answers"]=std::move(anrs);
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
    for_each([this,&test_name,&out,&tx](std::string name,std::string text,std::string vars)
    {
        out+=json::parse(QuestConsruct(tx,name,text,json::parse(vars)));
    });
    return to_string(out);
}

std::string Postgres::GetQuest(const std::string& name) const
{
    pqxx::work tx(con);
    auto tup=tx.exec_prepared("SelQuest",name).at(0).as<std::string,std::string,std::string>();
    // tx.commit();
    // tx.abort();
    return QuestConsruct(tx,name,std::get<0>(tup),json::parse(std::get<1>(tup)),json::parse(std::get<2>(tup)));
}

std::string Postgres::GetStudTests(const std::string& email) const
{
    json out;
    pqxx::work tx(con);
    tx.exec_prepared("SelStudTests",email).
    for_each([&out](int id,std::string name,std::string date)
    {
        json test;
        test["id"]=std::to_string(id);
        test["name"]=name;
        test["date_time"]=date;
        out+=test;
    });
    return to_string(out);
}

std::string Postgres::GetTestReview(int test_id) const
{
    pqxx::work tx(con);
    auto [name,answers,score]=tx.exec_prepared("SelTest",test_id).at(0).as<std::string,std::string,int>();
    json ansrs=json::parse(answers);
    json out;
    int i=0;
    tx.exec_prepared("SelQuests",name).
    for_each([&tx,&i,&ansrs,&out,this](std::string name,std::string text,std::string vars)
    {
        json quest;
        quest["quest"]["pictures"]=GetImgs(tx,name);
        quest["quest"]["text"]=text;
        json vs=json::parse(vars);
        if (vars.empty())
        {
            quest["answers"]["text"]=ansrs.at("answer").get<std::string>();
            quest["answers"]["isTrue"]=ansrs.at("right").get<bool>();
        }
        else
        {
            json anser=ansrs[i];
            json tru=json::parse(std::get<std::string>(tx.exec_prepared("SelAnsr",name).at(0).as<std::string>()));
            int a=0;
            int t=0;
            for (int j=0;j<vars.size();++j)
            {
                json var;
                var["heading"]=vars[j];
                if (tru[t]==j)
                {
                    var["isTrue"]=true;
                    ++t;
                }
                else
                    var["isTrue"]=false;
                if (anser[a]==j)
                {
                    var["isSelected"]=true;
                    ++a;
                }
                else
                    var["isSelected"]=false;
                quest["answers"].push_back(std::move(var));
            }
        }
        out.push_back(std::move(quest));
        ++i;
    });
    json real_out;
    real_out["test"]=std::move(out);
    real_out["score"]=score;
    return to_string(real_out);
}
//Inserts:
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
    json answers;
    int i=0;
    if (quest.at("options").is_array())
        for (const auto& el:quest.at("options"))
        {
            variants+=el.at("heading").get<std::string>();
            if (el.at("isTrue").get<bool>())
                answers+=i;
            ++i;
        }
    else
    {
        variants={};
        answers="";
    }
    tx.exec_prepared("InsQuest",quest.at("name").get<std::string>(),quest.at("text").get<std::string>(),to_string(variants),to_string(answers)); 
    for (const auto& el:quest.at("pictures"))
    {
        int id=tx.exec_prepared("InsImg",quest.at("name").get<std::string>(),el.at("caption").get<std::string>()).at(0)[0].as<int>();
        tx.exec_prepared("UpdateImg",AddImg(std::to_string(id),el.at("img").get<std::string>()),id);    
    }
    tx.commit();
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

std::string Postgres::SetResult (const std::string& test_name, const std::string& mail,const json& answers,const std::string date) const
{
    pqxx::work tx(con);
    json unchecked;
    json checked;
    int i=0;
    int k=0;
    for (const auto& [key,val]:answers.items())
    {
        auto answer=std::get<std::string>(tx.exec_prepared("SelAnsr",key).at(0).as<std::string>());
        if (answer!="")
        {
            json an;
            if (val==answer)
            {
                ++k;
                an["right"]=true;
            }
            else
                an["right"]=false;
            an["answer"]=json::parse(answer); 
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
    tx.exec_prepared("InsSavedTest",test_name,mail,to_string(checked),to_string(unchecked),date);
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
        ans["answer"]=val;
        ans["right"]=check.at("estimation")[i++];
        ansrs.insert(ansrs.begin()+std::stoi(key),to_string(ans));
    }
    tx.exec_prepared("UpdateChecking",nullptr,to_string(ansrs),check.at("score").get<int>(),test_id);
    tx.commit();
    //
}

