#ifndef _WZERROR_H
#define _WZERROR_H

#include <exception>
#include <boost/exception/all.hpp>
#include <string>

#define DUMPERROR(e) \
   do { \
      dumpError( e, __FILE__, __LINE__ ) ; \
   } while(0)

class WZError : public virtual std::exception,
                public virtual boost::exception
{
};

typedef boost::error_info<struct errinfo_robot, std::string> err_robot ;

typedef boost::error_info<struct errinfo_camera, std::string> err_camera ;

typedef boost::error_info<struct errinfo_scan, std::string> err_scan ;

typedef boost::error_info<struct errinfo_connect, std::string> err_connect ;

typedef boost::error_info<struct errinfo_pos, std::string> err_pos ;

typedef boost::error_info<struct errinfo_image, std::string> err_image ;

void dumpError( boost::exception const & e,
              const std::string &printFile, int printLine ) ;

#endif
