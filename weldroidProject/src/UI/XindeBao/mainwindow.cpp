#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pubfunc/mylineedit.h"
#include "Input/input.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <QTimer>
#include <thread>


using namespace std;
using namespace boost;
using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _adjustCamera_ptr(Singleton<AdjustCamera>::get()),
    _recogNeedleCamera_ptr(Singleton<RecogNeedleCamera>::get()),
    _recogWeldCamera_ptr(Singleton<RecogWeldCamera>::get()),
    m_weldCamera_open(false),
    m_needleCamera_open(false),
    btakeWeldPictures(false),
    btakeNeedlePictures(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineE_caliber->setAlignment(Qt::AlignCenter);
    ui->picture_weld->setScaledContents(true);
    ui->picture_needle->setScaledContents(true);
    qTimer = new QTimer(this);
    connect(ui->lineE_caliber, SIGNAL(clicked()), this, SLOT(lineE_caliber_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_startWelding_clicked()
{

}

void MainWindow::lineE_caliber_clicked()
{
    Input input(NULL,this);
    input.show();
    input.exec();
}

void MainWindow::update_lineE_caliber(std::string input)
{
    ui->lineE_caliber->setText(QString::fromStdString(input));
}

void MainWindow::on_openWeldCamera_clicked()
{
    if(!m_weldCamera_open)
    {
        QObject::connect(qTimer, SIGNAL(timeout()),this, SLOT(showWeldPicture())) ;
        qTimer->start(100);
        m_weldCamera_open = true;
    }
}

void MainWindow::showWeldPicture()
{
    cv::Mat mat = _recogWeldCamera_ptr->getImg();
    ui->picture_weld->setPixmap(QPixmap::fromImage(*((QImage*)(_recogWeldCamera_ptr->m_show_image))));
}

void MainWindow::on_closeWeldCamera_clicked()
{
    if(m_weldCamera_open)
    {
        qTimer->stop();
        QObject::disconnect(qTimer, SIGNAL(timeout()),this, SLOT(showWeldPicture())) ;
    }

    _recogWeldCamera_ptr->disconnectDevice();
    m_weldCamera_open = false;
}

void MainWindow::on_takeWeldPicture_clicked()
{
    std::thread([this]{
        btakeWeldPictures = true;
        while(btakeWeldPictures)
        {
            auto mat = _recogWeldCamera_ptr->takePicture();
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

void MainWindow::on_stopWeldTake_clicked()
{
    btakeWeldPictures = false;
}

void MainWindow::on_takeWeld_clicked()
{
    auto mat = _recogWeldCamera_ptr->takePicture();
    saveImg(mat);
}


void MainWindow::on_openNeedleCamera_clicked()
{
    if(!m_needleCamera_open)
    {
        QObject::connect(qTimer, SIGNAL(timeout()),this, SLOT(showNeedlePicture())) ;
        qTimer->start(100);
        m_needleCamera_open = true;
    }
}

void MainWindow::showNeedlePicture()
{
    cv::Mat mat = _recogNeedleCamera_ptr->getImg();
    ui->picture_needle->setPixmap(QPixmap::fromImage(*((QImage*)(_recogNeedleCamera_ptr->m_show_image))));
}

void MainWindow::on_closeNeedleCamera_clicked()
{
    if(m_needleCamera_open)
    {
        qTimer->stop();
        QObject::disconnect(qTimer, SIGNAL(timeout()),this, SLOT(showNeedlePicture())) ;
    }

    _recogNeedleCamera_ptr->disconnectDevice();
    m_needleCamera_open = false;
}

void MainWindow::on_takeNeedlePicture_clicked()
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

void MainWindow::on_stopNeedleTake_clicked()
{
    btakeNeedlePictures = false;
}

void MainWindow::on_takeNeedle_clicked()
{
    auto mat = _recogNeedleCamera_ptr->takePicture();
    saveImg(mat);
}
