#include "../../inc/reactor/ThreadPool.hh"

Handler::Handler(Event &&event)
: _tid(0)
, _handler_state(FREE)
, _event(std::move(event))
{

}

void Handler::Start()
{
    // run方法如何调用，需要有对象，需要有Thread类型的对象或者指针
    //该指针可以从arg这里传进来,所以pthread_create第四个参数就不能
    //是nullptr
    int ret = pthread_create(&_tid, nullptr, ThreadFunc, this);
    if(ret != 0)
    {
        perror("pthread_create");
        return;
    }
    _handler_state = BUSY;
}

void Handler::Join()
{
    if(_handler_state == BUSY)
    {
        pthread_join(_tid, nullptr);
        _handler_state = FREE;
    }
}

void *Handler::ThreadFunc(void *arg)
{
    Handler *handler = static_cast<Handler *>(arg);
    if(handler != nullptr)
    {
        handler->_event(); //Handler真正在干的事（执行MyEvent传进来的HandlerEvent）
    }

    pthread_exit(nullptr);
}