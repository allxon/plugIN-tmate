#ifndef     CONFIGSPLUGINJSON_H_
#define     CONFIGSPLUGINJSON_H_

#include "BasePluginObject.h"
#include <list>


#define JKEY_CONFIGS                    "configs"
#define JKEY_MODULES                    "modules"
#define JKEY_TIME                       "time"


class CConfigUpdatePluginJson: public CPluginUtil {
public:
    CConfigUpdatePluginJson(const char *configsJsonString, std::string accessKey);
    ~CConfigUpdatePluginJson();

    std::string GetAccessKey() { return m_accessKey; }
    void SetAccessKey(std::string accessKey) { m_accessKey = accessKey; }
    std::string GetModuleName() { return m_moduleName; }
    void SetModuleName(std::string moduleName) { m_moduleName = moduleName; }
    std::string GetUpdateEpoch() { return m_updateEpoch; }
    std::string GetVersion() { return m_version; }
    void SetUpdateEpoch(std::string updateEpoch) { m_updateEpoch = updateEpoch; }
    std::list<cJSON *> GetModulesParam() { return m_modules; }
    bool IsValidSignature() { return m_validSignature; }
    std::list<cJSON *> GetConfigs(std::string moduleName, std::string configType = JKEY_CONFIGS);
    const char *GetConfigsUpdateEpochTime(std::string moduleName);
    
private:
    std::string m_accessKey;
    std::string m_moduleName;
    std::string m_updateEpoch;
    std::string m_version;
    std::list<cJSON *> m_modules;
    bool m_validSignature;

    void SetVersion(std::string version) { m_version = version; }
    void ValidateConfigs(const char *configsJsonString);
};

#endif