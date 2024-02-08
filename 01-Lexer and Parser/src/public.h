#ifndef PUBLIC_H
#define PUBLIC_H
#include <QTextBrowser>
#include <QTextStream>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <map>
#include <unordered_set>
#include <fstream>
#include "ui_mainwindow.h"

class TextBrowserStreambuf : public std::streambuf
{
public:
    TextBrowserStreambuf(QTextBrowser* textBrowser) : textBrowser_(textBrowser)
    {
    }

protected:
    virtual int_type overflow(int_type c = traits_type::eof())
    {
        if (c != traits_type::eof())
        {
            char ch = traits_type::to_char_type(c);
            textBrowser_->moveCursor(QTextCursor::End);
            textBrowser_->insertPlainText(QString(ch));
        }
        return c;
    }

private:
    QTextBrowser* textBrowser_;
};


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::vector<std::string> read();//读文件名

    int main1(TextBrowserStreambuf *textBrowserStreambuf_state,TextBrowserStreambuf* textBrowserStreambuf_symbol,TextBrowserStreambuf* textBrowserStreambuf_input,std::vector<std::string> input);
    void process(const std::vector<std::string> &input,TextBrowserStreambuf *textBrowserStreambuf_state,TextBrowserStreambuf* textBrowserStreambuf_symbol,TextBrowserStreambuf* textBrowserStreambuf_input);


private:
    Ui::MainWindow *ui;
    std::vector<std::string> input_text = {};//读文件时用
};



#endif // PUBLIC_H
