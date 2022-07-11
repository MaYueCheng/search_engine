#ifndef SPLIT_TOOL_HH
#define SPLIT_TOOL_HH

#include "../cpp.hh"
#include "Configuration.hh"
#include "../CppJieba/Jieba.hpp"

using std::vector;
using std::string;

class SplitTool
{
public:
    SplitTool(Configuration &cpp_jieba_conf);
    ~SplitTool() = default;

    vector<string> Cut(const string &sentence);

private:
    string _dict_path;
    string _model_path;
    string _user_dict_path;
    string _idf_path;
    string _stop_word_path;
    cppjieba::Jieba _jieba;
};

#endif

