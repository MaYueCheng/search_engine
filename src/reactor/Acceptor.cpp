#include "../../inc/reactor/Socket.hh"

Acceptor::Acceptor(int monitor_fd)
{
    _connected_fd = accept(monitor_fd, nullptr, nullptr);
    if (_connected_fd == -1)
    {
        perror("accept");
        exit(0);
    }
}

int Acceptor::ConnectedFd() const
{
    return _connected_fd;
}