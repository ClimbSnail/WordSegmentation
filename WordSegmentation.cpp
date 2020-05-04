/****************************************
*             句子分词程序             *
*                                       *
*   2017年课程设计  by software20150117 *
****************************************/
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#define  uchar unsigned char
#define  uint unsigned int
using namespace std;

fstream file("segdict.txt");
struct Body	//子叶节点的结构定义
{
    string str;		//储存汉字
    bool IsWord;	//是否成词
    Body *brother;	//兄弟链
    Body *children;	//孩子链
};
struct Head	//头节点结构定义
{
    string str;		//储存汉字
    Body *next;		//子链
    bool IsWord;	//是否成词
};
vector<Head> H;//声明Head类型的容器对象
int len=0;//计算表头的数量
Body *p;//全局节点搜索指针
Body B[400000];//储存读取的单个汉子 避免地址重复
int B_count = 0 ;//计数B数组的储存数量
bool turn_children;//判断搜孩子还是搜兄弟的标志位

void Build_head()   //建立表头节点
{
    //构造表头
    for(uchar c1=0xB0; c1<=0xF7; ++c1)  //4*16+(1+7)=72 一排72个
    {
        for(uchar c2=0xA1; c2<=0xFE; ++c2) //5*16+14=94 一行94个
        {
            Head h;
            h.next = NULL;
            h.IsWord = true;
            h.str.push_back(c1);
            h.str.push_back(c2);
            H.push_back(h);
            len++;
        }
    }
}

void Insert_node(uchar c1, uchar c2) //插入单个词语
{
    if( H[(c1-0xB0)*94+(c2-0xA1)].next == NULL )    //如果表头节点下没有孩子
    {
        H[(c1-0xB0)*94+(c2-0xA1)].next = &B[B_count];   //该表头链接第一个body
        if( B[B_count].IsWord )     //如果这节点构成词，说明一句话的最后一个节点已构建(为叶子节点)
            turn_children = 0;  //让第二句话链接时 往它的兄弟链添加，当然下句话节点不是同一个时不会搜它兄弟链
        else{
            p = &B[B_count];    //位置指针移到此节点
            turn_children = 1;  //  标志的下回搜索它的子链
        }
    }
    else
    {
        if( !turn_children )    //搜索兄弟链
        {
            for(  ; p->brother && p->str.compare(B[B_count].str) ; p = p->brother );
            if(!p->str.compare(B[B_count].str))//有相同的
            {
                turn_children = 1;
                if( B[B_count].IsWord ) //如果这时候接入的节点是该句最后一个字(能构成词)
                    p->IsWord = B[B_count].IsWord;//把原先存在的这个节点IsWord标志为true
            }
            else    //没有找到相同的节点
            {
                p->brother = &B[B_count];//添加到兄弟链尾部
                p = p->brother;//位置指针移到此节点
                turn_children = 1;//下一轮搜索孩子链
                if( B[B_count].IsWord )//标志着下一个词开始添加
                    turn_children = 0;//在表头不为空时要优先搜索兄弟链
            }
        }
        else    // 搜索子链 p指针还停留在上个点
        {
            if( p->children == NULL )   //如果为空就添加这节点
            {
                if( B[B_count].IsWord ) //如果这是最后一个字
                    turn_children = 0;  //下个词首字优先搜索兄弟链
                p->children = &B[B_count]; //添加节点
                p = p->children;
            }
            else    //节点不为空，就先移动指针到孩子，以便搜索孩子的兄弟链
            {
                for( p=p->children ; p->brother && p->str.compare(B[B_count].str) ; p = p->brother );
                if(!p->str.compare(B[B_count].str))//找到相同节点
                {
                    if( p->children )   //有孩子就搜索子链
                        turn_children = 1;
                    if( B[B_count].IsWord )//如果这节点能构成词
                    {
                        p->IsWord = true;
                        turn_children = 0;//下个词第一个字优先搜索表头节点兄弟链
                    }
                }
                else    //找不到相同的就把节点添加到兄弟链最后一个
                {
                    p->brother = &B[B_count];//添加
                    p = p->brother;//指针移动到此节点
                    if( B[B_count].IsWord )
                        turn_children = 0;//下个词第一个字优先搜索表头节点兄弟链
                    else
                        turn_children = 1;//往孩子链搜索
                }
            }
        }
    }
    B_count++;
}

void Single_insert(uchar c1,uchar c2) //插入单行词语
{
    string temp_str;
    getline(file,temp_str); //读取行字符串
    p = H[(c1-0xB0)*94+(c2-0xA1)].next;   //每行开始插入时，位置记录指针指向对应表头的第一个body
    for(uint j=2; j<temp_str.length(); j+=2)
    {   //构造一个body节点
        B[B_count].brother = NULL;
        B[B_count].children = NULL;
        B[B_count].str = temp_str.substr(j,2);
        if( j == temp_str.length()-2 )
            B[B_count].IsWord = true;
        else    B[B_count].IsWord = false;
        Insert_node(c1,c2); //插入这个body节点
    }
}

void Build_tree()   //提取文件构建树
{
    string temp_str;//用来读取行字符串
    uchar c1,c2;
    int i;
    int num;
    while(!file.eof())  //读取文件
    {
        getline(file,temp_str);
        c1 = temp_str[0];       //获得国标码高两位
        c2 = temp_str[1];          //获得国标码低两位
        getline(file,temp_str); //读取行字符串
        num = atoi(temp_str.c_str());
        for(i=0; i<num; i++)
        {
            Single_insert(c1,c2);   //把这行插入
        }
    }
}
//汉字分词子函数
void Is_Chinese(uint &j,uint &pos,uchar c1,uchar c2,string str,uint china_num,uint succeed_num)
{
    string temp_str;            //用来提取每个汉字
    Body *body_p = H[ (c1-0xB0)*94 + (c2-0xA1) ].next; //获得对应表头的孩子指针
    for( j+=2 ; j < str.length() ; j+=2 )
    {
        temp_str = str.substr(j,2); //提取两个字节(即一个汉字)
        c1 = temp_str[0];   //临时储存 一会做比较用
        if( c1<0xB0 )  //如果于单字符和数字就跳出去分词主函数去处理
            break;
        for( ; temp_str.compare(body_p->str) && body_p->brother ; body_p = body_p->brother );
        if( !temp_str.compare(body_p->str) )    //如果循环结束条件是文字相同
        {
            china_num ++ ;  //成功匹配的数量加一
            if( body_p->IsWord ) succeed_num = china_num;//构成词则更新本次成功匹配的字数
            if( body_p->children != NULL )  //如果孩子不为空
            {
                body_p = body_p->children;  //则从孩子开始找
            }
            else break;
        }
        else   break;
    }
    cout<<str.substr(pos,succeed_num*2)<<"/";
    pos += succeed_num*2;
    j = pos;
}
void Segdict(string str)//分词主程序
{
    string temp_str;            //用来提取每个汉字
    uint pos;              //标志每次顿号的位置
    uint china_num;     //计算已判断的连续汉字个数
    uint succeed_num;
    for(uint j = 0, pos = 0; j < str.length() ;  )  //循环检测字符
    {
        uchar c1,c2;            //获得国标码
        temp_str = str.substr(j,2); //提取两个字节(即一个汉字)
        china_num = 1;
        succeed_num = china_num;
        c1 = temp_str[0];
        c2 = temp_str[1];
        if( c1>0xAF )   //汉字处理
            Is_Chinese(j, pos, c1, c2, str, china_num, succeed_num);//中文处理函数
        else //单字节的汉字处理
        {
            cout<<str.substr(pos,1); //输入一个字节长度的数字
            pos += 1;
            j = pos;   //标志重新变为pos位置
        }
    }
}

void Enter_string() //输入要分的句子
{
    cout<<"输入你要分词的句子(end结束):"<<endl;//测试文字:2017年调试了整整三天的代码，没有什么是熬夜解决不了的事。
    while(1)
    {
        string str;
        cin>>str;
        if(!str.compare("end"))     //如果输入end结束程序
            break;
        else
        {
            cout<<endl<<"句子分词成：  ";
            Segdict(str);   //调用分词总程序
        }
        cout<<endl<<endl;
    }
}

void Menu() //显示菜单(包含作者信息)
{
    cout<<endl<<"词语词典已加载成功！"<<endl<<endl<<endl<<endl;
    cout<<"             /****************************************"<<endl<<
        "             *             句子分词程序              *"<<endl<<
        "             *                                       *"<<endl<<
        "             *   2017年课程设计       by软件15.1何琦 *"<<endl<<
        "             ****************************************/"<<endl<<endl;
}

int main()  //主函数
{
    Build_head();//建立表头

    if( file == NULL )  //若字典文件不存在
    {
        cout<<"字典不存在，请关闭程序添加字典到本目录"<<endl;
        _sleep(3*1000);//延时3秒
        return 0;
    }
    Menu();     //显示菜单
    Build_tree();   //构建链表
    Enter_string(); //输入要分词的句子
    return 0;   //程序结束
}