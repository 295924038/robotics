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
enum UPLimitOperate{TRIGGER,RELEASE};
enum DownLimitOperate{TRIGGER,RELEASE};
enum LeftLimitOperate{TRIGGER,RELEASE};
enum RightLimitOperate{TRIGGER,RELEASE};
enum LimitDirection{};
enum MotorType{MOTORX,MOTORZ};

class Motor
{
public:
    Motor();
    ~Motor();

public:
    void connectDevice();//连接设备
    void disconnectDevice();//断开设备
    void reconnectDevice();
    void backHomeX();//回Home位置
    void backHomeZ();//回Home位置
    void relayOpen();
    void relayCLose();
    void moveUP(double steps);//上移
    void moveDown(double steps);//下移
    void moveLeft(double steps);//左移
    void moveRight(double steps);//右移
    void absoluteMoveX(double steps);//X绝对位移
    void absoluteMoveZ(double steps);//Z绝对位移
    void stopMoveX();//停止X移动
    void stopMoveZ();//停止Z移动
    void requestPosition();
    void getPositionX();//X位置反馈
    void getPositionZ();//Z位置反馈
    void bXHomeLost();//是否失去Home点
    void bZHomeLost();//是否失去Home点
    void UPLimit(UPLimitOperate opt);//上限位
    void DownLimit(DownLimitOperate opt);//下限位
    void leftLimit(LeftLimitOperate opt);//左限位
    void rightLimit(RightLimitOperate opt);//右限位
    void reset();//报错恢复重置
    void enableX();//使能X
    void disabledX();//去使能X
    void enableZ();//使能Z
    void disabledZ();//去使能Z

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
