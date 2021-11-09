#ifndef     COMMANDACKSPLUGINJSON_H_
#define     COMMANDACKSPLUGINJSON_H_

#include "BasePluginObject.h"
#include <list>


#define JKEY_CMD_ACKS                "commandAcks"
#define JKEY_COMMANDS                "commands"
#define JKEY_COMMAND_ID              "commandId"
#define JKEY_COMMAND_SRC             "commandSource"
#define JKEY_CMD_STATE               "commandState"
#define JKEY_RESULT                  "result"


class PLUGIN_API AckState {
public:
    const static std::string ACCEPTED;
    const static std::string ACKED;
    const static std::string REJECTED;
    const static std::string ERRORED;
};

class PLUGIN_API CCommandPluginJson: public CPluginUtil {
public:
    CCommandPluginJson(const char *commandsJsonString, std::string accessKey);
    ~CCommandPluginJson();

    std::string GetAccessKey() { return m_accessKey; }
    void SetAccessKey(std::string accessKey) { m_accessKey = accessKey; }
    std::string GetCommandId() { return m_commandId; }
    void SetCommandId(std::string commandId) { m_commandId = commandId; }
    std::string GetCommandSource() { return m_commandSource; }
    void SetCommandSource(std::string commandSource) { m_commandSource = commandSource; }
    std::string GetModuleName() { return m_moduleName; }
    void SetModuleName(std::string moduleName) { m_moduleName = moduleName; }
    std::list<cJSON *> GetCommands() { return m_commands; }
    bool IsValidSignature() { return m_validSignature; }
    
private:
    std::string m_accessKey;
    std::string m_commandId;
    std::string m_commandSource;
    std::string m_moduleName;
    std::list<cJSON *> m_commands;
    bool m_validSignature;

    void ValidateCommands(const char *commandsJsonString);
};


class PLUGIN_API CCommandAcksPluginJson: public CBasePluginObject {
public:
    CCommandAcksPluginJson();
    ~CCommandAcksPluginJson();

    cJSON *GetCommandAcksJsonObject() { return m_commandAcksJsonObject; }
    void SetCommandAcksJsonObject(cJSON *commandAcksJson) { m_commandAcksJsonObject = commandAcksJson; }
    void SetCommandsPluginObject(CCommandPluginJson * commandsPluginObj) { m_commandsObject = commandsPluginObj; }
    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted);
    cJSON *CreateCommandAcksAcceptedParamsObj(cJSON *commandAcksJson);
    cJSON *CreateCommandAcksAckedParamsObj(cJSON *commandAcksJson, cJSON *statesJson = NULL);
    cJSON *CreateCommandAcksRejectedParamsObj(cJSON *commandAcksJson);
    cJSON *CreateCommandAcksErroredParamsObj(cJSON *commandAcksJson);
    static cJSON *CreateCommandAcksAcceptedParamsObj(const char *appGUID, const char *commandId, const char *commandSource, const char *moduleName, cJSON *commandAcksJson);
    static cJSON *CreateCommandAcksAckedParamsObj(const char *appGUID, const char *commandId, const char *commandSource, const char *moduleName, cJSON *commandAcksJson, cJSON *statesJson = NULL);
    static cJSON *CreateCommandAcksRejectedParamsObj(const char *appGUID, const char *commandId, const char *commandSource, const char *moduleName, cJSON *commandAcksJson);
    static cJSON *CreateCommandAcksErroredParamsObj(const char *appGUID, const char *commandId, const char *commandSource, const char *moduleName, cJSON *commandAcksJson);
    static cJSON *CreateCommandAcksItemJson(const char *name, const char *result);
    static cJSON *CreateCommandAcksItemJson(const char *name, cJSON *result);

private:
    cJSON *m_commandAcksJsonObject;
    CCommandPluginJson *m_commandsObject;
};

#endif