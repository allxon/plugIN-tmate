#ifndef     PLUGINEXCEPTION_H_
#define     PLUGINEXCEPTION_H_

#include <exception>
#include <string>
#include "PluginEntry.h"


/**
 * @brief Exceptions about Allxon DMS plugIN can be caught by CPluginException.
 */
class PLUGIN_API CPluginException : virtual public std::exception
{
public:
    enum ErrorNumber
    {
        /** The format of some key of JSON object doesn't fit the plugIN specification. */
        INVALID_STRING_FORMAT = 1,
        /** The data size of some key, value or JSON object exceeds the plugIN specification. */
        DATA_EXCEED_MAX_LENGTH,
        /** The digits count of a number value exceeds the plugIN specification. */
        NUMBER_EXCEED_MAX_DIGIT_LENGTH,
        /** The value should be assigned but not. */
        REQUIRED_DATA,
        /** The key or value shouldn't be set to contain some key word. */
        USAGE_ERROR,
        /** Access key wasn't assigned. */
        ACCESSKEY_IS_MISSING,
#ifndef _WINDOWS
        /** Other errors. */
        ERROR = -1,
#else
        UNKOWN
#endif // !_WINDOWS
    };

    /**
     * @brief Construct a new CPluginException object
     * 
     * @param msg Error message.
     * @param errNum Error code.
     */
    CPluginException(const std::string& msg, int errNum):
        m_msg(msg),
        m_errorNum(errNum)
        {}
    
    /** @brief Destroy the CPluginException object */
    virtual ~CPluginException() throw () {}

    /** Getter of m_msg member. */
    virtual const char *what() const throw () { return m_msg.c_str(); }
    /** Getter of m_errorNum member. */
    virtual int getErrorNumber() const throw () { return m_errorNum; }

protected:
    int m_errorNum;
    std::string m_msg;
};

#endif