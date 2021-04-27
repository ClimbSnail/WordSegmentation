/****************************************
*             句子分词程序             *
*                                       *
*   2017年课程设计  by software20150117 *
****************************************/


#ifndef WORD_SEGMENTATION_HPP
#define WORD_SEGMENTATION_HPP

#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>

namespace WordSegmentation
{
#define uchar unsigned char
#define uint unsigned int

struct WordNode //字节点的结构定义
{
    std::string str;    //储存汉字(GB2312)
    bool IsWord;        //是否成词
    WordNode *brother;  //兄弟链
    WordNode *children; //孩子链
};

class Dictionary
{
private:
    std::vector<WordSegmentation::WordNode *> table_head; //声明Head类型的容器对象
    WordSegmentation::WordNode *p_search;
    std::fstream *p_ifstream;

public:
    Dictionary() : p_search(NULL), p_ifstream(NULL)
    {
        BuildHead();
    }
    ~Dictionary();

    bool BuildHead(void); //建立表头节点

    bool BuildTree(char *file_path = NULL); //提取文件构建树

    //插入单行词语
    void SingleInsert(uchar high, uchar low);

    bool InsertNode(WordSegmentation::WordNode *p_new_node); //插入单个词语

    std::string Segdict(std::string sentences); //分词主程序

    //汉字分词子函数
    int MaxChineseLen(int pos, uchar high, uchar low, std::string &sentences);
};
} // namespace WordSegmentation
#endif