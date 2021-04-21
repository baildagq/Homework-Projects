#include "createdialog.h"
#include "ui_createdialog.h"

#include <QIntValidator>
#include <QNetworkInterface>

CreateDialog::CreateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDialog),socket(NULL)
{
    ui->setupUi(this);

    initCreateRoom();
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, [=](){
        if(socket != nullptr) delete socket;
        socket = server->nextPendingConnection();
        server->close();

        QDialog::accept();

    });
}

CreateDialog::~CreateDialog()
{
    delete ui;
}

void CreateDialog::initCreateRoom(){
    QStringList stringList;
    stringList << "黑子" << "红子";
    ui->Select_Black_Red->addItems(stringList);
    ui->Select_Black_Red->setCurrentIndex(0);

    stringList.clear();
    stringList << "黑方" << "红方";

    for (auto &h : QNetworkInterface::allAddresses())
        if (h != QHostAddress::LocalHost && h.toIPv4Address()) {
            ip = h.toString();
            ui->setServerIP->setText(ip);
        }

    ui->setWaitTime->setValidator(new QIntValidator(5, 120, this));
    ui->setWaitTime->setText("30");

    ui->setServerPot->setValidator(new QIntValidator(0, 65535, this));
    ui->setServerPot->setText("6666");
}

void CreateDialog::accept(){
    if(server->isListening()) server->close();
    auto port = ui->setServerPot->text();

    QMessageBox loading(QMessageBox::NoIcon, "等待玩家加入房间..", tr(u8"正在监听%1:%2\n等待连接中...").arg(ip).arg(port), QMessageBox::Close | QMessageBox::Cancel);
    server->listen(QHostAddress::AnyIPv4, port.toInt());
}
