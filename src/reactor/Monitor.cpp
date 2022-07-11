#include "../../inc/reactor/Socket.hh"

Monitor::Monitor(const std::string &ip, const port_t &port)
: _server_addr(ip, port)
{
    _monitor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_monitor_fd < 0)
    {
        perror("socket");
        exit(0);
    }
    int on = 1;
    int s_ret = setsockopt(_monitor_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    if(s_ret)
    {
        perror("setsockopt");
    }
   
    int b_ret = bind(_monitor_fd,(sockaddr *)_server_addr.GetAddressPoint(), sizeof(sockaddr));
    if(b_ret == -1)
    {
        perror("bind");
        exit(0);
    }
    int l_ret = listen(_monitor_fd, SOMAXCONN);
    if(l_ret == -1)
    {
        perror("listen");
        exit(0);
    }
}

Monitor::~Monitor()
{
    close(_monitor_fd);
}

int Monitor::GetMonitorFd() const
{
    return _monitor_fd;
}
