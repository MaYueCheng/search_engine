#include "../../inc/key_commander/Dictionary.hh"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::set;
using std::ostringstream;

Dictionary::Dictionary(const string &dict_path, const string &index_path)
: _redis()
{
    LoadDict(dict_path);
    LoadIndex(index_path);
}

dict_t Dictionary::Query(const string &query_word)
{
    //先在Redis中找之间查找过的关键词对应的推荐关键词
    vector<string> recomm_keywords = _redis.Pop(query_word);
    //如果推荐关键词集合为空的话，代表之前该关键词没有查过（需要在索引和字典中查）
    if(recomm_keywords.empty() == true)
    {
        //如果查询的关键字是中文
        if(IsChinese(query_word))
        {
            //把query_word的每个汉字ch_chara取出来
            set<string> chinese_chs;
            for(auto iter = query_word.begin(); iter != query_word.end(); iter += 3)
            {
                if(iter == query_word.end() || (iter + 1) == query_word.end() || (iter + 2) == query_word.end())
                {
                    break;
                }
                else
                {
                    ostringstream oss;
                    oss << *(iter) << *(iter + 1) << *(iter + 2); // oss中存的就是一个chinese_ch(汉字)
                    string chinese_ch(oss.str());
                    //把query_word拆成的几个chinese_ch并放入chinese_chs中
                    chinese_chs.insert(chinese_ch);
                }
            }
            //把提取出来的一个个汉字去_index中查，提取出来_dict的行号
            set<int> line_nums;
            for(const auto &chinese_ch : chinese_chs)
            {
                //如果字典索引中有这个字
                if(_index.count(chinese_ch) != 0)
                {
                    //如果字典索引中有这个字，则把字典中包含这个字的词所在词典的行号输出
                    for(const auto &line_num : _index.at(chinese_ch))
                    {
                        line_nums.insert(line_num);
                    }
                }
                else
                {
                    cerr << "dictionary not included: " << chinese_ch << endl;
                    continue;
                }
            }
            //得到行号后，把所有跟query_word有一样的字的词(word)和其频率(frequency)提取出来，
            //交给KeyRecommander进一步计算（计算最小编辑距离）
            dict_t result;
            for(const auto &line_num : line_nums)
            {
                result.push_back(_dict.at(line_num - 1)); //因为dict.dat文件行号从1开始，加载到vector中是从0开始，所以需要-1
            }
            return result;
        }
        //如果查询的关键字是英文
        else
        {   
            //把query_word的每个字母(letter)提取出来，只有有相同字母的单词才去计算编辑距离
            set<string> letters;
            for(const auto &letter : query_word)
            {
                string temp(1, letter);
                letters.insert(temp);
            }
            //把提取出来的字母去_index中查，提取出来_dict中的行号
            set<int> line_nums;
            for(const auto &letter : letters)
            {
                for (const auto &line_num : _index.at(letter))
                {
                    line_nums.insert(line_num);
                }
            }
            //得到行号后，把所有相关的word和frequency取出来交给KeyRecommander去进一步计算（计算最小编辑距离）
            dict_t result;
            for(const auto &line_num : line_nums)
            {
                result.push_back(_dict.at(line_num - 1)); //因为dict.dat文件行号从1开始，加载到vector中是从0开始，所以需要-1
            }
            return result;
        }
    }
    //如果在Redis中查到了有该搜索关键字对应的推荐关键字，则把现成的直接返回
    else
    {
        dict_t result;
        for(const auto &recomm_keyword : recomm_keywords)
        {
            result.push_back({recomm_keyword, -1}); //频率-1代表是从Redis中取出来的数据
        }
        return result;
    }
}

void Dictionary::LoadDict(const string &dict_path)
{
    ifstream ifs(dict_path, std::ios::in);
    if(ifs.is_open())
    {
        //line为_dict中的一行数据
        string line;
        while(getline(ifs, line))
        {
            string word, str_frequency; // word为_dict的单词，str_frequency为单词的频率（字符串版）
            int frequency;          // frequency为单词的频率（数字版）
            istringstream iss(line);
            while(iss >> word >> str_frequency)
            {
                //将字符串的frequency（str_frequency）转化为frequency（数字版）
                frequency = std::stoi(str_frequency);
                //将单词和频率插入到_dict中
                _dict.push_back({word, frequency});
            }
        }
    }
    else
    {
        cerr << "error path: " << dict_path << endl;
        exit(0);
    }
}

void Dictionary::LoadIndex(const string &index_path)
{
    ifstream ifs(index_path, std::ios::in);
    if (ifs.is_open())
    {
        //line为_index中的一行数据
        string line;
        while (getline(ifs, line))
        {
            string word, str_line_num; // word为_index的单词，str_line_num为单词的行号（字符串版）
            int line_num;              // line_num为单词的行号（数字版）
            istringstream iss(line);
            while (iss >> word)
            {
                while(iss >> str_line_num)
                {
                    //将字符串的line_num(str_line_num)转化为line_num（数字版）
                    line_num = std::stoi(str_line_num);
                    //_index中没有该word就插入一个该word，否则在word对应的line_num中加入一个新的line_num
                    _index[word].insert(line_num);
                }
            }
            ++line_num;
        }
    }
    else
    {
        cerr << "error path: " << index_path << endl;
        exit(0);
    }
}

int Dictionary::EditDistance(const string &lhs, const string &rhs)
{
    //计算最小编辑距离-包括处理中英文
    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];
    for (size_t idx = 0; idx <= lhs_len; ++idx)
    {
        editDist[idx][0] = idx;
    }

    for (size_t idx = 0; idx <= rhs_len; ++idx)
    {
        editDist[0][idx] = idx;
    }

    std::string sublhs, subrhs;
    for (std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx)
    {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);

        for (std::size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx)
        {
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if (sublhs == subrhs)
            {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
            }
            else
            {
                editDist[dist_i][dist_j] = triple_min(
                    editDist[dist_i][dist_j - 1] + 1,
                    editDist[dist_i - 1][dist_j] + 1,
                    editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }

    return editDist[lhs_len][rhs_len];
}

size_t Dictionary::nBytesCode(const char ch)
{
	if(ch & (1 << 7))
	{
		int nBytes = 1;
		for(int idx = 0; idx != 6; ++idx)
		{
			if(ch & (1 << (6 - idx)))
			{
				++nBytes;	
			}
			else
				break;
		}
		return nBytes;
	}
	return 1;
}

size_t Dictionary::length(const std::string &str)
{
	std::size_t ilen = 0;
	for(std::size_t idx = 0; idx != str.size(); ++idx)
	{
		int nBytes = nBytesCode(str[idx]);
		idx += (nBytes - 1);
		++ilen;
	}
	return ilen;
}

int Dictionary::triple_min(const int &a, const int &b, const int &c)
{
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

void Dictionary::ShowDict()
{
    for(const auto &line: _dict)
    {
        cout << line.first << " " << line.second << endl; 
    }
    cout << endl;
}

void Dictionary::ShowIndex()
{
    for(const auto &line: _index)
    {
        cerr << line.first << " ";
        for(const auto &numbers: line.second)
        {
            cerr << numbers << " ";
        }
        cout << endl; 
    }
    cout << endl;
}

bool Dictionary::IsChinese(const string &keyword)
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