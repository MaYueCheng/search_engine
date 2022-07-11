#ifndef THREADPOOL_HH
#define THREADPOOL_HH

#include "../cpp.hh"
#include "NoCopy.hh"

#define BUSY true
#define FREE false
#define EXIT true
#define RUNNING false

using std::queue;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::function;
//EventQueue中的一个个Event就是一个个要执行的函数(MyEvent中的HandlerEvent())
using Event = function<void()>;

class MutexLock
: public NoCopy
{
public:
    MutexLock();
    ~MutexLock();

    void Lock();
    void Unlock();

    pthread_mutex_t *GetMutexPoint();
private:
    pthread_mutex_t _mutex;
};

class AutoMutexLock
{
public:
    AutoMutexLock(MutexLock &mutex);
    ~AutoMutexLock();

private:
    MutexLock &_mutex;
};

class Condition
: public NoCopy
{
public:
    Condition(MutexLock &mutex);
    ~Condition();

    void Wait();
    void Signal();
    void SignalAll();

private:
    pthread_cond_t _cond;
    MutexLock &_mutex;
};

class EventQueue
: public NoCopy
{
public:
    EventQueue();
    ~EventQueue() = default;

    bool Empty() const;

    void EnQueue(Event &&event);
    Event Dequeue();

    void AllWakeup();

private:
    queue<Event> _event_queue;
    MutexLock _mutex;
    Condition _condition;
    bool _pool_state;
};

class Handler
{
public:
    Handler(Event &&event);
    ~Handler() = default;

    void Start();
    void Join();

private:
    static void *ThreadFunc(void *arg);
private:
    pthread_t _tid;
    bool _handler_state;
    //handler要干的活
    Event _event;
};

class ThreadPool
{
public:
    ThreadPool(size_t handler_num);
    ~ThreadPool();

    void Start();

    void EventEnqueue(Event &&event);
    Event GetEvent();

private:
    void ThreadFunc();
private:
    size_t _handler_num;
    vector<unique_ptr<Handler>> _handlers;
    EventQueue _event_queue;
    bool _pool_state;
};

#endif
