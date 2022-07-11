#include "../../inc/reactor/MyEvent.hh"

using std::vector;
using std::ostringstream;
using std::endl;

MyEvent::MyEvent(const string &msg, const TcpPoint &p_tcp, KeyRecommander &key_recommander)
: _msg(msg)
, _p_tcp(p_tcp)
, _key_recommander(key_recommander)
{

}

void MyEvent::HandlerEvent()
{
    //hadler处理客户端发来的query_word，并得出5个recommander_keyword
    vector<string> v_recommand_keywords = _key_recommander.Query(_msg);
    ostringstream oss;
    for(const auto &recommand_keyword : v_recommand_keywords)
    {
        oss << recommand_keyword << endl;
    }

    //给客户端返回5个recommander_keyword（这个事情要EventLoop（IO线程）去做）
    string recommand_keywords(oss.str());
    _p_tcp->SendToLoop(recommand_keywords);
}