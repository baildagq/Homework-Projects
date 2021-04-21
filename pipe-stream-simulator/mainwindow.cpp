#include <QLCDNumber>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QBrush>
#include <QLinearGradient>

#include <QMouseEvent>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1300, 1000);
    ui->mainToolBar->hide();

    nDialog = new newDialog;

    initSomeWidget();
    initSomeData();
    initVertex();

    connect(ui->numBox_3, SIGNAL(valueChanged(int)), this, SLOT(setLimit_num(int)));
    connect(ui->inBox1_3, SIGNAL(valueChanged(int)), this, SLOT(setLimit_in1(int)));
    connect(ui->outBox1_3, SIGNAL(valueChanged(int)), this, SLOT(setLimit_out1(int)));
    connect(ui->outBox2_3, SIGNAL(valueChanged(int)), this, SLOT(setLimit_out2(int)));

    connect(ui->pushButton_new, &QPushButton::clicked, this, &MainWindow::on_clickedNew);
    connect(ui->pushButton_exit, &QPushButton::clicked, this, &MainWindow::on_actionExit_triggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSomeWidget(){
    ui->label->setFont(QFont("OutPot_1:velocity" , 11));
    ui->label_2->setFont(QFont("OutPot_2:velocity" , 11));
    ui->label_3->setFont(QFont("OutPot_3:velocity" , 11));

    ui->label_4->setFont(QFont("OutPot_3:flow" , 11));
    ui->label_5->setFont(QFont("OutPot_3:flow" , 11));
    ui->label_6->setFont(QFont("OutPot_3:flow" , 11));

    ui->label_7->setFont(QFont("OutPot_3:concentration" , 11));
    ui->label_8->setFont(QFont("OutPot_3:concentration" , 11));
    ui->label_9->setFont(QFont("OutPot_3:concentration" , 11));

    ui->num_3->setFont(QFont("OutPot_3:" , 10));
    ui->inPot1_3->setFont(QFont("OutPot_3:" , 10));
    ui->inPot2_3->setFont(QFont("OutPot_3:" , 10));
    ui->outPot1_3->setFont(QFont("OutPot_3:" , 10));
    ui->outPot2_3->setFont(QFont("OutPot_3:" , 10));
    ui->outPot3_3->setFont(QFont("OutPot_3:" , 10));
}

void MainWindow::initSomeData(){
    // first init the edge to the biggest amout
    edge_Num = 8;

    // init the inputEnd & outputEnd
    inputEnd[0] = 1;
    inputEnd[1] = 2;
    outputEnd[0] = 1;
    outputEnd[1] = 2;
    outputEnd[2] = 3;

    // init the begin of x & y spot
    // to adapt the 5, x:150  y:200
    // to adapt the 6, x:150  y:200
    // to adapt the 7, x:150  y:200
    // to adapt the 8, x:50  y:50
    x_begin = 45;
    y_begin = 45;

    // init the vertex of rhombus
    for(int i = 0; i < 200;i ++){
        vertex[i].x = 0;
        vertex[i].y = 0;
        vertex[i].select = false;
        vertex[i].width = 5;
    }

    // initiallize the 117 amout of leng is 1
    // then will not change the amout of leng
    for (int i=0; i<2*edge_Num*edge_Num-2*edge_Num+5; i++)
    {
        leng.push_back(1);
        velocity_result.push_back(0);
    }
    qDebug() << "size of leng " << leng.size();
    qDebug() << "size of velo_result" << velocity_result.size();

}

void MainWindow::initVertex(){
    for(int i = 0; i < 2*edge_Num*(edge_Num - 1) + 5; i ++)
        vertex[i].select = true;
    for(int i = 2*edge_Num*(edge_Num - 1) + 5; i < 120; i ++)
        vertex[i].select = false;

    int xuhao = 0;
    // make sure the Vertex of vertical Rhombus
    for(int i = 0;i < edge_Num;i ++){
        for(int j = 0;j < edge_Num - 1;j ++){
            vertex[xuhao].x = x_begin + i * 100;
            vertex[xuhao].y = y_begin + j * 100 + 100;
            xuhao ++;
        }
    }
    // make sure the Vertex of horizontal Rhombus
    for(int i = 0;i < edge_Num - 1;i ++){
        for(int j = 0;j < edge_Num;j ++){
            vertex[xuhao].x = x_begin + i * 100;
            vertex[xuhao].y = y_begin + j * 100 + 100;
            xuhao ++;
        }
    }
    // make sure the Vertex of input Rhombus
    for(int i = 0;i < 2;i ++){
        vertex[xuhao].x = x_begin + (inputEnd[i] - 1) * 100;
        vertex[xuhao].y = y_begin;
        xuhao ++;
    }
    // make sure the Vertex of output Rhombus
    for(int i = 0;i < 3;i ++){
        vertex[xuhao].x = x_begin + (outputEnd[i] - 1) * 100;
        vertex[xuhao].y = y_begin + 100 + (edge_Num - 1) * 100;
        xuhao ++;
    }
    for(int i = 0;i < 2*edge_Num*(edge_Num - 1) + 5; i++){
        vertex[i].width = 5;
    }
}

void MainWindow::initLeng(){
    for(int i = 0;i < 2*edge_Num*edge_Num-2*edge_Num+5; i ++)
        leng[i] = 1;
    for(int i = 2*edge_Num*edge_Num-2*edge_Num+5; i < 117 ; i ++)
        leng[i] = 0;
}

bool MainWindow::belong_to_Rect(int i, int x, int y){
    if(i < edge_Num*(edge_Num - 1)){
        if(x >= vertex[i].x-(vertex[i].width - 5)-5 && x <= vertex[i].x+(vertex[i].width - 5)+15)
            if(y >= vertex[i].y+10 && y <= vertex[i].y+100)
                return true;
    }
    if(i >= edge_Num*(edge_Num - 1)){
        if(x >= vertex[i].x+10 && x <= vertex[i].x+100)
            if(y >= vertex[i].y-(vertex[i].width - 5)-5 && y <= vertex[i].y+(vertex[i].width - 5)+15)
                return true;
    }
    return false;
}

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

void MainWindow::paintVerticalPipe(int i){
    double x = vertex[i].x; double y = vertex[i].y;
    QPointF points[12] = {
        QPointF(x, y),
        QPointF(x, y + 10.0),
        QPointF(x - vertex[i].width + 5, y + 10.0),
        QPointF(x - vertex[i].width + 5, y + 100.0),
        QPointF(x, y + 100.0),
        QPointF(x, y + 110.0),

        QPointF(x + 10.0, y + 110.0),
        QPointF(x + 10.0, y + 100.0),
        QPointF(x + vertex[i].width + 5, y + 100.0),
        QPointF(x + vertex[i].width + 5, y + 10.0),
        QPointF(x + 10.0, y + 10.0),
        QPointF(x + 10.0, y),
    };

    QPainter painter(this);
    painter.setBrush(getColor(i));
    painter.drawConvexPolygon(points, 12);
}

void MainWindow::paintHorizontalPipe(int i){
    double x = vertex[i].x; double y = vertex[i].y;
    QPointF points[12] = {
        QPointF(x, y),
        QPointF(x, y + 10.0),
        QPointF(x + 10, y + 10.0),
        QPointF(x + 10, y + vertex[i].width + 5),
        QPointF(x + 100, y + vertex[i].width + 5),
        QPointF(x + 100, y + 10.0),
        QPointF(x + 110.0, y + 10.0),

        QPointF(x + 110.0, y),
        QPointF(x + 100.0, y),
        QPointF(x + 100.0, y - vertex[i].width + 5),
        QPointF(x + 10.0, y - vertex[i].width + 5),
        QPointF(x + 10.0, y),
    };

    QPainter painter(this);
    painter.setBrush(getColor(i));
    painter.drawConvexPolygon(points, 12);
}

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

void MainWindow::mouseMoveEvent(QMouseEvent *e){
    QString str="("+QString::number(e->x())+","+QString::number(e->y())+")";
    QString info;
    for(int i = 0;i < 2*edge_Num*(edge_Num - 1);i ++){
        if(belong_to_Rect(i, e->x(), e->y())){

        }
    }
    QTextStream(&info) << tr("Mouse move track :")+str;
    statusBar()->showMessage (info);
}

void MainWindow::on_actionExit_triggered()
{
   this->close();
}

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

void MainWindow::on_clickedNew(bool checked){

    // updateData
    edge_Num = ui->numBox_3->value();
    init_x_y_begin();
    inputEnd[0] = ui->inBox1_3->value();
    inputEnd[1] = ui->inBox2_3->value();
    outputEnd[0] = ui->outBox1_3->value();
    outputEnd[1] = ui->outBox2_3->value();
    outputEnd[2] = ui->outBox3_3->value();
    initLeng();
    initVertex();

    // updateResualt & display
    calculate_flow_velocity();

    // updatePaint
    this->update();

    centralWidget()->setMouseTracking(checked);
}

void MainWindow::setLimit_num(int limit_max){
    ui->inBox1_3->setMaximum(limit_max);
    ui->inBox2_3->setMaximum(limit_max);
    ui->outBox1_3->setMaximum(limit_max);
    ui->outBox2_3->setMaximum(limit_max);
    ui->outBox3_3->setMaximum(limit_max);
}

void MainWindow::setLimit_in1(int limit_min){
    ui->inBox2_3->setMinimum(limit_min + 1);
}

void MainWindow::setLimit_out1(int limit_min){
    ui->outBox2_3->setMinimum(limit_min + 1);
    ui->outBox3_3->setMinimum(limit_min + 2);
}

void MainWindow::setLimit_out2(int limit_min){
    ui->outBox3_3->setMinimum(limit_min + 1);
}

void MainWindow::init_x_y_begin(){
    switch (edge_Num) {
    case 5:
        x_begin = 195; y_begin= 195;
        break;
    case 6:
        x_begin = 145; y_begin= 145;
        break;
    case 7:
        x_begin = 95; y_begin= 95;
        break;
    case 8:
        x_begin = 45; y_begin= 45;
        break;
    default:
        break;
    }
}

void MainWindow::calculate_flow_velocity(){
    // debug , after paint calculate the result & output
    velocity_result = caluconspeed(edge_Num, leng, inputEnd[0] - 1, inputEnd[1] - 1, outputEnd[0] - 1, outputEnd[1] - 1, outputEnd[2] - 1);

    ui->lcdNumber->display(velocity_result[2*edge_Num*(edge_Num - 1) + 2]);
    ui->lcdNumber_2->display(velocity_result[2*edge_Num*(edge_Num - 1) + 3]);
    ui->lcdNumber_3->display(velocity_result[2*edge_Num*(edge_Num - 1) + 4]);
}

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
