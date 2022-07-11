#include "../../inc/reactor/Socket.hh"

Address::Address(const std::string &ip, const port_t &port)
{
    memset(&_addr, 0, sizeof(sockaddr_in));
    _addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr.s_addr);
    _addr.sin_port = htons(port);
}

Address::Address(const sockaddr_in &addr)
: _addr(addr)
{

}

std::string Address::GetIP() const
{
    char temp[20] = {0};
    return std::string(inet_ntop(AF_INET, &_addr.sin_addr, temp, sizeof(temp)));
}

port_t Address::GetPort() const
{
    return ntohs(_addr.sin_port);
}

const sockaddr_in *Address::GetAddressPoint() const
{
    return &_addr;
}