#ifndef ERRORCODE_H
#define ERRORCODE_H
#include <string>
#include <map>
#include <cstdio>



enum ReturnStatus
{
    RESULT_OK = 0,


    ERR_END = -59

};

const char* getErrString(int err_code);

#endif // ERRORCODE_H
