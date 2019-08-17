#ifndef INTERFACE_H
#define INTERFACE_H

#include <QWidget>
#include "input.h"
#include "base/singleton.h"
#include "camera/adjustCamera.h"
#include "camera/recogNeedleCamera.h"
#include "camera/recogWeldCamera.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include "mainwindow.h"

namespace Ui {
class interface;
}

class MainWindow;

class interface : public QWidget
{
    Q_OBJECT

public:
    explicit interface(QWidget *parent = 0,MainWindow* mw = 0);
    ~interface();

private slots:

    void on_btn_startWelding_clicked();

    void lineE_caliber_clicked();

    void update_LineEdit(std::string number);

    void on_openWeldCamera_clicked();

    void on_closeWeldCamera_clicked();

    void on_takeWeldPicture_clicked();

    void on_stopWeldTake_clicked();

    void on_takeWeld_clicked();

    void on_openNeedleCamera_clicked();

    void on_closeNeedleCamera_clicked();

    void on_takeNeedlePicture_clicked();

    void on_stopNeedleTake_clicked();

    void on_takeNeedle_clicked();

    void on_btn_changeRole_clicked();

    void on_btn_quit_clicked();

    void on_btn_shutDown_clicked();

public slots:

    void showWeldPicture();

    void showNeedlePicture();

signals:

    void changeSeam();


private:
    Ui::interface *ui;
    MainWindow* mainWd;
    std::shared_ptr<AdjustCamera> _adjustCamera_ptr;
    std::shared_ptr<RecogNeedleCamera> _recogNeedleCamera_ptr;
    std::shared_ptr<RecogWeldCamera> _recogWeldCamera_ptr;
    QTimer* qTimer;
    QImage* m_show_image;
    bool m_weldCamera_open;
    bool m_needleCamera_open;
    bool btakeWeldPictures;
    bool btakeNeedlePictures;
    void* m_result_image;
    double m_dCaliber;//管径
};

void saveImg(const cv::Mat &mat, const std::string &path = "", const std::string &name = "");


#endif // INTERFACE_H
