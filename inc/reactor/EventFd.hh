#ifndef EVENT_FD_HH
#define EVENT_FD_HH

#include "../cpp.hh"

#define BUSY true 
#define FREE false 

using std::function;

using Function_t = function<void()>;

class EventFd
{
public:
    EventFd(const Function_t &Handle);
    ~EventFd();

    void Start();
    void End();

    void ReadHandle();

private:
    void WriteHandle();
private:
    int _event_fd;
    Function_t _Handle;
    bool _loop_state;
};

#endif