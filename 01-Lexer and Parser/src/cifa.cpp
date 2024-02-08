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
        // 空白符、换行符
        if (ch == ' ' || ch == '\t' || ch == '\n')
            continue;
        // 字母
        else if (isletter(ch))
        {
            // 字母后跟数字或字母
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
        // 数字
        else if (isdigit(ch))
        {
            // 暂时不考虑小数
            while (fin.peek() != EOF && isdigit(fin.peek())){
                char ch_ = fin.get();
                token += ch_;
                content += ch_;
            }
            // 数字后跟字母为未定义
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

//导入文本读文本
vector<string> MainWindow::read(){
    string fileName = QFileDialog::getOpenFileName(this,"","/Users/mac/Desktop/").toStdString();
    string content;
    vector<string> input;

    ifstream fin(fileName, ios::in);
    if (!fin.is_open())
    {
//        cout << "文件打开失败!" << endl;
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
    // 获取应用程序的当前工作目录
//    string currentPath = QCoreApplication::applicationDirPath().toStdString();
//    string file_name = currentPath + "/" + "test.cpp";
//    string file_name = "./test.cpp";

//    string file_name = "/Users/mac/Documents/study/c++/qt/cifa_yufa/test.cpp";//使用绝对路径
//    ifstream fin(filename, ios::in);
//    string content;
//    vector<string> input;

//    if (!fin.is_open())
//    {
//        cout << "文件打开失败!" << endl;
//        QMessageBox::warning(this,"Error",QStringLiteral("can't open the file!"));
//        return 0;
//    }
//    init();

//    input = parse(fin,content);
//    ui->code->setText(QString::fromStdString(content));

    process(input,textBrowserStreambuf_state,textBrowserStreambuf_symbol,textBrowserStreambuf_input);

    // debug();
}
