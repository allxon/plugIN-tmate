#ifndef _PLUGINDATA_H_
#define _PLUGINDATA_H_

#include "UpdatePluginJson.h"
#include "LocalCommandPluginJson.h"
#include <map>

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

class CUpdateProperty : public CUpdateInfo {
public:
    CUpdateProperty();
    CUpdateProperty(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType, std::string stringValue, std::list<std::map<std::string, std::string> > &tableValue);
    ~CUpdateProperty();

    std::string displayType;
    std::string stringValue;
    std::list<std::map<std::string, std::string> > tableValue;
};

class CUpdateState : public CUpdateInfo {
public:
    CUpdateState();
    CUpdateState(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType);
    ~CUpdateState();

    std::string displayType;
};

class CUpdateMetric : public CUpdateInfo {
public:
    CUpdateMetric();
    CUpdateMetric(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType, std::string displayUnit);
    ~CUpdateMetric();

    std::string displayType;
    std::string displayUnit;
};

class CUpdateEvent : public CUpdateInfo {
public:
    CUpdateEvent();
    CUpdateEvent(std::string name, std::string displayCategory, std::string displayName, std::string description);
    ~CUpdateEvent();
};

class CUpdateControlParam {
public:
    CUpdateControlParam();
    CUpdateControlParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues);
    ~CUpdateControlParam();

    std::string name;
    std::string displayName;
    std::string description;
    std::string displayType;
    bool required;
    std::string requiredOn;
    std::string displayFormat;
    std::list<std::string> displayValues;
};

class CUpdateCommandParam : public CUpdateControlParam {
public:
    CUpdateCommandParam();
    CUpdateCommandParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues, std::string defaultValue);
    ~CUpdateCommandParam();

    std::string defaultValue;
};

class CUpdateAlarmParam : public CUpdateControlParam {
public:
    CUpdateAlarmParam();
    CUpdateAlarmParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues);
    ~CUpdateAlarmParam();
};

class CUpdateCommand : public CUpdateInfo {
public:
    CUpdateCommand();
    CUpdateCommand(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string type, std::list<CUpdateCommandParam *> &params);
    ~CUpdateCommand();

    std::string type;
    std::list<CUpdateCommandParam *> params;
};

class CUpdateAlarm : public CUpdateInfo {
public:
    CUpdateAlarm();
    CUpdateAlarm(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::list<CUpdateAlarmParam *> &params);
    ~CUpdateAlarm();

    std::list<CUpdateAlarmParam *> params;
};

class CUpdateModule {
public:
    CUpdateModule();
    CUpdateModule(std::string moduleName, std::string displayName, std::string description, std::list<CUpdateProperty *> &properties,
        std::list<CUpdateState *> &states, std::list<CUpdateMetric *> &metrics, std::list<CUpdateEvent *> &events,
        std::list<CUpdateCommand *> &commands, std::list<CUpdateAlarm *> &alarms);
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
};

class CUpdateParams {
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

class CLocalCommandParam {
public:
    CLocalCommandParam();
    CLocalCommandParam(std::string name, std::string value);
    ~CLocalCommandParam();

    std::string name;
    std::string value;
};

class CLocalCommand {
public:
    CLocalCommand();
    CLocalCommand(std::string name, std::list<CLocalCommandParam *> &params);
    ~CLocalCommand();

    std::string name;
    std::list<CLocalCommandParam *> params;
};

class CLocalCommandParams {
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