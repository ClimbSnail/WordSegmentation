/****************************************
*             ���ӷִʳ���             *
*                                       *
*   2017��γ����  by software20150117 *
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
struct Body	//��Ҷ�ڵ�Ľṹ����
{
    string str;		//���溺��
    bool IsWord;	//�Ƿ�ɴ�
    Body *brother;	//�ֵ���
    Body *children;	//������
};
struct Head	//ͷ�ڵ�ṹ����
{
    string str;		//���溺��
    Body *next;		//����
    bool IsWord;	//�Ƿ�ɴ�
};
vector<Head> H;//����Head���͵���������
int len=0;//�����ͷ������
Body *p;//ȫ�ֽڵ�����ָ��
Body B[400000];//�����ȡ�ĵ������� �����ַ�ظ�
int B_count = 0 ;//����B����Ĵ�������
bool turn_children;//�ж��Ѻ��ӻ������ֵܵı�־λ

void Build_head()   //������ͷ�ڵ�
{
    //�����ͷ
    for(uchar c1=0xB0; c1<=0xF7; ++c1)  //4*16+(1+7)=72 һ��72��
    {
        for(uchar c2=0xA1; c2<=0xFE; ++c2) //5*16+14=94 һ��94��
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

void Insert_node(uchar c1, uchar c2) //���뵥������
{
    if( H[(c1-0xB0)*94+(c2-0xA1)].next == NULL )    //�����ͷ�ڵ���û�к���
    {
        H[(c1-0xB0)*94+(c2-0xA1)].next = &B[B_count];   //�ñ�ͷ���ӵ�һ��body
        if( B[B_count].IsWord )     //�����ڵ㹹�ɴʣ�˵��һ�仰�����һ���ڵ��ѹ���(ΪҶ�ӽڵ�)
            turn_children = 0;  //�õڶ��仰����ʱ �������ֵ�����ӣ���Ȼ�¾仰�ڵ㲻��ͬһ��ʱ���������ֵ���
        else{
            p = &B[B_count];    //λ��ָ���Ƶ��˽ڵ�
            turn_children = 1;  //  ��־���»�������������
        }
    }
    else
    {
        if( !turn_children )    //�����ֵ���
        {
            for(  ; p->brother && p->str.compare(B[B_count].str) ; p = p->brother );
            if(!p->str.compare(B[B_count].str))//����ͬ��
            {
                turn_children = 1;
                if( B[B_count].IsWord ) //�����ʱ�����Ľڵ��Ǹþ����һ����(�ܹ��ɴ�)
                    p->IsWord = B[B_count].IsWord;//��ԭ�ȴ��ڵ�����ڵ�IsWord��־Ϊtrue
            }
            else    //û���ҵ���ͬ�Ľڵ�
            {
                p->brother = &B[B_count];//��ӵ��ֵ���β��
                p = p->brother;//λ��ָ���Ƶ��˽ڵ�
                turn_children = 1;//��һ������������
                if( B[B_count].IsWord )//��־����һ���ʿ�ʼ���
                    turn_children = 0;//�ڱ�ͷ��Ϊ��ʱҪ���������ֵ���
            }
        }
        else    // �������� pָ�뻹ͣ�����ϸ���
        {
            if( p->children == NULL )   //���Ϊ�վ������ڵ�
            {
                if( B[B_count].IsWord ) //����������һ����
                    turn_children = 0;  //�¸����������������ֵ���
                p->children = &B[B_count]; //��ӽڵ�
                p = p->children;
            }
            else    //�ڵ㲻Ϊ�գ������ƶ�ָ�뵽���ӣ��Ա��������ӵ��ֵ���
            {
                for( p=p->children ; p->brother && p->str.compare(B[B_count].str) ; p = p->brother );
                if(!p->str.compare(B[B_count].str))//�ҵ���ͬ�ڵ�
                {
                    if( p->children )   //�к��Ӿ���������
                        turn_children = 1;
                    if( B[B_count].IsWord )//�����ڵ��ܹ��ɴ�
                    {
                        p->IsWord = true;
                        turn_children = 0;//�¸��ʵ�һ��������������ͷ�ڵ��ֵ���
                    }
                }
                else    //�Ҳ�����ͬ�ľͰѽڵ���ӵ��ֵ������һ��
                {
                    p->brother = &B[B_count];//���
                    p = p->brother;//ָ���ƶ����˽ڵ�
                    if( B[B_count].IsWord )
                        turn_children = 0;//�¸��ʵ�һ��������������ͷ�ڵ��ֵ���
                    else
                        turn_children = 1;//������������
                }
            }
        }
    }
    B_count++;
}

void Single_insert(uchar c1,uchar c2) //���뵥�д���
{
    string temp_str;
    getline(file,temp_str); //��ȡ���ַ���
    p = H[(c1-0xB0)*94+(c2-0xA1)].next;   //ÿ�п�ʼ����ʱ��λ�ü�¼ָ��ָ���Ӧ��ͷ�ĵ�һ��body
    for(uint j=2; j<temp_str.length(); j+=2)
    {   //����һ��body�ڵ�
        B[B_count].brother = NULL;
        B[B_count].children = NULL;
        B[B_count].str = temp_str.substr(j,2);
        if( j == temp_str.length()-2 )
            B[B_count].IsWord = true;
        else    B[B_count].IsWord = false;
        Insert_node(c1,c2); //�������body�ڵ�
    }
}

void Build_tree()   //��ȡ�ļ�������
{
    string temp_str;//������ȡ���ַ���
    uchar c1,c2;
    int i;
    int num;
    while(!file.eof())  //��ȡ�ļ�
    {
        getline(file,temp_str);
        c1 = temp_str[0];       //��ù��������λ
        c2 = temp_str[1];          //��ù��������λ
        getline(file,temp_str); //��ȡ���ַ���
        num = atoi(temp_str.c_str());
        for(i=0; i<num; i++)
        {
            Single_insert(c1,c2);   //�����в���
        }
    }
}
//���ִַ��Ӻ���
void Is_Chinese(uint &j,uint &pos,uchar c1,uchar c2,string str,uint china_num,uint succeed_num)
{
    string temp_str;            //������ȡÿ������
    Body *body_p = H[ (c1-0xB0)*94 + (c2-0xA1) ].next; //��ö�Ӧ��ͷ�ĺ���ָ��
    for( j+=2 ; j < str.length() ; j+=2 )
    {
        temp_str = str.substr(j,2); //��ȡ�����ֽ�(��һ������)
        c1 = temp_str[0];   //��ʱ���� һ�����Ƚ���
        if( c1<0xB0 )  //����ڵ��ַ������־�����ȥ�ִ�������ȥ����
            break;
        for( ; temp_str.compare(body_p->str) && body_p->brother ; body_p = body_p->brother );
        if( !temp_str.compare(body_p->str) )    //���ѭ������������������ͬ
        {
            china_num ++ ;  //�ɹ�ƥ���������һ
            if( body_p->IsWord ) succeed_num = china_num;//���ɴ�����±��γɹ�ƥ�������
            if( body_p->children != NULL )  //������Ӳ�Ϊ��
            {
                body_p = body_p->children;  //��Ӻ��ӿ�ʼ��
            }
            else break;
        }
        else   break;
    }
    cout<<str.substr(pos,succeed_num*2)<<"/";
    pos += succeed_num*2;
    j = pos;
}
void Segdict(string str)//�ִ�������
{
    string temp_str;            //������ȡÿ������
    uint pos;              //��־ÿ�ζٺŵ�λ��
    uint china_num;     //�������жϵ��������ָ���
    uint succeed_num;
    for(uint j = 0, pos = 0; j < str.length() ;  )  //ѭ������ַ�
    {
        uchar c1,c2;            //��ù�����
        temp_str = str.substr(j,2); //��ȡ�����ֽ�(��һ������)
        china_num = 1;
        succeed_num = china_num;
        c1 = temp_str[0];
        c2 = temp_str[1];
        if( c1>0xAF )   //���ִ���
            Is_Chinese(j, pos, c1, c2, str, china_num, succeed_num);//���Ĵ�����
        else //���ֽڵĺ��ִ���
        {
            cout<<str.substr(pos,1); //����һ���ֽڳ��ȵ�����
            pos += 1;
            j = pos;   //��־���±�Ϊposλ��
        }
    }
}

void Enter_string() //����Ҫ�ֵľ���
{
    cout<<"������Ҫ�ִʵľ���(end����):"<<endl;//��������:2017���������������Ĵ��룬û��ʲô�ǰ�ҹ������˵��¡�
    while(1)
    {
        string str;
        cin>>str;
        if(!str.compare("end"))     //�������end��������
            break;
        else
        {
            cout<<endl<<"���ӷִʳɣ�  ";
            Segdict(str);   //���÷ִ��ܳ���
        }
        cout<<endl<<endl;
    }
}

void Menu() //��ʾ�˵�(����������Ϣ)
{
    cout<<endl<<"����ʵ��Ѽ��سɹ���"<<endl<<endl<<endl<<endl;
    cout<<"             /****************************************"<<endl<<
        "             *             ���ӷִʳ���              *"<<endl<<
        "             *                                       *"<<endl<<
        "             *   2017��γ����       by���15.1���� *"<<endl<<
        "             ****************************************/"<<endl<<endl;
}

int main()  //������
{
    Build_head();//������ͷ

    if( file == NULL )  //���ֵ��ļ�������
    {
        cout<<"�ֵ䲻���ڣ���رճ�������ֵ䵽��Ŀ¼"<<endl;
        _sleep(3*1000);//��ʱ3��
        return 0;
    }
    Menu();     //��ʾ�˵�
    Build_tree();   //��������
    Enter_string(); //����Ҫ�ִʵľ���
    return 0;   //�������
}