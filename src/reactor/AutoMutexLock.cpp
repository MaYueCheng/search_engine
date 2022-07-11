#include "../../inc/reactor/ThreadPool.hh"

AutoMutexLock::AutoMutexLock(MutexLock &mutex)
: _mutex(mutex)
{
    _mutex.Lock();
}

AutoMutexLock::~AutoMutexLock()
{
    _mutex.Unlock();
}