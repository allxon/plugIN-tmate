#ifndef     PLUGINEXCEPTION_H_
#define     PLUGINEXCEPTION_H_

#include <exception>
#include <string>
#include "PluginEntry.h"


class PLUGIN_API CPluginException : virtual public std::exception
{
public:
    enum ErrorNumber
    {
        INVALID_STRING_FORMAT = 1,
        DATA_EXCEED_MAX_LENGTH,
        NUMBER_EXCEED_MAX_DIGIT_LENGTH,
        REQUIRED_DATA,
        USAGE_ERROR,
        ACCESSKEY_IS_MISSING,
#ifndef _WINDOWS
        ERROR = -1,
#else
        UNKOWN
#endif // !_WINDOWS
    };

    CPluginException(const std::string& msg, int errNum):
        m_msg(msg),
        m_errorNum(errNum)
        {}
    virtual ~CPluginException() throw () {}

    virtual const char *what() const throw () { return m_msg.c_str(); }
    virtual int getErrorNumber() const throw () { return m_errorNum; }

protected:
    int m_errorNum;
    std::string m_msg;
};

#endif