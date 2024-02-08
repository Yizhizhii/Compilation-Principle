#include "mainwindow.h"

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

vector<Token_table> parse(string content)
{
    vector<Token_table> res;
    char ch;
    int line_count = 0;
    int i=0;
    while (ch = content[i++], i!=content.length()+1)
    {
        string token = "";
        token = ch;
        TOKENCODE token_code;
        // §Ù
        if (ch == ' ' || ch == '\t')
            continue;
        else if (ch == '\n')
        {
            line_count++;
            continue;
        }
        // 
        else if (isletter(ch))
        {
            // 
            while (i!=content.length() && (isdigit(content[i]) || isletter(content[i])))
                token += content[i++];
            token_code = iskey(token);
            if (token_code == TK_IDENT)
                res.push_back({line_count, "idt", token});
            else
            {
                if (token == "else")
                    res.push_back({line_count, "", ""});
                res.push_back({line_count, token, token});
                if (token == "while")
                    res.push_back({line_count, "", ""});
            }
        }
        // 
        else if (isdigit(ch))
        {
            // §³
            while (i!=content.length() && isdigit(content[i]))
                token += content[i++];
            // ¦Ä
            if (i!=content.length() && isletter(content[i]))
            {
                while (i!=content.length() && (isdigit(content[i]) || isletter(content[i])))
                    token += content[i++];
                token_code = TK_UNDEF;
            }
            else
                token_code = TK_DIG, res.push_back({line_count, "dig", token});
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
            case '#':
            case '}':
                token_code = token_mp.at(token);
                break;
            case '{':
                res.push_back({line_count, "", ""});
                token_code = token_mp.at(token);
                break;
            case '=':
            case '>':
            case '<':
                if (i!=content.length() && content[i] == '=')
                    token += content[i++];
                token_code = token_mp.at(token);
                break;
            case '!':
                if (i!=content.length() && content[i] == '=')
                {
                    token += content[i++];
                    token_code = token_mp.at(token);
                    break;
                }
                else
                {
                    token_code = token_mp.at(token);
                    break;
                }
            case '&':
                if (i!=content.length() && content[i] == '&')
                {
                    token += content[i++];
                    token_code = token_mp.at(token);
                    break;
                }
                else
                {
                    token_code = TK_UNDEF;
                    break;
                }
            case '|':
                if (i!=content.length() && content[i] == '|')
                {
                    token += content[i++];
                    token_code = token_mp.at(token);
                    break;
                }
                else
                {
                    token_code = TK_UNDEF;
                    break;
                }
            case '/':
                if (i!=content.length() && (content[i] == '/' || content[i] == '*'))
                    token += content[i++];
                token_code = token_mp.at(token);
                break;
            case '*':
                if (i!=content.length() && content[i] == '/')
                    token += content[i++];
                token_code = token_mp.at(token);
                break;
            default:
                token_code = TK_UNDEF;
                break;
            }
            if (token_code != TK_UNDEF)
                res.push_back({line_count, token, token});
            if (token == "&&" || token == "||")
                res.push_back({line_count,"", ""});
        }
        // print_token(token_code, token);
    }
    res.push_back({line_count, "#", "#"});
    return res;
}

int MainWindow::main1(TextBrowserStreambuf *textBrowserStreambuf_state,TextBrowserStreambuf* textBrowserStreambuf_symbol,TextBrowserStreambuf* textBrowserStreambuf_input,TextBrowserStreambuf* textBrowserStreambuf_code,int go_end)
{
    string content = textEdit->toPlainText().toStdString();
    init();
    process(parse(content),textBrowserStreambuf_state,textBrowserStreambuf_symbol,textBrowserStreambuf_input,textBrowserStreambuf_code,go_end);
    // debug();
}
