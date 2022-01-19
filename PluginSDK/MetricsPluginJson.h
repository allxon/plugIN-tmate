#ifndef     METRICSPLUGINJSON_H_
#define     METRICSPLUGINJSON_H_

#include "BasePluginObject.h"


#define JKEY_METRICS                "metrics"

/**
 *  @brief CMetricsPluginJson generates a JSON request that conform to the format for notifyPluginMetric API.
 */
class PLUGIN_API CMetricsPluginJson: public CBasePluginObject {
public:
    CMetricsPluginJson();
    ~CMetricsPluginJson();

    cJSON *GetJsonrpcRequest(cJSON *paramJson);
    cJSON *GetJsonrpcRequest(cJSON *paramJson, bool unformatted);
    cJSON *CreateMetricsParamsObj(cJSON *metricsJson); 
    static cJSON *CreateMetricsItemJson(const char *name, double value);
};

#endif