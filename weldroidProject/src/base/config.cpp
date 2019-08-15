#include "base/config.h"

#include <iostream>
#include <vector>
#include <string>
using namespace std ;
#include <Eigen/Dense>

#include <boost/signals2/signal.hpp>
#include <boost/algorithm/string.hpp>

map<string, Config::ConfigInfo> Config::_mapPtree ;
recursive_mutex Config::_mtx ;
string Config::_robotName ;

Config::Config( const decltype(ConfigInfo::pt_ptr->get_child_optional("")) &pt )
    : _bWritten{false}
{
    lock_guard<recursive_mutex> lck(_mtx) ;
    _mapPtree[_fileName].refName = _fileName ;
    _pt_ptr.reset( new boost::property_tree::ptree(*pt) ) ;
    _mtx_ptr = &(_mapPtree[_fileName].mtx) ;
}

Config::Config( const char *pstr, bool bWritten, bool useRobot )
    :Config(string(pstr), bWritten, useRobot)
{
}

Config::Config( const string &fileName, bool bWritten, bool useRobot )
    :_fileName(fileName),
      _bWritten{bWritten}
{
    namespace fs = boost::filesystem ;
    using namespace boost::property_tree ;
    using namespace std ;
    if ( "" == _fileName) {
        _fileName = "etc/sys.info" ;
    }

    lock_guard<recursive_mutex> lck(_mtx) ;

    if ( _mapPtree.end() == _mapPtree.find(_fileName) ) {
        auto &configInfo = _mapPtree[_fileName] ;
        useRobot = false;
        configInfo.useRobot = useRobot;
        configInfo.bDirty = false ;
        configInfo.pt_ptr.reset( new ptree ) ;
        // 读入对应的机器人相关的配置文件， 如果没有则读入逻辑配置文件
        auto &refName = _mapPtree[_fileName].refName ;
        if ( useRobot && "" != _robotName && _fileName != "catch.info") {

            vector<string> str_vec ;
            boost::split(str_vec, _fileName, boost::is_any_of(".")) ;
            if ( str_vec.size() > 1 ) {
                refName = str_vec[0]+ "_" + _robotName + "." + str_vec[1];
            }
        }
        else {
            _mapPtree[_fileName].refName = _fileName ;
        }
        cout << "_mapPtree[_fileName].refName:" << _mapPtree[_fileName].refName << "\n" << flush ;
        try {
            if ( !fs::exists(_mapPtree[_fileName].refName)) {
                if ( !fs::exists(_fileName)) {
                    fs::create_directories(fs::path(_mapPtree[_fileName].refName).remove_filename()) ;
                    ofstream(_mapPtree[_fileName].refName) ;
                }
                else {
                    fs::copy( _fileName, _mapPtree[_fileName].refName) ;
                }
            }
            read_info( _mapPtree[_fileName].refName, *(_mapPtree[_fileName].pt_ptr) ) ;
        }
        catch ( const fs::filesystem_error &e ) {
        }
    }
    _mtx_ptr = &(_mapPtree[_fileName].mtx) ;
    _pt_ptr = _mapPtree[_fileName].pt_ptr ;
    _connRefChanged = _mapPtree[_fileName].refChanged_sig.connect([this](){
        this->updateRef();}) ;
}

Config::~Config()
{
    lock_guard<recursive_mutex> lck(_mtx) ;
    _connRefChanged.disconnect() ;
    sync() ;
}

/// 更新指向的配置文件[使用时的逻辑名不变，所有逻辑文件指向的配置文件将会发生改变]
/// 如Config("xxx").changeRobot("kuka") 则在所有使用Config("xxx")的地方，实际使用的将是"xxx_kuka"
/// @param [in] robotName 指向新的配置文件
void Config::changeRobot( const string &robotName )
{
    using namespace std ;
    namespace fs = boost::filesystem ;
    lock_guard<recursive_mutex> lck(_mtx) ;
    _robotName = robotName ;
    cout << _robotName << "\n" << flush;
    for ( auto &p : _mapPtree ) {
        const auto &fileName = p.first ;
        const auto &coninfo = p.second;
        if (!coninfo.useRobot)
            continue;

        vector<string> str_vec ;
        boost::split(str_vec, fileName, boost::is_any_of(".")) ;
        if ( str_vec.size() > 1 ) {
            auto refName = str_vec[0]+ "_" + _robotName + "." + str_vec[1];
            if ( !fs::exists(refName) ) {
                fs::copy_file(p.second.refName, refName);
            }
            Config(fileName).changeRef(refName) ;
        }
    }
}

void Config::changeRef( const string &fileName )
{
    using namespace boost::property_tree ;
    namespace fs = boost::filesystem ;
    lock_guard<recursive_mutex> lck(_mtx) ;
    if ( "etc/catch.info" == _fileName ) return ;
    auto &refName = _mapPtree[_fileName].refName ;
    auto &fileInfo = _mapPtree[_fileName] ;
    if ( fileName == refName ) return ;
    sync() ;
    fileInfo.pt_ptr.reset(new ptree) ;
    if ( fs::exists(fileName) ) {
        read_info(fileName, *(fileInfo.pt_ptr)) ;
    }
    refName = fileName ;
    _pt_ptr = fileInfo.pt_ptr ;
    fileInfo.refChanged_sig() ;
    cout << "changeRef:" << refName << endl << flush;
}

void Config::updateRef()
{
    lock_guard<recursive_mutex> lck(_mtx) ;
    sync() ;
    _pt_ptr = _mapPtree[_fileName].pt_ptr ;
    cout << "updateRef:" << _fileName << endl << flush;
}

auto Config::get_child_optional( const boost::property_tree::path &path ) const -> decltype(ConfigInfo::pt_ptr->get_child_optional(path))
{
    return _pt_ptr->get_child_optional(path) ;
}

bool Config::del( const boost::property_tree::path &p )
{
    lock_guard<recursive_mutex> lck(*_mtx_ptr) ;
    auto iter = _pt_ptr->to_iterator(_pt_ptr->find(p.dump())) ;
    if ( _pt_ptr->end() == iter ) return false ;
    _pt_ptr->erase(iter) ;
    _mapPtree[_fileName].bDirty = true ;
    return true ;
}

void Config::sync()
{
    lock_guard<recursive_mutex> lck(_mtx) ;
    if ( !_bWritten ) return ;
    auto &refName = _mapPtree[_fileName].refName ;
    if ( _mapPtree[_fileName].bDirty ) {
        boost::property_tree::write_info( refName, *_pt_ptr ) ;
        _mapPtree[_fileName].bDirty = false ;
    }
}

void Config::clear()
{
    lock_guard<recursive_mutex> lck(_mtx) ;
    _pt_ptr->clear() ;
}

