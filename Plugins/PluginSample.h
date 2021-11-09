#ifndef _PLUGINSAMPLE_H_
#define _PLUGINSAMPLE_H_

#include "../PluginSDK/PluginData.h"
#include "../PluginSDK/CommandAcksPluginJson.h"
#include "../PluginSDK/AlertsPluginJson.h"
#include "../PluginSDK/ConfigsPluginJson.h"


typedef enum
{
    CNFG_PLUGIN_APP,
    CNFG_LOCAL_COMMAND,
    CNFG_NA
} PluginConfigType;

class CPluginSampleConfig
{
public:
    CPluginSampleConfig();
    CPluginSampleConfig(const char *configFile);
    ~CPluginSampleConfig();

    virtual void GetSampleConfig(const char *configFile);
    std::string GetApiVersion() { return apiVersion; }
    bool GetMinify() { return minify; }
    std::string GetAppGUID() { return appGUID; }
    std::string GetAccessKey() { return accessKey; }
    std::string GetSampleName() { return sampleName; }

protected:
    std::string apiVersion = ApiVersion::v2;
    bool minify = true;
    std::string appGUID;
    std::string accessKey;
    std::string sampleName;
};

class CLocalCommandSampleConfig : public CPluginSampleConfig
{
public:
    CLocalCommandSampleConfig();
    CLocalCommandSampleConfig(const char *configFile);
    ~CLocalCommandSampleConfig();

    void GetSampleConfig(const char *configFile);
    std::string GetClientAppGUID() { return clientAppGUID; }
    std::string GetModuleName() { return moduleName; }
    std::string GetDestinationIP() { return destinationIP; }
    std::string GetSerialNumber() { return serialNumber; }

private:
    std::string clientAppGUID;
    std::string moduleName;
    std::string destinationIP;
    std::string serialNumber;
};

class CPluginSample
{
public:
    CPluginSample();
    CPluginSample(CPluginSampleConfig *sampleConfig);
    CPluginSample(std::string apiVersion, bool minify, std::string appGUID, std::string accessKey);
    ~CPluginSample();

    CUpdateParams *GetUpdateData();
    CLocalCommandParams *GetLocalCommandData();
    CUpdateModule *GetModule(std::string moduleName);
    CUpdatePluginJson *SetNotifyPluginUpdateFromFile(std::string jsonFile);
    char *SetNotifyCommandAcks(CCommandPluginJson *receivedCmds, std::string moduleName, std::string cmdState, cJSON *cmdAcks, cJSON *states = NULL);
    char *SetNotifyStates(std::string moduleName, cJSON *states);
    char *SetNotifyEvents(std::string moduleName, cJSON *events);
    char *SetNotifyMetrics(std::string moduleName, cJSON *metrics);
    char *SetNotifyAlerts(std::string moduleName, cJSON *alarms);
    static bool IsAlarmEnabled(std::string moduleName, CAlarmUpdatePluginJson *alarmUpdate, std::string alarmName);

    std::string apiVersion;
    bool minify;
    std::string appGUID;
    std::string accessKey;

    virtual CUpdatePluginJson *SetNotifyPluginUpdate();
    virtual CLocalCommandPluginJson *SetNotifyPluginLocalCommand();
    virtual bool AcceptReceivedCommand(cJSON *commandJson);
    virtual std::string ExecuteReceivedCommand(cJSON *commandJson, cJSON *cmdAck);

protected:
    CUpdateParams *pluginData;
    CLocalCommandParams *localCommandData;
};

#endif