#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constant.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(MainWindow_WIDTH, MainWindow_HEIGHT);

    qDebug() <<MainWindow_WIDTH << " " <<  MainWindow_HEIGHT ;

    chess_board = new ChessBoard(this);
    createDialog = new CreateDialog(this);
    joinDialog = new JoinDialog(this);

    initMenu();
    initData();


    connect(chess_board, &ChessBoard::signalMove, this, &MainWindow::netWriteMove);
    connect(&timer2, &QTimer::timeout, [=](){
        totalSecond --;
        ui->lcdNumber->display(totalSecond);
        if(totalSecond == 0){
            if( chess_board->player == chess_board->whoOperator)
                chess_board->signalLose();
            timer2.stop();
        }
    });
    connect(chess_board, &ChessBoard::signalLose,[=](){ netWriteLose(); });
    connect(chess_board, &ChessBoard::signalWin,[=](){ netWriteWin(); });

    timer1.start(50);
    connect(&timer1, &QTimer::timeout, [=](){
        qDebug() << "chess_board->Success_Or_Lose: " << chess_board->Success_Or_Lose;
        if(chess_board->Success_Or_Lose == 1){
            timer1.stop();
            timer2.stop();
            chess_board->signalWin();
            QMessageBox::information(this, "WIN","你赢了本场对战");
        }
        if(chess_board->Success_Or_Lose == -1){
            timer1.stop();
            timer2.stop();
            chess_board->signalLose();
            QMessageBox::information(this, "LOSE","你输了本场对战");
        }
    });

    connect(ui->pushButton, &QPushButton::clicked, chess_board, &ChessBoard::signalLose);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow::closeEvent(QCloseEvent *e)
{
    int ret = QMessageBox::question(this, tr("提示信息"), tr("是否退出游戏？"), QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
    {

        e->accept();
        this->close();
    }
    else
    {
        e->ignore();
    }
}*/

void MainWindow::initMenu(){
    //ui->actionExit->setEnabled(false);

    ui->actionAdmit_Lose->setEnabled(false);
    ui->actionAsk_for_peace->setEnabled(false);
   // ui->actionOverturn_ChessBoard->setEnabled(false);

    ui->actionCurrent_Setting->setEnabled(false);
}

void MainWindow::initData(){
    waitTime = 30;
    Suc_Or_Lose = 0;

    ui->label->setText("游戏未开始");
}


void MainWindow::on_actionNew_triggered()
{
    chess_board->initBoard();
}

void MainWindow::on_actionOpen_triggered()
{
    auto filename = QFileDialog::getOpenFileName(this,
                                                 tr(u8"打开残局"),
                                                 "",
                                                 tr(u8"文本文件 (*.txt)"));
    if (!filename.isEmpty())
        chess_board->loadBoardFromFile(filename);

}

void MainWindow::on_actionSave_triggered()
{
    auto filename = QFileDialog::getSaveFileName(this,
                                                 tr(u8"保存当前局面"),
                                                 "",
                                                 tr(u8"文本文件 (*.txt)"));
    if (!filename.isEmpty())
        chess_board->saveCurBoardToFile(filename);
}

void MainWindow::on_actionExit_triggered()
{

    this->close();
}


void MainWindow::on_actionAdmit_Lose_triggered()
{

}

void MainWindow::on_actionAsk_for_peace_triggered()
{

}

void MainWindow::on_actionOverturn_ChessBoard_triggered()
{
    chess_board->makeSound();
}


void MainWindow::on_actionCreate_room_triggered()
{
    if(createDialog->exec() == QDialog::Accepted){
        // socket 建立
        socket = createDialog->socket;

        chess_board->Success_Or_Lose = 0;


        timer1.start();
        // 更新当前服务器端属性
        chess_board->player = createDialog->ui->Select_Black_Red->currentIndex();
        waitTime = createDialog->ui->setWaitTime->text().toInt();

        // 连接槽函数，此时链接位置对server端影响不大，对 client 影响很大，因为要在
        //connect(socket, &QTcpSocket::readyRead, this, &MainWindow::netRead);

        // 更改当前菜单栏选项属性
        ui->actionNew->setEnabled(true);
        ui->actionOpen->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->actionAdmit_Lose->setEnabled(true);
        ui->actionAsk_for_peace->setEnabled(true);
        ui->actionOverturn_ChessBoard->setEnabled(true);
        ui->actionCurrent_Setting->setEnabled(true);

        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::netRead);
        netWriteServer();
        // 发出连接成功提示
        QMessageBox::information(this,
                                 tr(u8"Battle!"),
                                 tr(u8"连接成功！你将与IP为%1的用户对战").arg(socket->peerAddress().toString()));

    }
}

void MainWindow::on_actionJoin_room_triggered()
{
    if(joinDialog->exec() == QDialog::Accepted){
        socket = joinDialog->socket;


        chess_board->Success_Or_Lose = 0;
        timer1.start();
         // 更改当前菜单栏选项属性
        ui->actionNew->setEnabled(true);
        ui->actionOpen->setEnabled(true);
        ui->actionSave->setEnabled(true);
        //ui->actionExit->setEnabled(true);
        ui->actionAdmit_Lose->setEnabled(true);
        ui->actionAsk_for_peace->setEnabled(true);
        ui->actionOverturn_ChessBoard->setEnabled(true);
        ui->actionCurrent_Setting->setEnabled(true);

        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::netRead);

        // 发出连接成功提示
        QMessageBox::information(this,
                                 tr(u8"Battle!"),
                                 tr(u8"连接成功！你将与IP为%1的用户对战").arg(socket->peerAddress().toString()));
    }
}


void MainWindow::on_actionCurrent_Setting_triggered()
{

}

void MainWindow::on_actionHelp_triggered()
{

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(NULL,  tr("关于本软件"), tr("Chinese Chess Version 0.1.0 by Stout"));
}



void MainWindow::netRead() {
    auto data = socket->readAll();
    QDataStream st(&data, QIODevice::ReadOnly);
    while (!st.atEnd()) {
        int op;
        st >> op;
        if (op == 0)
            netReadServer(st);
        else if (op == 1)
            netReadMove(st);
        else if (op == 2)
            netReadLose(st);
        else if (op == 3)
            netReadWin(st);
    }
}

void MainWindow::netWriteServer(){

    QByteArray data;
    QDataStream st(&data, QIODevice::WriteOnly);

    st << 0;
    st << chess_board->player;
    st << waitTime;
    st << chess_board->whoOperator;
    st << chess_board->saveCurBoardToString();

    socket->write(data);
    socket->waitForBytesWritten();
    showCurrentOperator();
    resetTimerAndJudge();

    ui->actionAdmit_Lose->setEnabled(true);
    ui->actionCurrent_Setting->setEnabled(true);
}

void MainWindow::netReadServer(QDataStream &st){
    int opponentPlayer = 0;
    st >> opponentPlayer;
    if(opponentPlayer == 0)
        chess_board->player = 1;
    else
        chess_board->player = 0;

    st >> waitTime;
    int tempOpera;
    st >> tempOpera;

    QString currentBoard;
    st >> currentBoard;

    chess_board->loadBoardFromString(currentBoard);

    chess_board->whoOperator = tempOpera;
    showCurrentOperator();
    resetTimerAndJudge();
    ui->actionAdmit_Lose->setEnabled(true);
    ui->actionCurrent_Setting->setEnabled(true);
}

void MainWindow::netWriteMove(int xfrom, int yfrom, int xto, int yto){
    QByteArray data;
    QDataStream st(&data, QIODevice::WriteOnly);

    st << 1 << xfrom << yfrom << xto << yto;

    socket->write(data);
    socket->waitForBytesWritten();
    showCurrentOperator();
    resetTimerAndJudge();
}

void MainWindow::netReadMove(QDataStream &st){
    int xfrom, yfrom, xto, yto;
    st >> xfrom >> yfrom >> xto >> yto;
    chess_board->move(xfrom, yfrom, xto, yto);
    showCurrentOperator();
    resetTimerAndJudge();
}

void MainWindow::netWriteLose(){
    chess_board->Success_Or_Lose = -1;
    QByteArray data;
    QDataStream st(&data, QIODevice::WriteOnly);

    st << 2;

    socket->write(data);
    socket->waitForBytesWritten();

}

void MainWindow::netReadLose(QDataStream &st){
    chess_board->Success_Or_Lose = 1;
}

void MainWindow::netWriteWin(){
    chess_board->Success_Or_Lose = 1;
    QByteArray data;
    QDataStream st(&data, QIODevice::WriteOnly);

    st << 3;

    socket->write(data);
    socket->waitForBytesWritten();
}

void MainWindow::netReadWin(QDataStream &st){
    chess_board->Success_Or_Lose = -1;
}

void MainWindow::showCurrentOperator(){
    QString opera;
    if(chess_board->whoOperator == 0)
        opera = "黑方操作";
    else
        opera = "红方操作";
    ui->label->setText(opera);
}

void MainWindow::resetTimerAndJudge(){
    timer2.stop();
    timer2.start(1000);

    totalSecond = waitTime;
    ui->lcdNumber->display(totalSecond);
}

void MainWindow::on_pushButton_clicked()
{
     chess_board->Success_Or_Lose = -1;
}
