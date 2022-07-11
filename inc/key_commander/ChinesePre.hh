#ifndef CHINESE_PRE_HH
#define CHINESE_PRE_HH

#include "../cpp.hh"
//#include "KeywordPreprocessor.hh"
#include "Configuration.hh"
#include "SplitTool.hh"

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::set;

//建立中文的字典和字典索引
class ChinesePre
//: public KeywordPreprocessor
{
public:
    //传入中文语料库的配置文件的地址，和CppJieba的配置文件的配置文件的地址
    ChinesePre(const string &corpus_path, const string &cpp_jieba_conf);
    virtual ~ChinesePre() = default;

private:
    //建立中文词典
    void BuildDictionary();
    //建立中文词典索引
    void BuildIndex();
    //存储词典和词典索引
    void Store();
    //判断该字符串是否是中文
    bool IsChinese(const string &keyword);
private:
    Configuration _corpus;                  //语料库的配置类
    Configuration _cpp_jieba_conf;          //CppJieba的配置类
    SplitTool _split_tool;                  //分词工具类
    unordered_set<string> _files;           //语料库的集合
    unordered_map<string, int> _dict;       //词典（词，频率）的集合
    unordered_map<string, set<int>> _index; //索引（词，行号）的集合
};

#endif