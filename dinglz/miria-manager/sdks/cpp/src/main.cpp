#include<spdlog/spdlog.h>
#include"httplib.h"
#include"json.hpp"
#include"bot.hpp"

char* U2G(const char* utf8)
{
int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
wchar_t* wstr = new wchar_t[len+1];
memset(wstr, 0, len+1);
MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
char* str = new char[len+1];
memset(str, 0, len+1);
WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
if(wstr) delete[] wstr;
return str;
}

char* G2U(const char* gb2312)
{
int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
wchar_t* wstr = new wchar_t[len+1];
memset(wstr, 0, len+1);
MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
char* str = new char[len+1];
memset(str, 0, len+1);
WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
if(wstr) delete[] wstr;
return str;
}

int main(int argc, char** argv)
{
    init("80"); //初始化服务器端口
    using std::string;
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("MiraiManager c++sdk start running");
    httplib::Server server;
    server.Get("/friend_msg",[](const httplib::Request & req, httplib::Response &res){
        //spdlog::info("recv friend msg");
        using json = nlohmann::json;
        json j = json::parse(req.get_param_value("data"));
        spdlog::debug(j.dump().c_str());
        string message = j["msg"];
        string bot_id = j["bot_id"].dump();
        string user_id = j["user_id"].dump();
        spdlog::info("rev friend msg:{0} {1} {2}",bot_id.c_str(),user_id.c_str(),U2G(message.c_str()));
        if(message=="hello")
        {
            send_friend_msg("你好，我正运行着",bot_id,user_id);
        }
    });
    server.Get("/group_msg",[](const httplib::Request & req, httplib::Response &res){
        using json = nlohmann::json;
        json j = json::parse(req.get_param_value("data"));
        spdlog::debug(j.dump().c_str());    
        string message = j["msg"];
        string bot_id = j["bot_id"].dump();
        string group_id = j["group_id"].dump();
        string sender_id = j["sender_id"].dump();
        spdlog::info("rev group msg:{0} {1} {2} {3}",bot_id.c_str(),group_id.c_str(),sender_id.c_str(),U2G(message.c_str()));
        if(message=="hello")
        {
            send_group_msg("你好，我正运行着",bot_id,group_id);
        }
    });
    server.listen("0.0.0.0",666);//后面的填你的端口 如果你填666 那么你的http反向配置即为：http.url = http://localhost:666/
    return 0;
}
