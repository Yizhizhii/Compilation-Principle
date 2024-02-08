#include "mainwindow.h"
#include <QEventLoop>

using namespace std;
const int base = 100;

extern unordered_set<string> G;



vector<Code> codes;

vector<string> declarations;

vector<string> vars;

inline int next_quad()
{
    return codes.size();
}

void backpatch(const vector<int> &idx, int x)
{
    for (auto i : idx)
    {
        if (i < codes.size())
            codes[i].dst = to_string(x + base);
    }
}

void show_codes(QTextBrowser* code,TextBrowserStreambuf* textBrowserStreambuf_code)
{
    int n = codes.size();
    code->clear();
    cout.rdbuf(textBrowserStreambuf_code);//重定向到状态
    for (int i = 0; i < n; i++)
    {
        cout << 100 + i << " : " << codes[i] << endl;
    }
}

void emit_(string op, string src1, string src2, string dst)
{
    Code cd(op, src1, src2, dst);
    codes.push_back(cd);
}

bool check(State &s)
{
    if (s.place == "")
    {
        s.place = "v" + to_string(vars.size());
        vars.push_back(s.place);
        int t = next_quad();
        emit_(":=", "1", "", s.place);
        emit_("j", "", "", "0");
        int f = next_quad();
        emit_(":=", "0", "", s.place);
        emit_("j", "", "", "0");
        backpatch(s.true_list, t);
        backpatch(s.false_list, f);
        s.true_list = {t + 1};
        s.false_list = {f + 1};
        backpatch(s.true_list, next_quad());
        backpatch(s.false_list, next_quad());
    }
    return true;
}

bool MainWindow::assign_statement(State src, State dst)
{
    if (std::find(declarations.begin(), declarations.end(), dst.place) == declarations.end())
    {
        string error_message="";
        error_message+="error at line ";
        error_message+=to_string(dst.line_count);
        error_message+=": " + dst.place + " none redefinition\n";
//        cout << "error at line " << src.line_count << ": 变量重定义" << endl;
        QMessageBox::warning(this,"Error",QString::fromStdString(error_message));
        // 具体报错提示
        return false;
    }
    // 对src的判断
    check(src);
    emit_(":=", src.place, "", dst.place);
    return true;
}

bool MainWindow::declaration_statement(State v)
{
    if (std::find(declarations.begin(), declarations.end(), v.place) != declarations.end())
    {
        string error_message="";
        error_message+="error at line ";
        error_message+=to_string(v.line_count);
        error_message+=": " + v.place + " variable redefinition\n";
//        cout << "error at line " << v.line_count << ": 变量重定义" << endl;
        QMessageBox::warning(this,"Error",QString::fromStdString(error_message));
        return false;
    }
    declarations.push_back(v.place);
    return true;
}

bool cal_expression_statement(State &fa, State &src1, string op, State &src2)
{
    check(src1);
    check(src2);
    emit_(op, src1.place, src2.place, "v" + to_string(vars.size()));
    fa.place = "v" + to_string(vars.size());
    vars.push_back(fa.place);
    return true;
}

bool bool_expression_statement(State &fa, State expression)
{
    fa.true_list = {next_quad()};
    fa.false_list = {next_quad() + 1};
    emit_("jnz", expression.place, "", to_string(next_quad() + 2));
    emit_("j", "", "", "0");
    return true;
}

bool relop_expression_statement(State &fa, State &src1, string relop, State &src2)
{
    fa.true_list = {next_quad()};
    fa.false_list = {next_quad() + 1};
    check(src1);
    check(src2);
    emit_("j" + relop, src1.place, src2.place, "0");
    emit_("j", "", "", "0");

    return true;
}

bool and_expression_statement(State &fa, State &src1, State &M, State &src2)
{
    cout << "##########################" << endl;
    cout << M.next_quad << endl;
    cout << "##########################" << endl;
    backpatch(src1.true_list, M.next_quad);
    fa.true_list = src2.true_list;
    fa.false_list = src1.false_list;
    fa.false_list.insert(fa.false_list.end(), src2.false_list.begin(), src2.false_list.end());

    return true;
}

bool or_expression_statement(State &fa, State &src1, State &M, State &src2)
{
    backpatch(src1.false_list, M.next_quad);
    fa.true_list = src1.true_list;
    fa.true_list.insert(fa.true_list.end(), src2.true_list.begin(), src2.true_list.end());
    fa.false_list = src2.false_list;

    return true;
}

bool not_expression_statement(State &fa, State src)
{
    fa.false_list = src.true_list;
    fa.true_list = src.false_list;

    return true;
}

bool if_statement(State &fa, State &expression, State &M, State &s)
{
    backpatch(expression.true_list, M.next_quad);
    fa.next_list = expression.false_list;
    fa.next_list.insert(fa.next_list.end(), s.next_list.begin(), s.next_list.end());

    return true;
}

bool if_statement(State &fa, State &expression, State &M1, State &s1, State &N, State &M2, State &s2)
{
    backpatch(expression.true_list, M1.next_quad);
    backpatch(expression.false_list, M2.next_quad);
    fa.next_list = s1.next_list;
    fa.next_list.insert(fa.next_list.end(), N.next_list.begin(), N.next_list.end());
    fa.next_list.insert(fa.next_list.end(), s2.next_list.begin(), s2.next_list.end());

    return true;
}

bool M_statement(State &M)
{
    M.next_quad = next_quad();
    return true;
}

bool N_statement(State &N)
{
    N.next_list = {next_quad()};
    emit_("j", "", "", "0");

    return true;
}

bool while_statement(State &fa, State &M1, State &expression, State &M2, State &s)
{
    backpatch(s.next_list, M1.next_quad);
    backpatch(expression.true_list, M2.next_quad);
    fa.next_list = expression.false_list;
    emit_("j", "", "", to_string(M1.next_quad + base));

    return true;
}

bool statements(State &fa, State &L, State &M, State &s)
{
    backpatch(L.next_list, M.next_quad);
    fa.next_list = s.next_list;

    return true;
}

bool check_idt(State idt)
{
    if (find(declarations.begin(), declarations.end(), idt.place) == declarations.end())
        return false;
    return true;
}

extern vector<Production>
    productions;
extern vector<map<string, pair<int, int>>> ACTION;
extern vector<map<string, int>> GOTO;

void MainWindow::process(const vector<Token_table> &input,TextBrowserStreambuf *textBrowserStreambuf_state,TextBrowserStreambuf* textBrowserStreambuf_symbol,TextBrowserStreambuf* textBrowserStreambuf_input,TextBrowserStreambuf* textBrowserStreambuf_code,int go_end)
{
//    qDebug()<<1;
    int point = 0;
    vector<int> LR_states;
    vector<State> states;
    vector<string> symbols;
    LR_states.push_back(0), symbols.push_back("#");
    int go_end_=go_end;
    while (!LR_states.empty())
    {
        state->clear();
        cout.rdbuf(textBrowserStreambuf_state);//重定向到状态
        cout << "state:" << endl;
        for (auto x : LR_states)
            cout << x << ' ';
        cout << endl;

        symbol->clear();
        cout.rdbuf(textBrowserStreambuf_symbol);//重定向到符号
        cout << "symbols:" << endl;
        for (auto x : symbols)
            cout << x << ' ';
        cout << endl;

        input_->clear();
        cout.rdbuf(textBrowserStreambuf_input);//重定向到输入
        cout << "input:" << endl;
        for (int i = point; i < input.size(); i++)
            cout << input[i].type << ' ';
        cout << endl;

        int top = *LR_states.rbegin();
        string head = input[point].type;
        string token = input[point].token;
        if (ACTION[top].count(head) == 0)
        {
            // 报错
            string error_message="";
            error_message+="Syntax error at line ";
            error_message+=to_string(input[point].line + 1);
            error_message+="!\n";
            error_message+="ERROR MESSAGE: ";
//            cout << "Syntax error at line " << input[point].line + 1 << "!" << endl;
//            cout << "ERROR MESSAGE：";
            vector<string> v;
            string tmp;
            int t = point - 1;
            for (; input[t].token == ""; t--)
                ;
            error_message+=input[t].token;
            error_message+=" should follow: ";
//            cout << input[t].token << " 后应跟： ";
            for (auto token : G)
            {
                if (ACTION[top].count(token) != 0)
                {
                    if (token != "")
                        v.push_back(token);
                }
            }
            // for (int i = 0; i < input.size(); i++)
            // {
            //     if (ACTION[top].count(input[i].token) != 0)
            //     {
            //         if (input[i].token != "")
            //             v.push_back(input[i].token);
            //     }
            // }
            set<string> sh(v.begin(), v.end());
            v.assign(sh.begin(), sh.end());
            for (auto y : v){
                error_message+=y;
                error_message+=" ";
//                cout << y << " ";
            }
            error_message+="\nbut it actually follows: ";
            t=point;
            for(;t<input.size()&&input[t].type=="";t++)
                ;
            error_message+=input[t].type;
            error_message+=" \n";
//            cout << "实际是： " << head << ' ' << endl;

            // cout << "top: " << top << " head: " << head <<endl;

            QMessageBox::warning(this,"Error",QString::fromStdString(error_message));
            return;
        }
        else
        {
            auto op = ACTION[top][head];
            if (op.first == 0) // 移进
            {
                LR_states.push_back(op.second);
                symbols.push_back(head);
                states.push_back(State(token, input[point].line));
                point++;
                cout << endl;
            }
            else // 规约
            {
                if (op.second == -1)
                {
                    QMessageBox::information(nullptr, "", "accept!\n");
//                    cout << "accept!" << endl;
                    break;
                }
                else
                {
                    // 对op.second进行讨论
                    State fa = *states.rbegin();
                    int tag = 0;
                    int tag1 = 0;
                    int n = states.size();
                    if (mp.count(op.second))
                    {
                        if (mp.at(op.second) == "declaration")
                        {
                            if(!declaration_statement(states[n - 1]))
                                return;
                        }
                        else if (mp.at(op.second) == "if")
                        {
                            if_statement(fa, states[n - 4], states[n - 2], states[n - 1]);
                        }
                        else if (mp.at(op.second) == "if_else")
                        {
                            if_statement(fa, states[n - 8], states[n - 6], states[n - 5],
                                         states[n - 4], states[n - 2], states[n - 1]);
                        }
                        else if (mp.at(op.second) == "while")
                        {
                            while_statement(fa, states[n - 6], states[n - 4],
                                            states[n - 2], states[n - 1]);
                        }
                        else if (mp.at(op.second) == "assign")
                        {
                            if(!assign_statement(states[n - 2], states[n - 4]))
                                return;
                        }
                        else if (mp.at(op.second) == "or")
                        {
                            or_expression_statement(fa, states[n - 4], states[n - 2], states[n - 1]);
                            fa.place = "";
                        }
                        else if (mp.at(op.second) == "and")
                        {
                            and_expression_statement(fa, states[n - 4], states[n - 2], states[n - 1]);
                            fa.place = "";
                        }
                        else if (mp.at(op.second) == "not")
                        {
                            not_expression_statement(fa, states[n - 1]);
                        }
                        else if (mp.at(op.second) == "bool")
                        {
                            bool_expression_statement(fa, states[n - 1]);
                        }
                        else if (mp.at(op.second) == "relop")
                        {
                            relop_expression_statement(fa, states[n - 3], states[n - 2].place, states[n - 1]);
                            fa.place = "";
                        }
                        else if (mp.at(op.second) == "cal_merge")
                        {
                            cal_expression_statement(fa, states[n - 3], states[n - 2].place, states[n - 1]);
                            tag = 1;
                            for (int i = 0; i < 3; i++)
                                states.pop_back();
                        }
                        else if (mp.at(op.second) == "operator")
                        {
                            tag = tag1 = 1;
                        }
                        else if (mp.at(op.second) == "idt")
                        {
                            if (!check_idt(states[n - 1]))
                            {
                                string error_message="";
                                error_message+="error at line ";
                                error_message+=to_string(states[n - 1].line_count);
                                error_message+=": " + states[n - 1].place + " none redefinition\n";
                                QMessageBox::warning(this,"Error",QString::fromStdString(error_message));
//                                cout << "error at line " << states[n - 1].line_count << ":" << states[n - 1] << "未定义" << endl;
                                return;
                            }
                        }
                        else if (mp.at(op.second) == "bracket")
                        {
                            fa = states[n - 2];
                        }
                        else if (mp.at(op.second) == "M")
                        {
                            M_statement(fa);
                        }
                        else if (mp.at(op.second) == "N")
                        {
                            N_statement(fa);
                        }
                        else if (mp.at(op.second) == "statements")
                        {
                            statements(fa, states[n - 3], states[n - 2], states[n - 1]);
                        }
                    }
                    auto production = productions[op.second];
                    for (int i = 0; i < production.right.size(); i++)
                    {
                        symbols.pop_back(), LR_states.pop_back();
                        if (!tag)
                            states.pop_back();
                    }
                    top = *LR_states.rbegin();
                    symbols.push_back(production.left);
                    if (!tag1)
                        states.push_back(fa);
                    LR_states.push_back(GOTO[top][production.left]);
                    cout << "ok" << endl;
                }
                cout << endl;
            }
        }

        if(go_end_==false){
            // 创建事件循环
            QEventLoop loop;

            // 连接按钮的点击信号到事件循环的退出槽
            connect(portBtn_one, &QPushButton::clicked, &loop, &QEventLoop::quit);
            connect(portBtn_run, &QPushButton::clicked, &loop, &QEventLoop::quit);
            QObject::connect(portBtn_run,&QPushButton::clicked,[&]()mutable{
                go_end_ = true;
            });

            // 暂停程序的执行，等待按钮被点击
            loop.exec();
        }
        else{//自动运行
        }
    }

    vars.resize(0);
    declarations.resize(0);
    one_used = false;

    show_codes(code,textBrowserStreambuf_code);
    codes.resize(0);
}
