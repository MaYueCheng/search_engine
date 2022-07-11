#include "../../inc/key_commander/Configuration.hh"

using std::cerr;
using std::endl;
using std::ifstream;
using std::istringstream;

Configuration::Configuration(const string &file_path)
: _file_path(file_path)
{
    
}

config_map_t &Configuration::GetConfig()
{
    ifstream ifs(_file_path, std::ios::in); //打开配置文件
    if(ifs.is_open())
    {
        string line;
        while (getline(ifs, line))
        {
            string file_name, file_dir;
            istringstream iss(line);
            while (iss >> file_name >> file_dir)
            {
                _config_map.insert({file_name, file_dir});
            }
        }

        return _config_map;
    }
    else
    {
        cerr << "configuration file open error!" << endl;
        exit(0);
    }
}

stop_word_list_t &Configuration::GetStopWordList()
{
    ifstream ifs(_file_path, std::ios::in);
    if(ifs.is_open())
    {
        string line;
        while (getline(ifs, line))
        {
            string stop_word;
            istringstream iss(line);
            while (iss >> stop_word)
            {
                _stop_word_list.insert(stop_word);
            }
        }

        return _stop_word_list;
    }
    else
    {
        cerr << "stop_word file open error!" << endl;
        exit(0);
    }
}