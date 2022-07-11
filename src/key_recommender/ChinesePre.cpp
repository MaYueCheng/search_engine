#include "../../inc/key_commander/ChinesePre.hh"

using std::cerr;
using std::endl;
using std::ifstream;
using std::ostringstream;
using std::ofstream;

ChinesePre::ChinesePre(const string &corpus_path, const string &cpp_jieba_conf)
: _corpus(corpus_path)
, _cpp_jieba_conf(cpp_jieba_conf)
, _split_tool(_cpp_jieba_conf)
{
    //从语料库配置文件中读取出各个语料文件的绝对地址
    Configuration corpus(corpus_path);
    for(const auto &name_path: corpus.GetConfig())
    {
        _files.insert(name_path.second);
    }

    BuildDictionary();
    BuildIndex();
    Store();
}

void ChinesePre::BuildDictionary()
{
    for (const auto &file : _files)
    {
        //打开各个语料文件
        ifstream ifs(file, std::ios::in);
        vector<string> fragments; //存放一段话切成的每个片段
        if (ifs.is_open())
        {
            //轮询语料文件的每一行，把切割后的一行放入fragment中
            string line;
            while (getline(ifs, line))
            {
                fragments = _split_tool.Cut(line);

                //把一篇文章的分词结果放入词典_dict
                for(auto iter = fragments.begin(); iter != fragments.end();)
                {
                    if(IsChinese(*iter) == false)
                    {
                        iter = fragments.erase(iter);
                    }
                    else
                    {
                        //有该phrase则频率+1，没有则把新phrase插入并把频率初始化为1
                        _dict[*(iter)] += 1;
                        ++iter;
                    }
                }
                
                /* for (auto &phrase : fragments)
                {
                    //遍历删除非中文元素，并防止迭代器失效
                    for(auto iter = phrase.begin(); iter != phrase.end();)
                    {
                        if(*(iter) > 0 && *(iter) < 255)
                        {
                            iter = phrase.erase(iter);
                        }
                        else
                        {
                            ++iter;
                        }
                    }
                } */
            }
        }
        else
        {
            cerr << "file open error!" << endl;
        }
    }
}

void ChinesePre::BuildIndex()
{
    int line_num = 1;
    for (const auto &line : _dict)
    {
        string phrase = line.first;
        //因为UTF-8编码中，汉字占3个字节，所以每次迭代器要向后偏移3个字节
        for (auto iter = phrase.begin(); iter != phrase.end(); iter += 3)
        {
            //因为iter每次向后偏移三个，所以会跳过phrase.end()，需要在循环内再次判断
            if (iter == phrase.end() || (iter + 1) == phrase.end() || (iter + 2) == phrase.end())
            {
                break;
            }
            else
            {
                ostringstream oss;
                oss << *(iter) << *(iter + 1) << *(iter + 2); // oss中存的就是一个chinese_ch(汉字)
                string chinese_ch(oss.str());

                //如果_index中没有该chinese_ch就插入一个新的并插入行号；如果_index中有则插入一个行号
                _index[chinese_ch].insert(line_num);
            }
        }
        ++line_num;
    }
}

void ChinesePre::Store()
{
    /***************************存储字典****************************/
    const string dict("../data/dict.dat");
    //打开文件（如果不存在则创建，存在则截断）
    ofstream ofs_dict(dict, std::ios::out);
    //把_dict中的所有内容写到english_dict.dat中
    for (const auto &line : _dict)
    {
        ofs_dict << line.first << " " << line.second << endl;
    }
    ofs_dict.close();
    /***************************存储索引****************************/
    const string index("../data/dict_index.dat");
    //打开文件（如果不存在则创建，存在则截断）
    ofstream ofs_index(index, std::ios::out);
    //把_dict中的所有内容写到english_dict.dat中
    for (const auto &line : _index)
    {
        ofs_index << line.first << " ";
        for (const auto &line_num : line.second)
        {
            ofs_index << line_num << " ";
        }
        ofs_index << endl;
    }
    ofs_index.close();
}

bool ChinesePre::IsChinese(const string &keyword)
{
    for (int i = 0; i < keyword.length(); i++)
    {
        while (keyword[i] != 0)
        {
            if (keyword[i] & 0x80 && keyword[i] & 0x40 && keyword[i] & 0x20)
            {
                int byteCount = 0;
                if (keyword[i] & 0x10)
                {
                    byteCount = 4;
                    return true;
                }
                else
                {
                    byteCount = 3;
                    return true;
                }
            }
            else if (keyword[i] & 0x80 && keyword[i] & 0x40)
            {
                i += 2;
                return false;
            }
            else
            {
                i += 1;
                return false;
            }
        }
    }
    return false;
}