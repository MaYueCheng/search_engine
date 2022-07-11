#include "../inc/cpp.hh"
#include "../inc/DictProducer.hh"


DictProducer::DictProducer(const string &chinese_corpus, const string &cpp_jieba_conf,
                           const string &english_corpus, const string &stop_word_conf)
: _chinese_pre(chinese_corpus, cpp_jieba_conf)
, _english_pre(english_corpus, stop_word_conf)

{

}