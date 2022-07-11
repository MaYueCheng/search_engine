#include "../../inc/key_commander/KeyRecommander.hh"

using std::cout;
using std::endl;

KeyRecommander::KeyRecommander(const string &dict_path, const string &dict_index_path)
: _dictionary(dict_path, dict_index_path)
{
    
}

vector<string> KeyRecommander::Query(const string &query_word)
{
    /* 1.先在redis里进行查询，如果associated_word里的frequency == -1，则证明是redis里的结果（直接可以输出）
       2.将客户端输入的query_word经过第一次处理，得到和query_word相关的词（如果query_word为中文，则associated_word
         为词典中有和query_word有相同字的词；如果query_word为英文，则associated_word为词典中有和query_word有相同字母
         的单词）*/
    dict_t associated_words = _dictionary.Query(query_word);
    //如果associated_word的frequency != 1，证明是查询的_dict和_index 
    if(associated_words.at(0).second != -1)
    {
        int edit_distance; //query_word和associated_word之间的最小编辑距离
        for(const auto &associated_word : associated_words)
        {
            //计算两个词之间的最小编辑距离
            edit_distance = _dictionary.EditDistance(query_word, associated_word.first);
            //构造一个结果并把结果放入priority_queue
            CandidateResult result({associated_word.first, associated_word.second, edit_distance});
            _result_priority.push(result);
        }
        //输出结果（只输出五个推荐关键字）
        vector<string> recommand_keywords;
        for(int i = 0; i < 5; ++i)
        {
            recommand_keywords.push_back(_result_priority.top().word);
            _result_priority.pop();
        }
        return recommand_keywords;
    }
    //如果associate_word的frequency == -1，证明是从redis中取出的结果（可以直接输出）
    else
    {
        vector<string> recommand_keywords;
        for(const auto &associated_word : associated_words)
        {
            recommand_keywords.push_back(associated_word.first);
        }
        return recommand_keywords;
    }
}