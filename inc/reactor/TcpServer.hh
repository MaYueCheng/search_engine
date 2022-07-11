#ifndef TCP_SERVER_HH
#define TCP_SERVER_HH

#include "../cpp.hh"
#include "Socket.hh"
#include "EventLoop.hh"

using std::string;

class TcpServer
{
public:
    TcpServer(const string &ip, const port_t &port);
    ~TcpServer() = default;

    void Start();
    void End();

    void RegisterAllEvent(TcpFunction_t &&after_connection,
                          TcpFunction_t &&after_message,
                          TcpFunction_t &&after_close);

private:
    Monitor _monitor;
    EventLoop _event_loop;
};

#endif