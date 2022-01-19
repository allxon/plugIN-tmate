#ifndef     ALERTSPLUGINJSON_H_
#define     ALERTSPLUGINJSON_H_

#include "BasePluginObject.h"
#include "ConfigsPluginJson.h"
#include <list>


#define JKEY_ALARMS                     "alarms"
#define JKEY_ENABLED                    "enabled"
#define JKEY_ACTION                     "action"
#define JKEY_MESSAGE                    "message"


/**
 * @brief The value set of "action" key of an item in "alarms" array in notifyPluginAlert API
 * 
 */
class Action {
public:
    /** When the alarm has been triggered. */
    const static std::string trigger;
    /** When the alarm has been resolved. */
    const static std::string resolve;
};

/**
 * @brief CAlarmUpdatePluginJson parses received notifyPluginAlarmgUpdate API message JSON and get alarm settings data for
 *  CAlertsPluginJson object, which generates Alerts JSON request, use.
 * 
 */
class PLUGIN_API CAlarmUpdatePluginJson : public CConfigUpdatePluginJson {
public:
    CAlarmUpdatePluginJson(const char *alarmsJsonString, std::string accessKey);
    ~CAlarmUpdatePluginJson();

    std::list<cJSON *> GetAlarms(std::string moduleName);
    const char *GetAlarmsUpdateEpochTime(std::string moduleName);
};


/**
 * @brief CAlertsPluginJson generates a JSON request that conform to the format for notifyPluginAlert API.
 * 
 */
class PLUGIN_API CAlertsPluginJson: public CBasePluginObject {
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