#include "loginon.h"
#include "ui_loginon.h"
#include <iostream>
#include <QMessageBox>
#include "base/config.h"

using namespace std;

extern int user;
extern string getCurrentRole();
extern void changeCurrentUser(const QString&);
extern int getEqualUser(const QString&);

LoginOn::LoginOn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginOn)
{
    ui->setupUi(this);
    setWindowTitle("用户登录");
    ui->le_pwdInput->setEchoMode(QLineEdit::Password);
    connect(ui->le_pwdInput, SIGNAL(clicked()), this, SLOT(showInputUI()));
}

LoginOn::~LoginOn()
{
    delete ui;
}

void LoginOn::on_btn_loginOn_clicked()
{
    cout<<"=second ="<<getCurrentRole().c_str()<<endl;
    Config _sys_config("etc/sys.info");
    auto iUser = getEqualUser(ui->comb_roleLIst->currentText());
    auto passwd = _sys_config.get<std::string>("Passwd.user" + to_string(iUser));
    auto passwdInput = ui->le_pwdInput->text().toStdString();
    if(passwdInput != passwd)
    {
        QMessageBox::warning(this, "提示", "用户密码错误,请重新输入!");
    }else{
        changeCurrentUser(ui->comb_roleLIst->currentText());
        cout<<"=second change="<<getCurrentRole().c_str()<<endl;
        emit changeUser();
        this->hide();
    }
}

void LoginOn::on_btn_exit_clicked()
{
    accept();
}


void LoginOn::showInputUI()
{
    Input* inputUI = new Input(NULL,this);
    inputUI->setWindowFlags(Qt::WindowStaysOnTopHint);
    inputUI->show();
    inputUI->exec();
    delete inputUI;
}

void LoginOn::update_LineEdit(string number)
{
    ui->le_pwdInput->setText(QString::fromStdString(number));
}
