#include "my_map.h"

using namespace std;

// Go表
struct Go
{
    int pre;
    int next;
    int exist = -1;
    string changeStr;

    bool operator==(const Go &other) const
    {
        return (
            pre == other.pre &&
            next == other.next &&
            changeStr == other.changeStr);
    }
};

// 终结符集与非终结符集
unordered_set<string> nonTerminals;
unordered_set<string> terminals;

// 已存在族号
int Exist[20000] = {0};

// LR(1) 项目结构体
struct LR1Item
{
    Production production; // 产生式
    int dotIndex;          // 点的位置
    string lookahead;      // 向前查看符号

    LR1Item()
    {
        this->production = Production("G", {"S"});
        dotIndex = 0;
        lookahead = "#";
    }
    LR1Item(Production p, int dot, string lookahead) : production(p), dotIndex(dot), lookahead(lookahead) {}

    //// 重载 < 操作符，用于 LR1Item 的比较
    // bool operator<(const LR1Item& other) const {
    //     if (production.left != other.production.left) {
    //         return production.left < other.production.left;
    //     }
    //     if (dotIndex != other.dotIndex) {
    //         return dotIndex < other.dotIndex;
    //     }
    //     if (lookahead != other.lookahead) {
    //         return lookahead < other.lookahead;
    //     }
    //     return production.right < other.production.right;
    // }
    //  重载 == 操作符
    bool operator==(const LR1Item &other) const
    {
        return (
            production.left == other.production.left &&
            dotIndex == other.dotIndex &&
            lookahead == other.lookahead &&
            production.right == other.production.right);
    }

    bool operator!=(const LR1Item &other) const
    {
        return (
            production.left != other.production.left ||
            dotIndex != other.dotIndex ||
            lookahead != other.lookahead ||
            production.right != other.production.right);
    }
    bool operator<(const LR1Item &other) const
    {
        return (
            production.left != other.production.left ||
            dotIndex != other.dotIndex ||
            lookahead != other.lookahead ||
            production.right != other.production.right);
    }
    bool operator>(const LR1Item &other) const
    {
        return (
            production.left != other.production.left ||
            dotIndex != other.dotIndex ||
            lookahead != other.lookahead ||
            production.right != other.production.right);
    }

    friend ostream &operator<<(ostream &os, const LR1Item &item)
    {
        os << item.production.left << " -> ";
        for (size_t i = 0; i < item.production.right.size(); ++i)
        {
            if (static_cast<int>(i) == item.dotIndex)
            {
                os << ". ";
            }
            os << item.production.right[i] << " ";
        }
        if (item.dotIndex == static_cast<int>(item.production.right.size()))
        {
            os << ". ";
        }
        os << ", " << item.lookahead;
        return os;
    }
};

unordered_set<string> G;

// vector<string> GG[1000];

// First集
map<string, set<string>> First;

// LR(1) 项目集
vector<LR1Item> LR1ItemSet;

// LR(1) 项目集规范族
map<int, vector<LR1Item>> canonicalCollection;

// Go
vector<Go> go;

// set<char>FIRST[1000];
// bool vis[1000];

// 定义文法产生式集合
vector<Production> productions = {
    Production("Start", {"Program"}),

    // Program产生式
    Production("Program", {"Type", "idt", "(", "FormalParameters", ")", "M", "StatementBlock"}),

    // Type产生式
    Production("Type", {"int"}),
    Production("Type", {"void"}),

    // FormalParameters产生式
    Production("FormalParameters", {"ParameterList"}),
    Production("FormalParameters", {"void"}),

    // ParameterList产生式
    Production("ParameterList", {"Parameter", "Parameters"}),

    // Parameters产生式
    Production("Parameters", {",", "Parameter", "Parameters"}),
    Production("Parameters", {",", "Parameter"}),

    // Parameter产生式
    Production("Parameter", {"int", "idt"}),

    // StatementBlock产生式
    Production("StatementBlock", {"{", "InternalDeclaration", "StatementSequence", "}"}),
    Production("StatementBlock", {"{", "StatementSequence", "}"}),

    // InternalDeclaration产生式
    Production("InternalDeclaration", {"InternalVariableDeclaration", "InternalDeclaration"}),
    Production("InternalDeclaration", {"InternalVariableDeclaration"}),

    // InternalVariableDeclaration产生式
    Production("InternalVariableDeclaration", {"Parameter", ";"}),

    // StatementSequence产生式
    Production("StatementSequence", {"StatementSequence", "Statement"}), ///???????????????
    Production("StatementSequence", {"Statement"}),

    // Statement产生式
    Production("Statement", {"IfStatement"}),
    Production("Statement", {"WhileStatement"}),
    Production("Statement", {"ReturnStatement"}),
    Production("Statement", {"AssignmentStatement"}),

    // IfStatement产生式
    Production("IfStatement", {"if", "(", "BoolExpression", ")", "M", "StatementBlock"}),
    Production("IfStatement", {"if", "(", "BoolExpression", ")", "M", "StatementBlock", "N", "else", "M", "StatementBlock"}),

    // WhileStatement产生式
    Production("WhileStatement", {"while", "M", "(", "BoolExpression", ")", "M", "StatementBlock"}),

    // ReturnStatement产生式
    Production("ReturnStatement", {"return", ";"}),
    Production("ReturnStatement", {"return", "Expression", ";"}),

    // AssignmentStatement产生式
    Production("AssignmentStatement", {"idt", "=", "Expression", ";"}),

    // Expression产生式
    Production("BoolExpression", {"OrExpression"}),
    // Production("BoolExpression", {"AdditiveExpression", "BoolRelopAdditiveExpression"}),

    // Production("BoolRelopAdditiveExpression", {"relop", "AdditiveExpression"}),
    // Production("BoolRelopAdditiveExpression", {"BoolRelopAdditiveExpression", "relop", "AdditiveExpression"}),

    Production("Expression", {"OrExpression"}),
    Production("Expression", {"AdditiveExpression"}),
    Production("OrExpression", {"AndExpression"}),
    Production("OrExpression", {"OrExpression", "||", "M", "AndExpression"}),

    Production("AndExpression", {"NotExpression"}),
    Production("AndExpression", {"AndExpression", "&&", "M", "NotExpression"}),

    Production("NotExpression", {"CompareExpression"}),
    Production("NotExpression", {"!", "CompareExpression"}),

    Production("CompareExpression", {"AdditiveExpression"}),
    Production("CompareExpression", {"Expression", "relop", "AdditiveExpression"}),

    // Production("Expression", {"AdditiveExpression", "RelopAdditiveExpression"}),

    // // RelopAdditiveExpression产生式
    // Production("RelopAdditiveExpression", {"relop", "AdditiveExpression"}),
    // Production("RelopAdditiveExpression", {"RelopAdditiveExpression", "relop", "AdditiveExpression"}),

    // Operator产生式
    Production("relop", {">"}),
    Production("relop", {"<"}),
    Production("relop", {">="}),
    Production("relop", {"<="}),
    Production("relop", {"=="}),
    Production("relop", {"!="}),

    // AdditiveExpression产生式
    Production("AdditiveExpression", {"Term"}),
    Production("AdditiveExpression", {"Term", "OperatorTerm"}),

    // OperatorTerm产生式
    Production("OperatorTerm", {"+", "Term"}),
    Production("OperatorTerm", {"-", "Term"}),
    Production("OperatorTerm", {"OperatorTerm", "+", "Term"}),
    Production("OperatorTerm", {"OperatorTerm", "-", "Term"}),

    // Term产生式
    Production("Term", {"Factor"}),
    Production("Term", {"Factor", "OperatorFactor"}),

    // OperatorFactor产生式
    Production("OperatorFactor", {"*", "Factor"}),
    Production("OperatorFactor", {"/", "Factor"}),
    Production("OperatorFactor", {"OperatorFactor", "*", "Factor"}),
    Production("OperatorFactor", {"OperatorFactor", "/", "Factor"}),

    // Factor产生式
    Production("Factor", {"dig"}),
    Production("Factor", {"(", "Expression", ")"}),
    Production("Factor", {"idt"}),
    Production("Factor", {"idt", "FTYPE"}),

    // FTYPE产生式
    Production("FTYPE", {"FunctionCall"}),

    // FunctionCall产生式
    Production("FunctionCall", {"(", "ActualParameters", ")"}),
    Production("FunctionCall", {"(", ")"}),

    // ActualParameters产生式
    Production("ActualParameters", {"ActualParameterList"}),

    // ActualParameterList产生式
    Production("ActualParameterList", {"Expression", "Expressions"}),
    Production("Expressions", {",", "Parameter", "Expressions"}),
    Production("Expressions", {",", "Parameter"}),

    Production("M", {""}),
    Production("N", {""}),
};

// 判断是否为非终结符
bool isNonTerminal(string symbol)
{
    return nonTerminals.find(symbol) != nonTerminals.end();
}

// 获取文法中的所有符号（终结符和非终结符）
void getTerminalAndNonTerminalSymbols(vector<Production> productions)
{
    // 两次遍历产生式集合，将所有符号分别加入到对应集合中
    for (const auto &production : productions)
    {
        // 将产生式左部（非终结符号）加入到符号集合中
        nonTerminals.insert(production.left);
    }
    for (const auto &production : productions)
    {
        // 将产生式右部的符号加入到符号集合中
        for (const auto &symbol : production.right)
        {
            if (nonTerminals.find(symbol) == nonTerminals.end())
            {
                terminals.insert(symbol);
            }
        }
    }

    return;
}

// 获取文法中的所有符号（终结符和非终结符）
unordered_set<string> getAllSymbols()
{
    unordered_set<string> symbols;

    // 遍历非终结符集，将所有符号加入到集合中
    for (const auto &symbol : nonTerminals)
    {
        symbols.insert(symbol);
    }

    // 遍历终结符集，将所有符号加入到集合中
    for (const auto &symbol : terminals)
    {
        symbols.insert(symbol);
    }

    return symbols;
}
// 获取全部项目，构建项目集
void getAllItem(vector<Production> productions)
{
    for (auto production = productions.begin(); production != productions.end(); production++)
    {
        //
        LR1Item ct;
        ct.production = *production;
        for (int count = 0; count <= production->right.size(); count++)
        {
            ct.dotIndex = count;
            ct.lookahead = "#"; //?????????????????????????
            LR1ItemSet.push_back(ct);
        }
    }

    // 输出检视
//    for (auto it = LR1ItemSet.begin(); it != LR1ItemSet.end(); it++)
//    {
//        cout << it->production.left << " -> ";
//        for (int count = 0; count < it->production.right.size(); count++)
//        {
//            if (it->dotIndex < it->production.right.size())
//            {
//                if (it->dotIndex == count)
//                {
//                    cout << ". " << it->production.right.at(count) << " ";
//                }
//                else
//                {
//                    cout << it->production.right.at(count) << " ";
//                }
//            }
//            else
//            {
//                cout << it->production.right.at(count) << " ";
//                if (count == it->production.right.size() - 1)
//                {
//                    cout << ". ";
//                }
//            }
//        }
//        cout << endl;
//    }
    return;
}

// Calculate First set for a given symbol
void calculateFirst()
{
    // Get the production rules associated with the symbol
    while (1)
    {
        bool flag = true;
        for (const auto &production : productions)
        {
            // Check the first symbol in the production rule
            string leftSymbol = production.left;
            string nextSymbol = production.right[0];
            set<string> firstSet;
            // If the next symbol is a terminal, add it to the First set
            if (!isNonTerminal(nextSymbol))
            {
                if (!First[leftSymbol].count(nextSymbol))
                {
                    firstSet.insert(nextSymbol);
                    flag = false;
                }
            }
            else
            {
                if (!First[nextSymbol].empty())
                {
                    for (auto it = First[nextSymbol].begin(); it != First[nextSymbol].end(); it++)
                    {
                        if (!First[leftSymbol].count(*it))
                        {
                            firstSet.insert(*it);
                            flag = false;
                        }
                    }
                }
            }
            if (!firstSet.empty())
            {
                First[leftSymbol].insert(firstSet.begin(), firstSet.end());
            }
        }
        if (flag)
        {
            break;
        }
    }

    // First[symbol].insert(firstSet.begin(), firstSet.end());
    return;
}

set<string> getFirstSet(string symbols)
{
    set<string> firstSet;
    int len = symbols.size();
    // for (auto x : terminals)
    //     cout << x << endl;
    firstSet.insert(First[symbols].begin(), First[symbols].end());

    return firstSet;
}

bool isItemExist(LR1Item curItem)
{
    // bool flag = false;
    for (auto it = LR1ItemSet.begin(); it != LR1ItemSet.end(); it++)
    {
        if (curItem == *it)
        {
            return true;
        }
    }
    return false;
}

bool isItemcurExistcanonical(int curNum, LR1Item curItem)
{
    for (auto it = canonicalCollection[curNum].begin(); it != canonicalCollection[curNum].end(); it++)
    {
        if (curItem == *it)
        {
            return true;
        }
    }
    return false;
}

void calGo(int i);

string calLookahead(LR1Item curItem, LR1Item lastItem)
{
    LR1Item tmp = curItem;

    string head;
    if (lastItem.dotIndex + 1 < lastItem.production.right.size())
    { // 有后续串+lookhead
        head = lastItem.production.right.at(lastItem.dotIndex + 1);
        if (isNonTerminal(head)) // 如果是非终结符
        {
            for (auto it = First[head].begin(); it != First[head].end(); it++)
            {
                tmp.lookahead = *it;
            }
        }
        else
        {
            tmp.lookahead = head;
        }
    }
    else
    { // 只有lookhead
        head = lastItem.lookahead;
        if (isNonTerminal(head)) // 如果是非终结符
        {
            for (auto it = First[head].begin(); it != First[head].end(); it++)
            {
                tmp.lookahead = *it;
            }
        }
        else
        {
            tmp.lookahead = head;
        }
    }

    return tmp.lookahead;
}

void enterprocess(int curNum, LR1Item tmp);

// 算闭包
int inclosure(int curNum, LR1Item curItem, LR1Item lastItem)
{
    Exist[curNum] = 1; // 当前族置存在

    LR1Item tmp = curItem;
    string head;
    if (lastItem.dotIndex + 1 < lastItem.production.right.size())
    { // 有后续串+lookhead
        head = lastItem.production.right.at(lastItem.dotIndex + 1);
        if (isNonTerminal(head)) // 如果是非终结符
        {
            for (auto it = First[head].begin(); it != First[head].end(); it++)
            {
                tmp.lookahead = *it;
                // 判断当前项目是否存在于当前项目族中
                enterprocess(curNum, tmp);
            }
        }
        else
        {
            tmp.lookahead = head;
            // 判断当前项目是否存在于当前项目族中
            enterprocess(curNum, tmp);
        }
    }
    else
    { // 只有lookhead
        head = lastItem.lookahead;
        if (isNonTerminal(head)) // 如果是非终结符
        {
            for (auto it = First[head].begin(); it != First[head].end(); it++)
            {
                tmp.lookahead = *it;
                // 判断当前项目是否存在于当前项目族中
                enterprocess(curNum, tmp);
            }
        }
        else
        {
            tmp.lookahead = head;
            // 判断当前项目是否存在于当前项目族中
            enterprocess(curNum, tmp);
        }
    }

    return 1;
}

void enterprocess(int curNum, LR1Item tmp)
{
    if (!isItemcurExistcanonical(curNum, tmp))
    {

        canonicalCollection[curNum].push_back(tmp); // 插入当前

        if (tmp.dotIndex < tmp.production.right.size())
        { // dot不在末尾
            if (isNonTerminal(tmp.production.right.at(tmp.dotIndex)))
            {                                                              // 为非终结符，继续拓展计算，且不等于左式，防止左递归
                string nextsymbol = tmp.production.right.at(tmp.dotIndex); // 下一个左式
                for (auto nextp = LR1ItemSet.begin(); nextp != LR1ItemSet.end(); nextp++)
                { // 在项目集中检索对应左式的表达式
                    if (nextp->production.left == nextsymbol && nextp->dotIndex == 0)
                    { // s->.AB
                        inclosure(curNum, *nextp, tmp);
                    }
                }
            }
        }
    }
    return;
}

// 算闭包2
int outclosure(int curNum, LR1Item curItem)
{
    Exist[curNum] = 1; // 当前族置存在

    LR1Item tmp = curItem;
    // tmp.lookahead = calLookahead(curItem, lastItem);
    // 判断当前项目是否存在于当前项目族中
    if (!isItemcurExistcanonical(curNum, tmp))
    {

        canonicalCollection[curNum].push_back(tmp); // 插入当前

        if (tmp.dotIndex < tmp.production.right.size())
        { // dot不在末尾
            if (isNonTerminal(tmp.production.right.at(tmp.dotIndex)))
            {                                                              // 为非终结符，继续拓展计算，且不等于左式，防止左递归
                string nextsymbol = tmp.production.right.at(tmp.dotIndex); // 下一个左式
                for (auto nextp = LR1ItemSet.begin(); nextp != LR1ItemSet.end(); nextp++)
                { // 在项目集中检索对应左式的表达式
                    if (nextp->production.left == nextsymbol && nextp->dotIndex == 0)
                    { // s->.AB
                        inclosure(curNum, *nextp, tmp);
                    }
                }
            }
        }
    }
    return 1;
}

// 查找某个项目所在的族号
int search_zone_number(LR1Item cur)
{
    for (int i = 0; i < canonicalCollection.size(); i++)
    {
        for (auto it = canonicalCollection[i].begin(); it != canonicalCollection[i].end(); it++)
        {
            if (cur == *it)
            {
                return i;
            }
        }
    }
    return -1;
}

// 查找某个表达式对应的项目
LR1Item search_item(Production pro)
{
    for (auto it = LR1ItemSet.begin(); it != LR1ItemSet.end(); it++)
    {
        if (it->production == pro)
        {
            return *it;
        }
    }
    LR1Item i;
    return i; //????
}

int nextZoneNumber()
{
    for (int i = 0; i < sizeof(Exist) - 1; i++)
    {
        if (Exist[i] == 0)
        {
            return i;
        }
    }
    return -1;
}

bool findVec(Go curgo)
{
    for (auto it = go.begin(); it != go.end(); it++)
    {
        if (*it == curgo)
        {
            return true;
        }
    }
    return false;
}
// v1包含v2
bool isSubset(vector<LR1Item> v1, vector<LR1Item> v2)
{
    // sort(v1.begin(), v1.end());
    // sort(v2.begin(), v2.end());
    // unordered_set<LR1Item>a1;
    // unordered_set<LR1Item>a2;
    // for (auto x : v1) {
    //     a1.insert(x);
    // }
    // for (auto x : v2) {
    //     a2.insert(x);
    // }
    // if (a1 == a2) {
    //     return true;
    // }
    // return false;
    int m = v1.size();
    int n = v2.size();
    if (m == n)
    {
        for (auto it2 = v2.begin(); it2 != v2.end(); it2++)
        {
            bool flag2 = false;
            for (auto it1 = v1.begin(); it1 != v1.end(); it1++)
            {
                if (*it2 == *it1)
                {
                    flag2 = true;
                    break;
                }
            }
            if (!flag2)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}
int checkBelongto(vector<LR1Item> v)
{
    for (int i = 0; i < canonicalCollection.size() - 1; i++)
    {
        if (isSubset(canonicalCollection[i], v))
        {
            return i;
        }
    }
    return -1;
}

void calGo()
{

    while (1)
    {
        bool flag = true;
        for (int i = 0; i < canonicalCollection.size(); i++)
        {
            for (auto go_symbol = G.begin(); go_symbol != G.end(); go_symbol++)
            { // 遍历符号表
                bool have = false;
                int nextNumber = nextZoneNumber();
                for (auto it = canonicalCollection[i].begin(); it != canonicalCollection[i].end(); it++)
                { // 这个循环里的生成新的一族
                    // 遍历当前族所有表达式
                    if (it->dotIndex < it->production.right.size() && it->production.right.at(it->dotIndex) == *go_symbol)
                    {
                        have = true;
                        LR1Item nextp = *it;
                        nextp.dotIndex = it->dotIndex + 1;

                        outclosure(nextNumber, nextp);
                    }
                }
                if (have)
                {
                    if (checkBelongto(canonicalCollection[nextNumber]) != -1)
                    {

                        Go tmp;
                        tmp.pre = i;
                        tmp.next = checkBelongto(canonicalCollection[nextNumber]);
                        tmp.exist = 1;
                        tmp.changeStr = *go_symbol;

                        if (!findVec(tmp))
                        {
                            go.push_back(tmp);
                        }

                        Exist[nextNumber] = 0;
                        canonicalCollection[nextNumber].clear();
                    }
                    else
                    {
                        flag = false;

                        Go tmp;
                        tmp.pre = i;
                        tmp.next = nextNumber;
                        tmp.exist = 1;
                        tmp.changeStr = *go_symbol;

                        if (!findVec(tmp))
                        {
                            go.push_back(tmp);
                        }
                    }
                }
            }
        }
        if (flag)
        {
            break;
        }
    }

    return;
}

vector<map<string, pair<int, int>>> ACTION;
vector<map<string, int>> GOTO;

void calAction()
{
    int size = canonicalCollection.size();
    ACTION.resize(size);
    for (int i = 0; i < size; i++)
    {
        for (auto item : canonicalCollection[i])
        {
            if (item.dotIndex < item.production.right.size())
            {
                string symbol = item.production.right[item.dotIndex];
                if (!isNonTerminal(symbol))
                {
                    // ACTION[i][symbol] = { 0, go[i]. };
                    for (auto x : go)
                    {
                        if (x.pre == i && x.changeStr == symbol)
                        {
                            ACTION[i][symbol] = {0, x.next};
                            break;
                        }
                    }
                }
            }
            else
            {
                for (int j = 0; j < productions.size(); j++)
                {
                    if (productions[j] == item.production)
                    {
                        if (item.production.left == "Start" && item.lookahead == "#")
                            ACTION[i][item.lookahead] = {1, -1};
                        else
                            ACTION[i][item.lookahead] = {1, j};
                        break;
                    }
                }
            }
        }
    }
}

void calGoto()
{
    int size = canonicalCollection.size();
    GOTO.resize(size);
    /*for (int i = 0; i < go.size(); i++)
    {
        for (auto x = go.begin(); x != go.end();x++)
        {
            if (isNonTerminal(x->pre))
                GOTO[i][x->pre] = x->next;
        }
    }*/
    for (auto x : go)
    {
        GOTO[x.pre][x.changeStr] = x.next;
    }
}

void init()
{
    getTerminalAndNonTerminalSymbols(productions);

    G = getAllSymbols();

    getAllItem(productions);

    calculateFirst();

    outclosure(0, *LR1ItemSet.begin());

    calGo();

    calAction();
    calGoto();

    return;
}
