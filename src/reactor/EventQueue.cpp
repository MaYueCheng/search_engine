#include "../../inc/reactor/ThreadPool.hh"

EventQueue::EventQueue()
:_mutex()
, _condition(_mutex)
, _pool_state(RUNNING)
{

}

bool EventQueue::Empty() const
{
    if(_event_queue.size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void EventQueue::EnQueue(Event &&event)
{
    AutoMutexLock auto_mutex(_mutex); //利用RAII自动释放锁资源

    _event_queue.push(std::move(event));
    _condition.Signal();
}

Event EventQueue::Dequeue()
{
    AutoMutexLock auto_mutex(_mutex); //利用RAII自动释放锁资源

    //这里用while而不是if是防止if的虚假唤醒（如果本来task_queue为空，然后有一个线程produce了一个
    //产品，这样task_queue就不为空，要继续往下走，但是这期间又有一个线程consume了一个产品，导致
    // task_queue由为空了，但是之前的线程依旧还以为有产品准备consume故导致线程的崩溃）
    while(_pool_state == RUNNING && Empty() == true)
    {
        _condition.Wait();
    }
    //虽然当_event_queue为空时，front函数的返回值也为nullptr但是为了安全起见，还是用了一个if-else当
    //_pool_state == EXIT的时候 return nullptr
    if(_pool_state == RUNNING)
    {
        Event event = _event_queue.front();
        _event_queue.pop();

        return event;
    }
    else
    {
        return nullptr;
    }
}

void EventQueue::AllWakeup()
{
    _pool_state = EXIT;
    _condition.SignalAll();
}