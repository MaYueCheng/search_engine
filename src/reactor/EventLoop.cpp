#include "../../inc/reactor/EventLoop.hh"

using std::cout;
using std::cerr;
using std::endl;

EventLoop::EventLoop(int monitor_fd)
: _epfd(CreateEpoll())
, _monitor_fd(monitor_fd)
, _loop_state(FREE)
, _event_list(1024) //���øտ�ʼ��_event_list��СΪ1024�����ͬʱ��1024��event������
, _event_fd(CreateEventFd())
{
    EpollAdd(_monitor_fd); //����_monitor_fd
    EpollAdd(_event_fd);   //����_event_fd
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
            //���event��ready_num���ھ�������_event_list��ô��_event_list��������
            if (ready_num == static_cast<int>(_event_list.size()))
            {
                _event_list.resize(2 * ready_num); //���ǵ��˼����������޵�����
            }
            else
            {
                for (int i = 0; i < ready_num; ++i)
                {
                    //���_monitor_fd��Ӧ��˵��Ҫ�����µ�TCP����
                    if (_event_list.at(i).data.fd == _monitor_fd)
                    {
                        HandleNewConnection();
                    }
                    //���_event_fd��Ӧ��˵��handler�����event����Ҫ��ͻ��˷������
                    else if (_event_list.at(i).data.fd == _event_fd)
                    {
                        ReceiveNotice();
                        ExecuteFinishEvent();
                    }
                    //���_connected_fd��Ӧ��˵�������ӵ�TCP����Ϣ����
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
    //ע���¼�
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
    //ע���¼�
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
    //TCP�������¼���ע��
    tcp->RegisterConnection(std::move(_AfterConnect));
    tcp->RegisteMessage(std::move(_AfterMessage));
    tcp->RegisterClose(std::move(_AfterClose));

    int connected_fd = tcp->GetConnectedFd();
    if(connected_fd == -1)
    {
        perror("accept");
        exit(0);
    }
    EpollAdd(connected_fd); //���µ�TCP���Ӽ���epoll����
    _tcp.insert({connected_fd, tcp});
    
    //TCP������ɣ�ִ�е�һ���¼�(AfterConnection)
    tcp->ExecuteConnection();
}

void EventLoop::HandleEvent(int connected_fd)
{
    auto iter = _tcp.find(connected_fd);
    if(iter != _tcp.end())
    {
        //�ж�TCP�����Ƿ�Ͽ�
        bool tcp_close = iter->second->JudgeTcpClose();
        //���û�жϿ���ִ���յ���Ϣ��Ĳ���(AfterMessage)
        if(tcp_close == false)
        {
            iter->second->ExecuteMessage(); 
        }
        //����Ͽ���ִ�жϿ���Ĳ���(AfterClose)
        else
        {
            iter->second->ExecuteClose();
            EpollDel(connected_fd); //�ѸöϿ���TCP�Ƴ�epoll����
            _tcp.erase(iter);       //��TCP�����Ƴ��������ӵļ���
        }
    }
    else
    {
        cerr << "this TCP connection don't exist" << endl; //û�ҵ���TCP
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
    //��ǰ��������Դ����С����������
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