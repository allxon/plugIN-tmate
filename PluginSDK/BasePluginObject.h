#ifndef     BASEPLUGINOBJECT_H_
#define     BASEPLUGINOBJECT_H_

#include <stddef.h>
#include "PluginEntry.h"
#ifndef _WINDOWS
#include "../../Util/include/Comm.h"
#include "../../Util/include/cJSON.h"
#else
#include <string>
#include "cJSON/cJSON.h"
#endif // _WINDOWS


#define ASCII_START     32
#define ASCII_END       126

#define METHOD_AUTH_WORD             "?authorization="
#define ACCESS_KEY                   "accessKey"

#define JKEY_JSONRPC                 "jsonrpc"
#define JKEY_METHOD                  "method"
#define JKEY_PARAMS                  "params"
#define JKEY_EPOCH                   "epoch"
#define JKEY_APP_NAME                "appName"
#define JKEY_VERSION                 "version"
#define JKEY_APP_GUID                "appGUID"
#define JKEY_CLIENT_APP_GUID         "clientAppGUID"
#define JKEY_SERIAL_NUMBER           "serialNumber"
#define JKEY_DISPLAY_NAME            "displayName"
#define JKEY_TYPE                    "type"
#define JKEY_MODULE_NAME             "moduleName"
#define JKEY_NAME                    "name"
#define JKEY_DISPLAY_TYPE            "displayType"
#define JKEY_DISPLAY_UNIT            "displayUnit"
#define JKEY_VALUE                   "value"
#define JKEY_DESCRIPTION             "description"
#define JKEY_DISPLAY_CATEGORY        "displayCategory"
#define JKEY_ERROR                   "error"

#define MAX_NAME_LENGTH                 32
#define MAX_VALUE_STRING_LENGTH         512
#define MAX_VALUE_TABLE_LENGTH          1024
#define MAX_VALUE_TABLE_HEADER_LENGTH   128
#define MAX_DATA_NOTIFY_LENGTH          2048
#define MAX_MODULE_NAME_LENGTH          64

/**
 * @brief The version of Allxon DMS Agent API.
 * 
 */
class PLUGIN_API ApiVersion {
public:
    /** The Allxon DMS Agent API version 1 for test. This version of API is less security and not recommanded for use. */
    const static std::string v1;
    /** The Allxon DMS Agent API version 2. This needs to work with an Access Key for a plugIN app provided by Allxon.
     *  For security, each request to and from Allxon DMS Agent and plugIN must be signed with the Access Key. The process is
     *  aimed to against request messages being either manipulated from unauthorized parties.
     */
    const static std::string v2;
};

/**
 * @brief Allxon DMS Agent API to plugIN.
 * 
 */
class AgentPluginApi {
public:
    /** The Allxon DMS Agent API for plugIN use. To register the plugIN data display and control functions, and update properties
     *  as well. This API has to be invoked once the WebSocket connection between Agent and plugIN is established.
     */
    const static std::string notifyPluginUpdate;
    /** The Agent API that sent by Agent to notify users' commands set from DMS portal. Provides what commands need to be executed by
     *  plugIN to the device.
     */
    const static std::string notifyPluginCommand;
    /** The Agent API for plugIN use. To ack the message received from notifyPluginCommand and tell the server the commands
     *  execution results or errors occurred.
     */
    const static std::string notifyPluginCommandAck;
    /** The Agent API for a 3rd party client app use. To send commands to a plugIN to run some commands of the plugIN. */
    const static std::string notifyPluginLocalCommand;
    /** The Agent API for plugIN use. To update status of the device, which registered at notifyPluginUpdate, by plugIN to the server
     *  through Agent. States will show the latest data in the module page on portal.
     */
    const static std::string notifyPluginState;
    /** The Agent API for plugIN use. To update text status of device, which registered at notifyPluginUpdate, by plugIN to the server
     *  through Agent. Events will show the latest and historical data in the module page on portal.
     */
    const static std::string notifyPluginEvent;
    /** The Agent API for plugIN use. To update digits with unit data of device, which registered at notifyPluginUpdate, by plugIN
     *  to the server through Agent. Metrics will show the data as a line graph in the module page on portal.
     */
    const static std::string notifyPluginMetric;
    /** The Agent API that sent by Agent to notify users' alarm set from portal. Provides what alarms need to be triggered caused by
     *  something reached its threshold value by plugIN to the device.
     */
    const static std::string notifyPluginAlarmUpdate;
    /** The Agent API that sent by Agent to notify users' config set from portal. Provides what confis are modified by users on the
     *  portal
     */
    const static std::string notifyPluginConfigUpdate;
    /** The Agent API for plugIN use. To notify alerts to server that some alarms set by notifyPluginAlarmUpdate are triggered or
     *  resolved.
     */
    const static std::string notifyPluginAlert;
    /** The Agent API that sent by Agent to notify users' the events occurred on Agent, such as connectivity changes of server and
     *  Agent.
     */
    const static std::string notifyAgentEvent;
};

/**
 * @brief Message payload received from Allxon DMS Agent.
 * 
 */
class MessageType {
public:
    /** The received message is sent by notifyPluginCommand. */
    const static std::string api_notifyPluginCommand;
    /** The received message is sent by notifyPluginAlarmUpdate. */
    const static std::string api_notifyPluginAlarmUpdate;
    /** The received message is sent to report errors. */
    const static std::string api_notifyAgentEvent;
    /** The received message is an error message. */
    const static std::string error;
    /** The received message is unknown. */
    const static std::string others;
    /** Notify from Allxon DMS Agent about serverConnectivity: Connection state to server, either "online" or "offline." */
    const static std::string agentEventServerConnectivity;
    /** Server is online. One of the value of MessageType::agentEventServerConnectivity. */
    const static std::string online;
    /** Server is offline. One of the value of MessageType::agentEventServerConnectivity. */
    const static std::string offline;
};

/**
 * @brief The regular expressions for various data formats to check.
 * 
 */
class StringFormatter {
public:
    /** Regular expression to check if matched the format of key "name". */
    const static std::string nameFormat;
    /** Regular expression to check if matched the format of key "version". */
    const static std::string versionFormat;
    /** Regular expression to check if matched the format of key "name" for OTA command. */
    const static std::string otaCommand;
};


/**
 *  @brief CPluginUtil is a base class which provides utilities to help get or convert data from or needed by a JSON object.
 */
class PLUGIN_API CPluginUtil {
public:
    static cJSON * GetJsonFromFile(std::string jsonFileName);
    static char* Utilityltoa(long number, char *buff);
    static std::string GetJSONStringFieldValue(cJSON *json, std::string field);
    static bool GetJSONBooleanFieldValue(cJSON *json, std::string field);
    static char *GetMessageType(const char *payload, std::string *msgType);

protected:
    static bool GetSignDigest(std::string input, std::string *digest);
    static bool GetSignature(std::string digest, std::string accesskey, std::string *signkey);
    static bool VerifySignature(std::string digest, std::string accesskey, std::string signkey);
    static std::string GenerateRandomString(int size);
    static int GetNumberDigitsCount(double number);
    static bool RegExMatch(const char *strSt, std::string expPattern, bool showReason);
};


/**
 *  @brief CBasePluginObject is a base class of all other classes that generate JSON object to send requests to 
 *  the Allxon DMS server through Allxon DMS Agent APIs.
 */
class PLUGIN_API CBasePluginObject: public CPluginUtil {
public:
    CBasePluginObject();
    ~CBasePluginObject();

    /** @fn std::string GetAppGUID()
     *  @brief Getter of m_appGUID member.
     *  @return The appGUID that set in the object initial stage.
     */
    std::string GetAppGUID() { return m_appGUID; }

    /** @fn void SetAppGUID(std::string appGUID)
     *  @brief Setter of m_appGUID member.
     *  @param[in] Set appGUID for the object in the initial stage.
     */
    void SetAppGUID(std::string appGUID) { m_appGUID = appGUID; }

    /** @fn std::string GetAccessKey()
     *  @brief Getter of m_accessKey member.
     *  @return The accessKey that set in the object initial stage.
     */
    std::string GetAccessKey() { return m_accessKey; }

    /** @fn void SetAccessKey(std::string accessKey)
     *  @brief Setter of m_accessKey member.
     *  @return Set accessKey for the object in the initial stage.
     */
    void SetAccessKey(std::string accessKey) { m_accessKey = accessKey; }

    /** @fn std::string GetApiVersion()
     * @brief Getter of m_apiVersion member.
     * @return The apiVersion that set in the object initial stage. The default value is ApiVersion::v2.
     */
    std::string GetApiVersion() { return m_apiVersion; }

    /** @fn void SetApiVersion(std::string apiVersion)
     *  @brief Setter of m_apiVersion member.
     *  @return Set apiVersion for the object in the initial stage.
     */
    void SetApiVersion(std::string apiVersion) { m_apiVersion = apiVersion; }

    /** @fn std::string GetModuleName()
     *  @brief Getter of m_moduleName member.
     *  @return The moduleName that set in the object initial stage.
     */
    std::string GetModuleName() { return m_moduleName; }

    /** @fn void SetModuleName(std::string moduleName)
     *  @brief Setter of m_moduleName member.
     *  @return Set moduleName for the object in the initial stage.
     */
    void SetModuleName(std::string moduleName) { m_moduleName = moduleName; }

    static std::string GetAgentPluginApi(std::string apiVersion, std::string agentPluginApi);

protected:
    virtual cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetSignedRequestJson(std::string method, cJSON *requestJson, bool unformatted);

    static cJSON *CreateBaseDataItemJson(const char *name, const char *displayName = NULL, const char *description = NULL, const char *displayCategory = NULL);
    static cJSON *CreateBaseValueItemJson(const char *name, const char *value);
    static cJSON *CreateBaseValueItemJson(const char *name, cJSON *value);
    static cJSON *CreateBaseValueItemJson(const char *name, double value);
    static std::string GetSignedMethod(std::string method, std::string signature);

private:
    std::string m_appGUID;
    std::string m_accessKey;
    std::string m_apiVersion;
    std::string m_moduleName;
};

#endif