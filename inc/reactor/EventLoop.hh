#ifndef EVENT_LOOP_HH
#define EVENT_LOOP_HH

#include "../cpp.hh"
#include "Socket.hh"
#include "ThreadPool.hh"

#define BUSY true
#define FREE false

using std::shared_ptr;
using std::function;
using std::vector;
using std::map;

class TCP;
using TcpPoint = shared_ptr<TCP>;
using TcpFunction_t = function<void(const TcpPoint &p_tcp)>;
using FinishEvent_t = function<void()>;

class EventLoop
{
public:
    EventLoop(int monitor_fd);
    ~EventLoop();

    void Loop();
    void UnLoop();

    //TCP连接的三个半事件
    void RegisterConnection(TcpFunction_t &&func); //连接建立
    void RegisteMessage(TcpFunction_t &&func);     //消息到达
    void RegisterClose(TcpFunction_t &&func);      //连接断开
public:
    int CreateEventFd();
    void Notice();
    void ReceiveNotice();
    void RegistFinishEvent(FinishEvent_t &&finish_event);
    void ExecuteFinishEvent();

private:
    int CreateEpoll();
    void EpollAdd(int fd);
    void EpollDel(int fd);
    void EpollWait();

    void HandleNewConnection();
    void HandleEvent(int fd);
private:
    int _epfd;        //用一个_epfd代表一个唯一的epoll
    int _monitor_fd;
    bool _loop_state; //EventLoop的状态
    vector<epoll_event> _event_list; //存储着所有就绪的epoll_event;
    //EventLoop中都是一个个connected_fd，因为容器存数据本质上是一个拷贝操作，如果value是一个类的话那
    //么把一个类传进来会调用TCP的拷贝构造函数，这样会创建一个新的TCP而不是原来的TCP，但是如果使TCP &的
    //话该TCP是在函数内部创建的生命周期小于map的周期，不能使用。所以只能放一个指针，指向TCP。
    map<int, TcpPoint> _tcp; 
private:
    TcpFunction_t _AfterConnect;
    TcpFunction_t _AfterMessage;
    TcpFunction_t _AfterClose;
private:
    int _event_fd;                        //用一个_event_f代表两个线程沟通的通道
    vector<FinishEvent_t> _finish_events; // vector中存放的是所有接收到的要执行的函数
    MutexLock _mutex;                     //因为finish_events是多线程共享的资源（竞争使用），需要上锁
};

#endif