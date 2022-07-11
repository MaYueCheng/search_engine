#ifndef DICTIONARY_HH
#define DICTIONARY_HH

#include "../cpp.hh"
#include "MyRedis.hh"

using std::vector;
using std::pair;
using std::unordered_map;
using std::string;
using std::set;

using dict_t = vector<pair<string, int>>;
using index_t = unordered_map<string, set<int>>;

class Dictionary
{
public:
    Dictionary(const string &dict_path, const string &index_path);
    ~Dictionary() = default;

    dict_t Query(const string &keyword);

    //最小编辑距离算法
    int EditDistance(const string &lhs, const string &rhs);
    //测试
    void ShowDict();
    void ShowIndex();

private:
    //把离线的字典和字典索引加载到内存中
    void LoadDict(const string &dict_path);
    void LoadIndex(const string &index_path);
    bool IsChinese(const string &keyword);
    
    //最小编辑距离算法调用函数
    size_t nBytesCode(const char ch);
    size_t length(const string &str);
    int triple_min(const int &a, const int &b, const int &c);
private:
    dict_t _dict;   //词典（词，频率）
    index_t _index; //索引（英文：（字母，行号） 中文：（字，行号））
    MyRedis _redis; //用Redis用作缓存（相当于cookies）
};

#endif