#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class newDialog;
}

class newDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newDialog(QWidget *parent = 0);
    Ui::newDialog *ui;
    ~newDialog();

public slots:
    void setLimit_num(int);
    void setLimit_in1(int);
    void setLimit_out1(int);
    void setLimit_out2(int);

signals:

private:

};

#endif // NEWDIALOG_H
