#ifndef SOCKET_HH
#define SOCKET_HH

#include "../cpp.hh"
#include "NoCopy.hh"
#include "EventLoop.hh"
#include <nlohmann/json.hpp>
#include "../message.hh"

using json = nlohmann::json;
using std::string;
using std::shared_ptr;
using std::function;

using port_t = unsigned short;
class Address
{
public:
    Address() = default;
    Address(const string &ip, const port_t &port);
    Address(const sockaddr_in &addr);
    ~Address() = default;

    string GetIP() const;
    port_t GetPort() const;

    const sockaddr_in *GetAddressPoint() const;

private:
    sockaddr_in _addr;
};

class Monitor
: public NoCopy
{
public:
    Monitor() = default;
    Monitor(const string &ip, const port_t &port);
    ~Monitor();

    int GetMonitorFd() const;

private:
    Address _server_addr;
private:
    int _monitor_fd;
};

class Acceptor
{
public:
    Acceptor(int monitor_fd);
    ~Acceptor() = default;

    int ConnectedFd() const;

private:
    int _connected_fd;
};

class SocketIO
{
public:
    //防止隐式转换
    explicit SocketIO(int connected_fd);
    ~SocketIO() = default;

    int Receiven(char *buf, int len);
    int ReceiveLine(char *buf, int len);
    int Sendn(const char *buf, int len);

private:
    int _connected_fd;
};

//类的前向声明
class EventLoop;
class TCP;
using TcpPoint = shared_ptr<TCP>;
using TcpFunction_t = function<void(const TcpPoint &p_tcp)>;
class TCP
: public std::enable_shared_from_this<TCP>
{
public:
    TCP(int monitor_fd, EventLoop *event_loop);
    ~TCP() = default;

    //向对端发送数据
    void Send(const string &msg);
    void SendToLoop(const string &msg);
    //接收对端发来的数据
    string Receive();

    //获取_connected_fd
    int GetConnectedFd() const;

    bool JudgeTcpClose() const;

    //获取本端套接字
    string GetLocalAddress() const;
    //获取对端套接字
    string GetPeerAddress() const;

    //TCP连接的三个半事件
    //三个回调函数的注册
    void RegisterConnection(TcpFunction_t &&after_connection); //连接建立
    void RegisteMessage(TcpFunction_t &&after_message);        //消息到达
    void RegisterClose(TcpFunction_t &&after_close);           //连接断开
    //三个回调函数的执行
    void ExecuteConnection();
    void ExecuteMessage();
    void ExecuteClose();
private:
    void ToJson(json &json_message, const train_t &train_message);
private:
    Acceptor _acceptor;
    int _connected_fd;
    SocketIO _socket_io;
private:
    //放着三个回调函数的容器std::function
    TcpFunction_t _AfterConnect;
    TcpFunction_t _AfterMessage;
    TcpFunction_t _AfterClose;
private:
    EventLoop *_event_loop;
};

#endif