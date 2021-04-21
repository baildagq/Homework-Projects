# Qt 大作业文档

## 主要内容

* 效果预览
* 基本介绍
* 功能说明
* 实现思路
* 主要代码
* 后期工作
* 总结反思



## 效果预览

* 初始界面

![](E:\picture\Snipaste\Snipaste_2018-09-02_12-07-57.png)



* 计算界面

![](E:\picture\Snipaste\Snipaste_2018-09-02_12-51-51.png)



## 基本介绍

* 编译程序后，界面会初始化 8 x 8 各管道速度均为0的网格
* 新建表格有两种方式：
  1. 在菜单栏中选中File ,选中new 后弹出窗体，进行初始参数设置，并按OK进行创建
  2. 在主窗体页面，进行参数设置后，点击 New 这个button 进行创建
* 左键点击选择管道是否存在
* 右键点击跳出dialog进行管道宽度设置
* 运用鼠标在图上进行修改时，完成事件后会立刻重绘并计算
* 通过点击主界面和菜单栏中的 Exit 键可以退出窗体



## 功能说明

* 基本功能

  1. 窗体弹出

     * 通过File 菜单建立窗口时，可以弹出对话窗口

       ![](E:\picture\Snipaste\Snipaste_2018-09-02_12-55-10.png)

       

  2. 拥有菜单栏等，可以进行初始设置

  3. 可以基于鼠标和键盘事件进行芯片的手工设计，左键更换管道存在状态，右键更改存在的管道宽度

  4. 集成流速计算程序，可以算出所有管道的流速，并将三个输出管道的流速在页面上进行显示



* 加分项

  1. 管道宽度变化，可以调节管道宽度，融入修改后的流速计算程序

  2. 特色扩展功能：支持随流速变化的不同管道的颜色变化

     

## 实现思路

* 整体工程的实现思路是将图形通过不断执行的paintEvent 事件，将新建或经过鼠标点击修改后的状态绘画出来

* 主要用得到的是QPainter, 由初始输入边数定义好主结构体 和 一些表示性质的 vector，并根据主结构体和性质vector 进行后期绘图

  

## 主要代码

* 代码结构

  ![](E:\picture\Snipaste\Snipaste_2018-09-02_17-15-30.png)

  ![](E:\picture\Snipaste\Snipaste_2018-09-02_17-16-11.png)

* 关键代码

  * paintEvent

    ```c++
    void MainWindow::paintEvent(QPaintEvent *){
        QPainter p(this);
        p.drawLine(QPoint(900, 0), QPoint(900, 1000));
    
        if(edge_Num == -1)
            return;
        // draw the vertical rhombus
        for(int i = 0; i < edge_Num*(edge_Num - 1); i ++){
            if(vertex[i].select == false)
                continue;
            paintVerticalPipe(i);
        }
    
        // draw the horizontal rhombus
        for(int i = edge_Num*(edge_Num - 1); i < 2*edge_Num*(edge_Num - 1); i ++){
            if(vertex[i].select == false)
                continue;
            paintHorizontalPipe(i);
        }
    
        // draw the input and output rhombus (vertical)
        for(int i = 2*edge_Num*(edge_Num - 1); i < 2*edge_Num*(edge_Num - 1) + 5; i ++){
            if(vertex[i].select == false)
                continue;
            paintVerticalPipe(i);
        }
    
        paintColorTable();
    }
    ```

  * mousePressEvent

    ```c++
    void MainWindow::mousePressEvent(QMouseEvent *e){
        QString str="("+QString::number(e->x())+","+QString::number(e->y())+")";
        QString info;
    
        if(e->button() == Qt::LeftButton){
            QTextStream(&info) << tr("Mouse Left Button Pressed:")+str;
            for(int i = 0;i < 2*edge_Num*(edge_Num - 1);i ++){
                if(belong_to_Rect(i, e->x(), e->y())){
                    vertex[i].select = !vertex[i].select;
                    if(leng[i] == 0)
                        leng[i] = 1;
                    else{
                        leng[i] = 0;
                        vertex[i].width = 5;
                    }
                    calculate_flow_velocity();
                    update();
                    break;
                }
            }
        }
        if(e->button() == Qt::RightButton){
            QTextStream(&info) << tr("Mouse Right Button Pressed:")+str;
            for(int i = 0;i < 2*edge_Num*(edge_Num - 1);i ++){
                if(belong_to_Rect(i, e->x(), e->y())){
                    if(vertex[i].select == false)
                        continue;
                    bool ok;
                    double width = QInputDialog::getDouble(this, tr("Width"), tr("New Width (um):"), 200, 10, 1600, 1, &ok);
                    vertex[i].width= (width / 200) * 5;
                    leng[i] = 200 / width;
                    calculate_flow_velocity();
                    update();
                    break;
                }
            }
        }
    
        statusBar()->showMessage (info);
    }
    ```

  * on_actionNew_triggered

    ```c++
    void MainWindow::on_actionNew_triggered()
    {
        if(nDialog->exec() == QDialog::Accepted){
    
            // updateData
            edge_Num = nDialog->ui->numBox_2->value();
            init_x_y_begin();
            inputEnd[0] = nDialog->ui->inBox1_2->value();
            inputEnd[1] = nDialog->ui->inBox2_2->value();
            outputEnd[0] = nDialog->ui->outBox1_2->value();
            outputEnd[1] = nDialog->ui->outBox2_2->value();
            outputEnd[2] = nDialog->ui->outBox3_2->value();
            initLeng();
            initVertex();
    
            // updateResualt & display
            calculate_flow_velocity();
    
            // updatePaint
            this->update();
    
          //  centralWidget()->setMouseTracking(true);
        }
    }
    ```

  * 为保证输入无误的几个槽函数

    ```c++
    void setLimit_num(int);
    void setLimit_in1(int);
    void setLimit_out1(int);
    void setLimit_out2(int);
    ```

  * 实现随流速变化，颜色变化功能

    ```c++
    // 设立对照卡
    void MainWindow::paintColorTable(){
        QPainter painter(this);
    
        int table_begin_x = 880 - x_begin;
        int table_begin_y = y_begin + 100;
    
        painter.setRenderHint(QPainter::Antialiasing, true);
        QLinearGradient linearGradient(table_begin_x + 10, table_begin_y, table_begin_x + 10, table_begin_y + (edge_Num - 1) * 100);
        linearGradient.setColorAt(0.0, Qt::red);
        linearGradient.setColorAt(1.0, Qt::blue);
        painter.setBrush(QBrush(linearGradient));
        painter.fillRect(table_begin_x, table_begin_y, 20, (edge_Num - 1) * 100, linearGradient);
    }
    
    // 为相应的管道取出相应的颜色
    QColor MainWindow::getColor(int i){
        double velo = std::min(velocity_result[i]/200, 2.0) / 2;
        //if(velo == 0.0)
          //  return Qt::black;
        if(velo >= 0.4)
            return Qt::red;
        int r = velo /0.4 * 255;
        int b = 255 - velo/0.4*255;
        QColor s(r, 0, b);
        return s;
    }
    ```

    

## 后续工作

* 目前部分条件限制不是特别完善，比如宽度限制等，需要进一步优化简练
* 目前不支持浓度计算，希望能够推导出浓度算法
* 希望了解一点关于微通道网络自动设计的一点相关知识



## 总结反思

* 大作业过程并不顺利，遭遇的困难主要包括没有找到正确的学习知识的途径，没有确立正确的实现思路，已经对于UI下编程的不适应
* 学到的更多应该是在今后的学习怎样更好更快的学习知识，怎样学会借助周围人的力量，进行交流