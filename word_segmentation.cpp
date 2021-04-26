#include "word_segmentation.hpp"

WordSegmentation::Dictionary::~Dictionary()
{
    delete p_ifstream;

    p_search = NULL;
    p_ifstream = NULL;
}

bool WordSegmentation::Dictionary::BuildHead(void) //������ͷ�ڵ�
{
    //�����ͷ
    for (uchar row = 0xB0; row <= 0xF7; ++row) //4*16+(1+7)=72 һ��72��
    {
        for (uchar line = 0xA1; line <= 0xFE; ++line) //5*16+14=94 һ��94��
        {
            WordSegmentation::WordNode *head = new WordSegmentation::WordNode;
            head->children = NULL;
            head->brother = NULL;
            // �����ڵ�Ĭ�Ͼ���һ����
            head->IsWord = true;
            head->str.push_back(row);
            head->str.push_back(line);
            // �����ַ����ͷ
            table_head.push_back(head);
        }
    }
    return true;
}

bool WordSegmentation::Dictionary::BuildTree(char *file_path) //��ȡ�ļ�������
{
    //���ֵ��ļ�������
    if (NULL == file_path)
    {
        return false;
    }
    p_ifstream = new std::fstream(file_path);
    std::string temp_str; //������ȡ���ַ���z
    uchar high, low;
    int num;
    while (!p_ifstream->eof()) //��ȡ�ļ�
    {
        getline(*p_ifstream, temp_str); //��ȡ��ͷ
        high = temp_str[0];             //��ù������8λ
        low = temp_str[1];              //��ù������8λ
        getline(*p_ifstream, temp_str); //��ȡ����ͷ�µĴ�����
        num = atoi(temp_str.c_str());
        for (int cnt = 0; cnt < num; ++cnt)
        {
            SingleInsert(high, low); //�����в���
        }
    }
    return true;
}

//���뵥�д���
void WordSegmentation::Dictionary::SingleInsert(uchar high, uchar low)
{
    std::string temp_str;
    getline(*p_ifstream, temp_str); //��ȡ���ַ���
    //ÿ�п�ʼ����ʱ��λ�ü�¼ָ��ָ���Ӧ��ͷ�ĵ�һ��body
    int temp_str_len = temp_str.length();
    p_search = table_head[(high - 0xB0) * 94 + (low - 0xA1)];
    for (uint j = 2; j < temp_str_len; j += 2)
    {
        //����һ��body�ڵ�
        WordSegmentation::WordNode *p_new_node = new WordSegmentation::WordNode;
        p_new_node->brother = NULL;
        p_new_node->children = NULL;
        p_new_node->str = temp_str.substr(j, 2);
        if (j == temp_str_len - 2)
            p_new_node->IsWord = true;
        else
            p_new_node->IsWord = false;

        InsertNode(p_new_node); //�������body�ڵ�
    }
}

bool WordSegmentation::Dictionary::InsertNode(WordSegmentation::WordNode *p_new_node) //���뵥������
{
    if (NULL == p_new_node)
    {
        return false;
    }

    if (NULL == p_search->children) // û���ӽڵ�ʱ����Ӻ�����
    {
        p_search->children = p_new_node;
        p_search = p_search->children;
    }
    else if (!p_search->children->str.compare(p_new_node->str))
    {
        // �������Ѵ��ڣ��������Ƿ񹹳ɴ�
        if (p_new_node->IsWord)
        {
            p_search->children->IsWord = true;
        }
        p_search = p_search->children;
    }
    else
    {
        // ���ֵ����� �����Ƿ����ֵ��ͬ�Ľڵ�
        for (p_search = p_search->children;
             NULL != p_search->brother && p_search->brother->str.compare(p_new_node->str);
             p_search = p_search->brother)
            ;
        // û�ҵ�Ԫ����ͬ���ֵ� �����ڵ�
        if (NULL == p_search->brother)
        {
            p_search->brother = p_new_node;
        }
        else if (p_new_node->IsWord)
        {
            // ���ҵ� ���ұ��ʹ��ɴʣ�������״̬
            p_search->brother->IsWord = p_new_node->IsWord;
        }

        p_search = p_search->brother;
    }
    return true;
}

std::string WordSegmentation::Dictionary::Segdict(std::string sentences) //�ִ�������
{
    std::string temp_str; //������ʱ���ÿ������
    std::string result;   // ��ŷִʺ�Ľ��
    uchar high = 0;
    uchar low = 0;                                //��ù�����
    for (uint pos = 0; pos < sentences.length();) //ѭ������ַ�
    {
        temp_str = sentences.substr(pos, 2); //��ȡ�����ֽ�(��һ������)
        high = (uchar)temp_str[0];
        if (high >= 0xB0)
        {
            low = (uchar)temp_str[1];
            int maxlen = MaxChineseLen(pos, high, low, sentences);
            result = result + sentences.substr(pos, maxlen) + "/"; //���Ĵ�����
            pos = pos + maxlen;
        }    //���ִ���
        else //���ֽڵĺ��ִ���
        {
            result = result + sentences.substr(pos, 1); //���������ֻ������
            pos += 1;
        }
    }
    return result;
}

//���ִַ��Ӻ���
int WordSegmentation::Dictionary::MaxChineseLen(int pos, uchar high, uchar low, std::string &sentences)
{
    std::string temp_str;
    int maxlen = 2;
    int index = 0;                                                                                // ��ŷִʺ�Ľ��                                                                //������ȡÿ������
    WordSegmentation::WordNode *p_body = table_head[(high - 0xB0) * 94 + (low - 0xA1)]->children; //��ö�Ӧ��ͷ�ĺ���ָ��
    for (index = pos + 2; index < sentences.length(); index += 2)
    {
        temp_str = sentences.substr(index, 2); //��ȡ�����ֽ�(��һ������)
        if ((uchar)temp_str[0] < 0xB0)         //����ڵ��ַ������־�����ȥ�ִ�������ȥ����
        {
            maxlen = index - pos + 2;
            break;
        }

        // ����ǰ�ڵ����
        if (!p_body->str.compare(temp_str))
        {
            if (p_body->IsWord)
            {
                maxlen = index - pos + 2;
            }
            if (NULL != p_body->children) //������Ӳ�Ϊ��
            {
                p_body = p_body->children; //��Ӻ��ӿ�ʼ��
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
            else if (!temp_str.compare(p_body->str)) //���ѭ������������������ͬ
            {
                if (p_body->IsWord)
                    maxlen = index - pos + 2; //���ɴ�����±��γɹ�ƥ�������
                if (NULL != p_body->children) //������Ӳ�Ϊ��
                {
                    p_body = p_body->children; //��Ӻ��ӿ�ʼ��
                }
                else
                    break;
            }
        }
    }
    return maxlen;
}