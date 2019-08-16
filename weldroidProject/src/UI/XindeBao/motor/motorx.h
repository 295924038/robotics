#ifndef MOTORX_H
#define MOTORX_H
#include <iostream>

using namespace std;

class MotorX
{
public:
    MotorX();
    ~MotorX();

public:
    void connectDevice();//连接设备
    void disconnectDevice();//断开设备
    void backHome();//回Home位置
    void moveLeft();//左移
    void moveRight();//右移
    void absoluteMove(double);//绝对位移
    void stopMove();//停止移动
    void getPosition();//位置反馈
    void bHomeLost();//是否失去Home点
    void leftLimit();//左限位
    void rightLimit();//右限位
    void reset();//报错恢复重置
    void enable();//使能
    void disabled();//去使能
private:


};

#endif // MOTORX_H
