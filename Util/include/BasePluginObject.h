#ifndef     BASEPLUGINOBJECT_H_
#define     BASEPLUGINOBJECT_H_

#include <stddef.h>
#include "../../Util/include/Comm.h"
#include "../../Util/include/cJSON.h"


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


class ApiVersion {
public:
    const static std::string v1;
    const static std::string v2;
};

class AgentPluginApi {
public:
    const static std::string notifyPluginUpdate;
    const static std::string notifyPluginCommand;
    const static std::string notifyPluginCommandAck;
    const static std::string notifyPluginLocalCommand;
    const static std::string notifyPluginState;
    const static std::string notifyPluginEvent;
    const static std::string notifyPluginMetric;
    const static std::string notifyPluginAlarmUpdate;
    const static std::string notifyPluginAlert;
    const static std::string notifyAgentEvent;
};

class MessageType {
public:
    const static std::string api_notifyPluginCommand;
    const static std::string api_notifyPluginAlarmUpdate;
    const static std::string api_notifyAgentEvent;
    const static std::string error;
    const static std::string others;
    const static std::string agentEventServerConnectivity;
    const static std::string online;
    const static std::string offline;
};

class StringFormatter {
public:
    const static std::string nameFormat;
    const static std::string versionFormat;
    const static std::string otaCommand;
};


class CPluginUtil {
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


class CBasePluginObject: public CPluginUtil {
public:
    CBasePluginObject();
    ~CBasePluginObject();

    std::string GetAppGUID() { return m_appGUID; }
    void SetAppGUID(std::string appGUID) { m_appGUID = appGUID; }
    std::string GetAccessKey() { return m_accessKey; }
    void SetAccessKey(std::string accessKey) { m_accessKey = accessKey; }
    std::string GetApiVersion() { return m_apiVersion; }
    void SetApiVersion(std::string apiVersion) { m_apiVersion = apiVersion; }
    std::string GetModuleName() { return m_moduleName; }
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