#include "input.h"
#include "ui_input.h"

Input::Input(QWidget *parent,MainWindow* mainwind) :
    QDialog(parent),
    ui(new Ui::Input)
{
    ui->setupUi(this);
    mw = mainwind;
    connect(this, SIGNAL(sig_inputChanged(std::string)), mw, SLOT(update_lineE_caliber(std::string)));
    m_number = "";//need correct
}

Input::~Input()
{
    delete ui;
}

void Input::on_btn_n0_clicked()
{
    emit sig_inputChanged(m_number += "0");
}

void Input::on_btn_n1_clicked()
{
    emit sig_inputChanged(m_number += "1");
}

void Input::on_btn_n2_clicked()
{
    emit sig_inputChanged(m_number += "2");
}

void Input::on_btn_n3_clicked()
{
    emit sig_inputChanged(m_number += "3");
}

void Input::on_btn_n4_clicked()
{
    emit sig_inputChanged(m_number += "4");
}

void Input::on_btn_n5_clicked()
{
    emit sig_inputChanged(m_number += "5");
}

void Input::on_btn_n6_clicked()
{
    emit sig_inputChanged(m_number += "6");
}

void Input::on_btn_n7_clicked()
{
    emit sig_inputChanged(m_number += "7");
}

void Input::on_btn_n8_clicked()
{
    emit sig_inputChanged(m_number += "8");
}

void Input::on_btn_n9_clicked()
{
    emit sig_inputChanged(m_number += "9");
}

void Input::on_btn_point_clicked()
{
    emit sig_inputChanged(m_number += ".");
}

void Input::on_btn_ok_clicked()
{
    accept();
}

void Input::on_btn_clearOne_clicked()
{
    if(m_number.size() > 1)
    {
        emit sig_inputChanged(m_number = m_number.substr(0,m_number.size()-1));
    }else
    {
        emit sig_inputChanged(m_number = "");
    }
}

void Input::on_btn_clearAll_clicked()
{
    emit sig_inputChanged(m_number = "");
}
