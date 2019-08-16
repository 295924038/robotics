#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>
#include "camera/adjustCamera.h"
#include "camera/recogNeedleCamera.h"
#include "camera/recogWeldCamera.h"
#include <boost/shared_ptr.hpp>

namespace Ui {
class Preferences;
}

class Preferences : public QWidget
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

public:
    void initUI();

private slots:
    void on_btn_save_clicked();

    void on_btn_adjust_save_clicked();

    void on_btn_weld_save_clicked();

    void on_btn_needle_save_clicked();

private:
    Ui::Preferences *ui;
    std::shared_ptr<AdjustCamera> _adjustCamera_ptr;
    std::shared_ptr<RecogNeedleCamera> _recogNeedleCamera_ptr;
    std::shared_ptr<RecogWeldCamera> _recogWeldCamera_ptr;

};

#endif // PREFERENCES_H
