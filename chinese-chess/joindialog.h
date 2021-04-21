#ifndef JOINDIALOG_H
#define JOINDIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>

namespace Ui {
class JoinDialog;
}

class JoinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinDialog(QWidget *parent = 0);
    ~JoinDialog();

    void accept() override;
    void initJoinRoom();

public:
    Ui::JoinDialog *ui;
    QTcpSocket *socket;

private:
    QString ip;
    QString port;
    QHostAddress tempaddr;
};

#endif // JOINDIALOG_H
