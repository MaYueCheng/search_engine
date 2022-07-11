#include "../../inc/reactor/EventLoop.hh"

using std::cout;
using std::cerr;
using std::endl;

EventLoop::EventLoop(int monitor_fd)
: _epfd(CreateEpoll())
, _monitor_fd(monitor_fd)
, _loop_state(FREE)
, _event_list(1024) //设置刚开始的_event_list大小为1024（最多同时有1024个event就绪）
, _event_fd(CreateEventFd())
{
    EpollAdd(_monitor_fd); //监听_monitor_fd
    EpollAdd(_event_fd);   //监听_event_fd
}

EventLoop::~EventLoop()
{
    if(_epfd != 0)
    {
        close(_epfd);
    }

    if(_event_fd != 0)
    {
        close(_event_fd);
    }
}

int EventLoop::CreateEpoll()
{
    int epfd = epoll_create1(0);
    if(epfd == -1)
    {
        perror("epoll_create");
        exit(0);
    }
    return epfd;
}

void EventLoop::Loop()
{
    _loop_state = BUSY;
    cout << "Server is working!" << endl;
    while(_loop_state == BUSY)
    {
        EpollWait();
    }
}

void EventLoop::UnLoop()
{
    _loop_state = FREE;
}

void EventLoop::EpollWait()
{
    while(true)
    {
        int ready_num = epoll_wait(_epfd, _event_list.data(), _event_list.size(), -1);
        if(ready_num == -1 && errno == EINTR)
        {
            perror("epoll_wait");
            exit(0);
        }
        else
        {
            //如果event的ready_num等于就绪队列_event_list那么给_event_list扩容两倍
            if (ready_num == static_cast<int>(_event_list.size()))
            {
                _event_list.resize(2 * ready_num); //考虑到了监听超过上限的问题
            }
            else
            {
                for (int i = 0; i < ready_num; ++i)
                {
                    //如果_monitor_fd响应，说明要进来新的TCP连接
                    if (_event_list.at(i).data.fd == _monitor_fd)
                    {
                        HandleNewConnection();
                    }
                    //如果_event_fd响应，说明handler完成了event，需要向客户端返还结果
                    else if (_event_list.at(i).data.fd == _event_fd)
                    {
                        ReceiveNotice();
                        ExecuteFinishEvent();
                    }
                    //如果_connected_fd响应，说明已连接的TCP有消息到来
                    else
                    {
                        HandleEvent(_event_list.at(i).data.fd);
                    }
                }
            }
        }
    }

}

void EventLoop::EpollAdd(int fd)
{
    //注册事件
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret == -1)
    {
        perror("EpollAdd");
        return;
    }
}

void EventLoop::EpollDel(int fd)
{
    //注册事件
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &event);
    if(ret == -1)
    {
        perror("EpollDel");
        return;
    }
}

void EventLoop::HandleNewConnection()
{
    TcpPoint tcp(new TCP(_monitor_fd, this));
    //TCP三个半事件的注册
    tcp->RegisterConnection(std::move(_AfterConnect));
    tcp->RegisteMessage(std::move(_AfterMessage));
    tcp->RegisterClose(std::move(_AfterClose));

    int connected_fd = tcp->GetConnectedFd();
    if(connected_fd == -1)
    {
        perror("accept");
        exit(0);
    }
    EpollAdd(connected_fd); //把新的TCP连接加入epoll监听
    _tcp.insert({connected_fd, tcp});
    
    //TCP创建完成，执行第一个事件(AfterConnection)
    tcp->ExecuteConnection();
}

void EventLoop::HandleEvent(int connected_fd)
{
    auto iter = _tcp.find(connected_fd);
    if(iter != _tcp.end())
    {
        //判断TCP连接是否断开
        bool tcp_close = iter->second->JudgeTcpClose();
        //如果没有断开，执行收到消息后的操作(AfterMessage)
        if(tcp_close == false)
        {
            iter->second->ExecuteMessage(); 
        }
        //如果断开，执行断开后的操作(AfterClose)
        else
        {
            iter->second->ExecuteClose();
            EpollDel(connected_fd); //把该断开的TCP移出epoll监听
            _tcp.erase(iter);       //把TCP连接移出建立连接的集合
        }
    }
    else
    {
        cerr << "this TCP connection don't exist" << endl; //没找到该TCP
    }
}

void EventLoop::RegisterConnection(TcpFunction_t &&after_connection)
{
    _AfterConnect = std::move(after_connection);
}

void EventLoop::RegisteMessage(TcpFunction_t &&after_message)
{
    _AfterMessage = std::move(after_message);
}

void EventLoop::RegisterClose(TcpFunction_t &&after_close)
{
    _AfterClose = std::move(after_close);
}
    
int EventLoop::CreateEventFd()
{
    int event_fd = eventfd(10, 0);
    if(event_fd == -1)
    {
        perror("eventfd");
        exit(0);
    }
    return event_fd;
}

void EventLoop::Notice()
{
    uint64_t one = 1;
    int ret = write(_event_fd, &one, sizeof(uint64_t));
    if (ret != sizeof(uint64_t))
    {
        perror("write");
        exit(0);
    }
}

void EventLoop::ReceiveNotice()
{
    uint64_t one = 1;
    int ret = read(_event_fd, &one, sizeof(uint64_t));
    if (ret != sizeof(uint64_t))
    {
        perror("read");
        exit(0);
    }
}

void EventLoop::RegistFinishEvent(FinishEvent_t &&finish_event)
{
    //提前结束锁资源，减小互斥锁粒度
    {
    AutoMutexLock auto_mutex(_mutex);
    _finish_events.push_back(std::move(finish_event));
    }

    Notice();
}

void EventLoop::ExecuteFinishEvent()
{
    vector<FinishEvent_t> tmp;
    {
        AutoMutexLock auto_mutex(_mutex);
        tmp.swap(_finish_events);
    }

    //vector<Functor> _pengingsCb;
    for(auto &finish_event : tmp)
    {
        finish_event();
    }
}