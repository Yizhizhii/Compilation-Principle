#include "mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTextBrowser *state = ui->state;//显示状态
    QTextBrowser *symbol = ui->symbol;//显示状态
    QTextBrowser *input = ui->input;//显示状态

    // 创建自定义的 streambuf，并重定向 cout
    TextBrowserStreambuf textBrowserStreambuf_state(state);
    TextBrowserStreambuf textBrowserStreambuf_symbol(symbol);
    TextBrowserStreambuf textBrowserStreambuf_input(input);

    std::streambuf* oldCoutStreambuf = std::cout.rdbuf(&textBrowserStreambuf_state);

    QObject::connect(ui->getFile,&QPushButton::clicked,[this]()mutable{
        input_text=read();
    });

    QObject::connect(ui->start,&QPushButton::clicked,[this,textBrowserStreambuf_state,textBrowserStreambuf_symbol,textBrowserStreambuf_input]()mutable{
        main1(&textBrowserStreambuf_state,&textBrowserStreambuf_symbol,&textBrowserStreambuf_input,input_text);
    });


    // 恢复 cout 原来的 streambuf
    std::cout.rdbuf(oldCoutStreambuf);
}

MainWindow::~MainWindow()
{
    delete ui;
}
