#ifndef DICT_PRODUCER_HH
#define DICT_PRODUCER_HH

#include "../cpp.hh"
#include "ChinesePre.hh"
#include "EnglishPre.hh"
#include "SplitTool.hh"
#include "Configuration.hh"

class DictProducer
{
public:
    DictProducer(const string &chinese_corpus, const string &cpp_jieba_conf,
                 const string &english_corpus, const string &stop_word_conf);
    ~DictProducer() = default;

private:
    ChinesePre _chinese_pre;
    EnglishPre _english_pre;
};

#endif

