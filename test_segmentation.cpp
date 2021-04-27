/****************************************
*             ���ӷִʳ���             *
*                                       *
*   2017��γ����  by software20150117 *
****************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "word_segmentation.hpp"

void Menu() //��ʾ�˵�(����������Ϣ)
{
    std::cout<<"\n����ʵ��Ѽ��سɹ���\n\n\n\n"
        "             /****************************************"
        "             *             ���ӷִʳ���              *\n"
        "             *                                       *\n"
        "             *   2017��γ����       by���15.1���� *\n"
        "             ****************************************/\n\n";
}

int Run(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Please input file.\n");
        return true;
    }

    WordSegmentation::Dictionary *dict = new WordSegmentation::Dictionary();
    dict->BuildHead();                         //������ͷ
    bool ret = dict->BuildTree("dict_gb2312"); //��������
    Menu();

    if (false == ret)
    {
        printf("%s\n", "�ֵ䲻���ڣ���رճ�������ֵ䵽��Ŀ¼");
        usleep(3 * 1000000); //��ʱ3��
    }

    if (NULL == argv[1]) //������ľ����ļ�
    {
        printf("%s\n", "������Ҫ����ľ����ļ�");
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
    in_file.close();
    out_file.close();
    delete dict;
    dict = NULL;

    return true;
}

int main(int argc, char **argv)
{
    Run(argc, argv);
    return 0;
}