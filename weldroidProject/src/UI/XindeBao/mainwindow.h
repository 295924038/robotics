#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Input/input.h"
#include "base/singleton.h"
#include "camera/adjustCamera.h"
#include "camera/recogNeedleCamera.h"
#include "camera/recogWeldCamera.h"
#include <boost/shared_ptr.hpp>
#include <string>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_btn_startWelding_clicked();

    void lineE_caliber_clicked();

    void update_lineE_caliber(std::string input);

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

public slots:

    void showWeldPicture();

    void showNeedlePicture();

private:
    Ui::MainWindow *ui;
    Singleton<AdjustCamera> _adjustCamera;
    Singleton<RecogNeedleCamera> _recogNeedleCamera;
    Singleton<RecogWeldCamera> _recogWeldCamera;
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

void saveImg(const cv::Mat &mat, const std::string &path ="", const std::string &name = "");

#endif // MAINWINDOW_H
