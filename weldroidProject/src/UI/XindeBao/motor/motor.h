#ifndef MOTOR_H
#define MOTOR_H
#include <iostream>

using namespace std;

class Motor
{
public:
    Motor();
    ~Motor();

public:
    void connectDevice();//连接设备
    void disconnectDevice();//断开设备
    void backHome();//回Home位置
    void moveUP();//上移
    void moveDown();//下移
    void moveLeft();//左移
    void moveRight();//右移
    void absoluteMoveX(double);//X绝对位移
    void absoluteMoveZ(double);//Z绝对位移
    void stopMove();//停止移动
    void getPositionX();//X位置反馈
    void getPositionZ();//Z位置反馈
    void bXHomeLost();//是否失去Home点
    void bZHomeLost();//是否失去Home点
    void UPLimit();//上限位
    void DownLimit();//下限位
    void leftLimit();//左限位
    void rightLimit();//右限位
    void reset();//报错恢复重置
    void enable();//使能
    void disabled();//去使能

private:


};

#endif // MOTOR_H
