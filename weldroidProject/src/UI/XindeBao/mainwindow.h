#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "base/singleton.h"
#include "camera/adjustCamera.h"
#include "camera/recogNeedleCamera.h"
#include "camera/recogWeldCamera.h"
#include <QWidget>

namespace Ui {
class MainWindow;
}


enum role{consumer=0,admin,root};
enum systemState{BUSY=0,IDLE};

extern int user;
extern std::string getCurrentRole();
extern void changeCurrentUser(const QString&);
extern int getEqualUser(const QString&);


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:

    void inputShow();

    void updatePasswdInput();

    void closeWindow();

    void updateMenu();
private:
    Ui::MainWindow *ui;
    Singleton<AdjustCamera> _adjustCamera;
    Singleton<RecogNeedleCamera> _recogNeedleCamera;
    Singleton<RecogWeldCamera> _recogWeldCamera;
    QTabWidget* tabwidget;
    QWidget* _interfaceUI;
    QWidget* _preferencesUI;


};

#endif // MAINWINDOW_H
