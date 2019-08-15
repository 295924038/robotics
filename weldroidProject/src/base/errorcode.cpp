#include "base/errorcode.h"


typedef struct ErrString
{
    int iCode;
    const char* pString;

}errString;

static errString ErrorMap[] = {
//    {ERR_READ_CONFIG_FAILED , "read config failed"},


};

const char* getErrString(int err_code)
{
    if(err_code >= RESULT_OK ||  err_code <= ERR_END)
    {
        return "unknown exception.";
    }
    for(unsigned int i = 0; i< sizeof(ErrorMap)/sizeof(ErrorMap[0]); i++)
    {
        if(ErrorMap[i].iCode == err_code)
        {
            return ErrorMap[i].pString;
        }
    }

    return "unknown exception.";

}
