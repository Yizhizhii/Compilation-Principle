#pragma once
//#include <bits/stdc++.h>
//#include <iostream>

#include "public.h"
using namespace std;

const vector<string> key_word = {"int", "void", "if", "else", "while", "return"};

enum TOKENCODE
{
    TK_UNDEF = 0,
    // �ؼ���
    KW_INT,
    KW_VOID,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_RETURN,

    // ���
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

    // �ָ���
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

    // ��ʶ��
    TK_IDENT, // �Զ������������������

    // ����
    TK_DIG, //  0-9 digit

    // ������
    TK_END, // '#'
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
};

void debug();

void init();
