#ifndef     _LOG_H_
#define     _LOG_H_
#include "../Util/include/Utl_Log.h"

class Log
{
public:
#if (defined _SUNIX_PLATFORM_) || (defined _ARM_PLATFORM_)
    Log(
        std::string Path = "/mnt/user/SyncAgent",
        std::string TitleFileName = "device_plugin",
        unsigned int MaxSize = 512*1024
    ):path_(Path),titlefilename_(TitleFileName),maxsize_(MaxSize)
    {
        init();
    }
#else
    Log(
        // std::string Path = "/var/lib/SyncAgent",
        std::string Path = "",
        std::string TitleFileName = "plugIN",
        unsigned int MaxSize = 10*1024*1024
    ):path_(Path),titlefilename_(TitleFileName),maxsize_(MaxSize)
    {
        init();
    }
#endif
    std::string GetSelfPath();

private:
    void init();
    std::string path_;
    std::string titlefilename_;
    unsigned int maxsize_;
};

#endif
