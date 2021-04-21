#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QMessageBox>
#include <QTcpSocket>
#include <QString>

namespace Ui {
class CreateDialog;
}

class CreateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDialog(QWidget *parent = 0);
    ~CreateDialog();
    QTcpSocket *getSocket() const;

    void initCreateRoom();

    void accept() override;

public:
    Ui::CreateDialog *ui;
    QTcpSocket *socket;


private:
    QString ip;
    QTcpServer *server;
    QMessageBox *loading;
};

#endif // CREATEDIALOG_H
