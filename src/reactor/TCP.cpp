#include "../../inc/reactor/Socket.hh"

using std::bind;
using std::string;

TCP::TCP(int monitor_fd, EventLoop *event_loop)
: _acceptor(monitor_fd)
, _connected_fd(_acceptor.ConnectedFd())
, _socket_io(_connected_fd)
, _event_loop(event_loop)
{

}

void TCP::Send(const string &msg)
{
    _socket_io.Sendn(msg.c_str(), msg.size());
}

void TCP::SendToLoop(const string &msg)
{
    if(_event_loop != nullptr)
    {
        train_t train_message = {static_cast<int>(msg.size()), 2, msg};
        json json_message;
        ToJson(json_message, train_message);
        //handler干完event之后，把给客户端发送消息的函数注册给I/O线程（EventLoop），让它执行
        _event_loop->RegistFinishEvent(bind(&TCP::Send, this, json_message.dump()));
    }
}

string TCP::Receive()
{
    char buf[4096] = {0};
    _socket_io.ReceiveLine(buf, sizeof(buf));
    
    return string(buf);
}

int TCP::GetConnectedFd() const
{
    return _connected_fd;
}

bool TCP::JudgeTcpClose() const
{
    char buf[10] = {0};
    int ret = recv(_connected_fd, buf, sizeof(buf), MSG_PEEK);
    if(ret == 0)
    {
        return true; //TCP连接断开
    }
    else
    {
        return false; //TCP连接没有断开
    }
}

//获取本服务器套接字
string TCP::GetLocalAddress() const
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(sockaddr);
    int ret = getsockname(_acceptor.ConnectedFd(), (sockaddr *)&addr, &len);
    if(ret == -1)
    {
        perror("getsockname");
    }
    Address local_addr(addr);
    std::ostringstream oss;
    oss << "IP = " << local_addr.GetIP() << ", port = " << local_addr.GetPort();

    return oss.str();
}

//获取对端套接字
string TCP::GetPeerAddress() const
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(sockaddr);
    int ret = getpeername(_acceptor.ConnectedFd(), (sockaddr *)&addr, &len);
    if(ret == -1)
    {
        perror("getpeername");
    }
    Address peer_addr(addr);
    std::ostringstream oss;
    oss << "ip = " << peer_addr.GetIP() << ", port = " << peer_addr.GetPort();

    return oss.str();
}

void TCP::RegisterConnection(TcpFunction_t &&after_connection)
{
    _AfterConnect = std::move(after_connection);
}

void TCP::RegisteMessage(TcpFunction_t &&after_message)
{
    _AfterMessage = std::move(after_message);
}

void TCP::RegisterClose(TcpFunction_t &&after_close)
{
    _AfterClose = std::move(after_close);
}

void TCP::ExecuteConnection()
{
    _AfterConnect(shared_from_this());
}

void TCP::ExecuteMessage()
{
    _AfterMessage(shared_from_this());
}

void TCP::ExecuteClose()
{
    _AfterClose(shared_from_this());
}

void TCP::ToJson(json &json_message, const train_t &train_message)
{
    json_message = json{{"length", train_message.length}, 
                        {"id", train_message.id}, 
                        {"message", train_message.message}};
}