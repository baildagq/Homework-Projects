#include "chessboard.h"
#include "constant.h"
#include <QDebug>
#include <QPen>
#include <QTextCodec>
#include <QMessageBox>
#include <QUrl>

ChessBoard::ChessBoard(QWidget *parent) : QLabel(parent)
{
    resize(CHESSBOARD_WIDTH, CHESSBOARD_HEIGHT);
    qDebug() <<CHESSBOARD_WIDTH << " " <<  CHESSBOARD_HEIGHT ;
    QLabel::move(CHESSBOARD_STARTX, CHESSBOARD_STARTY);
    initBoard();
}


void ChessBoard::initBoard(){
    mplayer = new QMediaPlayer;
    mplayer->setMedia(QUrl("qrc:/audio/check.mp3"));


    pic_board.load("://image/image/background.png");

    blackChessPieces[0].load("://image/image/black_0");
    blackChessPieces[1].load("://image/image/black_1");
    blackChessPieces[2].load("://image/image/black_2");
    blackChessPieces[3].load("://image/image/black_3");
    blackChessPieces[4].load("://image/image/black_4");
    blackChessPieces[5].load("://image/image/black_5");
    blackChessPieces[6].load("://image/image/black_6");

    redChessPieces[0].load("://image/image/red_0");
    redChessPieces[1].load("://image/image/red_1");
    redChessPieces[2].load("://image/image/red_2");
    redChessPieces[3].load("://image/image/red_3");
    redChessPieces[4].load("://image/image/red_4");
    redChessPieces[5].load("://image/image/red_5");
    redChessPieces[6].load("://image/image/red_6");

    for(int i = 0;i < 9;i ++){
        for(int j = 0;j < 10;j ++){
            chess_Cor_Status[i][j].x = BOARD_STARTX + i * squareWidth;
            chess_Cor_Status[i][j].y = BOARD_STARTY + j * squareWidth;
            chess_Cor_Status[i][j].have = false;
            chess_Cor_Status[i][j].which = -1;
        }
    }

    int temp = 0;
    //black
    for(int j = 0;j < 9;j ++){
        chess_Cor_Status[j][0].which = temp; temp ++;
        chess_Cor_Status[j][0].have = true;
    }
    chess_Cor_Status[1][2].which = temp; temp ++;
    chess_Cor_Status[1][2].have = true;
    chess_Cor_Status[7][2].which = temp; temp ++;
    chess_Cor_Status[7][2].have = true;
    for(int j = 0;j < 9;j += 2){
        chess_Cor_Status[j][3].which = temp; temp ++;
        chess_Cor_Status[j][3].have = true;
    }

    // red
    for(int j = 0;j < 9;j += 2){
        chess_Cor_Status[j][9 - 3].which = temp; temp ++;
        chess_Cor_Status[j][9 - 3].have = true;
    }
    chess_Cor_Status[1][9 - 2].which = temp; temp ++;
    chess_Cor_Status[1][9 - 2].have = true;
    chess_Cor_Status[7][9 - 2].which = temp; temp ++;
    chess_Cor_Status[7][9 - 2].have = true;
    for(int j = 0;j < 9;j ++){
        chess_Cor_Status[j][9 - 0].which = temp; temp ++;
        chess_Cor_Status[j][9 - 0].have = true;
    }


    chess[0].kind = 4; chess[0].x = 0; chess[0].y = 0;
    chess[1].kind = 3; chess[1].x = 1; chess[1].y = 0;
    chess[2].kind = 2; chess[2].x = 2; chess[2].y = 0;
    chess[3].kind = 1; chess[3].x = 3; chess[3].y = 0;
    chess[4].kind = 0; chess[4].x = 4; chess[4].y = 0;
    chess[5].kind = 1; chess[5].x = 5; chess[5].y = 0;
    chess[6].kind = 2; chess[6].x = 6; chess[6].y = 0;
    chess[7].kind = 3; chess[7].x = 7; chess[7].y = 0;
    chess[8].kind = 4; chess[8].x = 8; chess[8].y = 0;

    chess[9].kind = 5; chess[9].x = 1; chess[9].y = 2;
    chess[10].kind= 5; chess[10].x = 7; chess[10].y = 2;

    chess[11].kind= 6; chess[11].x = 0; chess[11].y = 3;
    chess[12].kind= 6; chess[12].x = 2; chess[12].y = 3;
    chess[13].kind= 6; chess[13].x = 4; chess[13].y = 3;
    chess[14].kind= 6; chess[14].x = 6; chess[14].y = 3;
    chess[15].kind= 6; chess[15].x = 8; chess[15].y = 3;
    for(int i = 0;i  < 16;i ++) chess[i].color = true;

    chess[16].kind= 6; chess[16].x = 0; chess[16].y = 6;
    chess[17].kind= 6; chess[17].x = 2; chess[17].y = 6;
    chess[18].kind= 6; chess[18].x = 4; chess[18].y = 6;
    chess[19].kind= 6; chess[19].x = 6; chess[19].y = 6;
    chess[20].kind= 6; chess[20].x = 8; chess[20].y = 6;

    chess[21].kind= 5; chess[21].x = 1; chess[21].y = 7;
    chess[22].kind= 5; chess[22].x = 7; chess[22].y = 7;

    chess[23].kind= 4; chess[23].x = 0; chess[23].y = 9;
    chess[24].kind= 3; chess[24].x = 1; chess[24].y = 9;
    chess[25].kind= 2; chess[25].x = 2; chess[25].y = 9;
    chess[26].kind= 1; chess[26].x = 3; chess[26].y = 9;
    chess[27].kind= 0; chess[27].x = 4; chess[27].y = 9;
    chess[28].kind= 1; chess[28].x = 5; chess[28].y = 9;
    chess[29].kind= 2; chess[29].x = 6; chess[29].y = 9;
    chess[30].kind= 3; chess[30].x = 7; chess[30].y = 9;
    chess[31].kind= 4; chess[31].x = 8; chess[31].y = 9;
    for(int i = 16;i  < 32;i ++) chess[i].color = false;
    for(int i = 0;i < 32;i ++) chess[i].which = i;

    for(int i = 0;i < 2;i ++){
        trace[i].x = 4;
        trace[i].y = 0;
        trace[i].exist = 1;
    }
    trace[1].y = 9;

    select_one_self.x = -1;
    select_one_self.y = -1;
    select_one_self.kind = -1;
    select_one_self.which = -1;

    player = 0;
    Success_Or_Lose = 0;
    whoOperator = 0;
}

void ChessBoard::paintEvent(QPaintEvent *ev){
    Q_UNUSED(ev);
    this->setStyleSheet("border:2px solid black;");

    //int width = this->width();
    //int height = this->height();
    //QPixmap fitpixmap = pic_board.scaled(CHESSBOARD_WIDTH, CHESSBOARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充

    //this->setPixmap(fitpixmap);
    QPainter painter(this);
    painter.drawPixmap(0, 0, pic_board);

    if(player == 1){
        for(int i = 0;i < 9;i ++)
           for(int j = 0;j < 10;j ++){
               if(chess_Cor_Status[i][j].which == -1)
                   continue;
               int which = chess_Cor_Status[i][j].which;

               if(chess[which].color == true)
                   painter.drawPixmap(chess_Cor_Status[i][j].x - 26, chess_Cor_Status[i][j].y - 26, blackChessPieces[chess[which].kind]);
               if(chess[which].color == false)
                   painter.drawPixmap(chess_Cor_Status[i][j].x - 26, chess_Cor_Status[i][j].y - 26, redChessPieces[chess[which].kind]);
           }

    }
    else{
        for(int i = 0;i < 9;i ++)
           for(int j = 0;j < 10;j ++){
               if(chess_Cor_Status[8 - i][9 - j].which == -1)
                   continue;
               int which = chess_Cor_Status[8 - i][9 - j].which;

               if(chess[which].color == true)
                   painter.drawPixmap(chess_Cor_Status[i][j].x - 26, chess_Cor_Status[i][j].y - 26, blackChessPieces[chess[which].kind]);
               if(chess[which].color == false)
                   painter.drawPixmap(chess_Cor_Status[i][j].x - 26, chess_Cor_Status[i][j].y - 26, redChessPieces[chess[which].kind]);
           }

    }

    paintTrace();
}

void ChessBoard::paintTrace(){
    QPainter p(this);
    QPen pen(Qt::blue);
    pen.setWidth(3);
    p.setPen(pen);



    for(int i = 0;i < 2;i ++){
        if(trace[i].exist){

            int cor_x = trace[i].x;
            int cor_y = trace[i].y;

            if(player == 0){
                cor_x = 8 - cor_x;
                cor_y = 9 - cor_y;
            }


           int x = chess_Cor_Status[cor_x][cor_y].x;
           int y = chess_Cor_Status[cor_x][cor_y].y;
           int len = 17;
           p.drawLine(QPoint(x - chessCentralWidth , y - chessCentralWidth ),      QPoint(x - chessCentralWidth , y - chessCentralWidth + len )  );
           p.drawLine(QPoint(x - chessCentralWidth , y + chessCentralWidth - 17),  QPoint(x - chessCentralWidth , y + chessCentralWidth));

           p.drawLine(QPoint(x + chessCentralWidth , y - chessCentralWidth ),      QPoint(x + chessCentralWidth , y - chessCentralWidth + len )  );
           p.drawLine(QPoint(x + chessCentralWidth , y + chessCentralWidth - 17),  QPoint(x + chessCentralWidth , y + chessCentralWidth));

           p.drawLine(QPoint(x - chessCentralWidth , y - chessCentralWidth ),      QPoint(x - chessCentralWidth + len , y - chessCentralWidth));
           p.drawLine(QPoint(x + chessCentralWidth - len , y - chessCentralWidth ),QPoint(x + chessCentralWidth, y - chessCentralWidth));

           p.drawLine(QPoint(x - chessCentralWidth , y + chessCentralWidth ),      QPoint(x - chessCentralWidth + len , y + chessCentralWidth));
           p.drawLine(QPoint(x + chessCentralWidth - len , y + chessCentralWidth ),QPoint(x + chessCentralWidth, y + chessCentralWidth));
        }
    }

}

void ChessBoard::mousePressEvent(QMouseEvent *ev){
    QString str="("+QString::number(ev->x())+","+QString::number(ev->y())+")";
    QString info;

    if(whoOperator != player)
        return;
    if(ev->button() == Qt::LeftButton){
        int posx = findposx(ev->x());
        int posy = findposy(ev->y());
        if(!(posx >= 0 && posx <= 8 && posy >= 0 && posy <= 9))
            return;
        // 如果点中位置由棋子
        if(chess_Cor_Status[posx][posy].have){
            // 如果点中位置为 红色棋子
            if(chess_Cor_Status[posx][posy].which >= 16){
                // 如果此时操作玩家为黑
                if(player == 0){
                    // 如果此时之前已经选中自己将要操作的棋子
                    if(select_one_self.kind != -1){
                        // 如果此时选择的位置,自己之前选中的棋子可以跳到此处
                        if(can_jump(player, select_one_self, posx, posy)){
                            move(select_one_self.x ,select_one_self.y, posx, posy);
                            signalMove(select_one_self.x ,select_one_self.y, posx, posy);
                            return ;
                        }
                    }
                }
                // 如果当前操作玩家为红
                if(player == 1){
                    select_this(posx, posy);
                    return ;
                }
            }

            // 如果点中位置为 黑色棋子
            else if(chess_Cor_Status[posx][posy].which >= 0 && chess_Cor_Status[posx][posy].which < 16){
                // 如果当前操作玩家为红
                if(player == 1){
                    // 如果此时之前已经选中自己将要操作的棋子
                    if(select_one_self.kind != -1){
                        // 如果此时选择的位置,自己之前选中的棋子可以跳到此处
                        if(can_jump(player, select_one_self, posx, posy)){
                            move(select_one_self.x ,select_one_self.y, posx, posy);
                            signalMove(select_one_self.x ,select_one_self.y, posx, posy);
                            return ;
                        }
                    }
                }
                // 如果当前操作玩家为黑
                if(player== 0){
                    select_this(posx, posy);
                    return ;
                }
            }
        }

        // 如果选中位置没有棋子
        else if(!chess_Cor_Status[posx][posy].have){
                if(select_one_self.kind != -1){
                    if(can_jump(player, select_one_self, posx, posy)){
                         move(select_one_self.x ,select_one_self.y, posx, posy);
                         signalMove(select_one_self.x ,select_one_self.y, posx, posy);
                        return;
                    }
                }
             }

        update();
    }
}

int ChessBoard::findposx(int tempx){
    for(int i = 0;i < 9;i ++){
        if(tempx >= BOARD_STARTX + i*squareWidth - chessCentralWidth )
            if(tempx <= BOARD_STARTX + i*squareWidth + chessCentralWidth){
                if(player == 1)
                    return i;
                else return 8 - i;
            }
    }
}

int ChessBoard::findposy(int tempy){
    for(int i = 0;i < 10;i ++){
        if(tempy >= BOARD_STARTY + i*squareWidth - chessCentralWidth )
            if(tempy <= BOARD_STARTY + i*squareWidth + chessCentralWidth){
                if(player == 1)
                    return i;
                else return 9 - i;
            }
    }
}

bool ChessBoard::can_jump(int player, Chess self, int x , int y){
    switch (self.kind) {
        case 4:{
            if(x == self.x || y == self.y){
                if(!(x == self.x && y == self.y)){
                    if(x != self.x){
                        int x1 = std::min(x , self.x);
                        int x2 = std::max(x , self.x);
                        for(int i = x1 + 1; i <= x2 - 1;i ++)
                            if(chess_Cor_Status[i][y].have)
                                return false;
                    }
                    if(y != self.y){
                        int y1 = std::min(y , self.y);
                        int y2 = std::max(y , self.y);
                        for(int i = y1 + 1; i <= y2 - 1;i ++)
                            if(chess_Cor_Status[x][i].have)
                                return false;
                    }
                    return true;
                }
            }
            else
                return  false;
        }
            break;
        case 3:{
            int horseStepsx[8] = {1, 2, 2, 1, -1, -2, -2, -1};
            int horseStepsy[8] = {2, 1, -1, -2, -2, -1, 1, 2};
            int horseLegsx[8]  = {0, 1, 1, 0, 0, -1, -1, 0};
            int horseLegsy[8]  = {1, 0, 0, -1, -1, 0, 0, 1};

            for(int i = 0;i < 8;i ++){
                if(horseStepsx[i] + self.x == x && horseStepsy[i] + self.y == y){
                    if(chess_Cor_Status[horseLegsx[i] + self.x][horseLegsy[i] + self.y].have)
                        return false;
                    else
                        return true;
                }
            }
        }
            break;
        case 2:{
            int xiangStepsx[4] = {2, 2, -2, -2};
            int xiangStepsy[4] = {-2, 2, -2, 2};
            int xiangEyex[4]  = {1, 1, -1, -1};
            int xiangEyey[4]  = {-1, 1, -1, 1};

            if(player == 0 && y > 4) return false;
            if(player == 1 && y < 5) return false;

            for(int i = 0;i < 4;i ++){
                if(xiangStepsx[i] + self.x == x && xiangStepsy[i] + self.y == y){
                    if(chess_Cor_Status[xiangEyex[i] + self.x][xiangEyey[i] + self.y].have)
                        return false;
                    else
                        return true;
                }
            }
        }
            break;
        case 1:{
            if(x < 3 || x > 5) return false;
            int shiStepsx[4] = {1, 1, -1, -1};
            int shiStepsy[4] = {-1, 1, -1, 1};
            if(player == 0 && y > 2) return false;
            if(player == 1 && y < 7) return false;

            for(int i = 0;i < 4;i ++){
                if(shiStepsx[i] + self.x == x && shiStepsy[i] + self.y == y){
                        return true;
                }
            }
        }
            break;
        case 0:{
            if(x < 3 || x > 5) return false;
            int wangStepsx[4] = {0, 1, 0, -1};
            int wangStepsy[4] = {-1, 0, 1, 0};
            if((x == chess[4].x && y == chess[4].y ) || (x == chess[27].x && y == chess[27].y )){
                if(chess[4].x == chess[27].x){
                    for(int i = chess[4].y + 1;i <= chess[27].y - 1; i++){
                        if(chess_Cor_Status[chess[4].x][i].have)
                            return false;
                    }
                    return true;
                }
            }

            else{
                if(player == 0 && y > 2) return false;
                if(player == 1 && y < 7) return false;

                for(int i = 0;i < 4;i ++){
                    if(wangStepsx[i] + self.x == x && wangStepsy[i] + self.y == y){
                            return true;
                    }
                }
            }

        }
            break;
        case 5:{
            if(x == self.x || y == self.y){
                if(!(x == self.x && y == self.y)){
                    if(x != self.x){
                        int x1 = std::min(x , self.x);
                        int x2 = std::max(x , self.x);
                        int amout = 0;
                        for(int i = x1 + 1; i <= x2 - 1;i ++)
                            if(chess_Cor_Status[i][y].have) amout++;
                        if(chess_Cor_Status[x][y].have && amout == 1) return true;
                        if(!chess_Cor_Status[x][y].have && amout == 0) return true;

                        return false;
                    }
                    if(y != self.y){
                        int y1 = std::min(y , self.y);
                        int y2 = std::max(y , self.y);
                        int amout = 0;
                        for(int i = y1 + 1; i <= y2 - 1;i ++)
                            if(chess_Cor_Status[x][i].have) amout++;
                        if(chess_Cor_Status[x][y].have && amout == 1) return true;
                        if(!chess_Cor_Status[x][y].have && amout == 0) return true;

                        return false;
                    }
                }
            }
            return false;
        }
            break;
        case 6:{
            if(player == 0 && y < self.y) return false;
            if(player == 1 && y > self.y) return false;

            if(player == 0 && self.y <= 4 && x != self.x) return false;
            if(player == 1 && self.y >= 5 && x != self.x) return false;

            if(std::abs(x - self.x) + std::abs(y - self.y) != 1) return false;

            return true;
        }
            break;
        default:
            return true;
            break;
        }

    return false;
}

void ChessBoard::move(int fromx, int fromy, int tox, int toy){
    int select_which = chess_Cor_Status[fromx][fromy].which;

    // 原来选中的棋子所在位置性质更新
    chess_Cor_Status[fromx][fromy].which = -1;
    chess_Cor_Status[fromx][fromy].have = false;

    // 被吃掉棋子自身记录属性更新
    int which = chess_Cor_Status[tox][toy].which;
    if(which != -1){
        chess[which].x = -1;
        chess[which].y = -1;
    }

    // 将要跳到的位置性质更新
    chess_Cor_Status[tox][toy].have = true;

    chess_Cor_Status[tox][toy].which = select_which;

    chess[select_which].x = tox;
    chess[select_which].y = toy;

    // 画出之前选中棋子的位置
    trace[0].x = fromx;
    trace[0].y = fromy;
    trace[0].exist = true;
    // 画出跳完后棋子所在位置
    trace[1].x = tox;
    trace[1].y = toy;
    trace[1].exist = true;

    // 更改当前棋子选择状态
    select_one_self.kind = -1;

    // 更改当前玩家选择状态
    whoOperator = (whoOperator+ 1) % 2;

    // 更新棋盘
    update();
    if(which == 27 && player == 0)
            Success_Or_Lose = 1;
    if(which == 4 && player == 1)
            Success_Or_Lose = 1;



    QMessageBox *notice;
    int tempPlay = 0;
    for(int i = 0;i < 16;i ++){
        if(can_jump(tempPlay, chess[i], chess[27].x, chess[27].y)){
            qDebug() << "jiangjunlallalall";
            makeSound();
            notice = new QMessageBox(this);
            notice->setText("Notice, 将军！");
            notice->show();
            break;
        }
    }

    tempPlay = 1;
    for(int i = 16;i < 31;i ++){
        if(can_jump(tempPlay, chess[i], chess[4].x, chess[4].y)){
            qDebug() << "jiangjunlallalall";
            makeSound();
            notice = new QMessageBox(this);
            notice->setText("Notice, 将军！");
            notice->show();
            break;
        }
    }
}

void ChessBoard::select_this(int posx, int posy){
    // 进行更行当前选中操作棋子状态的操作
    select_one_self.x = posx;
    select_one_self.y = posy;
    select_one_self.which = chess_Cor_Status[posx][posy].which;
    select_one_self.kind = chess[select_one_self.which].kind;

    // 更新移动轨迹，画出当前选中将进行操作的棋子
    trace[0].x = select_one_self.x;
    trace[0].y = select_one_self.y;
    trace[0].exist = true;
    trace[1].exist = false;

    // 更新棋盘状态
    update();
}


QString ChessBoard::saveCurBoardToString(){

    QString playerList[2] = {"black", "red"};
    if(whoOperator == 1) {playerList[0] = "red"; playerList[1] = "black";}

    QString f;
    QTextStream st(&f);

    int num_black[7];
    int num_red[7];

    int blackPosx[7][5];
    int blackPosy[7][5];
    int redPosx[7][5];
    int redPosy[7][5];

    for(int i = 0;i < 7;i ++){
        num_black[i] = 0;
        num_red[i] = 0;
        for(int j = 0; j < 5;j ++){
            blackPosx[i][j] = -1;
            blackPosy[i][j] = -1;
            redPosx[i][j] =-1;
            redPosy[i][j] =-1;
        }
    }

    for(int i = 0;i < 7;i ++){
        for(int j = 0;j < 16;j ++){
            if(chess[j].kind == i){
                blackPosx[i][num_black[i]] = 8 - chess[j].x;
                blackPosy[i][num_black[i]] = chess[j].y;
                num_black[i] ++;
            }
        }
    }
    for(int i = 0;i < 7;i ++){
        for(int j = 16;j < 32;j ++){
            if(chess[j].kind == i){
                redPosx[i][num_red[i]] =8 - chess[j].x;
                redPosy[i][num_red[i]] = chess[j].y;
                num_red[i] ++;
            }
        }
    }

    for(int h = 0;h < 2;h ++){
        st << playerList[h] << '\n';
        if(playerList[h] == "black"){
            for(int i = 0; i < 7; i ++){
                st << num_black[i];
                for(int j = 0;j < num_black[i];j ++){
                    st << " <" << blackPosx[i][j] << ',' << blackPosy[i][j] << ">";
                }
                st << '\n';
            }
        }
        if(playerList[h] == "red"){
            for(int i = 0; i < 7; i ++){
                st << num_red[i];
                for(int j = 0;j < num_red[i];j ++){
                    st << " <" << redPosx[i][j] << ',' << redPosy[i][j] << ">";
                }
                st << '\n';
            }
        }
    }
    st.flush();
    return f;
}

void ChessBoard::saveCurBoardToFile(QString filename){
    QFile f(filename);
    f.open(QFile::WriteOnly | QFile::Text);
    QTextStream st(&f);
    st << saveCurBoardToString();
    st.flush();
    f.close();
}

#define update_Board_and_RedChess \
    chess[which].x = x;chess[which].y = y;chess[which].kind = j;chess[which].color = false;chess[which].which = which; \
    chess_Cor_Status[x][y].have = true;chess_Cor_Status[x][y].which = which;

#define update_Board_and_BlackChess \
    chess[which].x = x;chess[which].y = y;chess[which].kind = j;chess[which].color = true;chess[which].which = which; \
    chess_Cor_Status[x][y].have = true;chess_Cor_Status[x][y].which = which;

void ChessBoard::loadBoardFromString(const QString &data) {
    std::string str = data.toStdString();
    int pos = 0;

    auto gc = [&] {
        return str[pos++];
    };

    auto gs = [&] {
        char c = gc();
        std::string s;
        for (; c < 'a' || c > 'z'; c = gc());
        for (; c >= 'a' && c <= 'z'; s.push_back(c), c = gc());
        return s;
    };

    auto gn = [&] {
        char c = gc();
        int x = 0;
        for (; c < '0' || c > '9'; c = gc());
        for (; c >= '0' && c <= '9'; x = x * 10 + c - '0', c = gc());
        return x;
    };


    // update 清空棋盘上的所有棋子，清空棋子数组，
    for(int i = 0;i < 9;i ++){
        for(int j = 0;j < 10;j ++){
            chess_Cor_Status[i][j].have = false;
            chess_Cor_Status[i][j].which = -1;
        }
    }
    for(int i = 0;i < 32;i ++){
        chess[i].kind = -1;
        chess[i].x = -1;
        chess[i].y = -1;
        chess[i].which = -1;
    }


    // write
    for(int i = 0;i < 2;i ++){
        auto s = gs();
        if(s == "red"){
            whoOperator = 1;
            for(int j = 0;j < 7;j ++){
                int n = gn();
                switch (j) {
                case 0:{
                    int which = 27;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess
                    }
                }
                    break;
                case 1:{
                    int which = 26;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess

                        which = 28;
                        x = 8 - gn(); y = gn();
                        update_Board_and_RedChess
                    }
                }
                    break;
                case 2:{
                    int which = 25;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess

                        which = 29;
                        x = 8 - gn(); y = gn();
                        update_Board_and_RedChess
                    }
                }
                    break;
                case 3:{
                    int which = 24;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess

                        which = 30;
                        x = 8 - gn(); y = gn();
                        update_Board_and_RedChess
                    }
                }
                    break;
                case 4:{
                    int which = 23;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess

                        which = 31;
                        x = 8 - gn(); y = gn();
                        update_Board_and_RedChess
                    }
                }
                    break;
                case 5:{
                    int which = 21;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess

                        which = 22;
                        x = 8 - gn(); y = gn();
                        update_Board_and_RedChess
                    }
                }
                    break;
                case 6:{
                    int which = 16;
                    for(int k = 0;k < n;k ++){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_RedChess
                        which += 1;
                    }
                }
                    break;
                default:
                    break;
                }
            }
        }
        if(s == "black"){
            whoOperator = 0;
            for(int j = 0;j < 7;j ++){
                int n = gn();
                switch (j) {
                case 0:{
                    int which = 4;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess
                    }
                }
                    break;
                case 1:{
                    int which = 3;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess

                        which = 8 - which;
                        x = 8 - gn(); y = gn();
                        update_Board_and_BlackChess
                    }
                }
                    break;
                case 2:{
                    int which = 2;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess

                        which = 8 - which;
                        x = 8 - gn(); y = gn();
                        update_Board_and_BlackChess
                    }
                }
                    break;
                case 3:{
                    int which = 1;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess

                        which = 8 - which;
                        x = 8 - gn(); y = gn();
                        update_Board_and_BlackChess
                    }
                }
                    break;
                case 4:{
                    int which = 0;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess

                        which = 8 - which;
                        x = 8 - gn(); y = gn();
                        update_Board_and_BlackChess
                    }
                }
                    break;
                case 5:{
                    int which = 9;
                    if(n == 1){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess
                    }
                    if(n == 2){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess

                        which = 10;
                        x = 8 - gn(); y = gn();
                        update_Board_and_BlackChess
                    }
                }
                    break;
                case 6:{
                    int which = 11;
                    for(int k = 0;k < n;k ++){
                        int x = 8 - gn(); int y = gn();
                        update_Board_and_BlackChess
                        which += 1;
                    }
                }
                    break;
                default:
                    break;
                }
            }
        }
    }
     update();
}

void ChessBoard::loadBoardFromFile(const QString &filename) {
    QFile f(filename);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream st(&f);
    st.setCodec(QTextCodec::codecForName("GB2312"));

    loadBoardFromString(st.readAll());
}

void ChessBoard::makeSound(){
    mplayer->setVolume(30);
    mplayer->play();

}
