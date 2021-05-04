#ifndef     _WEBSOCKETCLIENT_H_
#define     _WEBSOCKETCLIENT_H_

#include "../websocket/websocketpp/config/asio_no_tls_client.hpp"
#include "../websocket/websocketpp/client.hpp"
#include "../Util/include/Utl_thread.h"
#include "../Util/include/Utl_mutex.h"
#include "../Util/include/BasePluginObject.h"
#include "../Util/include/UpdatePluginJson.h"
#include "../Util/include/CommandAcksPluginJson.h"
#include "../Util/include/AlertsPluginJson.h"
#include "../Plugins/PluginSample.h"
#include "Connection.h"

extern CConnection* connection;
extern CConnectionState* currConnState;
extern int retryConnectWebsocket;


#define TEST_UPDATE
#ifdef TEST_UPDATE
#define TEST_STATES_METRICS_EVENTS
#endif

#define IP_LOCALHOST        "127.0.0.1"
#define AGENT_WS_PORT       "5566"
#ifdef _ARM_PLATFORM_
#define PLUGIN_WORKING_FILE "/mnt/user/SyncAgent/pluginstate"
#endif


typedef websocketpp::client<websocketpp::config::asio_client> WebClient;


class CWEBCLIENTPARAM {
public:
    WebClient *pclient;
    websocketpp::connection_hdl phd;
};

class CWebSocketClient: public CPluginUtil {
public:
    CWebSocketClient(std::string ipaddr=IP_LOCALHOST, std::string port=AGENT_WS_PORT);
    ~CWebSocketClient();

    void Initial();
    bool IsErrorMessage(const char* command);
    bool StartWebClient();
    bool WebClientIsAlive() { return m_threadstart; }
    bool IsEndWebSocket() { return m_endWebSocket; }
    void SetEndWebSocket(bool setEnd) { m_endWebSocket = setEnd; }
    std::string GetException() { return m_exception; }
    std::string GetURL() { return m_url; }
    void NotifyDisconnectCondition();
    void UpdateThreadHandle();
    pthread_t GetThreadHandle() { return m_threadHandle.handle; }
    UTLMutex_t *GetNotifyMutex() { return &m_NotifyCMDmutex; }
    UTLCond_t *GetNotifyCondition() { return &m_NotifyCMDcond; }

    static void On_open(void* c, websocketpp::connection_hdl hdl);
    static void On_fail(void* c, websocketpp::connection_hdl hdl);
    static void On_message(void* c, websocketpp::connection_hdl hdl, WebClient::message_ptr msg);
    static void On_close(void* c, websocketpp::connection_hdl hdl);
    static int ExponentialRetryPause(int retryTimes);
    static void SignalHandler(int signal);
#ifdef _ARM_PLATFORM_
    static void ClearPluginState();
#endif

#if defined(TEST_UPDATE) || defined(TEST_LOCAL_COMMANDS)
    CPluginSample *GetSamplePlugin() { return m_plugin; }
    void SetSamplePlugin(CPluginSample *plugin) { m_plugin = plugin; }
    bool SendPluginNotify(CWebSocketClient *ptr, const char *notify);
#endif

#ifdef TEST_UPDATE
    void SendNotifyPluginUpdate();
    CCommandPluginJson *GetReceivedCommand() { return m_receivedCommand; }
    void SetReceivedCommand(CCommandPluginJson *receivedCommand) { m_receivedCommand = receivedCommand; }
    void ClearReceivedCommand() { delete(m_receivedCommand); m_receivedCommand = NULL; } 
    char *TestSDKSendCommandAcks(bool afterAccepted, CCommandPluginJson *receivedCommand);
    void TestSDKUpdateAlarms(const char *payload);
    void SetAlarmUpdateObject(CAlarmUpdatePluginJson *alarmUpdateObj) { m_alarmUpdateObj = alarmUpdateObj; }
    CAlarmUpdatePluginJson *GetAlarmUpdateObject() { return m_alarmUpdateObj; }

    void ResetUpdateThreadHandle() { m_threadUpdateHandle.handle = -1; }
    pthread_t GetUpdateThreadHandle() { return m_threadUpdateHandle.handle; }
    void ResetCommandThreadHandle() { m_threadCommandHandle.handle = -1; }
    pthread_t GetCommandThreadHandle() { return m_threadCommandHandle.handle; }
#endif
#ifdef TEST_STATES_METRICS_EVENTS
    void ResetDataThreadHandle() { m_threadDataHandle.handle = -1; }
    pthread_t GetDateThreadHandle() { return m_threadDataHandle.handle; }
#endif

#ifdef TEST_LOCAL_COMMANDS
    void SendNotifyPluginLocalCommand();
    char *TestSDKSendLocalCommand();
#endif

public:
    bool m_threadstart;
    bool m_wsConnectionOpened;
    std::string m_exception;
    WebClient m_client;
    CWEBCLIENTPARAM *m_pwebclient;
    int m_retryWSConnection;
    int m_retrySendUpdate;

private:
    std::string m_url;
    UTLDetachableThreadHandle_t m_threadHandle;
    std::list<std::string> *m_recvlist;
    UTLMutex_t m_NotifyCMDmutex;
    UTLCond_t m_NotifyCMDcond;
    bool m_endWebSocket;

#if defined(TEST_UPDATE) || defined(TEST_LOCAL_COMMANDS)
    CPluginSample *m_plugin;
#endif

#ifdef TEST_UPDATE
    CCommandPluginJson *m_receivedCommand;
    CAlarmUpdatePluginJson *m_alarmUpdateObj;
    UTLDetachableThreadHandle_t m_threadUpdateHandle;
    UTLDetachableThreadHandle_t m_threadCommandHandle;
#endif

#ifdef TEST_STATES_METRICS_EVENTS
    UTLDetachableThreadHandle_t m_threadDataHandle;
#endif
};

#endif
