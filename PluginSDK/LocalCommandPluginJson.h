#ifndef     LOCALCOMMANDPLUGINJSON_H_
#define     LOCALCOMMANDPLUGINJSON_H_

#include "BasePluginObject.h"


#define JKEY_COMMANDS                "commands"
#define JKEY_COMMAND_ID              "commandId"
#define JKEY_COMMAND_SRC             "commandSource"


/**
 * @brief The source of a command request.
 * 
 */
class CommandSource {
public:
    /** Value of commandSource. A command request from a local client app. */
    const static std::string local;
    /** Value of commandSource. A command request from remote site. */
    const static std::string remote;
};

/**
 *  @brief CLocalCommandPluginJson generates a JSON request that conform to the format for notifyPluginLocalCommand API.
 */
class PLUGIN_API CLocalCommandPluginJson: public CBasePluginObject {
public:
    CLocalCommandPluginJson();
    ~CLocalCommandPluginJson();

    /** @fn std::string GetClientAppGUID()
     *  @brief Getter of m_clientAppGUID member.
     *  @return The client app's GUID set when initiate a local command JSON object.
     */
    std::string GetClientAppGUID() { return m_clientAppGUID; }

    /** @fn void SetClientAppGUID(std::string clientAppGUID)
     *  @brief Setter of m_clientAppGUID member.
     *  @param[in] clientAppGUID GUID of the plugIN.
     */
    void SetClientAppGUID(std::string clientAppGUID) { m_clientAppGUID = clientAppGUID; }

    /** @fn std::string GetSerialNumber()
     *  @brief Getter of m_serialNumber member.
     *  @return The serial number of the device behind a gateway. Only required when sending commands to devices behind a gateway.
     */
    std::string GetSerialNumber() { return m_serialNumber; }

    /** @fn void SetSerialNumber(std::string serialNumber)
     *  @brief Setter of m_serialNumber member.
     *  @param[in] serialNumber The serial number of the device behind a gateway. Only required when sending commands to devcies
     *  behind a gateway.
     */
    void SetSerialNumber(std::string serialNumber) { m_serialNumber = serialNumber; }

    /** @fn cJSON *GetLocalCommandJsonObject()
     *  @brief Getter of m_localCommandJsonObject member.
     *  @return To store the JSONRPC object if set it by the programmer after called GetJsonrpcRequest() method.
     */
    cJSON *GetLocalCommandJsonObject() { return m_localCommandJsonObject; }

    /** @fn void SetLocalCommandJsonObject(cJSON *commandsJson)
     *  @brief Setter of m_localCommandJsonObject member.
     *  @param[in] commandsJson Set it after called GetJsonrpcrequest() method when needed.
     */
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