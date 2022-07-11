#include "../../inc/reactor/EchoServer.hh"
#include "../../inc/reactor/MyEvent.hh"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::bind;
using namespace std::placeholders;

EchoServer::EchoServer(const size_t handler_num, const string &ip, const port_t &port,
                       KeyRecommander &key_recommander)
: _thread_pool(handler_num)
, _tcp_server(ip, port)
, _key_recommander(key_recommander)
{

}

EchoServer::~EchoServer()
{
    //EchoServer结束后，自动调用thread_pool的析构函数
    //本来tcp_server也应该这么写的，之后有机会改一下（挖个坑）
    _tcp_server.End();
}

void EchoServer::Start()
{
    //启动ThreadPool（计算线程）
    _thread_pool.Start();
    //注册TCP连接的三件事
    _tcp_server.RegisterAllEvent(bind(&EchoServer::AfterConnection, this, _1),
                                 bind(&EchoServer::AfterMessage, this, _1),
                                 bind(&EchoServer::AfterClose, this, _1));
    //启动TcpServer（I/O线程）
    _tcp_server.Start();
}

void EchoServer::AfterConnection(const TcpPoint &p_tcp)
{
    cout << "one client has connected!(" << p_tcp->GetPeerAddress() << ")"<< endl; 
}

void EchoServer::AfterMessage(const TcpPoint &p_tcp)
{
    //收到客户端发来的消息（json格式）
    json json_message = json::parse(p_tcp->Receive());
    train_t train_message;
    ToTrain(json_message, train_message); //把json格式的message转化为通用火车协议
    //如果id为1，说明这是进行推荐关键字查询阶段
    if(train_message.id == 1)
    {
        cout << "client query keyword:---->" << train_message.message << endl;
        // MyEvent中的第一个参数其实是处理后的消息，这里先用recv_msg替代
        MyEvent my_event(train_message.message, p_tcp, _key_recommander);
        _thread_pool.EventEnqueue(bind(&MyEvent::HandlerEvent, my_event));
    }
    //如果id为3，说明这是进行网页查询阶段
    else if(train_message.id == 3)
    {

    }
}

void EchoServer::AfterClose(const TcpPoint &p_tcp)
{
    cout << "one client has disconnected!(" << p_tcp->GetPeerAddress() << ")"<< endl; 
}

void EchoServer::ToTrain(const json &json_message, train_t &train_message)
{
    json_message.at("length").get_to(train_message.length);
    json_message.at("id").get_to(train_message.id);
    json_message.at("message").get_to(train_message.message);
}
