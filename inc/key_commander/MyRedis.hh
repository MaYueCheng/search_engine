#ifndef MY_REDIS__HH
#define MY_REDIS__HH

#include "../cpp.hh"
#include <hiredis/hiredis.h>

using std::string;
using std::vector;

//这里我Redis用的List（双端队列）存储类型，但我们把它当作
//一个Queue类型使用（优先级高的先入先出）
class MyRedis
{
public:
    MyRedis();
    ~MyRedis();

    //向Redis中存储一个key+value
    void Push(string &key, string &value);
    //把所有key对应的value取出来
    vector<string> Pop(const string &key);

private:
    redisContext *_p_connect;
    redisReply *_p_reply;
};

#endif
