#include "../../inc/reactor/TcpServer.hh"

TcpServer::TcpServer(const std::string &ip, const port_t &port)
: _monitor(ip, port)
, _event_loop(_monitor.GetMonitorFd())
{

}

void TcpServer::Start()
{
    _event_loop.Loop();
}

void TcpServer::End()
{
    _event_loop.UnLoop();
}

void TcpServer::RegisterAllEvent(TcpFunction_t &&after_connection,
                                 TcpFunction_t &&after_message,
                                 TcpFunction_t &&after_close)
{
    _event_loop.RegisterConnection(std::move(after_connection));
    _event_loop.RegisteMessage(std::move(after_message));
    _event_loop.RegisterClose(std::move(after_close));
}
