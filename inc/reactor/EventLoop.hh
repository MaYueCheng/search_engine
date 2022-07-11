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

    //TCP���ӵ��������¼�
    void RegisterConnection(TcpFunction_t &&func); //���ӽ���
    void RegisteMessage(TcpFunction_t &&func);     //��Ϣ����
    void RegisterClose(TcpFunction_t &&func);      //���ӶϿ�
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
    int _epfd;        //��һ��_epfd����һ��Ψһ��epoll
    int _monitor_fd;
    bool _loop_state; //EventLoop��״̬
    vector<epoll_event> _event_list; //�洢�����о�����epoll_event;
    //EventLoop�ж���һ����connected_fd����Ϊ���������ݱ�������һ���������������value��һ����Ļ���
    //ô��һ���ഫ���������TCP�Ŀ������캯���������ᴴ��һ���µ�TCP������ԭ����TCP���������ʹTCP &��
    //����TCP���ں����ڲ���������������С��map�����ڣ�����ʹ�á�����ֻ�ܷ�һ��ָ�룬ָ��TCP��
    map<int, TcpPoint> _tcp; 
private:
    TcpFunction_t _AfterConnect;
    TcpFunction_t _AfterMessage;
    TcpFunction_t _AfterClose;
private:
    int _event_fd;                        //��һ��_event_f���������̹߳�ͨ��ͨ��
    vector<FinishEvent_t> _finish_events; // vector�д�ŵ������н��յ���Ҫִ�еĺ���
    MutexLock _mutex;                     //��Ϊfinish_events�Ƕ��̹߳������Դ������ʹ�ã�����Ҫ����
};

#endif