# Qt 大作业文档

## 主要内容

- 效果预览
- 基本介绍
- 功能说明
- 实现思路
- 主要代码
- 后期工作
- 总结反思



## 效果预览

- 初始界面

  ![1536587993745](C:\Users\28172\AppData\Local\Temp\1536587993745.png)

- 连接界面

  ![1536589333749](C:\Users\28172\AppData\Local\Temp\1536589333749.png)

- 游戏开始界面

  ![1536591895257](C:\Users\28172\AppData\Local\Temp\1536591895257.png)





## 基本介绍

- 游戏方式，启动两个游戏进程，点击 battle 后选择 create room 可以做为服务器端等待其他客户端申请加入房间
- 游戏局面逻辑上由创建房间的房主决定，在创建房间前，决定刷新局面开始新局，还是加载残局重新操作
- 保存文件，双方均可以在对局中进行进行保存操作



## 功能说明

* 支持文件保存，可以在程序开始后任意时刻进行保存
* 支持局面加载，需要在房间创建前由房主决定（个人觉得比较符合逻辑）
* 支持认输
* 支持将军提醒，可以在具有棋子将军条件后在之后棋子移动后进行弹窗信息提醒
* 支持将军音效，弹出信息提醒时可以播放将军音效



## 实现思路

* 数据存储：

  * 主要存储数据的结构为有两个，一个是棋盘上每个节点进行信息状态存储，一个是对32个棋子进行编号后储存

    ![1536594138468](C:\Users\28172\AppData\Local\Temp\1536594138468.png)

  * 另一个比较重要的数据存储为当前选中的棋子，实例出一个 chess类对象 self_select

* 整体思路：

  * 实际上，进行判断与走子处理时，服务器端与客户端的棋盘均为黑子在上，只不过会将设置为执黑棋的一方进行真正显示时，调到下方，在printEvent  函数中进行 player 判断后，进行绘制

* 胜负判断方法：

  * 在每个程序端，程序开始时，创建定时器，定时器结束时检查当前存储胜负情况的值的状态，如有改变，进行相应情景的结论执行



## 主要代码

* 文件结构

  ![1536594567845](C:\Users\28172\AppData\Local\Temp\1536594567845.png)

* 代码结构

  - mainWindow.cpp

    ![1536594685898](C:\Users\28172\AppData\Local\Temp\1536594685898.png)

    ![1536594717004](C:\Users\28172\AppData\Local\Temp\1536594717004.png)

    

  - chessBoard.cpp

    ![1536594839366](C:\Users\28172\AppData\Local\Temp\1536594839366.png)

    

## 主要代码

```c++
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
```

```c++
void MainWindow::on_actionCreate_room_triggered()
{
    if(createDialog->exec() == QDialog::Accepted){
        // socket 建立
        socket = createDialog->socket;



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
```

```c++
void MainWindow::on_actionJoin_room_triggered()
{
    if(joinDialog->exec() == QDialog::Accepted){
        socket = joinDialog->socket;


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
```

```c++
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

```



## 后续工作

- 目前部分自己想加的功能没有实现
- 希望能够对界面进行更加的美化一些



## 总结反思

- 大作业过程并不顺利，遭遇的困难主要包括没有找到正确的学习知识的途径，没有确立正确的实现思路，已经对于UI下编程的不适应
- 学到的更多应该是在今后的学习怎样更好更快的学习知识，怎样学会借助周围人的力量，进行交流