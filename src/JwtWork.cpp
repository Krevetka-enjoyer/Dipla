#include "jwt/jwt.hpp"

#include <map>
#include <string>
#include <sstream>

typedef std::map<std::string,std::string> map;
using namespace jwt::params;
struct JwtWork
{

static std::string VerifyToken (const std::string& token,const std::string& pub_key)
{
    auto dec_obj = jwt::decode(token, algorithms({"HS256"}), secret(pub_key), verify(true));
    std::stringstream s;
    s<<dec_obj.payload();
    return s.str();
}

static std::string CreateToken (map& pay,const std::string& pub_key)
{
    jwt::jwt_object obj{algorithm("HS256"), payload(pay), secret(pub_key)};
    obj.add_claim("exp", std::chrono::system_clock::now() + std::chrono::hours{12});
    return obj.signature();
}
};