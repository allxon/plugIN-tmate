#include "Log.h"

void Log::init()
{
    path_ = GetSelfPath();
    const std::string appPath = "/opt/allxon/plugIN";
    path_.replace(0, path_.find(appPath)+appPath.length(), "/var/log/allxon/plugIN");
    if(path_.empty() || titlefilename_.empty() || maxsize_== 0)
    {
        return void();
    }
    UTL_LOGInit(titlefilename_.c_str(), path_.c_str());
    UTL_LOGSetMax(maxsize_);
    UTL_LOG_DISABLE_LEVEL(UTL_LOG_LEVEL_DEBUG);
    UTL_LOGSetWriteToStdout(true);
    return void();
}

std::string Log::GetSelfPath() {
    char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
    else return "";
}
