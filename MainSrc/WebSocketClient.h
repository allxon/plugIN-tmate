#ifndef     _WEBSOCKETCLIENT_H_
#define     _WEBSOCKETCLIENT_H_

#include "../websocket/websocketpp/config/asio_client.hpp"
#include "../websocket/websocketpp/client.hpp"
#include "../Util/include/Utl_thread.h"
#include "../Util/include/Utl_mutex.h"
#include "../PluginSDK/BasePluginObject.h"
#include "../PluginSDK/UpdatePluginJson.h"
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
#define AGENT_WSS_PORT      "55688"


typedef websocketpp::client<websocketpp::config::asio_tls_client> WebClient;
typedef std::shared_ptr<boost::asio::ssl::context> ContextPtr;

class CWEBCLIENTPARAM {
public:
    WebClient *pclient;
    websocketpp::connection_hdl phd;
};

class CWebSocketClient: public CPluginUtil {
public:
    CWebSocketClient(std::string ipaddr=IP_LOCALHOST, std::string port=AGENT_WSS_PORT);
    ~CWebSocketClient();

    void Initial();
    bool IsErrorMessage(const char* command);
    bool StartWebClient();
    bool IsWebClientWorking() { return m_threadstart; }
    void SetWebClientWork(bool threadStart) { m_threadstart = threadStart; }
    bool IsConnectionOpened() { return m_wsConnectionOpened; }
    bool IsEndWebSocket() { return m_endWebSocket; }
    void SetEndWebSocket(bool setEnd) { m_endWebSocket = setEnd; }
    std::string GetURL() { return m_url; }
    void NotifyDisconnectCondition();
    void UpdateThreadHandle();
    pthread_t GetThreadHandle() { return m_threadHandle.handle; }
    UTLMutex_t *GetNotifyMutex() { return &m_NotifyCMDmutex; }
    UTLCond_t *GetNotifyCondition() { return &m_NotifyCMDcond; }
    std::string GetException() { return m_exception; }
    void SetException(std::string exceptionMsg) { m_exception = exceptionMsg; }

    static void On_open(void* c, websocketpp::connection_hdl hdl);
    static void On_fail(void* c, websocketpp::connection_hdl hdl);
    static void On_message(void* c, websocketpp::connection_hdl hdl, WebClient::message_ptr msg);
    static void On_close(void* c, websocketpp::connection_hdl hdl);
    static ContextPtr On_tls_init();
    static int ExponentialRetryPause(int retryTimes);
    static void SignalHandler(int signal);

#ifdef TEST_UPDATE
    CPluginSample *GetSamplePlugin() { return m_plugin; }
    void SetSamplePlugin(CPluginSample *plugin) { m_plugin = plugin; }
    bool SendPluginNotify(CWebSocketClient *ptr, const char *notify);
    void SendNotifyPluginUpdate();
    CCommandPluginJson *GetReceivedCommand() { return m_receivedCommand; }
    void SetReceivedCommand(CCommandPluginJson *receivedCommand) { m_receivedCommand = receivedCommand; }
    void ClearReceivedCommand() { delete(m_receivedCommand); m_receivedCommand = NULL; }

    void ResetUpdateThreadHandle() { m_threadUpdateHandle.handle = -1; }
    pthread_t GetUpdateThreadHandle() { return m_threadUpdateHandle.handle; }
    void ResetCommandThreadHandle() { m_threadCommandHandle.handle = -1; }
    pthread_t GetCommandThreadHandle() { return m_threadCommandHandle.handle; }
#endif
#ifdef TEST_STATES_METRICS_EVENTS
    void ResetDataThreadHandle() { m_threadDataHandle.handle = -1; }
    pthread_t GetDateThreadHandle() { return m_threadDataHandle.handle; }
#endif

public:
    WebClient m_client;
    CWEBCLIENTPARAM *m_pwebclient;
    int m_retryWSConnection;
    int m_retrySendUpdate;

private:
    std::string m_url;
    bool m_threadstart;
    bool m_wsConnectionOpened;
    UTLDetachableThreadHandle_t m_threadHandle;
    std::list<std::string> *m_recvlist;
    UTLMutex_t m_NotifyCMDmutex;
    UTLCond_t m_NotifyCMDcond;
    std::string m_exception;
    bool m_endWebSocket;

#ifdef TEST_UPDATE
    CPluginSample *m_plugin;
    CCommandPluginJson *m_receivedCommand;
    UTLDetachableThreadHandle_t m_threadUpdateHandle;
    UTLDetachableThreadHandle_t m_threadCommandHandle;
#endif
#ifdef TEST_STATES_METRICS_EVENTS
    UTLDetachableThreadHandle_t m_threadDataHandle;
#endif
};

#endif
