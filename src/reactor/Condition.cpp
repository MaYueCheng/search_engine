#include "../../inc/reactor/ThreadPool.hh"

Condition::Condition(MutexLock &mutex)
: _mutex(mutex)
{
    int ret = pthread_cond_init(&_cond, nullptr);
    if(ret != 0)
    {
        perror("pthread_cond_init");
        exit(0);
    }
}

Condition::~Condition()
{
    int ret = pthread_cond_destroy(&_cond);
    if(ret != 0)
    {
        perror("pthread_cond_destory");
        exit(0);
    }
}

void Condition::Wait()
{
    pthread_cond_wait(&_cond, _mutex.GetMutexPoint());
}

void Condition::Signal()
{
    pthread_cond_signal(&_cond);
}

void Condition::SignalAll()
{
    pthread_cond_broadcast(&_cond);
}