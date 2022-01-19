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


/**
 *  @brief Constant strings for the states of command acks.
 */
class PLUGIN_API AckState {
public:
    /** When command is retrieved by the plugIN */
    const static std::string ACCEPTED;
    /** When command has been acked by the module */
    const static std::string ACKED;
    /** When command is rejected by the plugIN */
    const static std::string REJECTED;
    /** When command has been errored by the module */
    const static std::string ERRORED;
};

/**
 *  @brief CCommandPluginJson parses received notifyPluginCommand API message JSON and get commands and their parameters data for devices use.
 */
class PLUGIN_API CCommandPluginJson: public CPluginUtil {
public:
    CCommandPluginJson(const char *commandsJsonString, std::string accessKey);
    ~CCommandPluginJson();

    /** @fn std::string GetAccessKey()
     *  @brief Getter of m_accessKey member.
     *  @return Return the access key of this plugIN that set in the object initial stage.
     */
    std::string GetAccessKey() { return m_accessKey; }

    /** @fn void SetAccessKey(std::string accessKey)
     *  @brief Setter of m_accessKey member.
     *  @param[in] accessKey Set access key for the object in the initial stage.
     */
    void SetAccessKey(std::string accessKey) { m_accessKey = accessKey; }

    /** @fn std::string GetCommandId()
     *  @brief Getter of m_commandId member.
     *  @return Return the assistId in MQTT message. For the reference of commandAcks.
     */
    std::string GetCommandId() { return m_commandId; }

    /** @fn void SetCommandId(std::string commandId)
     *  @brief Setter of m_commandId member.
     *  @param[in] commandId The assistId in MQTT message.
     */
    void SetCommandId(std::string commandId) { m_commandId = commandId; }

    /** @fn std::string GetCommandSource()
     *  @brief Getter of m_commandSource member.
     *  @return Local or remote commands request. For the reference of commandAcks.
     */
    std::string GetCommandSource() { return m_commandSource; }

    /** @fn void SetCommandSource(std::string commandSource)
     *  @brief Setter of m_commandSource member.
     *  @param[in] commandSource Local or remote commands request.
     */
    void SetCommandSource(std::string commandSource) { m_commandSource = commandSource; }

    /** @fn std::string GetModuleName()
     *  @brief Getter of m_moduleName member.
     *  @return The moduleName of the commands.
     */
    std::string GetModuleName() { return m_moduleName; }

    /** @fn void SetModuleName(std::string moduleName)
     *  @brief Setter of m_moduleName member.
     *  @param[in] moduleName The module name of the commands.
     */
    void SetModuleName(std::string moduleName) { m_moduleName = moduleName; }

    /** @fn std::string GetCommands()
     *  @brief Getter of m_commands member.
     *  @return A list that stores all items of JSON object of "commands" key.\n[Note] The list of cJSON objects will be deleted
     *  when the CComandPluginJson object be destoryed. Please make a copy if you need to add it to another JSON object.
     */
    std::list<cJSON *> GetCommands() { return m_commands; }

    /** @fn std::string IsValidSignature()
     *  @brief To get if the received command message is valid on epoch and signature after called ValidateCommands() to check.
     *  @return True for valid signature.\n
     *  False for invalid signature.
     */
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


/**
 *  @brief CCommandAcksPluginJson generates a JSON request that conform to the format for notifyPluginCommandAck API.
 */
class PLUGIN_API CCommandAcksPluginJson: public CBasePluginObject {
public:
    CCommandAcksPluginJson();
    ~CCommandAcksPluginJson();

    /** @fn cJSON *GetCommandAcksJsonObject()
     * @brief Getter of m_commandAcksJsonObject member.
     * @return The generated commandAcks JSON object.
     */
    cJSON *GetCommandAcksJsonObject() { return m_commandAcksJsonObject; }

    /** @fn void SetCommandAcksJsonObject(cJSON *commandAcksJson)
     * @brief Setter of m_commandAcksJsonObject member.
     * @param[in] commandAcksJson Store the generated commandAcks JSON object to m_commandAcksJsonObject.\n[Note] This JSON object will
     *  be added to the JSON object returned, so please make sure not to delete it.
     */
    void SetCommandAcksJsonObject(cJSON *commandAcksJson) { m_commandAcksJsonObject = commandAcksJson; }

    /** @fn void SetCommandsPluginObject(CCommandPluginJson * commandsPluginObj)
     * @brief Setter of m_commandsObject member.
     * @param[in] updateJson Set a CCommandPluginJson object to m_commandsObject member.
     */
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