#ifndef     CONFIGSPLUGINJSON_H_
#define     CONFIGSPLUGINJSON_H_

#include "BasePluginObject.h"
#include <list>


#define JKEY_CONFIGS                    "configs"
#define JKEY_MODULES                    "modules"
#define JKEY_TIME                       "time"


/**
 *  @brief CConfigUpdatePluginJson parses received notifyPluginConfigUpdate API message JSON and get configs data for devices use.
 */
class CConfigUpdatePluginJson: public CPluginUtil {
public:
    CConfigUpdatePluginJson(const char *configsJsonString, std::string accessKey);
    ~CConfigUpdatePluginJson();

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

    /** @fn std::string GetModuleName()
     *  @brief Getter of m_moduleName member.
     *  @return The moduleName of the notifyPluginConfigUpdate API.
     */
    std::string GetModuleName() { return m_moduleName; }

    /** @fn void SetModuleName(std::string moduleName)
     *  @brief Setter of m_moduleName member.
     *  @param[in] moduleName The moduleName of the notifyPluginConfigUpdate API.
     */
    void SetModuleName(std::string moduleName) { m_moduleName = moduleName; }

    /** @fn std::string GetUpdateEpoch()
     *  @brief Getter of m_updateEpoch member.
     *  @return The get the config update epoch time.
     */
    std::string GetUpdateEpoch() { return m_updateEpoch; }

    /** @fn void SetUpdateEpoch(std::string updateEpoch)
     *  @brief Setter of m_updateEpoch member.
     *  @param[in] updateEpoch To store the config update epoch time gotten from "modules" JSON.
     */
    void SetUpdateEpoch(std::string updateEpoch) { m_updateEpoch = updateEpoch; }

    /** @fn std::string GetVersion()
     *  @brief Get the plugIN app version while receiving notifyPluginConfigUpdate API.
     *  @return Version of the plugIN.
     */
    std::string GetVersion() { return m_version; }

    /** @fn std::list<cJSON *> GetModulesParam()
     *  @brief Getter of m_modules member. To get the list of JSON objects' string from "modules" JSON value about configs for various modules.
     *  @return The list of JSON objects' string from "modules" JSON value about configs for various modules.
     */
    std::list<cJSON *> GetModulesParam() { return m_modules; }

    /** @fn bool IsValidSignature()
     *  @brief To get if the received notifyPluginConfigUpdate message is valid on epoch and signature after called ValidConfig() to check.
     *  @return True for valid signature.\n
     *  False for invalid signature.
     */
    bool IsValidSignature() { return m_validSignature; }
    std::list<cJSON *> GetConfigs(std::string moduleName, std::string configType = JKEY_CONFIGS);
    const char *GetConfigsUpdateEpochTime(std::string moduleName);
    
private:
    std::string m_accessKey;
    std::string m_moduleName;
    std::string m_updateEpoch;
    std::string m_version;
    std::list<cJSON *> m_modules;
    bool m_validSignature;

    void ValidateConfigs(const char *configsJsonString);
};

#endif