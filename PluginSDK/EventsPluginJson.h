#ifndef     EVENTSPLUGINJSON_H_
#define     EVENTSPLUGINJSON_H_

#include "BasePluginObject.h"


#define JKEY_EVENTS                 "events"

class PLUGIN_API CEventsPluginJson: public CBasePluginObject {
public:
    CEventsPluginJson();
    ~CEventsPluginJson();

    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted = true);
    cJSON *CreateEventsParamsObj(cJSON *eventsJson); 
    static cJSON *CreateEventsItemJson(const char *name, const char *value); 
};

#endif