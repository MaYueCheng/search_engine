//C++��׼��ͷ�ļ�
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
#include <algorithm>  //�㷨��
#include <iomanip>    //setw()������ͷ�ļ�
#include <memory>     //����ָ�롢STL�ռ���������ͷ�ļ�
#include <functional> //��������

//C��׼��ͷ�ļ���C++�汾
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <climits> //realpath����ͷ�ļ�

//Linux API��ͷ�ļ�
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h> //�߳��йغ���������������������ͷ�ļ�
#include <unistd.h>  //sleep����ͷ�ļ�
#include <sys/types.h>
#include <sys/socket.h>  //socket������ͷ�ļ�
#include <arpa/inet.h>   //sockaddr_inͷ�ļ�
#include <sys/epoll.h>   //epoll������ͷ�ļ�
#include <poll.h>        //poll����ͷ�ļ�
#include <sys/eventfd.h> //eventfd����ͷ�ļ�

#define ERROR_PRINT()                       \
    {                                       \
        std::cout << __DATE__ << " "        \
                  << __TIME__ << " "        \
                  << __LINE__ << " "        \
                  << __func__ << " "        \
                  << __FILE__ << std::endl; \
    }

