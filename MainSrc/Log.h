#ifndef     _LOG_H_
#define     _LOG_H_
#include "../Util/include/Utl_Log.h"

class Log
{
public:
    Log(
        std::string Path = "",
        std::string TitleFileName = "plugIN",
        unsigned int MaxSize = 10*1024*1024
    ):path_(Path),titlefilename_(TitleFileName),maxsize_(MaxSize)
    {
        init();
    }
    std::string GetSelfPath();

private:
    void init();
    std::string path_;
    std::string titlefilename_;
    unsigned int maxsize_;
};

#endif
