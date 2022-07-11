#ifndef ECHO_SERVER_HH
#define ECHO_SERVER_HH

#include "../cpp.hh"
#include "ThreadPool.hh"
#include "TcpServer.hh"
#include "../key_commander/KeyRecommander.hh"
#include <nlohmann/json.hpp>
#include "../message.hh"

using json = nlohmann::json;
using std::string;

class EchoServer
{
public:
    EchoServer(const size_t handler_num, const string &ip, const port_t &port, 
               KeyRecommander &key_recommander);
    ~EchoServer();

    void Start();

private:
    //这是TCP三个阶段后的三件事
    void AfterConnection(const TcpPoint &p_tcp);
    void AfterMessage(const TcpPoint &p_tcp);
    void AfterClose(const TcpPoint &p_tcp);
private:
    void ToTrain(const json &json_message, train_t &train_message);
private:
    ThreadPool _thread_pool;
    TcpServer _tcp_server;
    KeyRecommander &_key_recommander;
};

#endif