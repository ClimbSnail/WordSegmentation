#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "word_segmentation.hpp"

void Menu() //锟斤拷示锟剿碉拷(锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷息)
{
    printf("\n%s\n\n\n\n", "锟斤拷锟斤拷实锟斤拷鸭锟斤拷爻晒锟斤拷锟?");
}

int main(int argc, char **argv) //锟斤拷锟斤拷锟斤拷
{
    if (argc != 2)
    {
        printf("Please input file.\n");
        return true;
    }

    WordSegmentation::Dictionary *dict = new WordSegmentation::Dictionary();
    dict->BuildHead();                            //锟斤拷锟斤拷锟斤拷头
    Menu();                                       //锟斤拷示锟剿碉拷
    bool ret = dict->BuildTree("dict_gb2312"); //锟斤拷锟斤拷锟斤拷锟斤拷
    if (false == ret)
    {
        printf("%s\n", "锟街典不锟斤拷锟节ｏ拷锟斤拷乇粘锟斤拷锟斤拷锟斤拷锟斤拷值涞斤拷锟侥柯?");
        usleep(3 * 1000000); //锟斤拷时3锟斤拷
    }

    if (NULL == argv[1]) //锟斤拷锟斤拷锟斤拷锟侥撅拷锟斤拷锟侥硷拷
    {
        printf("%s\n", "锟斤拷锟斤拷锟斤拷要锟斤拷锟斤拷锟侥撅拷锟斤拷锟侥硷拷");
        // _sleep(3 * 1000); //锟斤拷时3锟斤拷
        usleep(3 * 1000000); //锟斤拷时3锟斤拷
        return false;
    }
    std::fstream in_file(argv[1]);
    std::fstream out_file("translate_out", std::ios::out);

    //锟斤拷锟斤拷要锟街词的撅拷锟斤拷
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
    
    return 0; //锟斤拷锟斤拷锟斤拷锟?
}
