#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QStatusBar>
#include <QLabel>

#include "newdialog.h"
#include "ui_newdialog.h"
#include "calculate.h"


#include <vector>
struct Vertex_Rhombus
{
    double x;
    double y;
    double width; // the width is represent by pixel & the width is the width to center line
    bool select;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QWidget *canvas;

    void paintEvent(QPaintEvent *);
    void paintColorTable();
    void paintVerticalPipe(int i);
    void paintHorizontalPipe(int i);

    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);

    // init some setting of the ui widget
    void initSomeWidget();
    // init some elements of the mainWindow
    void initSomeData();
    // make sure the rhombus vertex
    void initVertex();
    //judge whether the mouse press belong to any rhombus
    bool belong_to_Rect(int i, int x, int y);
    // init the begin of x & y, to make the picture align center
    void init_x_y_begin();
    // init the leng after new | resize
    void initLeng();
    // to calculate the result after every update
    void calculate_flow_velocity();

    QColor getColor(int i);

private slots:
    void on_actionExit_triggered();

    void on_actionNew_triggered();

    void on_clickedNew(bool);

    void setLimit_num(int);
    void setLimit_in1(int);
    void setLimit_out1(int);
    void setLimit_out2(int);

private:
    Ui::MainWindow *ui;
    newDialog *nDialog;

    int edge_Num;
    int inputEnd[2];
    int outputEnd[3];
    int x_begin, y_begin;
    Vertex_Rhombus vertex[200];

    std::vector<double> leng;
    std::vector<double> velocity_result;
    std::vector<double> result;

};

#endif // MAINWINDOW_H
