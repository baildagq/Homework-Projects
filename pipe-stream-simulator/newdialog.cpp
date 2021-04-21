#include "newdialog.h"
#include "ui_newdialog.h"

newDialog::newDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newDialog)
{
    ui->setupUi(this);
    connect(ui->numBox_2, SIGNAL(valueChanged(int)), this, SLOT(setLimit_num(int)));
    connect(ui->inBox1_2, SIGNAL(valueChanged(int)), this, SLOT(setLimit_in1(int)));
    connect(ui->outBox1_2, SIGNAL(valueChanged(int)), this, SLOT(setLimit_out1(int)));
    connect(ui->outBox2_2, SIGNAL(valueChanged(int)), this, SLOT(setLimit_out2(int)));

}

newDialog::~newDialog()
{
    delete ui;
}


void newDialog::setLimit_num(int limit_max){
    ui->inBox1_2->setMaximum(limit_max);
    ui->inBox2_2->setMaximum(limit_max);
    ui->outBox1_2->setMaximum(limit_max);
    ui->outBox2_2->setMaximum(limit_max);
    ui->outBox3_2->setMaximum(limit_max);
}
void newDialog::setLimit_in1(int limit_min){
    ui->inBox2_2->setMinimum(limit_min + 1);
}
void newDialog::setLimit_out1(int limit_min){
    ui->outBox2_2->setMinimum(limit_min + 1);
    ui->outBox3_2->setMinimum(limit_min + 2);
}
void newDialog::setLimit_out2(int limit_min){
    ui->outBox3_2->setMinimum(limit_min + 1);
}
