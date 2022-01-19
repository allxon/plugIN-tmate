#ifndef     UPDATEPLUGINJSON_H_
#define     UPDATEPLUGINJSON_H_

#include "BasePluginObject.h"


#define JKEY_APP_NAME                   "appName"
#define JKEY_TYPE                       "type"
#define JKEY_START_COMMAND              "startCommand"
#define JKEY_STOP_COMMAND               "stopCommand"
#define JKEY_MODULES                    "modules"
#define JKEY_REQUIRED                   "required"
#define JKEY_REQUIRED_ON                "requiredOn"
#define JKEY_DISPLAY_FORMAT             "displayFormat"
#define JKEY_DISPLAY_VALUES             "displayValues"
#define JKEY_DEFAULT_VALUE              "defaultValue"
#define JKEY_LINK_URL                   "url"
#define JKEY_LINK_ALIAS                 "alias"
#define JKEY_DISPLAY_ON_PROPERTY        "displayOnProperty"
#define JKEY_DISPLAY_MASK               "displayMask"
#define JKEY_VALUE_ENCODING             "valueEncoding"
#define JVALUE_T_STRING                 "string"
#define JVALUE_T_TABLE                  "table"


/**
 *  @brief DataType contains the defined types of data or config in a plugIN module.
**/
class PLUGIN_API DataTypes {
public:
    /** Key of the static information data type in a module. */
    const static std::string properties;
    /** Key of the dynamic states data type in a module. */
    const static std::string states;
    /** Key of the events data type in a module. */
    const static std::string events;
    /** Key of the metrics data type in a module. */
    const static std::string metrics;
    /** Key of the function calls type in a module. */
    const static std::string commands;
    /** Key of the alarms type in a module. */
    const static std::string alarms;
    /** Key of the configs data type in a module. */
    const static std::string configs;
};

/**
 *  @brief AppType contains the defined plugIN app type.
**/
class AppType {
public:
    /** Value for in-band plugIN. */
    const static std::string ib;
    /** Value for out-of-band plugIN. */
    const static std::string oob;
};

/**
 *  @brief DisplayType contains the defined displayType of an individual data or config item.
 */
class PLUGIN_API DisplayType {
public:
    /** Value for string displayType. To display a property, state, command param, alarm param or config param as a string.
     *  The maximum string length is up to 512 Bytes.
     */
    const static std::string string;
    /** Value for table displayType. To display a property or state as a table. The maximum total table size is up to 1024 Bytes.
     *  And the maximum string length of each header and value is 128 Bytes.
     */
    const static std::string table;
    /** Value for link displayType. To display this property or state as a URL or IP address of a web page. Mandatory use HTTPS. */
    const static std::string link;
    /** Value for datetime displayType. To display a parameter of commands, alarms or configs as a datetime input. */
    const static std::string datetime;
    /** Value for switch displayType. To display a parameter of commands, alarms or configs as a switch button. */
    const static std::string iswitch;
    /** Value for checkbox displayType. To display a parameter of commands, alarms or configs as a checkbox input. */
    const static std::string icheckbox;
    /** Value for list displayType. To display a parameter of commands, alarms or configs as a list options. */
    const static std::string ilist;
    /** Value for file displayType. To display a parameter of commands, alarms or configs as a file input. */
    const static std::string file;
    /** Value for tos displayType. To display a parameter of commands as a TOS or EULA statement and link. */
    const static std::string tos;
    /** Value for temperature displayType. To display a metric as a temperature, and the data SHOULD be reported in unit of "Kelvin". */
    const static std::string temperature;
    /** Value for displayOn displayType. This property is invisible. To not display this property but e.g. as a reference to the 
     *  "displayOnProperty" for a command.
     */
    const static std::string displayOn;
    /** Value for valueFromProperty displayType. This property is invisible. To provide dynamic data taht a parameter of a command
     *  needs. It supports switch, checkbox and list displayType of command parameter.
     */
    const static std::string valueFromProperty;
};

/**
 *  @brief DataType contains the defined types of data or config in a plugIN module.
 */
class ValueEncoding {
public:
    /** Non-coding string. */
    const static std::string none;
    /** Encode the string inputs to base64 format. */
    const static std::string base64;
};

/**
 *  @brief CUpdatePluginJson generates a JSON request that conform to the format for notifyPluginUpdate API.
 */
class PLUGIN_API CUpdatePluginJson: public CBasePluginObject {
public:
    CUpdatePluginJson();
    ~CUpdatePluginJson();

    /** @fn cJSON *GetUpdateJsonObject()
     * @brief Getter of m_updateJsonObject member. To store the generated JSONRPC object for later use.
     * @return The generated JSONRPC object for notifyPluginUpdate API.
     */
    cJSON *GetUpdateJsonObject() { return m_updateJsonObject; }

    /** @fn void SetUpdateJsonObject(cJSON *updateJson)
     * @brief Setter of m_updateJsonObject member.
     * @param[in] updateJson Store m_updateJsonObject for later use.
     */
    void SetUpdateJsonObject(cJSON *updateJson) { m_updateJsonObject = updateJson; }

    /** @fn bool IsUpdated()
     * @brief Get the flag value to see if the registration request is sent. Check the flag to see if other JSON requests are valid to be sent.
     * @return True for sent notifyPluginUpdate request.\nFalse for the request of plugIN hasn't sent.
     */
    bool IsUpdated() { return m_isUpdated; }

    /** @fn void SetUpdated(bool isUpdated)
     * @brief Set the flag after sending registration JSON for notifyPluginUpdate JSON request.
     * @param[in] isUpdated Set True after sending notifyPluginUpdate JSON request.\nSet False to reset the flag.
     */
    void SetUpdated(bool isUpdated) { m_isUpdated = isUpdated; }
    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted);
    cJSON *RenewUpdateJsonObject(bool unformatted);
    cJSON *CreateUpdateParamsObj(const char *appName, const char *displayName, std::string appType, const char *version,
        const char *startCommand, const char *stopCommand,cJSON *modulesJson);
    static cJSON *CreateUpdateModulesItemJson(const char *moduleName, const char *displayName, const char *description,
        cJSON *properties, cJSON *states, cJSON *events, cJSON *metrics, cJSON *commands, cJSON *alarms, cJSON *configs);
    static cJSON *CreateUpdatePropertiesItemJson(const char *name, const char *displayName, const char *description,
        std::string displayType, const char *displayCategory, const char *value);
    static cJSON *CreateUpdatePropertiesItemJson(const char *name, const char *displayName, const char *description,
        std::string displayType, const char *displayCategory, cJSON *value);
    static cJSON *CreateUpdateStatesItemJson(const char *name, const char *displayName, const char *description, std::string displayType, const char *displayCategory);
    static cJSON *CreateUpdateEventsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory);
    static cJSON *CreateUpdateMetricsItemJson(const char *name, const char *displayName, const char *description, const char *displayUnit,
        std::string displayType, const char *displayCategory);
    static cJSON *CreateUpdateCommandsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory,
        cJSON *paramsJson, cJSON *displayOnPropertyJson = NULL);
    static cJSON *CreateUpdateCommandParamsItemJson(const char *name, const char *displayName, const char *description, const char *defaultValue,
        std::string displayType, bool required, const char *displayFormat, cJSON *displayValues, std::string valueFromProperty, bool displayMask = false,
        std::string requiredOn = "", std::string valueEncoding = ValueEncoding::none);
    static cJSON *CreateUpdateAlarmsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory,
        cJSON *paramsJson);
    static cJSON *CreateUpdateAlarmParamsItemJson(const char *name, const char *displayName, const char *description, const char *defaultValue,
        std::string displayType, bool required, const char *displayFormat = NULL, cJSON *displayValues = NULL, bool displayMask = false,
        std::string requiredOn = "", std::string valueEncoding = ValueEncoding::none);
    static cJSON *CreateUpdateConfigsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory,
        cJSON *paramsJson);
    static cJSON *CreateUpdateConfigParamsItemJson(const char *name, const char *displayName, const char *description, const char *defaultValue,
        std::string displayType, bool required, const char *displayFormat = NULL, cJSON *displayValues = NULL, bool displayMask = false,
        std::string requiredOn = "", std::string valueEncoding = ValueEncoding::none);

private:
    cJSON *m_updateJsonObject;
    bool m_isUpdated;

    static cJSON *CreateUpdateCfgsItemJson(const char *name, const char *displayName, const char *description, const char *displayCategory,
        cJSON *paramsJson);
    static cJSON *CreateUpdateConfigParamsItemJson(const char *name, const char *displayName, const char *description, std::string displayType,
        bool required, const char *displayFormat = NULL, cJSON *displayValues = NULL, std::string requiredOn = "", std::string defaultValue = "",
        std::string valueFromProperty = "", bool displayMask = false, std::string valueEncoding = ValueEncoding::none);
};

#endif