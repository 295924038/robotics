#include "interface.h"
#include "ui_interface.h"
#include "pubfunc/mylineedit.h"
#include "input.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <QTimer>
#include <thread>
#include "loginon.h"
#include <stdlib.h>
#include <future>
#include "base/errorcode.h"


using namespace std;
using namespace boost;
using namespace cv;



interface::interface(QWidget *parent,MainWindow* mw) :
    QWidget(parent),
    _adjustCamera_ptr(Singleton<AdjustCamera>::get()),
    _recogNeedleCamera_ptr(Singleton<RecogNeedleCamera>::get()),
    _recogWeldCamera_ptr(Singleton<RecogWeldCamera>::get()),
    m_weldCamera_open(false),
    m_needleCamera_open(false),
    btakeWeldPictures(false),
    btakeNeedlePictures(false),
    m_bWelding(false),
    ui(new Ui::interface)
{
    ui->setupUi(this);
    mainWd = mw;
    ui->lineE_caliber->setAlignment(Qt::AlignCenter);
    ui->picture_weld->setScaledContents(true);
    ui->picture_needle->setScaledContents(true);
    qTimer = new QTimer(this);
    connect(ui->lineE_caliber, SIGNAL(clicked()), this, SLOT(lineE_caliber_clicked()));
    connect(this, SIGNAL(sig_close()), mainWd, SLOT(closeWindow()));

}

interface::~interface()
{
    delete ui;
}


void interface::on_btn_startWelding_clicked()
{

    if(m_bWelding)
    {
        m_bWelding = false;
    }else{
        m_bWelding = true;
        std::future<int> f1 = std::async(std::launch::async,&interface::welding,this);
        std::future<int> f2 = std::async(std::launch::async,&interface::trackTip,this);


    }

}

int interface::welding()
{
    //初始化参数
    //串口发送指令，开启继电器
    //识别焊缝相机，设置焊缝画线

}

int interface::trackTip()
{

}

void interface::lineE_caliber_clicked()
{
    Input input(NULL,this);
    input.show();
    input.exec();
}

void interface::update_LineEdit(std::string number)
{
    ui->lineE_caliber->setText(QString::fromStdString(number));
}

void interface::on_openWeldCamera_clicked()
{
    if(!m_weldCamera_open)
    {
        QObject::connect(qTimer, SIGNAL(timeout()),this, SLOT(showWeldPicture())) ;
        qTimer->start(100);
        m_weldCamera_open = true;
    }
}

void interface::showWeldPicture()
{
    cv::Mat mat = _adjustCamera_ptr->getImg();
    ui->picture_weld->setPixmap(QPixmap::fromImage(*((QImage*)(_adjustCamera_ptr->m_show_image))));
}

void interface::on_closeWeldCamera_clicked()
{
    if(m_weldCamera_open)
    {
        qTimer->stop();
        QObject::disconnect(qTimer, SIGNAL(timeout()),this, SLOT(showWeldPicture())) ;
    }

    _adjustCamera_ptr->disconnectDevice();
    m_weldCamera_open = false;
}

void interface::on_takeWeldPicture_clicked()
{
    std::thread([this]{
        btakeWeldPictures = true;
        while(btakeWeldPictures)
        {
            auto mat = _adjustCamera_ptr->takePicture();
            saveImg(mat);
        }
    }).detach();
}

void saveImg(const cv::Mat &mat, const string &path, const string &name)
{
    namespace bf = boost::filesystem ;
    using namespace std ;
    string strDir = "picture" ;
    boost::posix_time::ptime pt = boost::posix_time::microsec_clock::universal_time() ;
    // 以当前时间为默认图片文件名
    string strName = boost::posix_time::to_iso_string(pt) + ".jpg";
    if ( "" != path ) {
        strDir = path ;
    }
    if ( "" != name ) {
        strName = name ;
    }
    if ( !mat.data ) return ;

    if ( !bf::exists(strDir) ) {
        bf::create_directory(strDir);
    }
    imwrite(strDir + "/" + strName , mat) ;
}

void interface::on_stopWeldTake_clicked()
{
    btakeWeldPictures = false;
}

void interface::on_takeWeld_clicked()
{
    auto mat = _adjustCamera_ptr->takePicture();
    saveImg(mat);
}


void interface::on_openNeedleCamera_clicked()
{
    if(!m_needleCamera_open)
    {
        QObject::connect(qTimer, SIGNAL(timeout()),this, SLOT(showNeedlePicture())) ;
        qTimer->start(100);
        m_needleCamera_open = true;
    }
}

void interface::showNeedlePicture()
{
    cv::Mat mat = _recogNeedleCamera_ptr->getImg();
    ui->picture_needle->setPixmap(QPixmap::fromImage(*((QImage*)(_recogNeedleCamera_ptr->m_show_image))));
}

void interface::on_closeNeedleCamera_clicked()
{
    if(m_needleCamera_open)
    {
        qTimer->stop();
        QObject::disconnect(qTimer, SIGNAL(timeout()),this, SLOT(showNeedlePicture())) ;
    }

    _recogNeedleCamera_ptr->disconnectDevice();
    m_needleCamera_open = false;
}

void interface::on_takeNeedlePicture_clicked()
{
    std::thread([this]{
        btakeNeedlePictures = true;
        while(btakeNeedlePictures)
        {
            auto mat = _recogNeedleCamera_ptr->takePicture();
            saveImg(mat);
        }
    }).detach();
}

void interface::on_stopNeedleTake_clicked()
{
    btakeNeedlePictures = false;
}

void interface::on_takeNeedle_clicked()
{
    auto mat = _recogNeedleCamera_ptr->takePicture();
    saveImg(mat);
}

void interface::on_btn_changeRole_clicked()
{
    LoginOn *ln = new LoginOn;

    ln->setWindowFlags(Qt::Widget |Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    connect(ln,SIGNAL(changeUser()),mainWd,SLOT(updateMenu()));
    ln->show();
    ln->exec();
    delete ln;
}

void interface::on_btn_quit_clicked()
{
    emit sig_close();
}

void interface::on_btn_shutDown_clicked()
{
    //    system("poweroff");
}
