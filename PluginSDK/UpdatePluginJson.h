#ifndef     UPDATEPLUGINJSON_H_
#define     UPDATEPLUGINJSON_H_

#include "BasePluginObject.h"


#define JKEY_APP_NAME                   "appName"
#define JKEY_TYPE                       "type"
#define JKEY_START_COMMAND              "startCommand"
#define JKEY_STOP_COMMAND               "stopCommand"
#define JKEY_MODULES                    "modules"
#define JKEY_REQUIRED                   "required"
#define JKEY_REQUIRED_ON                "requiredOn"
#define JKEY_DISPLAY_FORMAT             "displayFormat"
#define JKEY_DISPLAY_VALUES             "displayValues"
#define JKEY_DEFAULT_VALUE              "defaultValue"
#define JKEY_LINK_URL                   "url"
#define JKEY_LINK_ALIAS                 "alias"
#define JKEY_DISPLAY_ON_PROPERTY        "displayOnProperty"
#define JVALUE_T_STRING                 "string"
#define JVALUE_T_TABLE                  "table"


class DataTypes {
public:
    const static std::string properties;
    const static std::string states;
    const static std::string events;
    const static std::string metrics;
    const static std::string commands;
    const static std::string alarms;
};

class AppType {
public:
    const static std::string ib;
    const static std::string oob;
};

class DisplayType {
public:
    const static std::string string;
    const static std::string table;
    const static std::string link;
    const static std::string datetime;
    const static std::string iswitch;
    const static std::string icheckbox;
    const static std::string ilist;
    const static std::string file;
    const static std::string temperature;
    const static std::string displayOn;
    const static std::string valueFromProperty;
};


class CUpdatePluginJson: public CBasePluginObject {
public:
    CUpdatePluginJson();
    ~CUpdatePluginJson();

    cJSON *GetUpdateJsonObject() { return m_updateJsonObject; }
    void SetUpdateJsonObject(cJSON *updateJson) { m_updateJsonObject = updateJson; }
    bool IsUpdated() { return m_isUpdated; }
    void SetUpdated(bool isUpdated) { m_isUpdated = isUpdated; }
    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted);
    cJSON *RenewUpdateJsonObject(bool unformatted);
    cJSON *CreateUpdateParamsObj(const char *appName, const char *displayName, std::string appType, const char *version, const char *startCommand, const char *stopCommand,cJSON *modulesJson);
    static cJSON *CreateUpdateModulesItemJson(const char *moduleName, const char *displayName, const char *description, cJSON *properties, cJSON *states, cJSON *events,
        cJSON *metrics, cJSON *commands, cJSON *alarms);
    static cJSON *CreateUpdatePropertiesItemJson(const char *name, const char *displayName, const char *description,
        std::string displayType, const char *displayCategory, const char *value);
    static cJSON *CreateUpdatePropertiesItemJson(const char *name, const char *displayName, const char *description,
        std::string displayType, const char *displayCategory, cJSON *value);
    static cJSON *CreateUpdateStatesItemJson(const char *name, const char *displayName, const char *description, std::string displayType, const char *displayCategory);
    static cJSON *CreateUpdateEventsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory);
    static cJSON *CreateUpdateMetricsItemJson(const char *name, const char *displayName, const char *description, const char *displayUnit,
        std::string displayType, const char *displayCategory);
    static cJSON *CreateUpdateCommandsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory,
        cJSON *paramsJson, cJSON *displayOnPropertyJson = NULL);
    static cJSON *CreateUpdateCommandParamsItemJson(const char *name, const char *displayName, const char *description, const char *defaultValue, std::string displayType,
        bool required, const char *displayFormat, cJSON *displayValues, std::string valueFromProperty);
    static cJSON *CreateUpdateCommandParamsItemJson(const char *name, const char *displayName, const char *description, double defaultValue, std::string displayType,
        bool required, const char *displayFormat, cJSON *displayValues, std::string valueFromProperty);
    static cJSON *CreateUpdateAlarmsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory,
        cJSON *paramsJson);
    static cJSON *CreateUpdateAlarmParamsItemJson(const char *name, const char *displayName, const char *description, std::string displayType,
        bool required, const char *displayFormat = NULL, cJSON *displayValues = NULL);

private:
    cJSON *m_updateJsonObject;
    bool m_isUpdated;

    static cJSON *CreateUpdateCommandParamsItemJson(const char *name, const char *displayName, const char *description, std::string displayType,
        bool required, const char *displayFormat = NULL, cJSON *displayValues = NULL, std::string valueFromProperty = "");
};

#endif