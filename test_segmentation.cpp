/****************************************
*             句子分词程序             *
*                                       *
*   2017年课程设计  by software20150117 *
****************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "word_segmentation.h"

void Menu() //显示菜单(包含作者信息)
{
    std::cout<<"\n词语词典已加载成功！\n\n\n\n"
        "             /****************************************"
        "             *             句子分词程序              *\n"
        "             *                                       *\n"
        "             *   2017年课程设计       by软件15.1何琦 *\n"
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
    dict->BuildHead();                         //建立表头
    bool ret = dict->BuildTree("dict_gb2312"); //构建链表
    Menu();

    if (false == ret)
    {
        printf("%s\n", "字典不存在，请关闭程序添加字典到本目录");
        //usleep(3 * 1000000); //延时3秒
    }

    if (NULL == argv[1]) //待处理的句子文件
    {
        printf("%s\n", "请输入要处理的句子文件");
        // _sleep(3 * 1000); //延时3秒
        //usleep(3 * 1000000); //延时3秒
        return false;
    }
    std::fstream in_file(argv[1]);
    std::fstream out_file("translate_out", std::ios::out);

    //输入要分词的句子
    while (!in_file.eof())
    {
        std::string del_str;
        getline(in_file, del_str);
        printf("string: %s\n", del_str.c_str());
        printf("result: %s\n\n", dict->Segdict(del_str, WordSegmentation::MARRY_MODE_MIN).c_str());
        out_file << dict->Segdict(del_str, WordSegmentation::MARRY_MODE_MIN).c_str() << std::endl;
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