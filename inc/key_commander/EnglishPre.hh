#ifndef ENGLISH_PRE_HH
#define ENGLISH_PRE_HH

#include "../cpp.hh"
//#include "KeywordPreprocessor.hh"

using std::string;
using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::set;

class KeywordPreprocessor;

class EnglishPre
//: public KeywordPreprocessor
{
public:
    EnglishPre(const string &corpus_path, const string &stop_word_fig);
    virtual ~EnglishPre() = default;

private:
    //建立英文字典
    void BuildDictionary();
    void PushDict(const string &word); //把单词加入字典
    //建立英文字典索引
    void BuildIndex();
    //存储英文字典和字典索引
    void Store();
private:
    unordered_set<string> _stop_word;       //停止词
    unordered_set<string> _files;           //语料库的集合
    unordered_map<string, int> _dict;       //词典（词，频率）的集合
    unordered_map<string, set<int>> _index; //索引（词，行号）的集合
};

#endif