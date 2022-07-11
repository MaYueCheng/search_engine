#ifndef KEY_RECOMMANDER_HH
#define KEY_RECOMMANDER_HH

#include "../cpp.hh"
#include "Dictionary.hh"
#include "CandidateResult.hh"
#include <nlohmann/json.hpp>
#include "../message.hh"

using json = nlohmann::json;
using std::unique_ptr;
using std::string;
using std::priority_queue;
using std::vector;

class Greater
{
public:
    bool operator()(CandidateResult &lhs, CandidateResult &rhs)
    {
        if(lhs.distance > rhs.distance)
        {
            return true;
        }
        else if (lhs.distance < rhs.distance)
        {
            return false;
        }
        //如果候选词的编辑距离相同，比较他们出现的频率
        else
        {
            if(lhs.frequency > rhs.frequency)
            {
                return true;
            }
            else if(lhs.frequency < rhs.frequency)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        
    }
};

class KeyRecommander
{
public:
    KeyRecommander(const string &dict_path, const string &dict_index_path);
    ~KeyRecommander() = default;

    vector<string> Query(const string &query_word);
private:
    string _query_word;
    priority_queue<CandidateResult, vector<CandidateResult>, Greater> _result_priority;
    Dictionary _dictionary;
};

#endif