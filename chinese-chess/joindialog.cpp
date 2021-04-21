#include "joindialog.h"
#include "ui_joindialog.h"
#include <QIntValidator>

JoinDialog::JoinDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinDialog)
{
    ui->setupUi(this);
    initJoinRoom();

}

JoinDialog::~JoinDialog()
{
    delete ui;
}


void JoinDialog::initJoinRoom(){
    QStringList stringList;


    ui->setServerPot->setValidator(new QIntValidator(0, 65535, this));
    ui->setServerPot->setText("6666");

    socket = nullptr;
}

void JoinDialog::accept(){
    ip = ui->setServerIP->text();
    port = ui->setServerPot->text();
    bool legal = tempaddr.setAddress(ip);
    if(legal == false){
        QMessageBox::warning(this,
                             tr(u8"错误"),
                             tr(u8"你输入的ip地址有误，请重新输入"));
        return;
    }
    if(socket != nullptr)
        delete socket;

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, [=](){
        //QMessageBox::information(this, "Connet Successfuly", tr(u8"连接成功，点击OK开始对战"), QMessageBox::Ok);
        QDialog::accept();
    });
    socket->connectToHost(ip, port.toInt());
}
