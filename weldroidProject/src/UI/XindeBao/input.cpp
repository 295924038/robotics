#include "input.h"
#include "ui_input.h"

Input::Input(QWidget *parent,QWidget* w) :
    QDialog(parent),
    ui(new Ui::Input)
{
    ui->setupUi(this);
    interW = w;
    connect(this, SIGNAL(sig_inputChanged(std::string)), interW, SLOT(update_LineEdit(std::string)));
    m_number = "";//need correct
}

Input::Input(QWidget *parent,QDialog* d) :
    QDialog(parent),
    ui(new Ui::Input)
{
    ui->setupUi(this);
    interD = d;
    connect(this, SIGNAL(sig_inputChanged(std::string)), interD, SLOT(update_LineEdit(std::string)));
    m_number = "";//need correct
}


Input::~Input()
{
    delete ui;
}

void Input::on_btn_n0_clicked()
{
    m_number += "0";
    updateInput();
}

void Input::on_btn_n1_clicked()
{
    m_number += "1";
    updateInput();
}

void Input::on_btn_n2_clicked()
{
    m_number += "2";
    updateInput();
}

void Input::on_btn_n3_clicked()
{
    m_number += "3";
    updateInput();
}

void Input::on_btn_n4_clicked()
{
    m_number += "4";
    updateInput();
}

void Input::on_btn_n5_clicked()
{
    m_number += "5";
    updateInput();
}

void Input::on_btn_n6_clicked()
{
    m_number += "6";
    updateInput();
}

void Input::on_btn_n7_clicked()
{
    m_number += "7";
    updateInput();
}

void Input::on_btn_n8_clicked()
{
    m_number += "8";
    updateInput();
}

void Input::on_btn_n9_clicked()
{
    m_number += "9";
    updateInput();
}

void Input::on_btn_point_clicked()
{
    m_number += ".";
    updateInput();
}

void Input::on_btn_ok_clicked()
{
    accept();
}

void Input::on_btn_clearOne_clicked()
{
    if(m_number.size() > 1)
        m_number = m_number.substr(0,m_number.size()-1);
    else
        m_number = "";
    updateInput();
}

void Input::on_btn_clearAll_clicked()
{
    m_number = "";
    updateInput();
}

void Input::updateInput()
{
    emit sig_inputChanged(m_number);
}
