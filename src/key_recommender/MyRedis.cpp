#include "../../inc/key_commander/MyRedis.hh"

using std::cerr;
using std::endl;
using std::vector;

MyRedis::MyRedis()
{
    //Redis的ip和port为固定值
    string ip("127.0.0.1");
    unsigned short port = 6379;

    _p_connect = redisConnect(ip.c_str(), port);
    if (_p_connect != nullptr && _p_connect->err)
    {
        cerr << "connect error : " << _p_connect->errstr << endl;
        exit(0);
    }

}

MyRedis::~MyRedis()
{
    if (_p_connect != nullptr)
    {
        redisFree(_p_connect);
        _p_connect = nullptr;
    }
    if (_p_reply != nullptr)
    {
        freeReplyObject(_p_reply);
        _p_reply = nullptr;
    }
}

void MyRedis::Push(string &key, string &value)
{
    _p_reply = (redisReply *)redisCommand(_p_connect, "RPUSH %s %s", key.c_str(), value.c_str());
    if(_p_reply->integer == 0)
    {
        cerr << "redis rpush error!" << endl;
    }
    //防止_p_reply使用多次
    freeReplyObject(_p_reply);
    _p_reply = nullptr;
}

vector<string> MyRedis::Pop(const string &key)
{
    vector<string> result;

    _p_reply = (redisReply *)redisCommand(_p_connect, "LRANGE %s 0 -1" , key.c_str());

    for (int i = 0; i < _p_reply->elements; ++i)
    {
        string line_num = _p_reply->element[i]->str;
        result.push_back(line_num);
    }
    //result.push_back(_p_reply->str);
    //防止_p_reply被使用多次
    freeReplyObject(_p_reply);
    _p_reply = nullptr;

    return result;
}
