#include "../inc/cpp.hh"
#include "../inc/key_commander/KeyRecommander.hh"
#include "../inc/reactor/EchoServer.hh"

using std::cout;
using std::endl;
using std::string;

/*************************************************************************************
//默认已经由中英文语料库，生成了词典和词典索引
//如果觉得词典和词典索引不够大，可以在该配置文件中添加语料文件来扩充词典和词典索引
const string cpp_jieba_conf("../conf/cpp_jieba_config.conf");            // CppJieba配置文件
const string chinese_corpus_path("../conf/chinese_config.conf");         //中文语料库
const string english_corpus_path("../conf/english_config.conf");         //英文语料库
const string english_stop_word("../conf/english_stop_word_config.conf"); //英文停止词
*************************************************************************************/

const string dict_path("../data/dict.dat");             //已经生成的字典的地址
const string dict_index_path("../data/dict_index.dat"); //已经生成的字典索引的地址

int main()
{
    //如果有特殊需求，自己生成词典和词典索引
    /* DictProducer dict_producer(chinese_corpus_path, cpp_jieba_conf,
                               english_corpus_path, english_stop_word); */

    KeyRecommander key_commander(dict_path, dict_index_path);
    //工作线程数量默认为3个
    const size_t handler_num = 3;
    const string ip = "127.0.0.1";
    const port_t port = 1111;
    EchoServer server(handler_num, ip, port, key_commander);
    server.Start();

    return 0;
}