#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <mutex>

/// 单例类模板[线程安全] [实例类的线程安全需要自己实现]
template <typename Device>
class Singleton
{
public:
    /// 实例类的构造
    /// @param args 支持任意形式的构造函数
    template <typename ... Args>
    Singleton( Args && ... args )
    {
        std::lock_guard<std::recursive_mutex> lck( _mtx ) ;
        if ( nullptr == _device_ptr ) {
            _device_ptr.reset(new Device(std::forward<Args>(args)...)) ;
        }
    }

    /// &符号的重载
    Device *operator&()
    {
        return _device_ptr.get() ;
    }

    /// 获取当前实例，获取前必须要确保构造已经执行
    static std::shared_ptr<Device> get()
    {
        return _device_ptr ;
    }

    /// ->符号的重载
    std::shared_ptr<Device> operator->()
    {
        return _device_ptr ;
    }

    /// 析构
    ~Singleton()
    {
        _device_ptr.reset() ;
    }

private:
    /// 全局线程同步锁，每一个类对应一个
    static std::recursive_mutex _mtx ;
    /// 指向实例对象的智能指针，每个实例对象一个
    static std::shared_ptr<Device> _device_ptr ;
};

template <typename Device>
std::recursive_mutex Singleton<Device>::_mtx ;

template <typename Device>
std::shared_ptr<Device> Singleton<Device>::_device_ptr ;

#endif // SINGLETON_H
