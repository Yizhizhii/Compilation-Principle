#include "my_map.h"
#include <QCoreApplication>

inline void print_token(TOKENCODE code, const string &token)
{
    cout << '(' << code << ", " << token << ')' << endl;
}

inline bool isletter(const char &ch)
{
    return (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z');
}

inline TOKENCODE iskey(const string &token)
{
    int len = key_word.size();
    for (int i = 0; i < len; i++)
        if (token == key_word[i])
            return TOKENCODE(i + 1);
    return TOKENCODE::TK_IDENT;
}

vector<string> parse(ifstream &fin,string &content)
{
    vector<string> res;
    char ch;
    while (ch = fin.get(), !fin.eof())
    {
        content+=ch;
        string token = "";
        token = ch;
        TOKENCODE token_code;
        // �հ׷������з�
        if (ch == ' ' || ch == '\t' || ch == '\n')
            continue;
        // ��ĸ
        else if (isletter(ch))
        {
            // ��ĸ������ֻ���ĸ
            while (fin.peek() != EOF && (isdigit(fin.peek()) || isletter(fin.peek()))){
                char ch_ = fin.get();
                token += ch_;
                content += ch_;
            }
            token_code = iskey(token);
            if (token_code == TK_IDENT)
                res.push_back("idt");
            else
                res.push_back(token);
        }
        // ����
        else if (isdigit(ch))
        {
            // ��ʱ������С��
            while (fin.peek() != EOF && isdigit(fin.peek())){
                char ch_ = fin.get();
                token += ch_;
                content += ch_;
            }
            // ���ֺ����ĸΪδ����
            if (fin.peek() != EOF && isletter(fin.peek()))
            {
                while (fin.peek() != EOF && (isdigit(fin.peek()) || isletter(fin.peek()))){
                    char ch_ = fin.get();
                    token += ch_;
                    content += ch_;
                }
                token_code = TK_UNDEF;
            }
            else
                token_code = TK_DIG, res.push_back("dig");
        }
        else
        {
            int tag = 0;
            switch (ch)
            {
            case '+':
            case '-':
            case ',':
            case ';':
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case '#':
                token_code = token_mp.at(token);
                break;
            case '=':
            case '>':
            case '<':
                if (fin.peek() != EOF && fin.peek() == '='){
                    char ch_ = fin.get();
                    token += ch_;
                    content += ch_;
                }
                token_code = token_mp.at(token);
                break;
            case '!':
                if (fin.peek() != EOF && fin.peek() == '=')
                {
                    char ch_ = fin.get();
                    token += ch_;
                    content += ch_;
                    token_code = token_mp.at(token);
                    break;
                }
                else
                {
                    token_code = TK_UNDEF;
                    break;
                }
            case '/':
                if (fin.peek() != EOF && (fin.peek() == '/' || fin.peek() == '*')){
                    char ch_ = fin.get();
                    token += ch_;
                    content += ch_;
                }
                token_code = token_mp.at(token);
                break;
            case '*':
                if (fin.peek() != EOF && fin.peek() == '/'){
                    char ch_ = fin.get();
                    token += ch_;
                    content += ch_;
                }
                token_code = token_mp.at(token);
                break;
            default:
                token_code = TK_UNDEF;
                break;
            }
            if (token_code != TK_UNDEF)
                res.push_back(token);
        }
        // print_token(token_code, token);
    }
    res.push_back("#");
    return res;
}

//�����ı����ı�
vector<string> MainWindow::read(){
    string fileName = QFileDialog::getOpenFileName(this,"","/Users/mac/Desktop/").toStdString();
    string content;
    vector<string> input;

    ifstream fin(fileName, ios::in);
    if (!fin.is_open())
    {
//        cout << "�ļ���ʧ��!" << endl;
        QMessageBox::warning(this,"Error",QStringLiteral("can't open the file!"));
        return {};
    }
    init();

    input = parse(fin,content);
    ui->code->setText(QString::fromStdString(content));
    fin.close();

    return input;
}

int MainWindow::main1(TextBrowserStreambuf *textBrowserStreambuf_state,TextBrowserStreambuf* textBrowserStreambuf_symbol,TextBrowserStreambuf* textBrowserStreambuf_input,vector<string> input)
{
    // ��ȡӦ�ó���ĵ�ǰ����Ŀ¼
//    string currentPath = QCoreApplication::applicationDirPath().toStdString();
//    string file_name = currentPath + "/" + "test.cpp";
//    string file_name = "./test.cpp";

//    string file_name = "/Users/mac/Documents/study/c++/qt/cifa_yufa/test.cpp";//ʹ�þ���·��
//    ifstream fin(filename, ios::in);
//    string content;
//    vector<string> input;

//    if (!fin.is_open())
//    {
//        cout << "�ļ���ʧ��!" << endl;
//        QMessageBox::warning(this,"Error",QStringLiteral("can't open the file!"));
//        return 0;
//    }
//    init();

//    input = parse(fin,content);
//    ui->code->setText(QString::fromStdString(content));

    process(input,textBrowserStreambuf_state,textBrowserStreambuf_symbol,textBrowserStreambuf_input);

    // debug();
}
