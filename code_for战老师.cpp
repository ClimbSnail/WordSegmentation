#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
//---------------------------------------------------------------------------
ofstream fout("output.txt");

const int START1 = 0XB0, START2 = 0XA1, END1 = 0XF8, END2 = 0XFF;
const int MAXWORDLEN = 48;

struct ThirdPlusRec
{
    string Key;
    bool IsWord;
    ThirdPlusRec *L, *R;
    ThirdPlusRec(string cc = "", bool isword = false, ThirdPlusRec* b = 0, ThirdPlusRec* s = 0):
        Key(cc), IsWord(isword), L(b), R(s) {}
};

struct SecondRec
{
    string Key;
    bool IsWord;
    ThirdPlusRec *Child;
    SecondRec(string cc = "", bool isword = false, ThirdPlusRec* s = 0):
        Key(cc), IsWord(isword), Child(s) {}
};

struct HeadRec
{
    string Key;
    vector<SecondRec> Sec;
};
//---------------------------------------------------------------------------
class Dictionary
{
    int HASH[END1 - START1][END2 - START2];
    vector<HeadRec> Dict;
    ifstream fin;

    string GetLine();
    int GetNumber();
    void LoadDictionary();
    void ProcessThird(string s, ThirdPlusRec* &Child);
    void PrintThird(string s, ThirdPlusRec* Child);
    void SkipNonChinese(string src, vector<string> &dest, unsigned &start, unsigned &end);
    int BinarySearch(unsigned x, string Sec);
    ThirdPlusRec* SeqSearch(ThirdPlusRec* p, string cc);
public:
    Dictionary(string FileName);
    void PrintDict();
    void Segment(string src, vector<string> &dest);
    void SegFileToFile(string FileNameSrc, string FileNameDest);
};
//---------------------------------------------------------------------------
void Dictionary::SegFileToFile(string FileNameSrc, string FileNameDest)
{
    string s;
    const int MAXLEN = 1000;
    char t[MAXLEN];
    ifstream SrcFile(FileNameSrc.c_str());
    while (SrcFile.getline(t, MAXLEN))
        s += string(t);
    vector<string> dest;
    Segment(s, dest);
    ofstream DestFile(FileNameDest.c_str());
    for (unsigned i = 0; i < dest.size(); i++)
        DestFile << dest[i] << endl;
}

unsigned CharToInt(char c)
{
    return unsigned((unsigned char)c);
}

bool IsCC(char c)
{
    unsigned val = CharToInt(c);
    return val >= START1 && val < END1;
}

bool IsEC(char c)
{
    unsigned val = CharToInt(c);
    return val < 0x80;
}

void Dictionary::SkipNonChinese(string src, vector<string> &dest,
                                unsigned &StartPos, unsigned &EndPos)
{
    unsigned StrLen = src.length();
    while (EndPos < StrLen && !IsCC(src[EndPos]))
    {
        if (!IsEC(src[EndPos]))
            EndPos++;
        EndPos++;
    }
    if (EndPos > StartPos)
    {
        dest.push_back(src.substr(StartPos, EndPos - StartPos));
        StartPos = EndPos;
    }
}

int Dictionary::BinarySearch(unsigned x, string Sec)
{
    int L = 0, R = Dict[x].Sec.size() - 1;
    while (L <= R)
    {
        int M = (L + R) / 2;
        if (Dict[x].Sec[M].Key == Sec)
            return M;
        else if (Dict[x].Sec[M].Key < Sec)
            L = M + 1;
        else
            R = M - 1;
    }
    return -1;
}

ThirdPlusRec* Dictionary::SeqSearch(ThirdPlusRec* p, string cc)
{
    while (p != 0)
    {
        if (p->Key == cc)
            return p;
        else
            p = p->L;
    }
    return 0;
}

void Dictionary::Segment(string src, vector<string> &dest)
{
    unsigned StrLen = src.length();
    unsigned StartPos = 0, EndPos;
    while (StartPos < StrLen)
    {
        EndPos = StartPos;
        SkipNonChinese(src, dest, StartPos, EndPos);
        if (StartPos >= StrLen) return;

        unsigned SegLen = 2;
        string HeadCC = src.substr(StartPos, 2);
        int HeadIndex = HASH[CharToInt(HeadCC[0]) - START1][CharToInt(HeadCC[1]) - START2];
        if (HeadIndex >= 0)
        {
            string SecCC = src.substr(StartPos + 2, 2);
            if (SecCC.length() > 0 && IsCC(SecCC[0]))
            {
                int B2 = BinarySearch(HeadIndex, SecCC);
                if (B2 >= 0)    // 双字部分存在
                {
                    if (Dict[HeadIndex].Sec[B2].IsWord) //是双字词
                        SegLen += 2;
                    EndPos = StartPos + 4;
                    ThirdPlusRec *p = Dict[HeadIndex].Sec[B2].Child;
                    while (EndPos < StrLen && (p = SeqSearch(p, src.substr(EndPos, 2))) != 0)
                    {
                        EndPos += 2;
                        if (p->IsWord)
                            SegLen = EndPos - StartPos;
                        p = p->R;
                    }
                }
            }
        }
        dest.push_back(src.substr(StartPos, SegLen));
        StartPos += SegLen;
    }
}

void Dictionary::PrintThird(string s, ThirdPlusRec* Child)
{
    for (ThirdPlusRec* p = Child; p != 0; p = p->L)
    {
        string t = s + p->Key;
        if (p->IsWord)
            fout << t << endl;
        if (p->R != 0)
            PrintThird(t, p->R);
    }
}

void Dictionary::PrintDict()
{
    for (unsigned i = 0; i < Dict.size(); i++)
    {
        fout << Dict[i].Key << endl;
        int n = Dict[i].Sec.size();
        fout << n << endl;
        for (int j = 0; j < n; j++)
        {
            string s = Dict[i].Key + Dict[i].Sec[j].Key;
            if (Dict[i].Sec[j].IsWord)
                fout << s << endl;
            if (Dict[i].Sec[j].Child != 0)
                PrintThird(s, Dict[i].Sec[j].Child);
        }
    }
}

Dictionary::Dictionary(string FileName)
{
    fin.open(FileName.c_str());
    for (int i = 0; i < END1 - START1; i++)
        for (int j = 0; j < END2 - START2; j++)
            HASH[i][j] = -1;
    LoadDictionary();
}

int StrToInt(string s)
{
    int t = 0;
    for (unsigned i = 0; i < s.length(); i++)
        t = t * 10 + int(s[i] - '0');
    return t;
}

string Dictionary::GetLine()
{
    char cstr[MAXWORDLEN];
    fin.getline(cstr, MAXWORDLEN);
    return string(cstr);
}

int Dictionary::GetNumber()
{
    char cstr[MAXWORDLEN];
    fin.getline(cstr, MAXWORDLEN);
    string s(cstr);
    return StrToInt(s);
}

void Dictionary::LoadDictionary()
{
    char cstr[MAXWORDLEN];
    string s;
    int n, k = 0;
    while (fin.getline(cstr, MAXWORDLEN))
    {
        s = cstr;
        HeadRec H;
        H.Key = s.substr(0, 2);
        int m1 = (unsigned char)s[0] - START1,
        m2 = (unsigned char)s[1] - START2;
        HASH[m1][m2] = k++;
        n = GetNumber();
        for (int i = 0; i < n; i++)
        {
            s = GetLine();
            string t = s.substr(2, 2);

            int SIZE = H.Sec.size();
            if (SIZE == 0 || SIZE > 0 && H.Sec[SIZE - 1].Key != t)
            {
                SecondRec sec(t, (s.length() == 4), 0);
                H.Sec.push_back(sec);
            }
            SIZE = H.Sec.size();
            if (s.length() > 4)
                ProcessThird(s.substr(4, MAXWORDLEN), H.Sec[SIZE - 1].Child);
        }
        Dict.push_back(H);
    }
}

void Dictionary::ProcessThird(string s, ThirdPlusRec* &Child)
{
    int Len = s.length();
    string t = s.substr(0, 2);
    ThirdPlusRec* LAST = Child;
    if (Child == 0)
        LAST = Child = new ThirdPlusRec(t, (Len == 2), 0, 0);
    else
    {
        while (LAST->L != 0)
            LAST = LAST->L;
        if (LAST->Key != t)
        {
            LAST->L = new ThirdPlusRec(t, (Len == 2), 0, 0);
            LAST = LAST->L;
        }
    }
    if (Len > 2)
        ProcessThird(s.substr(2, MAXWORDLEN), LAST->R);
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    Dictionary S("segdict.txt");
//    S.SegFileToFile("src.txt", "dest.txt");
    vector<string> dest;
    string s = "中华人民共和国成立于1949年。";
    s += "中华民国成立于1912年。";
    s += "周恩来领导了1927年的南昌起义。";
    s += "他从马上下来。";
    S.Segment(s, dest);
    for (unsigned i = 0; i < dest.size(); i++)
        cout << dest[i] << endl;

    cin.get();       

//    S.PrintDict();
    return 0;
}
 
