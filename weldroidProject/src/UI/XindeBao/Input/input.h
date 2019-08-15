#ifndef INPUT_H
#define INPUT_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class Input;
}
class MainWindow;

class Input : public QDialog
{
    Q_OBJECT

public:
    explicit Input(QWidget *parent = 0,MainWindow* mainwind = 0);
    ~Input();

private slots:

    void on_btn_n0_clicked();

    void on_btn_n1_clicked();

    void on_btn_n2_clicked();

    void on_btn_n3_clicked();

    void on_btn_n4_clicked();

    void on_btn_n5_clicked();

    void on_btn_n6_clicked();

    void on_btn_n7_clicked();

    void on_btn_n8_clicked();

    void on_btn_n9_clicked();

    void on_btn_point_clicked();

    void on_btn_ok_clicked();

    void on_btn_clearOne_clicked();

    void on_btn_clearAll_clicked();


signals:
    void sig_inputChanged(std::string userInput);

public:
    std::string m_number;

private:
    Ui::Input *ui;
    MainWindow *mw;
};

#endif // INPUT_H
