#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QMediaPlayer>

struct Chess
{
    // 除了select one self 需要改变外，存储固定32个棋子的数组中对于每个妻子的性质不进行更新
    int kind;
    bool color;
    int which;

    // 棋子被移动或被吃掉时会进行更新，（可能以后会用到）
    int x;
    int y;
};

struct Coordinates_Status
{
    int x;
    int y;
    bool have;
    int which;
};

struct Trace
{
    int x;
    int y;
    bool exist;
};


class ChessBoard : public QLabel
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent * ev);
    void paintTrace();
    void mousePressEvent(QMouseEvent *ev);
    void initBoard();

    int findposx(int tempx);
    int findposy(int tempy);

    bool can_jump(int player, Chess self, int x , int y);
    void move(int xfrom, int yfrom, int xto, int yto);
    void select_this(int posx, int posy);

    QString saveCurBoardToString();
    void saveCurBoardToFile(QString filename);
    void loadBoardFromString(const QString &curBoard);
    void loadBoardFromFile(const QString &filename);

    void beginGame();
    void makeSound();

signals:
    void signalMove(int, int, int, int);
    void signalLose();
    void signalWin();


public slots:

public:
    int player;
    int Success_Or_Lose;
    Coordinates_Status chess_Cor_Status[9][10];
    Chess chess[32];
    int whoOperator;

private:
    QPixmap pic_board;
    QPixmap blackChessPieces[7];
    QPixmap redChessPieces[7];


    Trace trace[2];

    //int select_one_self;
    Chess select_one_self;

    QMediaPlayer *mplayer;


};

#endif // CHESSBOARD_H
