#ifndef _PLUGINDATA_H_
#define _PLUGINDATA_H_

#include "UpdatePluginJson.h"
#include "LocalCommandPluginJson.h"
#include <map>
#include <list>
#define JKEY_APIVERSION         "apiVersion"
#define JKEY_UNFORMATTED_JSON   "unformattedJson"
#define JKEY_DESTINATION_IP     "destinationIP"


class CUpdateInfo {
public:
    CUpdateInfo();
    CUpdateInfo(std::string name, std::string displayCategory, std::string displayName, std::string description);
    ~CUpdateInfo();

    std::string name;
    std::string displayCategory;
    std::string displayName;
    std::string description;
};

class PLUGIN_API CUpdateProperty : public CUpdateInfo {
public:
    CUpdateProperty();
    CUpdateProperty(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType, std::string stringValue, std::list<std::map<std::string, std::string> > &tableValue,
        std::map<std::string, std::list<std::string> > const& valueFromProperty = std::map<std::string, std::list<std::string> >());
    ~CUpdateProperty();

    std::string displayType;
    std::string stringValue;
    std::list<std::map<std::string, std::string> > tableValue;
    std::map<std::string, std::list<std::string> > valueFromProperty;
};

class PLUGIN_API CUpdateState : public CUpdateInfo {
public:
    CUpdateState();
    CUpdateState(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType);
    ~CUpdateState();

    std::string displayType;
};

class PLUGIN_API CUpdateMetric : public CUpdateInfo {
public:
    CUpdateMetric();
    CUpdateMetric(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType, std::string displayUnit);
    ~CUpdateMetric();

    std::string displayType;
    std::string displayUnit;
};

class PLUGIN_API CUpdateEvent : public CUpdateInfo {
public:
    CUpdateEvent();
    CUpdateEvent(std::string name, std::string displayCategory, std::string displayName, std::string description);
    ~CUpdateEvent();
};

class PLUGIN_API CUpdateConfigParam {
public:
    CUpdateConfigParam();
    CUpdateConfigParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues,
        std::string defaultValue = "", bool displayMask = false, std::string valueEncoding = ValueEncoding::none);
    ~CUpdateConfigParam();

    std::string name;
    std::string displayName;
    std::string description;
    std::string displayType;
    bool required;
    std::string requiredOn;
    std::string displayFormat;
    std::list<std::string> displayValues;
    std::string defaultValue;
    bool displayMask;
    std::string valueEncoding;
};

class PLUGIN_API CUpdateCommandParam : public CUpdateConfigParam {
public:
    CUpdateCommandParam();
    CUpdateCommandParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues,
        std::string defaultValue, std::string valueFromProperty = "", bool displayMask = false, std::string valueEncoding = ValueEncoding::none);
    ~CUpdateCommandParam();

    std::string valueFromProperty;
};

class PLUGIN_API CUpdateAlarmParam : public CUpdateConfigParam {
public:
    CUpdateAlarmParam();
    CUpdateAlarmParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues,
        std::string defaultValue = "", bool displayMask = false, std::string valueEncoding = ValueEncoding::none);
    ~CUpdateAlarmParam();
};

class PLUGIN_API CUpdateCommand : public CUpdateInfo {
public:
    CUpdateCommand();
    CUpdateCommand(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string type, std::list<CUpdateCommandParam *> &params,
        std::map<std::string, std::list<std::string> > const& displayOnProperty = std::map<std::string, std::list<std::string> >());
    ~CUpdateCommand();

    std::string type;
    std::list<CUpdateCommandParam *> params;
    std::map<std::string, std::list<std::string> > displayOnProperty;
};

class PLUGIN_API CUpdateAlarm : public CUpdateInfo {
public:
    CUpdateAlarm();
    CUpdateAlarm(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::list<CUpdateAlarmParam *> &params);
    ~CUpdateAlarm();

    std::list<CUpdateAlarmParam *> params;
};

class CUpdateConfig : public CUpdateInfo {
public:
    CUpdateConfig();
    CUpdateConfig(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::list<CUpdateConfigParam *> &params);
    ~CUpdateConfig();

    std::list<CUpdateConfigParam *> params;
};

class PLUGIN_API CUpdateModule {
public:
    CUpdateModule();
    CUpdateModule(std::string moduleName, std::string displayName, std::string description, std::list<CUpdateProperty *> &properties,
        std::list<CUpdateState *> &states, std::list<CUpdateMetric *> &metrics, std::list<CUpdateEvent *> &events,
        std::list<CUpdateCommand *> &commands, std::list<CUpdateAlarm *> &alarms, std::list<CUpdateConfig *> &configs);
    ~CUpdateModule();

    cJSON *GetUpdateModule();

    std::string moduleName;
    std::string displayName;
    std::string description;
    std::list<CUpdateProperty *> properties;
    std::list<CUpdateState *> states;
    std::list<CUpdateMetric *> metrics;
    std::list<CUpdateEvent *> events;
    std::list<CUpdateCommand *> commands;
    std::list<CUpdateAlarm *> alarms;
    std::list<CUpdateConfig *> configs;
};

class PLUGIN_API CUpdateParams {
public:
    CUpdateParams();
    CUpdateParams(std::string appGUID, std::string appName, std::string displayName, std::string type, std::string version,
        std::string startCommand, std::string stopCommand, std::list<CUpdateModule *> &modules);
    ~CUpdateParams();

    CUpdatePluginJson *GetNotifyPluginUpdate();
    CUpdatePluginJson *GetNotifyPluginUpdate(const char *apiVersion, const char *accessKey);
    void UpdateNotifyPluginUpdateData();

    std::string appGUID;
    std::string appName;
    std::string displayName;
    std::string type;
    std::string version;
    std::string startCommand;
    std::string stopCommand;
    std::list<CUpdateModule *> modules;

private:
    CUpdatePluginJson *pluginUpdateObj;
};

class PLUGIN_API CLocalCommandParam {
public:
    CLocalCommandParam();
    CLocalCommandParam(std::string name, std::string value);
    ~CLocalCommandParam();

    std::string name;
    std::string value;
};

class PLUGIN_API CLocalCommand {
public:
    CLocalCommand();
    CLocalCommand(std::string name, std::list<CLocalCommandParam *> &params);
    ~CLocalCommand();

    std::string name;
    std::list<CLocalCommandParam *> params;
};

class PLUGIN_API CLocalCommandParams {
public:
    CLocalCommandParams();
    CLocalCommandParams(std::string clientAppGUID, std::string appGUID, std::string serialNumber, std::string moduleName,
        std::list<CLocalCommand *> &commands);
    ~CLocalCommandParams();

    CLocalCommandPluginJson *GetNotifyPluginLocalCommand();
    CLocalCommandPluginJson *GetNotifyPluginLocalCommand(const char *apiVersion, const char *accessKey);

    std::string clientAppGUID;
    std::string appGUID;
    std::string serialNumber;
    std::string moduleName;
    std::list<CLocalCommand *> commands;

private:
    CLocalCommandPluginJson *localCommandObj;
};
#endif