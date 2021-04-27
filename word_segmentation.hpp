/****************************************
*             ���ӷִʳ���             *
*                                       *
*   2017��γ����  by software20150117 *
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

struct WordNode //�ֽڵ�Ľṹ����
{
    std::string str;    //���溺��(GB2312)
    bool IsWord;        //�Ƿ�ɴ�
    WordNode *brother;  //�ֵ���
    WordNode *children; //������
};

class Dictionary
{
private:
    std::vector<WordSegmentation::WordNode *> table_head; //����Head���͵���������
    WordSegmentation::WordNode *p_search;
    std::fstream *p_ifstream;

public:
    Dictionary() : p_search(NULL), p_ifstream(NULL)
    {
        BuildHead();
    }
    ~Dictionary();

    bool BuildHead(void); //������ͷ�ڵ�

    bool BuildTree(char *file_path = NULL); //��ȡ�ļ�������

    //���뵥�д���
    void SingleInsert(uchar high, uchar low);

    bool InsertNode(WordSegmentation::WordNode *p_new_node); //���뵥������

    std::string Segdict(std::string sentences); //�ִ�������

    //���ִַ��Ӻ���
    int MaxChineseLen(int pos, uchar high, uchar low, std::string &sentences);
};
} // namespace WordSegmentation
#endif