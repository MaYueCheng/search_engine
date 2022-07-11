#include "../../inc/key_commander/EnglishPre.hh"
#include "../../inc/key_commander/Configuration.hh"

using std::string;
using std::ifstream;
using std::istringstream;
using std::ofstream;
using std::endl;

EnglishPre::EnglishPre(const string &corpus_path, const string &stop_word_fig)
{
    //从语料配置文件中读取出各个语料文件
    Configuration corpus(corpus_path);
    for(const auto &pair: corpus.GetConfig())
    {
        _files.insert(pair.second);
    }

    //从停止词配置文件读取出停止词文件
    Configuration stop_word(stop_word_fig);
    for(const auto &word: stop_word.GetStopWordList())
    {
        _stop_word.insert(word);
    }

    BuildDictionary();
    BuildIndex();
    Store();
}

void EnglishPre::BuildDictionary()
{
    //遍历每个语料文件
    for (const auto &corpus_path : _files)
    {
        ifstream ifs(corpus_path); //打开该语料文件
        string line, word;
        while (ifs >> line)
        {
            //如果是非字母则变成空格，如果是大写字母变成小写字母
            for (auto &ch : line)
            {
                ch = tolower(ch);
                if (isdigit(ch) || ispunct(ch))
                {
                    ch = ' ';
                }
            }

            istringstream iss(line);
            while (iss >> word)
            {
                PushDict(word);
            }
        }
    }
}

void EnglishPre::PushDict(const string &word)
{
    //如果该单词是停止词，则跳过
    if (_stop_word.count(word))
    {
        return;
    }
    else
    {
        //有该word则频率+1，没有则把新word插入并把频率初始化为1
        _dict[word] += 1;
    }
}

void EnglishPre::BuildIndex()
{
    set<int> a_set, b_set, c_set, d_set, e_set, f_set, g_set, h_set, i_set,
        j_set, k_set, l_set, m_set, n_set, o_set, p_set, q_set, r_set,
        s_set, t_set, u_set, v_set, w_set, x_set, y_set, z_set;

    int line_num = 1;
    //遍历_dict中的每一个单词
    for(const auto &line: _dict)
    {
        for(const auto &ch: line.first)
        {
            switch(ch)
            {
                case 'a': a_set.insert(line_num); break;
                case 'b': b_set.insert(line_num); break;
                case 'c': c_set.insert(line_num); break;
                case 'd': d_set.insert(line_num); break;
                case 'e': e_set.insert(line_num); break;
                case 'f': f_set.insert(line_num); break;
                case 'g': g_set.insert(line_num); break;
                case 'h': h_set.insert(line_num); break;
                case 'i': i_set.insert(line_num); break;
                case 'j': j_set.insert(line_num); break;
                case 'k': k_set.insert(line_num); break;
                case 'l': l_set.insert(line_num); break;
                case 'm': m_set.insert(line_num); break;
                case 'n': n_set.insert(line_num); break;
                case 'o': o_set.insert(line_num); break;
                case 'p': p_set.insert(line_num); break;
                case 'q': q_set.insert(line_num); break;
                case 'r': r_set.insert(line_num); break;
                case 's': s_set.insert(line_num); break;
                case 't': t_set.insert(line_num); break;
                case 'u': u_set.insert(line_num); break;
                case 'v': v_set.insert(line_num); break;
                case 'w': w_set.insert(line_num); break;
                case 'x': x_set.insert(line_num); break;
                case 'y': y_set.insert(line_num); break;
                case 'z': z_set.insert(line_num); break;
            }
        }
        ++line_num;
    }
    _index.insert({"a", a_set});
    _index.insert({"b", b_set});
    _index.insert({"c", c_set});
    _index.insert({"d", d_set});
    _index.insert({"e", e_set});
    _index.insert({"f", f_set});
    _index.insert({"g", g_set});
    _index.insert({"h", h_set});
    _index.insert({"i", i_set});
    _index.insert({"j", j_set});
    _index.insert({"k", k_set});
    _index.insert({"l", l_set});
    _index.insert({"m", m_set});
    _index.insert({"n", n_set});
    _index.insert({"o", o_set});
    _index.insert({"p", p_set});
    _index.insert({"q", q_set});
    _index.insert({"r", r_set});
    _index.insert({"s", s_set});
    _index.insert({"t", t_set});
    _index.insert({"u", u_set});
    _index.insert({"v", v_set});
    _index.insert({"w", w_set});
    _index.insert({"x", x_set});
    _index.insert({"y", y_set});
    _index.insert({"z", z_set});
}

void EnglishPre::Store()
{
    /***************************存储字典****************************/
    const string dict("../data/dict.dat");
    //打开文件（如果不存在则创建，存在则截断）
    ifstream ifs(dict, std::ios::in);
    string line;
    int max_line_num = 0;
    while(getline(ifs, line))
    {
        ++max_line_num;
    }
    ifs.close();
    ofstream ofs_dict(dict, std::ios::app);
    if(ofs_dict.is_open())
    {
        //把_dict中的所有内容写到english_dict.dat中
        for (const auto &line : _dict)
        {
            ofs_dict << line.first << " " << line.second << endl;
        }
        ofs_dict.close();
    }
    else
    {
        std::cerr << "ofstream" << std::endl;
        exit(0);
    }

    /***************************存储索引****************************/
    const string index("../data/dict_index.dat");
    //打开文件（如果不存在则创建，存在则截断）
    ofstream ofs_index(index, std::ios::app);
    if(ofs_index.is_open())
    {
        //把_dict中的所有内容写到english_dict.dat中
        for (const auto &line : _index)
        {
            ofs_index << line.first << " ";
            for (const auto &line_num : line.second)
            {
                ofs_index << line_num + max_line_num << " ";
            }
            ofs_index << endl;
        }
        ofs_index.close();
    }
    else
    {
        std::cerr << "ofstream" << endl;
        exit(0);
    }
    
}