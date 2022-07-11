#include "../../inc/reactor/ThreadPool.hh"

using std::unique_ptr;
using std::bind;

ThreadPool::ThreadPool(size_t handler_num)
: _handler_num(handler_num)
, _event_queue()
, _pool_state(RUNNING)
{
    _handlers.reserve(handler_num);
}

ThreadPool::~ThreadPool()
{
    if(_pool_state == RUNNING)
    {
        while(_event_queue.Empty() == false);
        _pool_state = EXIT;
        //如果线程在退出之前处于pthread_cond_wait状态，我们就将所有的线程唤醒
        _event_queue.AllWakeup();
        for (const auto &thread : _handlers)
        {
            thread->Join();
        }
    }
}

void ThreadPool::Start()
{
    for(int i = 0; i < _handler_num; ++i)
    {
        //告诉Handler他要干什么
        unique_ptr<Handler> p_handler(new Handler(bind(&ThreadPool::ThreadFunc, this)));
        _handlers.push_back(std::move(p_handler)); //unique_ptr不能进行拷贝和复制，只能用移动构造函数
    }

    //for(const auto &thread: _threads)
    for(const auto &thread: _handlers)
    {
        thread->Start();
    }
}

void ThreadPool::EventEnqueue(Event &&event)
{
    if(event)
    {
        _event_queue.EnQueue(std::move(event));
    }
}

Event ThreadPool::GetEvent()
{
    return _event_queue.Dequeue();
}

void ThreadPool::ThreadFunc()
{
    //线程池没有退出就一直获取event
    while(_pool_state == RUNNING)
    {
        Event event = GetEvent();
        if (event)
        {
            event();
        }
    }
}

