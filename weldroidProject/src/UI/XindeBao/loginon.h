#ifndef LOGINON_H
#define LOGINON_H

#include <QDialog>
#include "input.h"

namespace Ui {
class LoginOn;
}

class LoginOn : public QDialog
{
    Q_OBJECT

public:
    explicit LoginOn(QWidget *parent = 0);
    ~LoginOn();

signals:
    void changeUser();

private slots:

    void inputShow();

    void updateInPutLine();

    void on_btn_loginOn_clicked();

    void on_btn_exit_clicked();

private:
    Ui::LoginOn *ui;
    Input* inputUI;
};

#endif // LOGINON_H
