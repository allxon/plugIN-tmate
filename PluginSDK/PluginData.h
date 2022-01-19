#ifndef _PLUGINDATA_H_
#define _PLUGINDATA_H_

#include "UpdatePluginJson.h"
#include "LocalCommandPluginJson.h"
#include <map>
#include <list>
#define JKEY_APIVERSION         "apiVersion"
#define JKEY_UNFORMATTED_JSON   "unformattedJson"
#define JKEY_DESTINATION_IP     "destinationIP"

/**
 * @brief CUpdateInfo is a base class for plugIN info types data. (Properties, States, Metrics and Events.) To store JSON properties
 *  for properties, states, metrics and events.
 */
class CUpdateInfo {
public:
    CUpdateInfo();
    CUpdateInfo(std::string name, std::string displayCategory, std::string displayName, std::string description);
    ~CUpdateInfo();

    /** @brief The "programming" name of the item. The name must match this regular expression ^[a-zA-Z][a-zA-Z0-9_-]*$. The name
     *  must be unique within the module. The maximum length is 32. */
    std::string m_name;
    /** @brief The "programming" name of the category. The name must match this regular expression ^[a-zA-Z][a-zA-Z0-9_-]*$. The name
     *  must be unique within the data type. */
    std::string m_displayCategory;
    /** @brief A name for human display. */
    std::string m_displayName;
    /** @brief A description for human display. */
    std::string m_description;
};

/**
 * @brief CUpdateProperty is a class to store a plugIN property data of a module.
 */
class PLUGIN_API CUpdateProperty : public CUpdateInfo {
public:
    CUpdateProperty();
    CUpdateProperty(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType, std::string stringValue, std::list<std::map<std::string, std::string> > &tableValue,
        std::map<std::string, std::list<std::string> > const& valueFromProperty = std::map<std::string, std::list<std::string> >());
    ~CUpdateProperty();

    /** @brief The displayType of the property item. (Refer to DisplayType) */
    std::string m_displayType;
    /** @brief Value when displayType is DisplayType::string. */
    std::string m_stringValue;
    /** @brief Value set when displayType is DisplayType::table.
     *   * list: Table columns.
     *   * map:
     *     * first: Header
     *     * second: Value
     */
    std::list<std::map<std::string, std::string> > m_tableValue;
    /** @brief Value set when displayType is DisplayType::valueFromProperty.
     *   * first: A property name.
     *   * second: A set of the property possible values.
     */
    std::map<std::string, std::list<std::string> > m_valueFromProperty;
};

/**
 * @brief CUpdateState is a class to store a plugIN state data of a module.
 * 
 */
class PLUGIN_API CUpdateState : public CUpdateInfo {
public:
    CUpdateState();
    CUpdateState(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType);
    ~CUpdateState();

    /** @brief The displayType of the state item. (Refer to DisplayType) */
    std::string m_displayType;
};

/**
 * @brief CUpdateMetric is a class to store a plugIN metric data of a module.
 * 
 */
class PLUGIN_API CUpdateMetric : public CUpdateInfo {
public:
    CUpdateMetric();
    CUpdateMetric(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string displayType, std::string displayUnit);
    ~CUpdateMetric();

    /** @brief The displayType of the metric item. Supported predefined metric DisplayType::temperature. */
    std::string m_displayType;
    /** @brief The unit of the metric. */
    std::string m_displayUnit;
};

/**
 * @brief CUpdateEvent is a class to store a plugIN event data of a module.
 * 
 */
class PLUGIN_API CUpdateEvent : public CUpdateInfo {
public:
    CUpdateEvent();
    CUpdateEvent(std::string name, std::string displayCategory, std::string displayName, std::string description);
    ~CUpdateEvent();
};

/**
 * @brief CUpdateConfigParam is a base class of CUpdateCommandParam and CUpdateAlarmParam classes that store JSON properties for configs,
 *  commands, or alarms' params. It also uses as a config's param object.
 * 
 */
class PLUGIN_API CUpdateConfigParam {
public:
    CUpdateConfigParam();
    CUpdateConfigParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues,
        std::string defaultValue = "", bool displayMask = false, std::string valueEncoding = ValueEncoding::none);
    ~CUpdateConfigParam();

    virtual cJSON *GetParamJson();

    /** @brief The "programming" name of the item. The name must match this regular expression ^[a-zA-Z][a-zA-Z0-9_-]*$. The name
     *  must be unique within the module. The maximum length is 32.
     */
    std::string m_name;
    /** @brief A name for human display. */
    std::string m_displayName;
    /** @brief A description for human display. */
    std::string m_description;
    /** @brief DisplayType of the parameter of a config item. */
    std::string m_displayType;
    /** @brief Indicates if this parameter is mandatory or not. */
    bool m_required;
    /** @brief Indicates if this parameter is mandatory depending on the other parameter. */
    std::string m_requiredOn;
    /** @brief A property for DisplayType::datetime displayType. The datetime format. */
    std::string m_displayFormat;
    /** @brief The "programming" values of the DisplayType::iswitch button, DisplayType::icheckbox input or drop-down DisplayType::ilist displayTypes. */
    std::list<std::string> m_displayValues;
    /** @brief The default value of this config parameter if any. It should be one of the displayValues of DisplayType::iswitch button,
     *  DisplayType::icheckbox input or drop-down DisplayType::ilist. */
    std::string m_defaultValue;
    /** @brief A property for DisplayType::string displayType. true/false, indicates if the string inputs are masked or not. */
    bool m_displayMask;
    /** @brief A property for DisplayType::string displayType. Set as ValueEncoding::base64 to encode the string inputs to base64 format. */
    std::string m_valueEncoding;
};

/**
 * @brief CUpdateCommandParam is a class to store the data of a parameter of a command.
 * 
 */
class PLUGIN_API CUpdateCommandParam : public CUpdateConfigParam {
public:
    CUpdateCommandParam();
    CUpdateCommandParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues,
        std::string defaultValue, std::string valueFromProperty = "", bool displayMask = false, std::string valueEncoding = ValueEncoding::none);
    ~CUpdateCommandParam();

    cJSON *GetParamJson();
    /** @brief A property for "switch", "checkbox" or "list" displayTypes. Retrieve displayValues and/or defaultValue data referring
     *  to a specific property.
     */
    std::string m_valueFromProperty;
};

/**
 * @brief CUpdateAlarmParam is a class to store the data of a parameter of an alarm.
 * 
 */
class PLUGIN_API CUpdateAlarmParam : public CUpdateConfigParam {
public:
    CUpdateAlarmParam();
    CUpdateAlarmParam(std::string name, std::string displayName, std::string description, std::string displayType, 
        bool required, std::string requiredOn, std::string displayFormat, std::list<std::string> &displayValues,
        std::string defaultValue = "", bool displayMask = false, std::string valueEncoding = ValueEncoding::none);
    ~CUpdateAlarmParam();

    cJSON *GetParamJson();
};

/**
 * @brief CUpdateCommand is a class to store a plugin command data in a module.
 * 
 */
class PLUGIN_API CUpdateCommand : public CUpdateInfo {
public:
    CUpdateCommand();
    CUpdateCommand(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string type, std::list<CUpdateCommandParam *> &params,
        std::map<std::string, std::list<std::string> > const& displayOnProperty = std::map<std::string, std::list<std::string> >());
    CUpdateCommand(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::string type, std::list<std::list<CUpdateCommandParam *> > &paramsArray,
        std::map<std::string, std::list<std::string> > const& displayOnProperty = std::map<std::string, std::list<std::string> >());
    ~CUpdateCommand();

    /** @brief "asynchronous", the command completes sometime after being called. After the command completes, the result, and any
     *  outputs, are available. */
    std::string m_type;
    /** @brief Parameters of this command. */
    std::list<CUpdateCommandParam *> m_params;
    /** @brief Parameters of this command. (For 2D array params) */
    std::list<std::list<CUpdateCommandParam *> > m_paramsArray;
    /** @brief The names of a set of properties. If the value is set, this command will be shown when the property's value is contained
     *  in the set.
     *   * first - property name: A property name
     *   * second - value criteria: A list of property values (string type) for the property. To check if the property value is in
     *  the list, set the command as visible.
     */
    std::map<std::string, std::list<std::string> > m_displayOnProperty;
};

/**
 * @brief CUpdateAlarm is a class to store a plugIN alarm data in a module.
 * 
 */
class PLUGIN_API CUpdateAlarm : public CUpdateInfo {
public:
    CUpdateAlarm();
    CUpdateAlarm(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::list<CUpdateAlarmParam *> &params);
    CUpdateAlarm(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::list<std::list<CUpdateAlarmParam *> > &paramsArray);
    ~CUpdateAlarm();

    /** @brief Parameters of this command. */
    std::list<CUpdateAlarmParam *> m_params;
    /** @brief Parameters of this alarm. (For 2D array params) */
    std::list<std::list<CUpdateAlarmParam *> > m_paramsArray;
};

/**
 * @brief CUpdateConfig is a class to store a plugin config data in a module.
 * 
 */
class CUpdateConfig : public CUpdateInfo {
public:
    CUpdateConfig();
    CUpdateConfig(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::list<CUpdateConfigParam *> &params);
    CUpdateConfig(std::string name, std::string displayCategory, std::string displayName, std::string description,
        std::list< std::list<CUpdateConfigParam *> > &paramsArray);
    ~CUpdateConfig();

    /** @brief Parameters of this config. */
    std::list<CUpdateConfigParam *> m_params;
    /** @brief Parameters of this config. (For 2D array params) */
    std::list< std::list<CUpdateConfigParam *> > m_paramsArray;
};

/**
 * @brief CUpdateModule is a class to store the data of a module.
 * 
 */
class PLUGIN_API CUpdateModule {
public:
    CUpdateModule();
    CUpdateModule(std::string moduleName, std::string displayName, std::string description, std::list<CUpdateProperty *> &properties,
        std::list<CUpdateState *> &states, std::list<CUpdateMetric *> &metrics, std::list<CUpdateEvent *> &events,
        std::list<CUpdateCommand *> &commands, std::list<CUpdateAlarm *> &alarms, std::list<CUpdateConfig *> &configs);
    ~CUpdateModule();

    cJSON *GetUpdateModule();

    /** @brief The "programming" name of the module. The name must match this regular expression ^[a-zA-Z][a-zA-Z0-9_-]*$. The name
     *  must be unique within the plugIN. The maximum length is 64. */
    std::string m_moduleName;
    /** @brief A name for human display. */
    std::string m_displayName;
    /** @brief A description for human display. */
    std::string m_description;
    /** @brief Provides module static information, such as firmware version, hardware configuration, ..., etc. It's a list of
     *  CUpdateProperty objects. */
    std::list<CUpdateProperty *> m_properties;
    /** @brief Provides module dynamic states, such as power state, ..., etc. It's a list of CUpdateState objects. */
    std::list<CUpdateState *> m_states;
    /** @brief Provides module metrics, such as CPU temperature, voltage, ..., etc. It's a list of CUpdateMetric objects. */
    std::list<CUpdateMetric *> m_metrics;
    /** @brief Provides module events, such as intrusion detection, ..., etc. It's a list of CUpdateEvent objects. */
    std::list<CUpdateEvent *> m_events;
    /** @brief rovides module function calls, such as power cycling, set hardware configurations, ..., etc. It's a list of
     *  CUpdateCommand objects. */
    std::list<CUpdateCommand *> m_commands;
    /** @brief Provides module alarms, such as CPU overloading, thermo, ..., etc. It's a list of CUpdateAlarm objects. */
    std::list<CUpdateAlarm *> m_alarms;
    /** @brief Provides module configs, such as scheduling, ..., etc. It's a list of CUpdateConfig objects. */
    std::list<CUpdateConfig *> m_configs;
};

/**
 * @brief CUpdateParams is a class to store the data of the params of notifyPluginUpdate API.
 * 
 */
class PLUGIN_API CUpdateParams {
public:
    CUpdateParams();
    CUpdateParams(std::string appGUID, std::string appName, std::string displayName, std::string type, std::string version,
        std::string startCommand, std::string stopCommand, std::list<CUpdateModule *> &modules);
    ~CUpdateParams();

    CUpdatePluginJson *GetNotifyPluginUpdate();
    CUpdatePluginJson *GetNotifyPluginUpdate(const char *apiVersion, const char *accessKey);
    void UpdateNotifyPluginUpdateData();

    /** @brief The GUID of the plugIN. */
    std::string m_appGUID;
    /** @brief The "programming" name of the plugIN.  The name must match this regular expression ^[a-zA-Z][a-zA-Z0-9_-]*$.
     *  The name must be unique for all plugINs. */
    std::string m_appName;
    /** @brief A name for human display. */
    std::string m_displayName;
    /** @brief Type of a plugIN app.
     *  * AppType::ib for in-band plugIN.
     *  * AppType::oob for out-of-band plugIN. */
    std::string m_type;
    /** @brief The version of the plugIN.  The version uses a sequence of three digits (Major.Minor.Patch), i.e. must match this
     *  regular expression ^[0-9]+[.][0-9]+[.][0-9]+$. */
    std::string m_version;
    /** @brief The command name to start up this plugIN. */
    std::string m_startCommand;
    /** @brief The command name to stop this plugIN. */
    std::string m_stopCommand;
    /** @brief A set of hardware or software modules controlled by this plugIN. It's a list of CUpdateModule objects. */
    std::list<CUpdateModule *> m_modules;

private:
    CUpdatePluginJson *m_pluginUpdateObj;
};

/**
 * @brief CLocalCommandParam is a class to store the data of a parameter of a local command.
 * 
 */
class PLUGIN_API CLocalCommandParam {
public:
    CLocalCommandParam();
    CLocalCommandParam(std::string name, std::string value);
    ~CLocalCommandParam();

    /** @brief The name of the parameter of a local command. */
    std::string m_name;
    /** @brief The value of a command's parameter. */
    std::string m_value;
};

/**
 * @brief CLocalCommand is a class to store a local command object, which is one of "commands" in a local command request.
 * 
 */
class PLUGIN_API CLocalCommand {
public:
    CLocalCommand();
    CLocalCommand(std::string name, std::list<CLocalCommandParam *> &params);
    ~CLocalCommand();

    /** @brief The name of the local command. */
    std::string m_name;
    /** @brief A set of name and value pairs of parameters (CLocalCommandParam) for the command. The maximum total command size is up
     *  to 1024 Bytes. Don't set this item when this command doesn't have any parameters. */
    std::list<CLocalCommandParam *> m_params;
};

/**
 * @brief CLocalCommandParams is a class to store the params object of a local command request in a client plugIN app.
 * 
 */
class PLUGIN_API CLocalCommandParams {
public:
    CLocalCommandParams();
    CLocalCommandParams(std::string clientAppGUID, std::string appGUID, std::string serialNumber, std::string moduleName,
        std::list<CLocalCommand *> &commands);
    ~CLocalCommandParams();

    CLocalCommandPluginJson *GetNotifyPluginLocalCommand();
    CLocalCommandPluginJson *GetNotifyPluginLocalCommand(const char *apiVersion, const char *accessKey);

    /** @brief The GUID of the sender plugIN. */
    std::string m_clientAppGUID;
    /** @brief The GUID of the target plugIN. */
    std::string m_appGUID;
    /** @brief The serial number of the device behind a gateway.  Only required when sending commands to devices behind a gateway. */
    std::string m_serialNumber;
    /** @brief The name of the module. */
    std::string m_moduleName;
    /** @brief A set of local commands. (CLocalCommand) */
    std::list<CLocalCommand *> m_commands;

private:
    CLocalCommandPluginJson *m_localCommandObj;
};
#endif