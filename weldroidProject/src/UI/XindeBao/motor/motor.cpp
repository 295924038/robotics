#include "motor/motor.h"
#include "base/config.h"
#include <iostream>
#include <thread>

#include <boost/thread.hpp>
#include <boost/algorithm/hex.hpp>


#include "base/core.h"
#include "base/base.h"
#include "base/wzerror.h"

using namespace std ;
using namespace boost::asio;

Motor::Motor()
{
    connectDevice();
}

//连接设备
void Motor::connectDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    try {
        if ( !_serialPort_ptr ) {
            Config config("etc/sys.info") ;
            _serialPort_ptr.reset( new  serial_port(_service, config.get<std::string>("Motor.serialPort")) );
            _serialPort_ptr->set_option(serial_port::baud_rate(config.get<int>("Motor.baudRate"))) ;
            _serialPort_ptr->set_option(serial_port::flow_control(serial_port::flow_control::none)) ;
            _serialPort_ptr->set_option(serial_port::parity(serial_port::parity::none)) ;
            _serialPort_ptr->set_option(serial_port::stop_bits(serial_port::stop_bits::one)) ;
            _serialPort_ptr->set_option(serial_port::character_size(serial_port::character_size(8))) ;
            boost::asio::async_read( *_serialPort_ptr, boost::asio::buffer(_readBuffer, 2),
                                     boost::bind( &Motor::recvReply, this, _1, _2 ) ) ;
            _thService = std::thread([this]() {_service.run();}) ;
            std::cout << "connect motor success\n" << std::flush ;
        }
    }
    catch ( const boost::system::system_error &e ) {
        std::cout << e.what() << std::endl ;
    }
    catch ( const boost::exception &e ) {
        DUMPERROR(e) ;
    }
}
//断开设备
void Motor::disconnectDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if ( _serialPort_ptr ) {
        if ( _serialPort_ptr->is_open() ) {
            //this->stop() ;
            _service.stop() ;
            _serialPort_ptr->close();
            _thService.join() ;
        }
        _serialPort_ptr = nullptr;
    }
}
void Motor::reconnectDevice()
{
    disconnectDevice();
    connectDevice();

}

//回Home位置
void Motor::backHome(MotorType mt)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[2] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if(mt == MOTORX)
        {
            _cmd[1] = 0xE1 ;
        }
        else//motorZ
        {
            _cmd[1] = 0xE6 ;
        }

        _cmd[2] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

void Motor::setWeld(Control rc)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        _cmd[1] = 0xEB ;
        if(rc == OPEN)
        {
            _cmd[2] = 0x01 ;
        }else{
            _cmd[2] = 0x00 ;
        }
        _cmd[3] = 0x00 ;
        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

//左移
void Motor::move(Direction d,int steps)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[4] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if( d == LEFT)
        {
            _cmd[1] = 0xE2 ;
        }else if(d == RIGHT){
            _cmd[1] = 0xE3 ;
        }else if(d == UP){
            _cmd[1] = 0xE7 ;
        }else if(d == DOWN){
            _cmd[1] = 0xE8 ;
        }

        steps = std::abs(steps);
        for ( int i = 0; i < 2; ++i ) {
            _cmd[3-i] = steps % 256 ;
            steps >>= 8 ;
        }
        _cmd[4] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

//停止移动
void Motor::stopMove(MotorType mt)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if(mt == MOTORX)
        {
            _cmd[1] = 0xE5 ;
        }else{
            _cmd[1] = 0xEA ;
        }
        _cmd[2] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}


void Motor::requestPosition()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        _cmd[1] = 0xD1 ;
        _cmd[2] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

//X位置反馈
void Motor::getPositionX()
{

}
//Z位置反馈
void Motor::getPositionZ()
{

}
//是否失去Home点
void Motor::bXHomeLost()
{

}

//报错恢复重置
void Motor::reset()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[4] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0xA5 ;
        _cmd[1] = 0xDD ;
        _cmd[2] = 0xFF ;
        _cmd[3] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

//使能
void Motor::enable(MotorType mt)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if(mt == MOTORX)
        {
            _cmd[1] = 0xDB ;
        }else{
            _cmd[1] = 0xDC ;
        }

        _cmd[2] = 0x01 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}
//去使能
void Motor::disabled(MotorType mt)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if(mt == MOTORX)
        {
            _cmd[1] = 0xDB ;
        }else{
            _cmd[1] = 0xDC ;
        }
        _cmd[2] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

//X绝对位移
void Motor::absMove(MotorType mt,int steps)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[6] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if(mt == MOTORX)
        {
            _cmd[1] = 0xE4 ;
        }else{
            _cmd[1] = 0xE9 ;
        }

        steps = std::abs(steps);
        for ( int i = 0; i < 3; ++i ) {
            _cmd[4-i] = steps % 256 ;
            steps >>= 8 ;
        }
        _cmd[5] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }

}

//限位
void Motor::limit(Direction d,LimitOperate opt)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0xA5 ;

        if(d == LEFT)
        {
            if(opt == TRIGGER)
            {
                _cmd[1] = 0xD9 ;
                _cmd[2] = 0x00 ;//00 触发;01 恢复
            }else if(opt == RELEASE){
                _cmd[1] = 0xD9 ;
                _cmd[2] = 0x01 ;
            }
        }else if(d == RIGHT){
            if(opt == TRIGGER)
            {
                _cmd[1] = 0xDA ;
                _cmd[2] = 0x00 ;
            }else if(opt == RELEASE){
                _cmd[1] = 0xDA ;
                _cmd[2] = 0x01 ;
            }
        }else if(d == UP){
            if(opt == TRIGGER)
            {
                _cmd[1] = 0xD7 ;
                _cmd[2] = 0x00 ;
            }else if(opt == RELEASE){
                _cmd[1] = 0xD7 ;
                _cmd[2] = 0x01 ;
            }
        }else if(d == DOWN){
            if(opt == TRIGGER)
            {
                _cmd[1] = 0xD8 ;
                _cmd[2] = 0x00 ;
            }else if(opt == RELEASE){
                _cmd[1] = 0xD9 ;
                _cmd[2] = 0x01 ;
            }
        }
        _cmd[3] = 0x00 ;
        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

void Motor::Led(LedType lt,Control c)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if(lt == LED1)
        {
            if(c == OPEN)
            {
                _cmd[1] = 0xDE ;
                _cmd[2] = 0x01 ;
            }else{
                _cmd[1] = 0xDE ;
                _cmd[2] = 0x00 ;
            }
        }else if(lt == LED2){
            if(c == OPEN)
            {
                _cmd[1] = 0xDF ;
                _cmd[2] = 0x01 ;
            }else{
                _cmd[1] = 0xDF ;
                _cmd[2] = 0x00 ;
            }
        }
        _cmd[3] = 0x01 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

void Motor::recvReply( const boost::system::error_code &ec, size_t )
{
    if ( !ec ) {
        std::string out ;
        boost::algorithm::hex(_readBuffer, std::back_inserter(out)) ;
        std::cout << out << "\n" << std::flush ;
        boost::asio::async_read( *_serialPort_ptr, boost::asio::buffer(_readBuffer, 2),
                                 boost::bind( &Motor::recvReply, this, _1, _2 ) ) ;
        //        emit start_sig();
        _bReply = true ;
        _condReply.notify_one() ;
    }
}

void Motor::waitReply()
{
    using namespace std::chrono ;
    Millsecond m1 = steady_clock::now() ;
    std::unique_lock<std::mutex> lck(_mtxReply) ;
    // 考虑到异常情况，设置超时时间
    _condReply.wait_for( lck, 100_ms, [this]{return _bReply ;}) ;
    Millsecond m2 = steady_clock::now() ;
    std::cout << (m2 - m1).count() << std::endl ;
    std::this_thread::sleep_for( 50_ms ) ;
}
