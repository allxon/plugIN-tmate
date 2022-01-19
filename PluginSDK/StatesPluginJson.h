#ifndef     STATESPLUGINJSON_H_
#define     STATESPLUGINJSON_H_

#include "BasePluginObject.h"


#define JKEY_STATES                 "states"

/**
 *  @brief CStatesPluginJson generates a JSON request that conform to the format for notifyPluginState API.
 */
class PLUGIN_API CStatesPluginJson: public CBasePluginObject {
public:
    CStatesPluginJson();
    ~CStatesPluginJson();

    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted);
    cJSON *CreateStatesParamsObj(cJSON *statesJson);
    static cJSON *CreateStatesItemJson(const char *name, const char *value);
    static cJSON *CreateStatesItemJson(const char *name, cJSON *value);
};

#endif