//C++标准库头文件
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <deque>
#include <queue>
#include <forward_list>
#include <list>
#include <stack>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>  //算法库
#include <iomanip>    //setw()函数的头文件
#include <memory>     //智能指针、STL空间配置器的头文件
#include <functional> //函数对象

//C标准库头文件的C++版本
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <climits> //realpath函数头文件

//Linux API的头文件
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h> //线程有关函数、互斥锁、条件变量头文件
#include <unistd.h>  //sleep函数头文件
#include <sys/types.h>
#include <sys/socket.h>  //socket函数族头文件
#include <arpa/inet.h>   //sockaddr_in头文件
#include <sys/epoll.h>   //epoll函数族头文件
#include <poll.h>        //poll函数头文件
#include <sys/eventfd.h> //eventfd函数头文件

#define ERROR_PRINT()                       \
    {                                       \
        std::cout << __DATE__ << " "        \
                  << __TIME__ << " "        \
                  << __LINE__ << " "        \
                  << __func__ << " "        \
                  << __FILE__ << std::endl; \
    }

