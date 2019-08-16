#ifndef MOTORZ_H
#define MOTORZ_H


class MotorZ
{
public:
    MotorZ();

public:
    void connectDevice();//连接设备
    void disconnectDevice();//断开设备
    void backHome();//回Home位置
    void moveUP();//上移
    void moveDown();//下移
    void absoluteMove();//绝对位移
    void stopMove();//停止移动
    void enable();//使能
    void disabled();//去使能
    void getPosition();//位置反馈
    void bHomeLost();//是否失去Home点
    void UPLimit();//上限位
    void DownLimit();//下限位
    void reset();//报错恢复重置
};

#endif // MOTORZ_H
