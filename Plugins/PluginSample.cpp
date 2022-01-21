#include "PluginSample.h"
#include "../PluginSDK/StatesPluginJson.h"
#include "../PluginSDK/EventsPluginJson.h"
#include "../PluginSDK/MetricsPluginJson.h"
#include "../PluginSDK/ConfigsPluginJson.h"
#include "../PluginSDK/PluginException.h"
#include "../Util/include/Utl_Log.h"

using namespace std;

#define JKEY_CNFG_APIVERSION    "apiVersion"
#define JKEY_CNFG_MINIFIED      "unformattedJson"
#define JKEY_CNFG_APPGUID       "appGUID"
#define JKEY_CNFG_ACCESSKEY     "accessKey"
#define JKEY_CNFG_SAMPLENAME    "sampleName"
#define JKEY_CNFG_CLIENTAPPGUID "clientAppGUID"
#define JKEY_CNFG_MODULENAME    "moduleName"
#define JKEY_CNFG_DESTINATIONIP "destinationIP"
#define JKEY_CNFG_SERIALNUMBER  "serialNumber"

CPluginSampleConfig::CPluginSampleConfig()
{
    this->apiVersion = ApiVersion::v2;
    this->minify = true;
}

CPluginSampleConfig::CPluginSampleConfig(const char *configFile)
{
    GetSampleConfig(configFile);
}

CPluginSampleConfig::~CPluginSampleConfig()
{
}

void CPluginSampleConfig::GetSampleConfig(const char *configFile)
{
    UTL_LOG_INFO("configFile: %s", configFile);
    if (configFile && strlen(configFile) >= 0)
    {
        try {
            cJSON *config = CPluginUtil::GetJsonFromFile(configFile);
            if (config)
            {
                cJSON *apiVersionItem = cJSON_GetObjectItem(config, JKEY_CNFG_APIVERSION);
                if (apiVersionItem) apiVersion = string(apiVersionItem->valuestring);

                cJSON *appGUIDItem = cJSON_GetObjectItem(config, JKEY_CNFG_APPGUID);
                if (appGUIDItem) appGUID = string(appGUIDItem->valuestring);

                cJSON *accessKeyItem = cJSON_GetObjectItem(config, JKEY_CNFG_ACCESSKEY);
                if (accessKeyItem) accessKey = string(accessKeyItem->valuestring);

                cJSON *minifyItem = cJSON_GetObjectItem(config, JKEY_CNFG_MINIFIED);
                if (minifyItem) minify = minifyItem->type == cJSON_False ? false : true;

                cJSON *sampleNameItem = cJSON_GetObjectItem(config, JKEY_CNFG_SAMPLENAME);
                if (sampleNameItem) sampleName = string(sampleNameItem->valuestring);
#ifdef DEBUG
                UTL_LOG_INFO("apiVersion: %s, minify: %d, appGUID: %s, accessKey: %s", apiVersion.c_str(), minify, appGUID.c_str(), accessKey.c_str());
#endif
            }
        }
        catch (const exception & e)
        {
            UTL_LOG_ERROR("Exception: %s\n", e.what());
        }
    }
}

CLocalCommandSampleConfig::CLocalCommandSampleConfig():CPluginSampleConfig()
{
}

CLocalCommandSampleConfig::CLocalCommandSampleConfig(const char *configFile):CPluginSampleConfig(configFile)
{
    GetSampleConfig(configFile);
}

CLocalCommandSampleConfig::~CLocalCommandSampleConfig()
{
}

void CLocalCommandSampleConfig::GetSampleConfig(const char *configFile)
{
    UTL_LOG_INFO("configFile: %s", configFile);
    if (configFile && strlen(configFile) >= 0)
    {
        try {
            cJSON *config = CPluginUtil::GetJsonFromFile(configFile);
            if (config)
            {
                cJSON *apiVersionItem = cJSON_GetObjectItem(config, JKEY_CNFG_APIVERSION);
                if (apiVersionItem) apiVersion = string(apiVersionItem->valuestring);

                cJSON *appGUIDItem = cJSON_GetObjectItem(config, JKEY_CNFG_APPGUID);
                if (appGUIDItem) appGUID = string(appGUIDItem->valuestring);

                cJSON *accessKeyItem = cJSON_GetObjectItem(config, JKEY_CNFG_ACCESSKEY);
                if (accessKeyItem) accessKey = string(accessKeyItem->valuestring);

                cJSON *minifyItem = cJSON_GetObjectItem(config, JKEY_CNFG_MINIFIED);
                if (minifyItem) minify = minifyItem->type == cJSON_False ? false : true;

                cJSON *sampleNameItem = cJSON_GetObjectItem(config, JKEY_CNFG_SAMPLENAME);
                if (sampleNameItem) sampleName = string(sampleNameItem->valuestring);

                cJSON *clientAppGUIDItem = cJSON_GetObjectItem(config, JKEY_CNFG_CLIENTAPPGUID);
                if (clientAppGUIDItem) clientAppGUID = string(clientAppGUIDItem->valuestring);

                cJSON *moduleNameItem = cJSON_GetObjectItem(config, JKEY_CNFG_MODULENAME);
                if (moduleNameItem) moduleName = string(moduleNameItem->valuestring);

                cJSON *destinationIPItem = cJSON_GetObjectItem(config, JKEY_CNFG_DESTINATIONIP);
                if (destinationIPItem) destinationIP = string(destinationIPItem->valuestring);

                cJSON *serialNumberItem = cJSON_GetObjectItem(config, JKEY_CNFG_SERIALNUMBER);
                if (serialNumberItem) serialNumber = string(serialNumberItem->valuestring);
// #ifdef DEBUG
//                 UTL_LOG_INFO("apiVersion: %s, minify: %d, appGUID: %s, accessKey: %s, clientAppGUID: %s, moduleName: %s, destinationIP: %s, serialNumber: %s",
//                     apiVersion.c_str(), minify, appGUID.c_str(), accessKey.c_str(), clientAppGUID.c_str(), moduleName.c_str(), destinationIP.c_str(), serialNumber.c_str());
// #endif
            }
        }
        catch (const exception & e)
        {
            UTL_LOG_ERROR("Exception: %s\n", e.what());
        }
    }
}

CUpdateConfigParam * GetUpdateConfigParam(cJSON *paramJson)
{
    string name = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_NAME);
    string displayName = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_DISPLAY_NAME);
    string description = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_DESCRIPTION);
    string displayType = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_DISPLAY_TYPE);
    string defaultValue = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_DEFAULT_VALUE);
    bool required = CPluginUtil::GetJSONBooleanFieldValue(paramJson, JKEY_REQUIRED);
    string requiredOn = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_REQUIRED_ON);
    string displayFormat;
    list<string> displayValues;
    bool displayMask = false;
    string valueEncoding;
    if (displayType.compare(DisplayType::string) == 0)
    {
        displayMask = CPluginUtil::GetJSONBooleanFieldValue(paramJson, JKEY_DISPLAY_MASK);
        valueEncoding = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_VALUE_ENCODING);
    }
    else if (displayType.compare(DisplayType::datetime) == 0) displayFormat = CPluginUtil::GetJSONStringFieldValue(paramJson, JKEY_DISPLAY_FORMAT);
    else if (displayType.compare(DisplayType::icheckbox) == 0 || displayType.compare(DisplayType::iswitch) == 0 ||
        displayType.compare(DisplayType::ilist) == 0)
    {
        cJSON *displayValuesJson = cJSON_GetObjectItem(paramJson, JKEY_DISPLAY_VALUES);
        cJSON *value;
        cJSON_ArrayForEach(value, displayValuesJson)
        {
            displayValues.push_back(value->valuestring);
        }
    }
    // UTL_LOG_INFO("%s, %s, %s, %s, %d, %s, %s, %p, %s, %d, %s", name.c_str(), displayName.c_str(), description.c_str(), displayType.c_str(),
    //     required, requiredOn.c_str(), displayFormat.c_str(), displayValues, defaultValue.c_str(), displayMask, valueEncoding.c_str());
    return new CUpdateConfigParam(name, displayName, description, displayType, required, requiredOn, displayFormat, displayValues,
        defaultValue, displayMask, valueEncoding);
}

CUpdateAlarmParam * GetUpdateAlarmParam(cJSON *paramJson)
{
    CUpdateConfigParam *cfgParam = GetUpdateConfigParam(paramJson);

    return new CUpdateAlarmParam(cfgParam->m_name, cfgParam->m_displayName, cfgParam->m_description, cfgParam->m_displayType,
        cfgParam->m_required, cfgParam->m_requiredOn, cfgParam->m_displayFormat, cfgParam->m_displayValues, cfgParam->m_defaultValue,
        cfgParam->m_displayMask, cfgParam->m_valueEncoding);
}

CUpdateCommandParam * GetUpdateCommandParam(cJSON *paramJson)
{
    CUpdateConfigParam *cfgParam = GetUpdateConfigParam(paramJson);
    string valueFromProperty;
    if (cfgParam->m_displayType.compare(DisplayType::icheckbox) == 0 ||
        cfgParam->m_displayType.compare(DisplayType::iswitch) == 0 ||
        cfgParam->m_displayType.compare(DisplayType::ilist) == 0)
    {
        valueFromProperty = CPluginUtil::GetJSONStringFieldValue(paramJson, DisplayType::valueFromProperty.c_str());
        // UTL_LOG_INFO("valueFromProperty: %s", valueFromProperty.c_str());
    }
    
    return new CUpdateCommandParam(cfgParam->m_name, cfgParam->m_displayName, cfgParam->m_description, cfgParam->m_displayType,
        cfgParam->m_required, cfgParam->m_requiredOn, cfgParam->m_displayFormat, cfgParam->m_displayValues, cfgParam->m_defaultValue,
        valueFromProperty);
}

CPluginSample::CPluginSample()
{
    this->apiVersion = ApiVersion::v2;
    this->minify = true;
}

CPluginSample::CPluginSample(CPluginSampleConfig *sampleConfig)
{
    if (sampleConfig)
    {
        this->apiVersion = sampleConfig->GetApiVersion();
        this->minify = sampleConfig->GetMinify();
        this->appGUID = sampleConfig->GetAppGUID();
        if (ApiVersion::v2.compare(apiVersion) == 0) this->accessKey = sampleConfig->GetAccessKey();
// #ifdef DEBUG
//         UTL_LOG_INFO("apiVersion: %s, minify: %d, appGUID: %s, accessKey: %s", apiVersion.c_str(), minify, appGUID.c_str(), accessKey.c_str());
// #endif
    }
    else UTL_LOG_INFO("Please get plugin sample config first. Usage: device_plugin plugin_config_[sample].json");
}

CPluginSample::CPluginSample(string apiVersion, bool minify, string appGUID, string accessKey)
{
    this->apiVersion = apiVersion;
    this->minify = minify;
    this->appGUID = appGUID;
    if (ApiVersion::v2.compare(apiVersion) == 0) this->accessKey = accessKey;
}

CPluginSample::~CPluginSample()
{
    UTL_LOG_INFO("Base destructor.");
    if (pluginData) delete(pluginData);
}

CUpdateParams *CPluginSample::GetUpdateData()
{
    return pluginData;
}

CLocalCommandParams *CPluginSample::GetLocalCommandData()
{
    return localCommandData;
}

CUpdateModule *CPluginSample::GetModule(std::string moduleName)
{
    if (moduleName.empty()) return NULL;

    list<CUpdateModule *> modules = pluginData->m_modules;
    list<CUpdateModule *>::iterator itm;
    for (itm=modules.begin(); itm!=modules.end(); itm++)
    {
        if (moduleName.compare((*itm)->m_moduleName) == 0) return *itm;
    }
    return NULL;
}

CUpdatePluginJson *CPluginSample::SetNotifyPluginUpdateFromFile(string jsonFile)
{
    try {
        cJSON *updateParam = CPluginUtil::GetJsonFromFile(jsonFile);
#ifdef DEBUG
        UTL_LOG_INFO("File: %s, JSON: %s", jsonFile.c_str(), cJSON_PrintUnformatted(updateParam));
#endif
        if (updateParam)
        {
            string appGuid = CPluginUtil::GetJSONStringFieldValue(updateParam, JKEY_APP_GUID);
            if (appGuid.compare(appGUID) != 0)
            {
                UTL_LOG_WARN("The appGUID isn't consistent with the one set in the plugin config file. Please check its correction.");
                return NULL;
            }
            cJSON *modulesJson = cJSON_GetObjectItem(updateParam, JKEY_MODULES);
            if (!cJSON_IsArray(modulesJson))
            {
                UTL_LOG_WARN("\"modules\" should be an array. Terminated parsing json file.");
                return NULL;
            }
            
            // modules
            list<CUpdateModule *> lModules;
            if (modulesJson)
            {
                cJSON *moduleJson;
                cJSON_ArrayForEach (moduleJson, modulesJson)
                {
                    // properties
                    cJSON *propertiesJson = cJSON_GetObjectItem(moduleJson, DataTypes::properties.c_str());
                    if (propertiesJson && !cJSON_IsArray(propertiesJson))
                    {
                        UTL_LOG_WARN("\"properties\" should be an array. Terminated parsing json file.");
                        return NULL;
                    }
                    list<CUpdateProperty *> lProperties;
                    if (propertiesJson)
                    {
                        cJSON *propertyJson;
                        cJSON_ArrayForEach(propertyJson, propertiesJson)
                        {
                            string name = CPluginUtil::GetJSONStringFieldValue(propertyJson, JKEY_NAME);
                            string displayCategory = CPluginUtil::GetJSONStringFieldValue(propertyJson, JKEY_DISPLAY_CATEGORY);
                            string displayName = CPluginUtil::GetJSONStringFieldValue(propertyJson, JKEY_DISPLAY_NAME);
                            string description = CPluginUtil::GetJSONStringFieldValue(propertyJson, JKEY_DESCRIPTION);
                            string displayType = CPluginUtil::GetJSONStringFieldValue(propertyJson, JKEY_DISPLAY_TYPE);
                            list<map<string, string> > tableValue;
                            map<string, list<string> > valueFromProperty;
                            if (displayType.compare(DisplayType::string) == 0 || displayType.compare(DisplayType::displayOn) == 0)
                            {
                                tableValue.clear();
                                string value = CPluginUtil::GetJSONStringFieldValue(propertyJson, JKEY_VALUE);
                                lProperties.push_back(new CUpdateProperty(name, displayCategory, displayName, description,
                                    displayType, value, tableValue));
                            }
                            else if (displayType.compare(DisplayType::table) == 0)
                            {
                                cJSON *tableJson = cJSON_GetObjectItem(propertyJson, JKEY_VALUE);
                                UTL_LOG_INFO("value: %s", cJSON_PrintUnformatted(tableJson));
                                cJSON *row;
                                cJSON_ArrayForEach(row, tableJson)
                                {
                                    map<string, string> rowItem;
                                    UTL_LOG_INFO("row: %s", cJSON_PrintUnformatted(row));
                                    cJSON *column;
                                    cJSON_ArrayForEach(column, row)
                                    {
                                        rowItem.insert(pair<string, string>(column->string, column->valuestring));
                                        tableValue.push_back(rowItem);
                                    }
                                }
                                lProperties.push_back(new CUpdateProperty(name, displayCategory, displayName, description,
                                    displayType, "", tableValue));
                            }
                            else if (displayType.compare(DisplayType::link) == 0)
                            {
                                cJSON *linkJson = cJSON_GetObjectItem(propertyJson, JKEY_VALUE);
                                if (linkJson)
                                {
                                    map<string, string> linkItem;
                                    string url = CPluginUtil::GetJSONStringFieldValue(linkJson, JKEY_LINK_URL);
                                    string alias = CPluginUtil::GetJSONStringFieldValue(linkJson, JKEY_LINK_ALIAS);
                                    linkItem.insert(pair<string, string>(url, alias));
                                    tableValue.push_back(linkItem);
                                    lProperties.push_back(new CUpdateProperty(name, displayCategory, displayName, description,
                                        displayType, "", tableValue));
                                }
                            }
                            else if (displayType.compare(DisplayType::valueFromProperty) == 0)
                            {
                                cJSON *valueFmPropJson = cJSON_GetObjectItem(propertyJson, JKEY_VALUE);
                                if (valueFmPropJson)
                                {
                                    cJSON *valueItem;
                                    cJSON_ArrayForEach(valueItem, valueFmPropJson)
                                    {
                                        // Handle displayValues data
                                        cJSON *displayValues = cJSON_GetObjectItem(valueItem, JKEY_DISPLAY_VALUES);
                                        if (displayValues && cJSON_IsArray(displayValues) == cJSON_True)
                                        {
                                            cJSON *optionValue;
                                            list<string> listDisplayValues;
                                            cJSON_ArrayForEach(optionValue, displayValues)
                                            {
                                                listDisplayValues.push_back(cJSON_GetStringValue(optionValue));
                                            }
                                            if (!listDisplayValues.empty()) valueFromProperty.insert(pair<string, list<string> >(JKEY_DISPLAY_VALUES, listDisplayValues));
                                        }
                                        // Handle defaultValue data if any.
                                        string defaultValue = CPluginUtil::GetJSONStringFieldValue(valueItem, JKEY_DEFAULT_VALUE);
                                        if (!defaultValue.empty())
                                        {
                                            list<string> listDefaultValue;
                                            listDefaultValue.push_back(defaultValue);
                                            valueFromProperty.insert(pair<string, list<string> >(JKEY_DEFAULT_VALUE, listDefaultValue));
                                        }
#ifdef DEBUG
                                        for (auto it = valueFromProperty.begin(); it!=valueFromProperty.end(); it++)
                                        {
                                            UTL_LOG_INFO("valueFromProperty: %s", (*it).first.c_str());
                                            list<string> lValues = (*it).second;
                                            for (auto itl = lValues.begin(); itl!=lValues.end(); itl++)
                                            {
                                                UTL_LOG_INFO("value: %s", (*itl).c_str());
                                            }
                                        }
#endif
                                        lProperties.push_back(new CUpdateProperty(name, displayCategory, displayName, description,
                                            displayType, "", tableValue, valueFromProperty));
                                    }
                                }
                            }
                        }
                    }

                    // states
                    cJSON *statesJson = cJSON_GetObjectItem(moduleJson, DataTypes::states.c_str());
                    if (statesJson && !cJSON_IsArray(statesJson))
                    {
                        UTL_LOG_WARN("\"states\" should be an array. Terminated parsing json file.");
                        return NULL;
                    }
                    list<CUpdateState *> lStates;
                    if (statesJson)
                        {
                        cJSON *stateJson;
                        cJSON_ArrayForEach(stateJson, statesJson)
                        {
                            string name = CPluginUtil::GetJSONStringFieldValue(stateJson, JKEY_NAME);
                            string displayCategory = CPluginUtil::GetJSONStringFieldValue(stateJson, JKEY_DISPLAY_CATEGORY);
                            string displayName = CPluginUtil::GetJSONStringFieldValue(stateJson, JKEY_DISPLAY_NAME);
                            string description = CPluginUtil::GetJSONStringFieldValue(stateJson, JKEY_DESCRIPTION);
                            string displayType = CPluginUtil::GetJSONStringFieldValue(stateJson, JKEY_DISPLAY_TYPE);
                            lStates.push_back(new CUpdateState(name, displayCategory, displayName, description, displayType));
                        }
                    }

                    // metrics
                    cJSON *metricsJson = cJSON_GetObjectItem(moduleJson, DataTypes::metrics.c_str());
                    if (metricsJson && !cJSON_IsArray(metricsJson))
                    {
                        UTL_LOG_WARN("\"metrics\" should be an array. Terminated parsing json file.");
                        return NULL;
                    }
                    list<CUpdateMetric *> lMetrics;
                    if (metricsJson)
                    {
                        cJSON *metricJson;
                        cJSON_ArrayForEach(metricJson, metricsJson)
                        {
                            string name = CPluginUtil::GetJSONStringFieldValue(metricJson, JKEY_NAME);
                            string displayCategory = CPluginUtil::GetJSONStringFieldValue(metricJson, JKEY_DISPLAY_CATEGORY);
                            string displayName = CPluginUtil::GetJSONStringFieldValue(metricJson, JKEY_DISPLAY_NAME);
                            string description = CPluginUtil::GetJSONStringFieldValue(metricJson, JKEY_DESCRIPTION);
                            string displayType = CPluginUtil::GetJSONStringFieldValue(metricJson, JKEY_DISPLAY_TYPE);
                            string displayUnit = CPluginUtil::GetJSONStringFieldValue(metricJson, JKEY_DISPLAY_UNIT);
                            lMetrics.push_back(new CUpdateMetric(name, displayCategory, displayName, description, displayType, displayUnit));
                        }
                    }

                    // events
                    cJSON *eventsJson = cJSON_GetObjectItem(moduleJson, DataTypes::events.c_str());
                    if (eventsJson && !cJSON_IsArray(eventsJson))
                    {
                        UTL_LOG_WARN("\"events\" should be an array. Terminated parsing json file.");
                        return NULL;
                    }
                    list<CUpdateEvent *> lEvents;
                    if (eventsJson)
                    {
                        cJSON *eventJson;
                        cJSON_ArrayForEach(eventJson, eventsJson)
                        {
                            string name = CPluginUtil::GetJSONStringFieldValue(eventJson, JKEY_NAME);
                            string displayCategory = CPluginUtil::GetJSONStringFieldValue(eventJson, JKEY_DISPLAY_CATEGORY);
                            string displayName = CPluginUtil::GetJSONStringFieldValue(eventJson, JKEY_DISPLAY_NAME);
                            string description = CPluginUtil::GetJSONStringFieldValue(eventJson, JKEY_DESCRIPTION);
                            lEvents.push_back(new CUpdateEvent(name, displayCategory, displayName, description));
                        }
                    }

                    // commands
                    cJSON *commandsJson = cJSON_GetObjectItem(moduleJson, DataTypes::commands.c_str());
                    if (commandsJson && !cJSON_IsArray(commandsJson))
                    {
                        UTL_LOG_WARN("\"commands\" should be an array. Terminated parsing json file.");
                        return NULL;
                    }
                    list<CUpdateCommand *> lCommands;
                    if (commandsJson) {
                        cJSON *commandJson;
                        bool is2DArray = false;
                        cJSON_ArrayForEach(commandJson, commandsJson)
                        {
                            cJSON *paramsJson = cJSON_GetObjectItem(commandJson, JKEY_PARAMS);
                            if (paramsJson != NULL && !cJSON_IsArray(paramsJson))
                            {
                                UTL_LOG_WARN("\"params\" of a command should be an array. Terminated parsing json file.");
                                return NULL;
                            }
                            list<list<CUpdateCommandParam *> > lcmdParams;
                            if (paramsJson)
                            {
                                cJSON *paramJson;
                                cJSON_ArrayForEach(paramJson, paramsJson)
                                {
                                    list<CUpdateCommandParam *>lcmdRowParams;
                                    is2DArray = cJSON_IsArray(paramJson);
                                    UTL_LOG_INFO("is2DArray: %d", is2DArray);
                                    if (is2DArray)
                                    {
                                        cJSON *paramRowJson;
                                        cJSON_ArrayForEach(paramRowJson, paramJson)
                                        {
                                            lcmdRowParams.push_back(GetUpdateCommandParam(paramRowJson));
                                        }
                                    }
                                    else
                                    {
                                        lcmdRowParams.push_back(GetUpdateCommandParam(paramJson));
                                    }
                                    lcmdParams.push_back(lcmdRowParams);
                                }
                            }
                            string name = CPluginUtil::GetJSONStringFieldValue(commandJson, JKEY_NAME);
                            string displayCategory = CPluginUtil::GetJSONStringFieldValue(commandJson, JKEY_DISPLAY_CATEGORY);
                            string displayName = CPluginUtil::GetJSONStringFieldValue(commandJson, JKEY_DISPLAY_NAME);
                            string description = CPluginUtil::GetJSONStringFieldValue(commandJson, JKEY_DESCRIPTION);
                            string type = CPluginUtil::GetJSONStringFieldValue(commandJson, JKEY_TYPE);
                            cJSON *displayOnPropertyJson = cJSON_GetObjectItem(commandJson, JKEY_DISPLAY_ON_PROPERTY);
                            map<string, list<string> > displayOnProperty;
                            if (displayOnPropertyJson)
                            {
                                if (cJSON_IsArray(displayOnPropertyJson))
                                {
                                    cJSON *property;
                                    cJSON_ArrayForEach(property, displayOnPropertyJson)
                                    {
                                        cJSON *element;
                                        cJSON_ArrayForEach(element, property)
                                        {
                                            list<string> propValues;
                                            cJSON *propValuesJson = cJSON_GetObjectItem(element, element->string);
                                            if (cJSON_IsArray(propValuesJson))
                                            {
                                                cJSON *propValueJson;
                                                cJSON_ArrayForEach(propValueJson, propValuesJson)
                                                {
                                                    string propValue = cJSON_GetStringValue(propValueJson);
                                                    if (!propValue.empty()) propValues.push_back(propValue);
                                                }
                                            }
                                            if (!propValues.empty()) displayOnProperty.insert(pair<string, list<string> >(string(element->string), propValues));
                                        }
                                    }
                                }
                            }
                            //lCommands.push_back(new CUpdateCommand(name, displayCategory, displayName, description, type, lcmdParams, displayOnProperty));
                            if (is2DArray)
                            {
                                // list<list<CUpdateCommandParam *> >::iterator ita;
                                // for (ita = lcmdParams.begin(); ita != lcmdParams.end(); ita++)
                                // {
                                //     list<CUpdateCommandParam *>::iterator itb;
                                //     for (itb = (*ita).begin(); itb != (*ita).end(); itb++)
                                //     {
                                //         CUpdateCommandParam *cmdParam = *itb;
                                //         UTL_LOG_INFO("cmdParam: %p name- %s, required- %d, displayType- %s, requiredOn- %s",
                                //             cmdParam, cmdParam->m_name.c_str(), cmdParam->m_required, cmdParam->m_displayType.c_str(), cmdParam->m_requiredOn.c_str());
                                //     }
                                // }
                                lCommands.push_back(new CUpdateCommand(name, displayCategory, displayName, description, type, lcmdParams, displayOnProperty));
                            }
                            else
                            {
                                list<CUpdateCommandParam *> tmpLCommandParams;
                                list<list<CUpdateCommandParam *> >::iterator it;
                                for(it = lcmdParams.begin(); it != lcmdParams.end(); it++)
                                {
                                    list<CUpdateCommandParam *> lRowParams = *it;
                                    list<CUpdateCommandParam *>::iterator itR = lRowParams.begin();
                                    tmpLCommandParams.push_back(*itR);
                                }
                                lCommands.push_back(new CUpdateCommand(name, displayCategory, displayName, description, type, tmpLCommandParams, displayOnProperty));
                            }
                        }
                    }

                    // alarms
                    cJSON *alarmsJson = cJSON_GetObjectItem(moduleJson, DataTypes::alarms.c_str());
                    if (alarmsJson && !cJSON_IsArray(alarmsJson))
                    {
                        UTL_LOG_WARN("\"alarms\" should be an array. Terminated parsing json file.");
                        return NULL;
                    }
                    list<CUpdateAlarm *> lAlarms;
                    if (alarmsJson)
                    {
                        cJSON *alarmJson;
                        bool is2DArray = false;
                        cJSON_ArrayForEach(alarmJson, alarmsJson)
                        {
                            cJSON *paramsJson = cJSON_GetObjectItem(alarmJson, JKEY_PARAMS);
                            if (paramsJson && !cJSON_IsArray(paramsJson))
                            {
                                UTL_LOG_WARN("\"params\" of a alarm should be an array. Terminated parsing json file.");
                                return NULL;
                            }
                            list<list<CUpdateAlarmParam *> > lalarmParams;
                            if (paramsJson)
                            {
                                cJSON *paramJson;
                                cJSON_ArrayForEach(paramJson, paramsJson)
                                {
                                    list<CUpdateAlarmParam *> lalarmRowParams;
                                    is2DArray = cJSON_IsArray(paramJson);
                                    UTL_LOG_INFO("is2DArray: %d", is2DArray);
                                    if (is2DArray)
                                    {
                                        cJSON *paramRowJson;
                                        cJSON_ArrayForEach(paramRowJson, paramJson)
                                        {
                                            lalarmRowParams.push_back(GetUpdateAlarmParam(paramRowJson));
                                        }
                                    }
                                    else
                                    {
                                        lalarmRowParams.push_back(GetUpdateAlarmParam(paramJson));
                                    }
                                    lalarmParams.push_back(lalarmRowParams);
                                }
                            }
                            string name = CPluginUtil::GetJSONStringFieldValue(alarmJson, JKEY_NAME);
                            string displayCategory = CPluginUtil::GetJSONStringFieldValue(alarmJson, JKEY_DISPLAY_CATEGORY);
                            string displayName = CPluginUtil::GetJSONStringFieldValue(alarmJson, JKEY_DISPLAY_NAME);
                            string description = CPluginUtil::GetJSONStringFieldValue(alarmJson, JKEY_DESCRIPTION);
                            if (is2DArray)
                            {
                                // list<list<CUpdateAlarmParam *> >::iterator ita;
                                // for (ita = lalarmParams.begin(); ita != lalarmParams.end(); ita++)
                                // {
                                //     list<CUpdateAlarmParam *>::iterator itb;
                                //     for (itb = (*ita).begin(); itb != (*ita).end(); itb++)
                                //     {
                                //         CUpdateAlarmParam *almParam = *itb;
                                //         UTL_LOG_INFO("almParam: %p name- %s, required- %d, displayType- %s, requiredOn- %s",
                                //             almParam, almParam->m_name.c_str(), almParam->m_required, almParam->m_displayType.c_str(), almParam->m_requiredOn.c_str());
                                //     }
                                // }
                                lAlarms.push_back(new CUpdateAlarm(name, displayCategory, displayName, description, lalarmParams));
                            }
                            else
                            {
                                list<CUpdateAlarmParam *> tmpLAlarmParams;
                                list<list<CUpdateAlarmParam *> >::iterator it;
                                for(it = lalarmParams.begin(); it != lalarmParams.end(); it++)
                                {
                                    list<CUpdateAlarmParam *> lRowParams = *it;
                                    list<CUpdateAlarmParam *>::iterator itR = lRowParams.begin();
                                    tmpLAlarmParams.push_back(*itR);
                                }
                                lAlarms.push_back(new CUpdateAlarm(name, displayCategory, displayName, description, tmpLAlarmParams));
                            }
                        }
                    }

                    // configs
                    cJSON *configsJson = cJSON_GetObjectItem(moduleJson, DataTypes::configs.c_str());
                    if (configsJson && !cJSON_IsArray(configsJson))
                    {
                        UTL_LOG_WARN("\"configs\" should be an array. Terminated parsing json file.");
                        return NULL;
                    }
                    list<CUpdateConfig *> lConfigs;
                    if (configsJson)
                    {
                        cJSON *configJson;
                        bool is2DArray = false;
                        cJSON_ArrayForEach(configJson, configsJson)
                        {
                            cJSON *paramsJson = cJSON_GetObjectItem(configJson, JKEY_PARAMS);
                            if (paramsJson && !cJSON_IsArray(paramsJson))
                            {
                                UTL_LOG_WARN("\"params\" of a config should be an array. Terminated parsing json file.");
                                return NULL;
                            }
                            list <list<CUpdateConfigParam *> > lconfigParams;
                            if (paramsJson)
                            {
                                cJSON *paramJson;
                                cJSON_ArrayForEach(paramJson, paramsJson)
                                {
                                    list<CUpdateConfigParam *> lconfigRowParams;
                                    is2DArray = cJSON_IsArray(paramJson);
                                    if (is2DArray)
                                    {
                                        UTL_LOG_INFO("2D array");
                                        cJSON *paramRowJson;
                                        cJSON_ArrayForEach(paramRowJson, paramJson)
                                        {
                                            lconfigRowParams.push_back(GetUpdateConfigParam(paramRowJson));
                                        }
                                    }
                                    else
                                    {
                                        lconfigRowParams.push_back(GetUpdateConfigParam(paramJson));
                                    }
                                    lconfigParams.push_back(lconfigRowParams);
                                }
                            }
                            string name = CPluginUtil::GetJSONStringFieldValue(configJson, JKEY_NAME);
                            string displayCategory = CPluginUtil::GetJSONStringFieldValue(configJson, JKEY_DISPLAY_CATEGORY);
                            string displayName = CPluginUtil::GetJSONStringFieldValue(configJson, JKEY_DISPLAY_NAME);
                            string description = CPluginUtil::GetJSONStringFieldValue(configJson, JKEY_DESCRIPTION);
                            if (is2DArray)
                            {
                                // list<list<CUpdateConfigParam *> >::iterator ita;
                                // for (ita = lconfigParams.begin(); ita != lconfigParams.end(); ita++)
                                // {
                                //     list<CUpdateConfigParam *>::iterator itb;
                                //     for (itb = (*ita).begin(); itb != (*ita).end(); itb++)
                                //     {
                                //         CUpdateConfigParam *cfgParam = *itb;
                                //         UTL_LOG_INFO("cfgParam: %p name- %s, required- %d, displayType- %s, requiredOn- %s",
                                //             cfgParam, cfgParam->m_name.c_str(), cfgParam->m_required, cfgParam->m_displayType.c_str(), cfgParam->m_requiredOn.c_str());
                                //     }
                                // }
                                lConfigs.push_back(new CUpdateConfig(name, displayCategory, displayName, description, lconfigParams));
                            }
                            else
                            {
                                list<CUpdateConfigParam *> tmpLConfigParams;
                                list<list<CUpdateConfigParam *> >::iterator it;
                                for(it = lconfigParams.begin(); it != lconfigParams.end(); it++)
                                {
                                    list<CUpdateConfigParam *> lRowParams = *it;
                                    list<CUpdateConfigParam *>::iterator itR = lRowParams.begin();
                                    tmpLConfigParams.push_back(*itR);
                                }
                                lConfigs.push_back(new CUpdateConfig(name, displayCategory, displayName, description, tmpLConfigParams));
                            }
                        }
                    }
                    string moduleName = CPluginUtil::GetJSONStringFieldValue(moduleJson, JKEY_MODULE_NAME);
                    string displayName = CPluginUtil::GetJSONStringFieldValue(moduleJson, JKEY_DISPLAY_NAME);
                    string description = CPluginUtil::GetJSONStringFieldValue(moduleJson, JKEY_DESCRIPTION);
                    lModules.push_back(new CUpdateModule(moduleName, displayName, description,
                        lProperties, lStates, lMetrics, lEvents, lCommands, lAlarms, lConfigs));
                }
            }
            string appName = CPluginUtil::GetJSONStringFieldValue(updateParam, JKEY_APP_NAME);
            string displayName = CPluginUtil::GetJSONStringFieldValue(updateParam, JKEY_DISPLAY_NAME);
            string type = CPluginUtil::GetJSONStringFieldValue(updateParam, JKEY_TYPE);
            string version = CPluginUtil::GetJSONStringFieldValue(updateParam, JKEY_VERSION);
            string startCommand = CPluginUtil::GetJSONStringFieldValue(updateParam, JKEY_START_COMMAND);
            string stopCommand = CPluginUtil::GetJSONStringFieldValue(updateParam, JKEY_STOP_COMMAND);
            pluginData = new CUpdateParams(appGUID, appName, displayName, type, version, startCommand, stopCommand, lModules);
            return pluginData->GetNotifyPluginUpdate(apiVersion.c_str(), accessKey.c_str());
        }

        return NULL;
    }
    catch (const CPluginException& e)
    {
        UTL_LOG_ERROR("%s", e.what());
    }

    return NULL;
}

char *CPluginSample::SetNotifyCommandAcks(CCommandPluginJson *receivedCmds, string moduleName, string cmdState, cJSON *cmdAcks, cJSON *states)
{
    try {
        CCommandAcksPluginJson *cmdAcksObj = new CCommandAcksPluginJson();
        cmdAcksObj->SetCommandsPluginObject(receivedCmds);
        cmdAcksObj->SetApiVersion(apiVersion);
        cmdAcksObj->SetAppGUID(appGUID);
        if (ApiVersion::v2.compare(apiVersion) == 0) cmdAcksObj->SetAccessKey(accessKey);
        cmdAcksObj->SetModuleName(moduleName);

        cJSON *cmdAckParams;
        if (AckState::ACCEPTED.compare(cmdState) == 0) cmdAckParams = cmdAcksObj->CreateCommandAcksAcceptedParamsObj(cmdAcks);
        else if (AckState::ACKED.compare(cmdState) == 0) cmdAckParams = cmdAcksObj->CreateCommandAcksAckedParamsObj(cmdAcks, states);
        else if (AckState::REJECTED.compare(cmdState) == 0) cmdAckParams = cmdAcksObj->CreateCommandAcksRejectedParamsObj(cmdAcks);
        else if (AckState::ERRORED.compare(cmdState) == 0) cmdAckParams = cmdAcksObj->CreateCommandAcksErroredParamsObj(cmdAcks);
        else return NULL;
        cJSON *cmdAckJsonRpc = cmdAcksObj->GetJsonrpcRequest(cmdAckParams, minify);
        char *cmdAckJsonRpcString = minify? cJSON_PrintUnformatted(cmdAckJsonRpc) : cJSON_Print(cmdAckJsonRpc);
        cJSON_Delete(cmdAckJsonRpc);
        delete(cmdAcksObj);
        cmdAcksObj = NULL;

        return cmdAckJsonRpcString;
    }
    catch (const CPluginException& e)
    {
        UTL_LOG_ERROR("%s", e.what());
    }

    return NULL;
}

char *CPluginSample::SetNotifyStates(std::string moduleName, cJSON *states)
{
    try {
        CStatesPluginJson *statesObj = new CStatesPluginJson();
        statesObj->SetApiVersion(apiVersion);
        statesObj->SetAppGUID(appGUID);
        if (ApiVersion::v2.compare(apiVersion) == 0) statesObj->SetAccessKey(accessKey);
        statesObj->SetModuleName(moduleName);

        cJSON *stateJsonRpc = statesObj->GetJsonrpcRequest(statesObj->CreateStatesParamsObj(states), minify);
        char *stateJsonRpcString = minify? cJSON_PrintUnformatted(stateJsonRpc) : cJSON_Print(stateJsonRpc);
        cJSON_Delete(stateJsonRpc);
        delete(statesObj);

        return stateJsonRpcString;
    }
    catch (const CPluginException& e)
    {
        UTL_LOG_ERROR("%s", e.what());
    }

    return NULL;
}

char *CPluginSample::SetNotifyEvents(string moduleName, cJSON *events)
{
    try {
        CEventsPluginJson *eventsObj = new CEventsPluginJson();
        eventsObj->SetApiVersion(apiVersion);
        eventsObj->SetAppGUID(appGUID);
        if (ApiVersion::v2.compare(apiVersion) == 0) eventsObj->SetAccessKey(accessKey);
        eventsObj->SetModuleName(moduleName);

        cJSON *eventJsonRpc = eventsObj->GetJsonrpcRequest(eventsObj->CreateEventsParamsObj(events), minify);
        char *eventJsonRpcString = minify? cJSON_PrintUnformatted(eventJsonRpc) : cJSON_Print(eventJsonRpc);
        cJSON_Delete(eventJsonRpc);
        delete(eventsObj);

        return eventJsonRpcString;
    }
    catch (const CPluginException& e)
    {
        UTL_LOG_ERROR("%s", e.what());
    }

    return NULL;
}

char *CPluginSample::SetNotifyMetrics(string moduleName, cJSON *metrics)
{
    try {
        CMetricsPluginJson *metricsObj = new CMetricsPluginJson();
        metricsObj->SetApiVersion(apiVersion);
        metricsObj->SetAppGUID(appGUID);
        if (ApiVersion::v2.compare(apiVersion) == 0) metricsObj->SetAccessKey(accessKey);
        metricsObj->SetModuleName(moduleName);

        cJSON *metricJsonRpc = metricsObj->GetJsonrpcRequest(metricsObj->CreateMetricsParamsObj(metrics), minify);
        char *metricJsonRpcString = minify? cJSON_PrintUnformatted(metricJsonRpc) : cJSON_Print(metricJsonRpc);
        cJSON_Delete(metricJsonRpc);
        delete(metricsObj);

        return metricJsonRpcString;
    }
    catch (const CPluginException& e)
    {
        UTL_LOG_ERROR("%s", e.what());
    }

    return NULL;
}

char *CPluginSample::SetNotifyAlerts(string moduleName, cJSON *alarms)
{
    try {
        CAlertsPluginJson *alertsObj = new CAlertsPluginJson();
        alertsObj->SetApiVersion(apiVersion);
        alertsObj->SetAppGUID(appGUID);
        if (ApiVersion::v2.compare(apiVersion) == 0) alertsObj->SetAccessKey(accessKey);
        alertsObj->SetModuleName(moduleName);

        cJSON *alertJsonRpc = alertsObj->GetJsonrpcRequest(alertsObj->CreateAlarmsParamsObj(alarms), minify);
        char *alertJsonRpcString = minify? cJSON_PrintUnformatted(alertJsonRpc) : cJSON_Print(alertJsonRpc);
        cJSON_Delete(alertJsonRpc);
        delete(alertsObj);

        return alertJsonRpcString;
    }
    catch (const CPluginException& e)
    {
        UTL_LOG_ERROR("%s", e.what());
    }

    return NULL;
}

bool CPluginSample::IsAlarmEnabled(string moduleName, CAlarmUpdatePluginJson *alarmUpdate, string alarmName)
{
    bool enabled = false;

    if (alarmUpdate)
    {
        list<cJSON *> alarms = alarmUpdate->GetAlarms(moduleName);
        list<cJSON *>::iterator ita;
        for (ita = alarms.begin(); ita != alarms.end(); ita++)
        {
            if (alarmName.compare(CPluginUtil::GetJSONStringFieldValue(*ita, JKEY_NAME)) != string::npos)
            {
                enabled = CPluginUtil::GetJSONBooleanFieldValue(*ita, JKEY_ENABLED);
            }
        }
    }

    return enabled;
}

CUpdatePluginJson *CPluginSample::SetNotifyPluginUpdate()
{
    return NULL;
}

CLocalCommandPluginJson *CPluginSample::SetNotifyPluginLocalCommand()
{
    return NULL;
}

bool CPluginSample::AcceptReceivedCommand(cJSON *commandJson)
{
    return false;
}

string CPluginSample::ExecuteReceivedCommand(cJSON *commandJson, cJSON *cmdAck)
{
    string cmdState;
    return cmdState;
}