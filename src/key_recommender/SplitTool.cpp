#include "../../inc/key_commander/SplitTool.hh"

using std::vector;
using std::string;

SplitTool::SplitTool(Configuration &cpp_jieba_conf)
: _dict_path(cpp_jieba_conf.GetConfig().at("jieba.dict.utf8"))
, _model_path(cpp_jieba_conf.GetConfig().at("hmm_model.utf8"))
, _user_dict_path(cpp_jieba_conf.GetConfig().at("user.dict.utf8"))
, _idf_path(cpp_jieba_conf.GetConfig().at("idf.utf8"))
, _stop_word_path(cpp_jieba_conf.GetConfig().at("stop_words.utf8"))
, _jieba(_dict_path, _model_path, _user_dict_path, _idf_path, _stop_word_path)
{

}

vector<string> SplitTool::Cut(const string &sentence)
{
    vector<string> fragment; //存储切割之后的片段
    _jieba.CutForSearch(sentence, fragment);

    return fragment;
}