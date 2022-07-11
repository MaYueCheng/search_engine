#ifndef MY_EVENT_HH
#define MY_EVENT_HH

#include "../cpp.hh"
#include "Socket.hh"
#include "../key_commander/KeyRecommander.hh"

using std::string;

class MyEvent
{
public:
    MyEvent(const string &msg, const TcpPoint &p_tcp, KeyRecommander &key_recommander);
    ~MyEvent() = default;

    //这里是handler真正要干的事情
    void HandlerEvent();

private:
    string _msg;
    TcpPoint _p_tcp;
    KeyRecommander &_key_recommander;
};

#endif