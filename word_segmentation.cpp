/****************************************
*             句子分词程序             *
*                                       *
*   2017年课程设计  by software20150117 *
****************************************/


#include "word_segmentation.h"

WordSegmentation::Dictionary::~Dictionary()
{
    delete p_ifstream;

    p_search = NULL;
    p_ifstream = NULL;
}

bool WordSegmentation::Dictionary::BuildHead(void) //建立表头节点
{
    //构造表头
    for (uchar row = 0xB0; row <= 0xF7; ++row) //4*16+(1+7)=72 一列72个
    {
        for (uchar line = 0xA1; line <= 0xFE; ++line) //5*16+14=94 一行94个
        {
            WordSegmentation::WordNode *head = new WordSegmentation::WordNode;
            head->children = NULL;
            head->brother = NULL;
            // 单个节点默认就是一个词
            head->IsWord = true;
            head->str.push_back(row);
            head->str.push_back(line);
            // 将汉字放入表头
            table_head.push_back(head);
        }
    }
    return true;
}

bool WordSegmentation::Dictionary::BuildTree(char *file_path) //提取文件构建树
{
    //若字典文件不存在
    if (NULL == file_path)
    {
        return false;
    }
    p_ifstream = new std::fstream(file_path);
    std::string temp_str; //用来读取行字符串z
    uchar high, low;
    int num;
    while (!p_ifstream->eof()) //读取文件
    {
        getline(*p_ifstream, temp_str); //读取词头
        high = temp_str[0];             //获得国标码高8位
        low = temp_str[1];              //获得国标码低8位
        getline(*p_ifstream, temp_str); //读取本词头下的词数量
        num = atoi(temp_str.c_str());
        for (int cnt = 0; cnt < num; ++cnt)
        {
            SingleInsert(high, low); //把这行插入
        }
    }
    return true;
}

//插入单行词语
void WordSegmentation::Dictionary::SingleInsert(uchar high, uchar low)
{
    std::string temp_str;
    getline(*p_ifstream, temp_str); //读取行字符串
    //每行开始插入时，位置记录指针指向对应表头的第一个body
    int temp_str_len = temp_str.length();
    p_search = table_head[(high - 0xB0) * 94 + (low - 0xA1)];
    for (uint j = 2; j < temp_str_len; j += 2)
    {
        //构造一个body节点
        WordSegmentation::WordNode *p_new_node = new WordSegmentation::WordNode;
        p_new_node->brother = NULL;
        p_new_node->children = NULL;
        p_new_node->str = temp_str.substr(j, 2);
        if (j == temp_str_len - 2)
            p_new_node->IsWord = true;
        else
            p_new_node->IsWord = false;

        InsertNode(p_new_node); //插入这个body节点
    }
}

bool WordSegmentation::Dictionary::InsertNode(WordSegmentation::WordNode *p_new_node) //插入单个词语
{
    if (NULL == p_new_node)
    {
        return false;
    }

    if (NULL == p_search->children) // 没有子节点时，添加孩子链
    {
        p_search->children = p_new_node;
        p_search = p_search->children;
    }
    else if (!p_search->children->str.compare(p_new_node->str))
    {
        // 孩子链已存在，更新下是否构成词
        if (p_new_node->IsWord)
        {
            p_search->children->IsWord = true;
        }
        p_search = p_search->children;
    }
    else
    {
        // 在兄弟链上 搜索是否存在值相同的节点
        for (p_search = p_search->children;
             NULL != p_search->brother && p_search->brother->str.compare(p_new_node->str);
             p_search = p_search->brother)
            ;
        // 没找到元素相同的兄弟 则插入节点
        if (NULL == p_search->brother)
        {
            p_search->brother = p_new_node;
        }
        else if (p_new_node->IsWord)
        {
            // 已找到 并且本词构成词，更新下状态
            p_search->brother->IsWord = p_new_node->IsWord;
        }

        p_search = p_search->brother;
    }
    return true;
}

std::string WordSegmentation::Dictionary::Segdict(std::string sentences, bool marry_mode) //分词主程序
{
    std::string temp_str; //用来临时存放每个汉字
    std::string result;   // 存放分词后的结果
    uchar high = 0;
    uchar low = 0;                                //获得国标码
    for (uint pos = 0; pos < sentences.length();) //循环检测字符
    {
        temp_str = sentences.substr(pos, 2); //提取两个字节(即一个汉字)
        high = (uchar)temp_str[0];
        if (high >= 0xB0)
        {
            low = (uchar)temp_str[1];
            int maxlen = GetChineseLen(pos, high, low, sentences, marry_mode);
            result = result + sentences.substr(pos, maxlen) + "/"; //中文处理函数
            pos = pos + maxlen;
        }    //汉字处理
        else //单字节的汉字处理
        {
            result = result + sentences.substr(pos, 1); //这里输出的只是数字
            pos += 1;
        }
    }
    return result;
}

//汉字分词子函数
int WordSegmentation::Dictionary::GetChineseLen(int pos, uchar high, uchar low, std::string &sentences, bool marry_mode)
{
    std::string temp_str;
    int maxlen = 2;
    int index = 0;                                                                                // 存放分词后的结果                                                                //用来提取每个汉字
    WordSegmentation::WordNode *p_body = table_head[(high - 0xB0) * 94 + (low - 0xA1)]->children; //获得对应表头的孩子指针
    for (index = pos + 2; index < sentences.length(); index += 2)
    {
        temp_str = sentences.substr(index, 2); //提取两个字节(即一个汉字)
        if ((uchar)temp_str[0] < 0xB0)         //如果于单字符和数字就跳出去分词主函数去处理
        {
            maxlen = index - pos + 2;
            break;
        }

        // 跟当前节点相等
        if (!p_body->str.compare(temp_str))
        {
            if (p_body->IsWord)
            {
                maxlen = index - pos + 2;
                if (marry_mode == MARRY_MODE_MIN)
                {
                    //如果是最早匹配 匹配到就可以结束循环
                    break;
                }
            }
            if (NULL != p_body->children) //如果孩子不为空
            {
                p_body = p_body->children; //则从孩子开始找
            }
            else
                break;
        }
        else
        {
            for (p_body = p_body->brother;
                 p_body && temp_str.compare(p_body->str); // && p_body->brother;
                 p_body = p_body->brother)
                ;
            if (NULL == p_body)
            {
                break;
            }
            else if (!temp_str.compare(p_body->str)) //如果循环结束条件是文字相同
            {
                if (p_body->IsWord)
                {
                    maxlen = index - pos + 2; //构成词则更新本次成功匹配的字数
                    if (marry_mode == MARRY_MODE_MIN)
                    {
                        //如果是最早匹配 匹配到就可以结束循环
                        break;
                    }
                }
                if (NULL != p_body->children) //如果孩子不为空
                {
                    p_body = p_body->children; //则从孩子开始找
                }
                else
                    break;
            }
        }
    }
    return maxlen;
}