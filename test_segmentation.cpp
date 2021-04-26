#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "word_segmentation.hpp"

void Menu() //��ʾ�˵�(����������Ϣ)
{
    printf("\n%s\n\n\n\n", "����ʵ��Ѽ��سɹ���?");
}

int main(int argc, char **argv) //������
{
    if (argc != 2)
    {
        printf("Please input file.\n");
        return true;
    }

    WordSegmentation::Dictionary *dict = new WordSegmentation::Dictionary();
    dict->BuildHead();                            //������ͷ
    Menu();                                       //��ʾ�˵�
    bool ret = dict->BuildTree("segdict_gb2312"); //��������
    if (false == ret)
    {
        printf("%s\n", "�ֵ䲻���ڣ���رճ��������ֵ䵽��Ŀ�?");
        usleep(3 * 1000000); //��ʱ3��
    }

    if (NULL == argv[1]) //�������ľ����ļ�
    {
        printf("%s\n", "������Ҫ�����ľ����ļ�");
        // _sleep(3 * 1000); //��ʱ3��
        usleep(3 * 1000000); //��ʱ3��
        return false;
    }
    std::fstream in_file(argv[1]);
    std::fstream out_file("translate_out", std::ios::out);

    //����Ҫ�ִʵľ���
    while (!in_file.eof())
    {
        std::string del_str;
        getline(in_file, del_str);
        printf("string: %s\n", del_str.c_str());
        printf("result: %s\n\n", dict->Segdict(del_str).c_str());
        out_file << dict->Segdict(del_str).c_str() << std::endl;
    }
    out_file.close();
    return 0; //�������?
}