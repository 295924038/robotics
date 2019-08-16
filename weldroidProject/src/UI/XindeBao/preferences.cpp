#include "preferences.h"
#include "ui_preferences.h"
#include "base/config.h"
#include <iostream>
#include "base/singleton.h"

using namespace std;


Preferences::Preferences(QWidget *parent) :
    QWidget(parent),
    _adjustCamera_ptr(Singleton<AdjustCamera>::get()),
    _recogNeedleCamera_ptr(Singleton<RecogNeedleCamera>::get()),
    _recogWeldCamera_ptr(Singleton<RecogWeldCamera>::get()),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    initUI();
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::initUI()
{
    Config _sys_config("etc/sys.info");
    ui->le_adjust_exposure->setText(QString::number(_sys_config.get<int>("AdjustCamera.Exposure",0)));
    ui->le_adjust_gain->setText(QString::number(_sys_config.get<int>("AdjustCamera.Gain",0)));
    ui->le_adjust_offsetX->setText(QString::number(_sys_config.get<int>("AdjustCamera.AOI.OffsetX",0)));
    ui->le_adjust_offsetY->setText(QString::number(_sys_config.get<int>("AdjustCamera.AOI.OffsetY",0)));
    ui->le_adjust_width->setText(QString::number(_sys_config.get<int>("AdjustCamera.AOI.Width",0)));
    ui->le_adjust_height->setText(QString::number(_sys_config.get<int>("AdjustCamera.AOI.Height",0)));
    ui->le_weld_exposure->setText(QString::number(_sys_config.get<int>("RecogNeedleCamera.Exposure",0)));
    ui->le_weld_gain->setText(QString::number(_sys_config.get<int>("RecogNeedleCamera.Gain",0)));
    ui->le_weld_offsetX->setText(QString::number(_sys_config.get<int>("RecogNeedleCamera.AOI.OffsetX",0)));
    ui->le_weld_offsetY->setText(QString::number(_sys_config.get<int>("RecogNeedleCamera.AOI.OffsetY",0)));
    ui->le_weld_width->setText(QString::number(_sys_config.get<int>("RecogNeedleCamera.AOI.Width",0)));
    ui->le_weld_height->setText(QString::number(_sys_config.get<int>("RecogNeedleCamera.AOI.Height",0)));
    ui->le_needle_exposure->setText(QString::number(_sys_config.get<int>("RecogWeldCamera.Exposure",0)));
    ui->le_needle_gain->setText(QString::number(_sys_config.get<int>("RecogWeldCamera.Gain",0)));
    ui->le_needle_offsetX->setText(QString::number(_sys_config.get<int>("RecogWeldCamera.AOI.OffsetX",0)));
    ui->le_needle_offsetY->setText(QString::number(_sys_config.get<int>("RecogWeldCamera.AOI.OffsetY",0)));
    ui->le_needle_width->setText(QString::number(_sys_config.get<int>("RecogWeldCamera.AOI.Width",0)));
    ui->le_needle_height->setText(QString::number(_sys_config.get<int>("RecogWeldCamera.AOI.Height",0)));
}


void Preferences::on_btn_save_clicked()
{

}

void Preferences::on_btn_weld_save_clicked()
{
    Config _sys_config("etc/sys.info");
    _sys_config.put<int>("RecogWeldCamera.Exposure",ui->le_weld_exposure->text().toInt());
    _sys_config.get<int>("RecogWeldCamera.Gain",ui->le_weld_gain->text().toInt());
    int offsetX,offsetY,width,height;
    offsetX = ui->le_weld_offsetX->text().toInt();
    offsetY = ui->le_weld_offsetY->text().toInt();
    width = ui->le_weld_width->text().toInt();
    height = ui->le_weld_height->text().toInt();
    _sys_config.get<int>("RecogWeldCamera.AOI.OffsetX",offsetX);
    _sys_config.get<int>("RecogWeldCamera.AOI.OffsetY",offsetY);
    _sys_config.get<int>("RecogWeldCamera.AOI.Width",width);
    _sys_config.get<int>("RecogWeldCamera.AOI.Height",height);
    _sys_config.sync();
    _recogWeldCamera_ptr->setExposure(ui->le_weld_exposure->text().toInt());
    _recogWeldCamera_ptr->setGain(ui->le_weld_gain->text().toInt());
    _recogWeldCamera_ptr->setRoi(offsetX,offsetY,width,height);
}

void Preferences::on_btn_adjust_save_clicked()
{
    Config _sys_config("etc/sys.info");
    _sys_config.put<int>("AdjustCamera.Exposure",ui->le_adjust_exposure->text().toInt());
    _sys_config.get<int>("AdjustCamera.Gain",ui->le_adjust_gain->text().toInt());
    int offsetX,offsetY,width,height;
    offsetX = ui->le_adjust_offsetX->text().toInt();
    offsetY = ui->le_adjust_offsetY->text().toInt();
    width = ui->le_adjust_width->text().toInt();
    height = ui->le_adjust_height->text().toInt();
    _sys_config.get<int>("AdjustCamera.AOI.OffsetX",offsetX);
    _sys_config.get<int>("AdjustCamera.AOI.OffsetY",offsetY);
    _sys_config.get<int>("AdjustCamera.AOI.Width",width);
    _sys_config.get<int>("AdjustCamera.AOI.Height",height);
    _sys_config.sync();
    _adjustCamera_ptr->setExposure(ui->le_adjust_exposure->text().toInt());
    _adjustCamera_ptr->setGain(ui->le_adjust_gain->text().toInt());
    _adjustCamera_ptr->setRoi(offsetX,offsetY,width,height);
}

void Preferences::on_btn_needle_save_clicked()
{
    Config _sys_config("etc/sys.info");
    _sys_config.put<int>("RecogNeedleCamera.Exposure",ui->le_needle_exposure->text().toInt());
    _sys_config.get<int>("RecogNeedleCamera.Gain",ui->le_needle_gain->text().toInt());
    int offsetX,offsetY,width,height;
    offsetX = ui->le_needle_offsetX->text().toInt();
    offsetY = ui->le_needle_offsetY->text().toInt();
    width = ui->le_needle_width->text().toInt();
    height = ui->le_needle_height->text().toInt();
    _sys_config.get<int>("RecogNeedleCamera.AOI.OffsetX",offsetX);
    _sys_config.get<int>("RecogNeedleCamera.AOI.OffsetY",offsetY);
    _sys_config.get<int>("RecogNeedleCamera.AOI.Width",width);
    _sys_config.get<int>("RecogNeedleCamera.AOI.Height",height);
    _sys_config.sync();
    _recogNeedleCamera_ptr->setExposure(ui->le_adjust_exposure->text().toInt());
    _recogNeedleCamera_ptr->setGain(ui->le_adjust_gain->text().toInt());
    _recogNeedleCamera_ptr->setRoi(offsetX,offsetY,width,height);
}
