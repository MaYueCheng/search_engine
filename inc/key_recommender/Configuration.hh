#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include "../cpp.hh"

using std::string;
using std::unordered_map;
using std::unordered_set;

using config_map_t = unordered_map<string, string>;
using stop_word_list_t = unordered_set<string>;

class Configuration
{
public:
    Configuration(const string &file_path);
    ~Configuration() = default;

    config_map_t &GetConfig();

    stop_word_list_t &GetStopWordList();

private:
    string _file_path;                //配置文件的路径
    config_map_t _config_map;         //配置文件的内容（文件名，文件内容）
    stop_word_list_t _stop_word_list; //停止词
};

#endif