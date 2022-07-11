#include "../../inc/reactor/ThreadPool.hh"

MutexLock::MutexLock()
{
    int ret = pthread_mutex_init(&_mutex, nullptr);
    if(ret != 0)
    {
        perror("pthread_mutex_init");
        exit(0);
    }
}

MutexLock::~MutexLock()
{
    int ret = pthread_mutex_destroy(&_mutex);
    if(ret != 0)
    {
        perror("pthread_mutex_destory");
        exit(0);
    }
}

void MutexLock::Lock()
{
    int ret = pthread_mutex_lock(&_mutex);
    if(ret != 0)
    {
        perror("pthread_mutex_lock");
    }
}

void MutexLock::Unlock()
{
    int ret = pthread_mutex_unlock(&_mutex);
    if(ret != 0)
    {
        perror("pthread_mutex_unlock");
    }
}

pthread_mutex_t *MutexLock::GetMutexPoint()
{
    return &_mutex;
}