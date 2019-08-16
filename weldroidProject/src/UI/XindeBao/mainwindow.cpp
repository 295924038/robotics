#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pubfunc/mylineedit.h"
#include "input.h"
#include <iostream>
#include "interface.h"
#include "preferences.h"

using namespace std;

int user = role::consumer;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tabwidget = ui->weldingProject;
    _interfaceUI = new interface(NULL,this);
    _preferencesUI = new Preferences;
    updateMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void changeCurrentUser(const QString& strRole)
{
    cout<<"before , user :"<<user<<endl;
    if( 0 == strRole.toStdString().compare("普通用户"))
    {
        user = role::consumer;
    }
    if( 0 == strRole.toStdString().compare("专家模式"))
    {
        user = role::admin;
    }
    if(0 == strRole.toStdString().compare("厂家模式"))
    {
        user = role::root;
    }
    cout<<"after , user :"<<user<<endl;

}
std::string getCurrentRole()
{
    string currRole;
    switch (user)
    {
    case 0:
        currRole = "普通用户";
        break;
    case 1:
        currRole = "专家模式";
        break;
    case 2:
        currRole = "厂家模式";
        break;
    default:
        break;
    }
    return currRole;
}

int getEqualUser(const QString& strRole)
{
    int iUser = 0;
    if( 0 == strRole.toStdString().compare("普通用户"))
    {
        iUser = role::consumer;
    }
    if( 0 == strRole.toStdString().compare("专家模式"))
    {
        iUser = role::admin;
    }
    if(0 == strRole.toStdString().compare("厂家模式"))
    {
        iUser = role::root;
    }
    return iUser;

}

void MainWindow::inputShow()
{
    //    keyboardUI->setWindowFlags(Qt::WindowStaysOnTopHint);
    //    keyboardUI->show();
    //    keyboardUI->exec();
}


void MainWindow::updatePasswdInput()
{
    //    ui->passwdInput->setText(keyboardUI->inputVal);
}


void MainWindow::closeWindow()
{
    this->close();
}


void MainWindow::updateMenu()
{
    cout<<"updateMenu"<<endl;
    auto tabCount = tabwidget->count();
    cout<<"before ,tabCount"<<tabCount<<endl;

    tabwidget->clear();
    if(user == role::consumer)
    {
        tabwidget->addTab(_interfaceUI,"自动焊接");
    }
    if(user == role::admin || user == role::root)
    {
        tabwidget->addTab(_interfaceUI,"自动焊接");
        tabwidget->addTab(_preferencesUI,"参数调整");
    }

    cout<<"after ,tabCount"<< tabwidget->count()<<endl;
}
