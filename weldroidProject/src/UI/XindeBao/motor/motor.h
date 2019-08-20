#ifndef MOTOR_H
#define MOTOR_H
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>

#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <boost/timer.hpp>

using namespace std;
enum LimitOperate{TRIGGER,RELEASE};
enum Direction{UP,DOWN,LEFT,RIGHT};
enum MotorType{MOTORX,MOTORZ};
enum LedType{LED1,LED2};
enum Control{OPEN,CLOSE};


class Motor
{
public:
    Motor();
    ~Motor();

public:
    //send
    void connectDevice();//连接设备
    void disconnectDevice();//断开设备
    void reconnectDevice();
    void backHome(MotorType mt);//回Home位置
    void move(Direction d,int steps);//上移
    void absMove(MotorType mt,int steps);//X绝对位移
    void stopMove(MotorType mt);//停止X移动
    void requestPosition();
    void limit(Direction d,LimitOperate opt);//上限位
    void reset();//报错恢复重置
    void enable(MotorType mt);//使能X
    void disabled(MotorType mt);//去使能X
    void Led(LedType lt,Control c);
    void setWeld(Control c);
    //receive
    void getPositionX();//X位置反馈
    void getPositionZ();//Z位置反馈
    void bXHomeLost();//是否失去Home点
    void bZHomeLost();//是否失去Home点
    /// 存放命令
    unsigned char _cmd[16] ;

    /// 存放应答指令
    unsigned char _readBuffer[16] ;

    /// 线程同步锁
    std::recursive_mutex _mtx ;
    /// 条件变量
    std::condition_variable _condReply ;
    /// 条件变量锁
    std::mutex _mtxReply ;
    /// 是否已经应答
    bool _bReply ;
    /// asio 服务管理
    boost::asio::io_service _service ;
    /// 激光控制通信管理
    std::shared_ptr<boost::asio::serial_port> _serialPort_ptr ;

    bool _bOpened ;

    std::thread _thService ;

private:
    /// 设备应答
    void recvReply( const boost::system::error_code &ec, size_t ) ;
    /// 等待设备应答
    void waitReply() ;



};

#endif // MOTOR_H
