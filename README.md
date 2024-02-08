# Tongji University's 2023 Fall Compilation-Principle Course Projects🌟🌟


- **Project 01** 完成的词法、语法分析器的工作过程为：先由词法分析器对源程序文件进行读取，解析单词符号为二元组形式，并将其送入语法分析器；语法分析器按照给定文法生成 *LR(1)* 规范项目集族，再生成 *ACTION* 表和 *GOTO* 表，利用两个表对给定输入串进行移进规约分析；由 *UI* 界面显示各项内容与移进规约的进栈出栈过程。

- **Project 02** 语义分析的任务是在语法分析的基础上，考虑代码的含义与功能，并将类 C 语言源代码转化成另一种更简洁、通用的中间代码的形式。常见的中间代码有逆波兰式、三元式、间按三元式、四元式等，本项目选用的是四元式🔥。在分析的过程中，还需要检查语义错误，如使用未定义变量、或变量重定义等错误。
