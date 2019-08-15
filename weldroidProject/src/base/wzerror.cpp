#include "base/wzerror.h"

#include <iostream>

void dumpError( boost::exception const & e,
      const std::string &printFile, int printLine )
{
   std::cout << "[" << *boost::get_error_info<boost::throw_file>(e) << "]"
             << "[" << *boost::get_error_info<boost::throw_function>(e) << "]"
             << "[" << *boost::get_error_info<boost::throw_line>(e) << "]"
             << "printFile[" << printFile << "]" << "printLine[" << printLine << "]"
             << std::endl ;
   std::cout << diagnostic_information(e) << std::endl;
}
