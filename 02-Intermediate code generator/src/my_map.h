#pragma once
#include <bits/stdc++.h>
#include <unordered_set>
using namespace std;

const vector<string> key_word = {"int", "void", "if", "else", "while", "return"};

// 定义文法产生式结构体
struct Production
{
    string left;          // 左部
    vector<string> right; // 右部

    Production()
    {
    }
    Production(string l, vector<string> r) : left(l), right(r) {}

    bool operator==(const Production &other) const
    {
        return (
            left == other.left &&
            right == other.right);
    }
};

enum TOKENCODE
{
    TK_UNDEF = 0,
    // 关键字
    KW_INT,
    KW_VOID,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_RETURN,

    // 算符
    TK_PLUS,   // '+'  plus
    TK_MINUS,  // '-'  minus
    TK_STAR,   // '*'  star
    TK_DIV,    // '/'  divide
    TK_ASSIGN, // '='  assign
    TK_EQ,     // "==" equal
    TK_GT,     // '>'  greater than
    TK_GEQ,    // ">=" greater than and equal
    TK_LT,     // '<'  less than
    TK_LEQ,    // "<=" less than and equal
    TK_NEQ,    // "!=" not equal

    // bool
    TK_NOT, // "!"  not
    TK_AND, // "&&" and
    TK_OR,  // "||" or

    // 分隔符
    TK_SEM,    // ';'  semicolon
    TK_COM,    // ','  comma
    TK_DSLASH, // "//" double slash
    TK_LSLASH, // "/*" left slash
    TK_RSLASH, // "*/" right slash
    TK_LPA,    // '('  left parentheses
    TK_RPA,    // ')'  right parentheses
    TK_LBRK,   // '['  left bracket
    TK_RBRK,   // ']'  right bracket
    TK_LBRC,   // '{'  left brace
    TK_RBRC,   // '}'  right brace

    // 标识符
    TK_IDENT, // 自定义变量名、函数名等

    // 数字
    TK_DIG, //  0-9 digit

    // 结束符
    TK_END, // '#'

    // 辅助符号
    EPSILON,
};

const map<string, TOKENCODE> token_mp = {
    {"+", TK_PLUS},
    {"-", TK_MINUS},
    {"*", TK_STAR},
    {"/", TK_DIV},
    {"!=", TK_NEQ},
    {">", TK_GT},
    {">=", TK_GEQ},
    {"<", TK_LT},
    {"<=", TK_LEQ},
    {"=", TK_ASSIGN},
    {"==", TK_EQ},
    {"#", TK_END},
    {"//", TK_DSLASH},
    {"/*", TK_LSLASH},
    {"*/", TK_RSLASH},
    {",", TK_COM},
    {";", TK_SEM},
    {"(", TK_LPA},
    {")", TK_RPA},
    {"[", TK_LBRK},
    {"]", TK_RBRK},
    {"{", TK_LBRC},
    {"}", TK_RBRC},
    {"idt", TK_IDENT},
    {"dig", TK_DIG},
    {"int", KW_INT},
    {"void", KW_VOID},
    {"if", KW_IF},
    {"else", KW_ELSE},
    {"while", KW_WHILE},
    {"return", KW_RETURN},
    {"", EPSILON},
    {"!", TK_NOT},
    {"||", TK_OR},
    {"&&", TK_AND},
};

const map<int, string> mp = {
    {9, "declaration"},
    {15, "statements"},
    {21, "if"},
    {22, "if_else"},
    {23, "while"},
    {26, "assign"},
    // {27, "bool_expression"},
    // {28, "expression"},
    {31, "or"},
    {33, "and"},
    {35, "not"},
    {36, "bool"},
    {37, "relop"},
    {45, "cal_merge"},
    {48, "cal_merge"},
    {49, "cal_merge"},
    {51, "cal_merge"},
    {54, "cal_merge"},
    {55, "cal_merge"},
    {46, "operator"},
    {47, "operator"},
    {52, "operator"},
    {53, "operator"},
    {56, "dig"},
    {58, "idt"},
    {10, "bracket"},
    {11, "bracket"},
    {57, "bracket"},
    {67, "M"},
    {68, "N"},
};

struct Token_table
{
    int line;
    string type;
    string token;
};

//void process(const vector<Token_table> &input);

void debug();

void init();

extern vector<Production> productions;
extern vector<map<string, pair<int, int>>> ACTION;
extern vector<map<string, int>> GOTO;

extern unordered_set<string> G;
