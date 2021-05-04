#ifndef     ALERTSPLUGINJSON_H_
#define     ALERTSPLUGINJSON_H_

#include "BasePluginObject.h"
#include <list>


#define JKEY_ALARMS                     "alarms"
#define JKEY_MODULES                    "modules"
#define JKEY_ENABLED                    "enabled"
#define JKEY_ACTION                     "action"
#define JKEY_TIME                       "time"
#define JKEY_MESSAGE                    "message"


class Action {
public:
    const static std::string trigger;
    const static std::string resolve;
};

class CAlarmUpdatePluginJson: public CPluginUtil {
public:
    CAlarmUpdatePluginJson(const char *alarmsJsonString, std::string accessKey);
    ~CAlarmUpdatePluginJson();

    std::string GetAccessKey() { return m_accessKey; }
    void SetAccessKey(std::string accessKey) { m_accessKey = accessKey; }
    std::string GetModuleName() { return m_moduleName; }
    void SetModuleName(std::string moduleName) { m_moduleName = moduleName; }
    std::string GetUpdateEpoch() { return m_updateEpoch; }
    void SetUpdateEpoch(std::string updateEpoch) { m_updateEpoch = updateEpoch; }
    std::list<cJSON *> GetModulesParam() { return m_modules; }
    bool IsValidSignature() { return m_validSignature; }
    std::list<cJSON *> GetAlarms(std::string moduleName);
    const char *GetAlarmsUpdateEpochTime(std::string moduleName);
    
private:
    std::string m_accessKey;
    std::string m_moduleName;
    std::string m_updateEpoch;
    std::list<cJSON *> m_modules;
    bool m_validSignature;

    void ValidateAlarms(const char *alarmsJsonString);
};


class CAlertsPluginJson: public CBasePluginObject {
public:
    CAlertsPluginJson();
    ~CAlertsPluginJson();

    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted);
    cJSON *CreateAlarmsParamsObj(cJSON *alarmsJson);
    static cJSON *CreateAlarmsParamsObj(const char *appGUID, const char *moduleName, cJSON *alarmsJson); 
    static cJSON *CreateAlarmsItemJson(const char *name, const char *action, const char *message);
};

#endif