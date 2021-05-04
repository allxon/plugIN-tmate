#ifndef     LOCALCOMMANDPLUGINJSON_H_
#define     LOCALCOMMANDPLUGINJSON_H_

#include "BasePluginObject.h"


#define JKEY_COMMANDS                "commands"
#define JKEY_COMMAND_ID              "commandId"
#define JKEY_COMMAND_SRC             "commandSource"


class CommandSource {
public:
    const static std::string local;
    const static std::string remote;
};

class CLocalCommandPluginJson: public CBasePluginObject {
public:
    CLocalCommandPluginJson();
    ~CLocalCommandPluginJson();

    std::string GetClientAppGUID() { return m_clientAppGUID; }
    void SetClientAppGUID(std::string clientAppGUID) { m_clientAppGUID = clientAppGUID; }
    std::string GetSerialNumber() { return m_serialNumber; }
    void SetSerialNumber(std::string serialNumber) { m_serialNumber = serialNumber; }
    cJSON *GetLocalCommandJsonObject() { return m_localCommandJsonObject; }
    void SetLocalCommandJsonObject(cJSON *commandsJson) { m_localCommandJsonObject = commandsJson; }
    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted);
    cJSON *CreateLocalCommandParamsObj(cJSON *commandsJson);
    static cJSON *CreateLocalCommandParamsObj(const char *clientAppGUID, const char *serialNumber, const char *appGUID, const char *commandId, const char *commandSource, const char *moduleName, cJSON *commandsJson);
    static cJSON *CreateCommandParamsItemJson(const char *name, const char *value);
    static cJSON *CreateCommandsItemJson(const char *name, cJSON *params);

private:
    cJSON *m_localCommandJsonObject;
    std::string m_clientAppGUID;
    std::string m_serialNumber;
};

#endif