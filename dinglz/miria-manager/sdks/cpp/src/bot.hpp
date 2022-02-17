#pragma once
#include"httplib.h"
using std::string;

string server_port;
httplib::Client * client;

void init(string port)
{
    server_port = port;
    client = new httplib::Client("http://localhost:"+port);
    client->set_url_encode(true);
}

void send_friend_msg(string msg,string bot_id,string friend_id)
{
    client->Post(string("/api/send_friend_msg_single?id="+friend_id+"&use="+bot_id+"&msg="+msg).c_str());
}

void send_group_msg(string msg,string bot_id,string group_id)
{
    client->Post(string("/api/send_group_msg_single?id="+group_id+"&msg="+msg+"&use="+bot_id).c_str());
}