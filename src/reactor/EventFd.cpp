#include "../../inc/reactor/EventFd.hh"

EventFd::EventFd(const Function_t &Handle)
: _event_fd(-1)
, _Handle(std::move(Handle))
, _loop_state(BUSY)
{
    int event_fd = eventfd(10, 0);
    if(event_fd == -1)
    {
        perror("eventfd");
        exit(0);
    }
    _event_fd = event_fd;
}

EventFd::~EventFd()
{
    if(_event_fd != 0)
    {
        close(_event_fd);
    }
}

void EventFd::Start()
{
    pollfd pfd;
    pfd.fd = _event_fd;
    while(_loop_state == BUSY)
    {
        int ready_num = poll(&pfd, 1, -1);
        if(ready_num == -1 && errno == EINTR)
        {
            continue;
        }
        else if(ready_num == -1)
        {
            perror("poll");
        }
        else
        {
            if(pfd.revents & POLL_IN)
            {
                ReadHandle();
                _Handle();
            }
        }
    }
    
}

void EventFd::End()
{
    _loop_state = FREE;
}

void EventFd::ReadHandle()
{
    uint64_t one = 1;
    int ret = write(_event_fd, &one, sizeof(uint64_t));
    if(ret != sizeof(uint64_t))
    {
        perror("write");
        exit(0);
    }
}

void EventFd::WriteHandle()
{
    uint64_t one = 1;
    int ret = read(_event_fd, &one, sizeof(uint64_t));
    if(ret != sizeof(uint64_t))
    {
        perror("read");
        exit(0);
    }
}

