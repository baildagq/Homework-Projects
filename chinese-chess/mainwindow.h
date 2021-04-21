#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chessboard.h"
#include "createdialog.h"
#include "ui_createdialog.h"
#include "joindialog.h"
#include "ui_joindialog.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//    void closeEvent(QCloseEvent *e);

    void initServer();

    void initMenu();

    void initData();

    void netRead();

    void netWriteServer();
    void netReadServer(QDataStream &st);

    void netWriteMove(int xfrom, int yfrom, int xto, int yto);
    void netReadMove(QDataStream &st);

    void netWriteLose();
    void netReadLose(QDataStream &st);

    void netWriteWin();
    void netReadWin(QDataStream &st);
    void showCurrentOperator();
    void resetTimerAndJudge();

signals:

private slots:
    void on_actionNew_triggered();

    void on_actionAbout_triggered();

    void on_actionCreate_room_triggered();

    void on_actionJoin_room_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void on_actionAsk_for_peace_triggered();

    void on_actionOverturn_ChessBoard_triggered();

    void on_actionHelp_triggered();

    void on_actionAdmit_Lose_triggered();

    void on_actionCurrent_Setting_triggered();


    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    ChessBoard *chess_board;

    QTcpSocket *socket;

    CreateDialog *createDialog;
    JoinDialog *joinDialog;

    int waitTime;

    int Suc_Or_Lose;
    QTimer timer1; // to judge weather succeed or lose
    QTimer timer2; // to count the time
    int totalSecond;
};

#endif // MAINWINDOW_H
