#ifndef MESSAGE_HH
#define MESSAGE_HH

#include "cpp.hh"

using std::string;

//私有火车协议
struct train_t
{
    int length;     // message的长度
    int id;         // message类型
    string message; //具体传递内容
};

#endif