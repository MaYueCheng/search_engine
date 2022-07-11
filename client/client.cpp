#include "../inc/cpp.hh"
#include "../inc/message.hh"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

void ToJson(json &json_message, const train_t &train_message)
{
    json_message = json{{"length", train_message.length}, 
                        {"id", train_message.id}, 
                        {"message", train_message.message}};
}

void ToTrain(const json &json_message, train_t &train_message)
{
    json_message.at("length").get_to(train_message.length);
    json_message.at("id").get_to(train_message.id);
    json_message.at("message").get_to(train_message.message);
}

int main(int argc, char *argv[])
{
    int connected_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(connected_fd < 0)
    {
        perror("socket");
        return 0;
    }

    sockaddr_in serverAddr;
    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    string port(argv[2]);
    serverAddr.sin_port = htons(static_cast<uint16_t>(stoi(port)));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t addrLen = sizeof(serverAddr);

    if(connect(connected_fd, (struct sockaddr*)&serverAddr, addrLen) < 0)
    {
        perror("connect");
        close(connected_fd);
        return 0;
    }
    cout << "TCP has connected!" << endl;

    while(true)
    {
        cerr << "input query keyword: ";
        string word;
        cin >> word;
        //把输入的query_word包装成火车协议，并转化为json
        train_t train_message = {static_cast<int>(word.size()), 1, word}; //query_word对应的id为1
        json json_message;
        ToJson(json_message, train_message);

        //send发送数据，若对端断开，操作系同会发送一个SIGPIPE信号，导致程序退出，
        //该信号的默认处理方式是退出当前进程
        //解决方案，send第四个参数，MSG_NOSIGNAL
        size_t ret = send(connected_fd, json_message.dump().c_str(), 
                          json_message.dump().size(), MSG_NOSIGNAL); //对端断开send直接出错退出了
        send(connected_fd, "\n", 1, MSG_NOSIGNAL);
        if (ret == -1)
        {
            perror("send");
            return 0;
        }

        char buffer[2048] = {0};
        ret = recv(connected_fd, buffer, sizeof(buffer), 0);
        if (ret == 0)
        {
            perror("recv");
        }
        else if (ret == -1)
        {
            perror("recv");
        }
        else
        {
            string str_buffer(buffer);
            json recv_json_message = json::parse(str_buffer);
            train_t recv_train_message;
            ToTrain(recv_json_message, recv_train_message);
            //如果收到的id为2，代表返回的是推荐关键字(recommander_keyword)
            if(recv_train_message.id == 2)
            {
                cout << recv_train_message.message << endl;
            }
            //如果收到的id为4，代表返回的是结果网页
            else if(recv_train_message.id == 4)
            {

            }
        }
    }

    return 0;
}