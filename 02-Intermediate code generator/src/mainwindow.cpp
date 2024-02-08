#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QTimer>
#include <QVBoxLayout>

extern vector<Code> codes;

extern vector<string> declarations;

extern vector<string> vars;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //删除中间窗口
    QWidget* p = takeCentralWidget();
    if(p)
        delete p;
    setDockNestingEnabled(true);//打开dock嵌套

    input_ = new QTextBrowser();
    state = new QTextBrowser();
    symbol = new QTextBrowser();
    code = new QTextBrowser();
    Linput = new QLabel("输入串");
    Lstate = new QLabel("语法状态栈");
    Lsymbol = new QLabel("语法符合栈");
    Lcode = new QLabel("中间代码");

    //dock界面初始化
    dockInit();

    // 创建自定义的 streambuf，并重定向 cout
    TextBrowserStreambuf textBrowserStreambuf_state(state);
    TextBrowserStreambuf textBrowserStreambuf_symbol(symbol);
    TextBrowserStreambuf textBrowserStreambuf_input(input_);
    TextBrowserStreambuf textBrowserStreambuf_code(code);

    std::streambuf* oldCoutStreambuf = std::cout.rdbuf(&textBrowserStreambuf_state);

    myConnect();//连接槽函数

    one_used = false;
    QObject::connect(portBtn_one,&QPushButton::clicked,[this,textBrowserStreambuf_state,textBrowserStreambuf_symbol,textBrowserStreambuf_input,textBrowserStreambuf_code]()mutable{
        if(one_used==false){
            one_used = true;    
            main1(&textBrowserStreambuf_state,&textBrowserStreambuf_symbol,&textBrowserStreambuf_input,&textBrowserStreambuf_code,false);
        }
    });
    QObject::connect(portBtn_run,&QPushButton::clicked,[this,textBrowserStreambuf_state,textBrowserStreambuf_symbol,textBrowserStreambuf_input,textBrowserStreambuf_code]()mutable{
        if(one_used==false){
            qDebug()<<"1:"<<"one_used:"<<one_used;
            main1(&textBrowserStreambuf_state,&textBrowserStreambuf_symbol,&textBrowserStreambuf_input,&textBrowserStreambuf_code,true);
        }
    });


    // 恢复 cout 原来的 streambuf
    std::cout.rdbuf(oldCoutStreambuf);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete leftdock;
    delete rightdock;
}

void MainWindow::dockInit(){
    //左侧dock设置
    leftdock = new QDockWidget;
    leftdock->setObjectName("leftdock");

    //删除标题栏
    QWidget* lTitleBar = leftdock->titleBarWidget();
    QWidget* lEmptyWidget = new QWidget();
    leftdock->setTitleBarWidget(lEmptyWidget);
    delete lTitleBar;

    textEdit = new QTextEdit(leftdock);//创建textedit
    textEdit->setPlaceholderText("你的代码");
    QVBoxLayout *layout = new QVBoxLayout(leftdock);//将text添加到leftdock
    layout->addWidget(textEdit);
    // 设置布局管理器
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    leftdock->setWidget(widget);

    //右侧dock设置
    rightdock = new QDockWidget;
    rightdock->setObjectName("rightdock");
    rightdock->setWindowTitle("编译结果");

    //语法分析
    portBtn_yufa = new QPushButton(this);
    portBtn_yufa->setIcon(QIcon("./icon/yufa.png"));	//按钮添加图片
    portBtn_yufa->setToolTip("语法分析");			//鼠标移动到按钮上时显示提示
    portBtn_yufa->setIconSize(QSize(30,30));
    //中间代码
    portBtn_code = new QPushButton(this);
    portBtn_code->setIcon(QIcon("./icon/code.png"));	//按钮添加图片
    portBtn_code->setToolTip("中间代码");			//鼠标移动到按钮上时显示提示
    portBtn_code->setIconSize(QSize(30,30));

    show_yufa();

    //添加dockwdget
    this->addDockWidget(Qt::LeftDockWidgetArea, leftdock);
    this->addDockWidget(Qt::RightDockWidgetArea, rightdock);
    splitDockWidget(leftdock,rightdock,Qt::Horizontal);

    connect(portBtn_yufa, SIGNAL(clicked()), this, SLOT(show_yufa()));
    connect(portBtn_code, SIGNAL(clicked()), this, SLOT(show_code()));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    int width1 = int(this->width()*0.7);//左边的停靠窗宽是主界面的0.15倍
    int width2 = int(this->width()*0.3);//右边的停靠窗宽是主界面的0.85倍
    this->resizeDocks({leftdock,rightdock},{width1,width2},Qt::Horizontal);//设置左右水平布局
}

//连接信号和槽函数
void MainWindow::myConnect()
{
    QSize iconSize(50, 50);  // 设置图标的宽度和高度
    //工具栏添加图片
    QPushButton *portBtn_new = new QPushButton(this);
    portBtn_new->setIcon(QIcon("./icon/new.png"));	//按钮添加图片
    portBtn_new->setToolTip("新建");			//鼠标移动到按钮上时显示提示
    portBtn_new->setIconSize(iconSize);

    QPushButton *portBtn_open = new QPushButton(this);
    portBtn_open->setIcon(QIcon("./icon/open.png"));	//按钮添加图片
    portBtn_open->setToolTip("打开");			//鼠标移动到按钮上时显示提示
    portBtn_open->setIconSize(iconSize);

    QPushButton *portBtn_save = new QPushButton(this);
    portBtn_save->setIcon(QIcon("./icon/save.png"));	//按钮添加图片
    portBtn_save->setToolTip("保存");			//鼠标移动到按钮上时显示提示
    portBtn_save->setIconSize(iconSize);

    QPushButton *portBtn_save2 = new QPushButton(this);
    portBtn_save2->setIcon(QIcon("./icon/save2.png"));	//按钮添加图片
    portBtn_save2->setToolTip("另存为");			//鼠标移动到按钮上时显示提示
    portBtn_save2->setIconSize(iconSize);

    portBtn_run = new QPushButton(this);
    portBtn_run->setIcon(QIcon("./icon/runAll.png"));	//按钮添加图片
    portBtn_run->setToolTip("全部运行");			//鼠标移动到按钮上时显示提示
    portBtn_run->setIconSize(iconSize);

    portBtn_one = new QPushButton(this);
    portBtn_one->setIcon(QIcon("./icon/oneStep.png"));	//按钮添加图片
    portBtn_one->setToolTip("单步执行");			//鼠标移动到按钮上时显示提示
    portBtn_one->setIconSize(iconSize);

    QPushButton *portBtn_quit = new QPushButton(this);
    portBtn_quit->setIcon(QIcon("./icon/quit.png"));	//按钮添加图片
    portBtn_quit->setToolTip("退出");			//鼠标移动到按钮上时显示提示
    portBtn_quit->setIconSize(iconSize);

    QPushButton *portBtn_help = new QPushButton(this);
    portBtn_help->setIcon(QIcon("./icon/help.png"));	//按钮添加图片
    portBtn_help->setToolTip("帮助");			//鼠标移动到按钮上时显示提示
    portBtn_help->setIconSize(iconSize);

    ui->mainToolBar->addWidget(portBtn_new);
    ui->mainToolBar->addWidget(portBtn_open);
    ui->mainToolBar->addSeparator(); //添加分 隔符
    ui->mainToolBar->addWidget(portBtn_save);
    ui->mainToolBar->addWidget(portBtn_save2);
    ui->mainToolBar->addSeparator(); //添加分 隔符
    ui->mainToolBar->addWidget(portBtn_run);
    ui->mainToolBar->addWidget(portBtn_one);
    ui->mainToolBar->addSeparator(); //添加分 隔符
    ui->mainToolBar->addWidget(portBtn_quit);
    ui->mainToolBar->addWidget(portBtn_help);//根据工具栏添加方式选择添加按钮方式
    ui->mainToolBar->addSeparator(); //添加分 隔符

    connect(portBtn_new, SIGNAL(clicked()), this, SLOT(newFile()));//打开文件
    connect(portBtn_open, SIGNAL(clicked()), this, SLOT(read()));//打开文件
    connect(portBtn_save, SIGNAL(clicked()), this, SLOT(save()));//保存
    connect(portBtn_save2, SIGNAL(clicked()), this, SLOT(save2()));//另存为
    connect(portBtn_quit, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));//退出
    connect(portBtn_help, SIGNAL(clicked()), this, SLOT(help()));//帮助
}

//help按钮槽函数
void MainWindow::help(){
    QMessageBox::aboutQt(this,tr("关于Qt"));
}

//新建文件槽函数
void MainWindow::newFile(){
    // 新建文件名
    QFileDialog fileDialog(nullptr, "新建文件", "", "(*.cpp);;所有文件 (*)");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);

    // 显示文件对话框
    if (fileDialog.exec() == QDialog::Accepted) {
        // 获取用户选择的文件路径
        QString filePath = fileDialog.selectedFiles().first();
        fileName = filePath.toStdString();
        // 创建一个文件对象
        QFile file(filePath);

        textEdit->setPlaceholderText("新建文件成功！");
    }
}

//读文件槽函数
void MainWindow::read(){
    fileName = QFileDialog::getOpenFileName(this,"","").toStdString();
    string content;

    ifstream fin(fileName, ios::in);
    if (!fin.is_open())
    {
        QMessageBox::warning(this,"Error",QString::fromStdString(fileName));
//        qDebug()<<QString::fromStdString(fileName);
        QMessageBox::warning(this,"Error",QStringLiteral("can't open the file!"));
    }
    else{
        stringstream buffer;
        buffer << fin.rdbuf(); // 将文件流的缓冲区内容读入 stringstream
        content = buffer.str();
        textEdit->setPlaceholderText("你的代码");
        textEdit->setText(QString::fromStdString(content));

        codes.resize(0);
        vars.resize(0);
        declarations.resize(0);
        one_used = false;
    }
    fin.close();
}

//保存槽函数
void MainWindow::save(){
    // 创建一个文件对象
    QFile file(QString::fromStdString(fileName));
    // 打开文件，写入内容
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 写入需要保存的内容
        out << textEdit->toPlainText();

        // 关闭文件
        file.close();
        QMessageBox::information(this,"info",QStringLiteral("save!"));
    }
}

//另存为槽函数
void MainWindow::save2(){
    QFileDialog fileDialog(nullptr, "保存文件", "", "(*.cpp);;所有文件 (*)");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);

    // 显示文件对话框
    if (fileDialog.exec() == QDialog::Accepted) {
        // 获取用户选择的文件路径
        QString filePath = fileDialog.selectedFiles().first();

        // 创建一个文件对象
        QFile file(filePath);

        // 打开文件，写入内容
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // 写入需要保存的内容
            out << textEdit->toPlainText();

            // 关闭文件
            file.close();
        }
    }
}

//语法分析界面槽函数
void MainWindow::show_yufa()
{
    // 设置按钮水平布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(portBtn_yufa);
    buttonLayout->addWidget(portBtn_code);
    // 设置按钮和文本浏览器的垂直布局
    QVBoxLayout *layout_right = new QVBoxLayout(rightdock);
    layout_right->addLayout(buttonLayout);
    layout_right->addWidget(Lstate);
    layout_right->addWidget(state);
    layout_right->addWidget(Lsymbol);
    layout_right->addWidget(symbol);
    layout_right->addWidget(Linput);
    layout_right->addWidget(input_);
    // 创建小部件并设置布局
    QWidget *widget1 = new QWidget();
    widget1->setLayout(layout_right);
    rightdock->setWidget(widget1);
//    rightdock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
}

//中间代码界面槽函数
void MainWindow::show_code()
{
    // 设置按钮水平布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(portBtn_yufa);
    buttonLayout->addWidget(portBtn_code);
    // 设置按钮和文本浏览器的垂直布局
    QVBoxLayout *layout_right = new QVBoxLayout(rightdock);
    layout_right->addLayout(buttonLayout);
    layout_right->addWidget(Lcode);
    layout_right->addWidget(code);
    // 创建小部件并设置布局
    QWidget *widget1 = new QWidget();
    widget1->setLayout(layout_right);
    rightdock->setWidget(widget1);
}

//打开文件槽函数
void MainWindow::on_actionOpen_triggered()
{
    read();
}

//新建文件槽函数
void MainWindow::on_actionNew_triggered()
{
    newFile();
}

//保存文件槽函数
void MainWindow::on_actionSave_triggered(){
    save();
}

//另存为文件槽函数
void MainWindow::on_actionSave2_triggered(){
    save2();
}

//关于Qt槽函数
void MainWindow::on_actionQt_triggered()
{
    QMessageBox::aboutQt(this,tr("关于Qt"));
}

//工具栏槽函数
void MainWindow::on_actionTool_triggered(bool checked){
    if(checked)
    {
        ui->mainToolBar->show();
    }
    else
    {
        ui->mainToolBar->hide();
    }
}

//显示编译结果界面槽函数
void MainWindow::on_actionBian_triggered(bool checked){
    if(checked)
    {
        rightdock->show();
    }
    else
    {
        rightdock->hide();
    }
}

//关于作者槽函数
void MainWindow::on_actionAuthor_triggered()
{
    QMessageBox box;
    box.setWindowTitle(tr("关于制作者"));
    box.setIcon(QMessageBox::Information);
    box.setText(tr("Author : TJ \n"
                   "编译原理大作业 \n"));
    box.addButton(tr("确定"),QMessageBox::AcceptRole);
    if(box.exec() == QMessageBox::Accepted)
        box.close();
}

//动作关闭程序槽函数
void MainWindow::on_actionClose_triggered()
{
    close();
}
