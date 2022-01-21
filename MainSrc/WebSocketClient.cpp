#include "WebSocketClient.h"
#include "Log.h"
#include "../PluginSDK/PluginException.h"
#include "../PluginSDK/StatesPluginJson.h"
#include "../PluginSDK/EventsPluginJson.h"
#include "../PluginSDK/MetricsPluginJson.h"
#include "../PluginSDK/LocalCommandPluginJson.h"
#include "../Util/include/Utl_file.h"
#include "../Util/include/Utl_Log.h"
#include "../Util/include/vpl_error.h"
#include "ConcreteStates.h"
#include "../Plugins/TmatePlugin.h"

using namespace std;


bool needCheckStates = false;
bool needSendStates = false;
bool gotSigInt = false;

void SendNotifyPluginStates(CWebSocketClient *ptr, bitset<4> statesUpdated)
{
#ifdef DEBUG
    UTL_LOG_INFO("states updated? %s", statesUpdated.to_string<char,string::traits_type,string::allocator_type>().c_str());
#endif
    if (statesUpdated == StateUpdated::none) return;
    
    CTmatePlugin *plugin = (CTmatePlugin *) ptr->GetSamplePlugin();
    cJSON *states = cJSON_CreateArray();
    if ((statesUpdated & StateUpdated::version) == StateUpdated::version) cJSON_AddItemToArray(states, plugin->AddVersionState());
    if ((statesUpdated & StateUpdated::status) == StateUpdated::status) cJSON_AddItemToArray(states, plugin->AddStatusState());
    if ((statesUpdated & StateUpdated::web) == StateUpdated::web) cJSON_AddItemToArray(states, plugin->AddWebLinkState());
    if ((statesUpdated & StateUpdated::ssh) == StateUpdated::ssh) cJSON_AddItemToArray(states, plugin->AddSshLinkState());
    char *statesNotifyString = plugin->SetNotifyStates("tmateWebConsole", states);
    ptr->SendPluginNotify(ptr, statesNotifyString);
    free(statesNotifyString);
}

bool needSendCmdAcks = false;
void SendNotifyPluginCommandAcks(CWebSocketClient *ptr)
{
    if (!ptr) return;

    CTmatePlugin *plugin = (CTmatePlugin *) ptr->GetSamplePlugin();
    if (!plugin) return;

    CCommandPluginJson *receivedCommand = ptr->GetReceivedCommand();
    if (!receivedCommand) return;

    string moduleName = receivedCommand->GetModuleName();
    if (!plugin->GetModule(moduleName))
    {
        UTL_LOG_WARN("Cannot find the moduleName of this command.");
    }

    list<cJSON *> commands = receivedCommand->GetCommands();
    list<cJSON *>::iterator itc;
    for (itc=commands.begin(); itc!=commands.end(); itc++)
    {
        cJSON *command = *itc;
        string cmdName = CPluginUtil::GetJSONStringFieldValue(command, JKEY_NAME);
        map<string, string> params;
        cJSON *cmdParamsJson = cJSON_GetObjectItem(command, JKEY_PARAMS);
        if (cmdParamsJson && cJSON_IsArray(cmdParamsJson))
        {
            cJSON *cmdParamJson;
            cJSON_ArrayForEach(cmdParamJson, cmdParamsJson)
            {
                params.insert(pair<string, string>(CPluginUtil::GetJSONStringFieldValue(cmdParamJson, JKEY_NAME),
                    CPluginUtil::GetJSONStringFieldValue(cmdParamJson, JKEY_VALUE)));
            }
        }
        string reason;
        bool accepted = plugin->AcceptReceivedCommand(cmdName, params, reason);
        // UTL_LOG_INFO("=+=+=+ command %s, reason: %s", accepted? "Accepted" : "Rejected", reason.c_str());
        cJSON *cmdAcks = cJSON_CreateArray();
        cJSON *cmdAck = cJSON_CreateObject();
        cJSON_AddStringToObject(cmdAck, JKEY_NAME, cmdName.c_str());
        cJSON_AddStringToObject(cmdAck, JKEY_RESULT, reason.c_str());
        cJSON_AddItemToArray(cmdAcks, cmdAck);
        char *cmdAcksJsonRpcString = plugin->SetNotifyCommandAcks(receivedCommand, moduleName, accepted? AckState::ACCEPTED : AckState::REJECTED, cmdAcks);
        if (cmdAcksJsonRpcString == NULL) continue;
        ptr->SendPluginNotify(ptr, cmdAcksJsonRpcString);
        free(cmdAcksJsonRpcString);
        sleep(2);

        // run what accepted to execute and send 2nd cmdack afterword.
        if (accepted)
        {
            cJSON *execAck = cJSON_CreateObject();
            string execState = plugin->ExecuteReceivedCommand(cmdName, params, execAck);
            cJSON *execAcks = cJSON_CreateArray();
            cJSON_AddItemToArray(execAcks, execAck);
            char *execAcksJsonRpcString = plugin->SetNotifyCommandAcks(receivedCommand, moduleName, execState, execAcks);
            if (execAcksJsonRpcString == NULL)
            {
                ptr->ClearReceivedCommand();
                return;
            }
            ptr->SendPluginNotify(ptr, execAcksJsonRpcString);
            UTL_LOG_INFO("Acks Command #2: %s", execAcksJsonRpcString);
            free(execAcksJsonRpcString);
#ifdef TEST_STATES_METRICS_EVENTS
            needSendStates = true;
#endif
        }
    }
    ptr->ClearReceivedCommand();
}

static UTLTHREAD_FN_DECL ClientWorkThread(void* arg)
{
    CWebSocketClient *ptr = (CWebSocketClient*)arg;

    try {
        // Create a connection to the given URI and queue it for connection once
        // the event loop starts
        websocketpp::lib::error_code ec;
        UTL_LOG_INFO("==> start to connect to : %s", ptr->GetURL().c_str());
        WebClient::connection_ptr con = ptr->m_client.get_connection(ptr->GetURL(), ec);
        ptr->m_client.connect(con);

        // Start the ASIO io_service run loop
        ptr->m_client.run();
    } catch(const exception & e) {
        ptr->SetWebClientWork(false);
        ptr->SetException(e.what());
        if (ptr->GetSamplePlugin()) ptr->GetSamplePlugin()->GetUpdateData()->GetNotifyPluginUpdate()->SetUpdated(false);
        UTL_LOG_ERROR("Exception: %s\n", e.what());
        // UTL_LOG_INFO("is websocket alive: %d", ptr->IsWebClientWorking()); 
    } catch(websocketpp::lib::error_code e) {
        ptr->SetWebClientWork(false);
        UTL_LOG_ERROR("Exception: %s\n", e.message().c_str());
    } catch(...) {
        ptr->SetWebClientWork(false);
        UTL_LOG_ERROR("Other exception\n");
    }
    ptr->UpdateThreadHandle();
    ptr->NotifyDisconnectCondition();
    return 0;
}

void CWebSocketClient::On_open(void* c, websocketpp::connection_hdl hdl)
{
    retryConnectWebsocket = 0;
    currConnState = connection->getCurrentState();
    if (currConnState == &CAgentAlive::getInstance())
    {
        CAgentAlive* state = (CAgentAlive*)currConnState;
        state->setNewStateReason(CAgentAlive::WEBSOCKET_CONNECTED);
        connection->toggle();
    }
    CWebSocketClient *ptr = (CWebSocketClient*)c;
    
    ptr->m_pwebclient->pclient = &ptr->m_client;
    ptr->m_pwebclient->phd = hdl;
    UTL_LOG_INFO("Websocket connection established.");
    ptr->m_wsConnectionOpened = true;
}

void CWebSocketClient::On_fail(void* c, websocketpp::connection_hdl hdl)
{
    CWebSocketClient *ptr = (CWebSocketClient*)c;
    
    ptr->m_client.get_alog().write(websocketpp::log::alevel::app, "Connection Failed");
    ptr->m_client.close(hdl, websocketpp::close::status::normal, "Connection closed by server.");
    ptr->m_client.stop();
    ptr->m_threadstart = false;
    ptr->m_wsConnectionOpened = false;
    ptr->m_exception = "";
    ptr->m_retrySendUpdate = 0;
    ptr->m_retryWSConnection = 0;
    ptr->NotifyDisconnectCondition();
#ifdef TEST_UPDATE
    if (ptr->GetSamplePlugin()) ptr->GetSamplePlugin()->GetUpdateData()->GetNotifyPluginUpdate()->SetUpdated(false);
#endif
    currConnState = connection->getCurrentState();
    if (currConnState == &CAgentAlive::getInstance())
    {
        CAgentAlive* state = (CAgentAlive*)currConnState;
        state->setNewStateReason(CAgentAlive::WEBSOCKET_DISCONNECTED);
        connection->toggle();
    }
}

void CWebSocketClient::On_message(void* c, websocketpp::connection_hdl hdl, WebClient::message_ptr msg)
{
    CWebSocketClient *ptr = (CWebSocketClient*)c;
    const char *payload = msg->get_payload().c_str();
    UTL_LOG_INFO("Got ADM message:%s", payload);

    string msgType;
    char *error = CPluginUtil::GetMessageType(payload, &msgType);
    UTL_LOG_INFO("Message Type: %s", msgType.c_str());
    if (MessageType::error.compare(msgType) == 0) // Error Response
    {
        cJSON *errorJson = cJSON_Parse(error);
        UTL_LOG_WARN("Got error message: %s", error);
        if (errorJson) {
            string errCode = GetJSONStringFieldValue(errorJson, "code");
            string errMessage = GetJSONStringFieldValue(errorJson, "message");
            UTL_LOG_ERROR("Error code: %s, message: %s", errCode.c_str(), errMessage.c_str());
            int errCodeInt = abs(stoi(errCode));
            if (errCodeInt == 32000)
            {
                currConnState = connection->getCurrentState();
                if (currConnState == &CPluginRegistered::getInstance())
                {
                    CPluginRegistered* state = (CPluginRegistered*)currConnState;
                    state->setNewStateReason(CConnectionState::DEVICE_OFFLINE);
                    connection->toggle();
                }
#ifdef TEST_UPDATE
                UTL_LOG_INFO("Set waiting for send notifyPluginUpdate");
                if (ptr->GetSamplePlugin()) ptr->GetSamplePlugin()->GetUpdateData()->GetNotifyPluginUpdate()->SetUpdated(false);
#endif
            }
            else if (errCodeInt == 32600 || errCodeInt == 32601 || errCodeInt == 32602 || errCodeInt == 32603 || errCodeInt == 32604
                || errCodeInt == 32700)
            {
                // TODO: handle error - just do nothing
            }
            cJSON_Delete(errorJson);
        }
        return;
    }
#ifdef TEST_UPDATE
    else if (MessageType::api_notifyPluginCommand.compare(msgType) == 0) // notifyPluginCommand
    {
        CTmatePlugin *plugin = (CTmatePlugin *) ptr->GetSamplePlugin();
        if (!plugin) return;
        // Create CCommandPluginJson object for parsing received message of command
        CCommandPluginJson *receivedCommand = new CCommandPluginJson(payload, plugin->accessKey);
        // Check command validation before set to commandAcks object
        if (!receivedCommand->IsValidSignature())
        {
            UTL_LOG_WARN("Invalid command format.");
            delete(receivedCommand);
            return;
        }
        
        ptr->SetReceivedCommand(receivedCommand);
        needSendCmdAcks = true;

        return;
    }
    else if (MessageType::api_notifyPluginAlarmUpdate.compare(msgType) == 0) // notifyPluginAlarmUpdate
    {
        CTmatePlugin *plugin = (CTmatePlugin *) ptr->GetSamplePlugin();
        if (!plugin) return;
        plugin->UpdateAlarmsData(payload);

        return;
    }
#endif
    else if (MessageType::api_notifyAgentEvent.compare(msgType) == 0) // notifyAgentEvent
    {
        cJSON *serverConnectivity = cJSON_Parse(error);
        if (serverConnectivity != NULL)
        {
            UTL_LOG_INFO("notifyAgentEvent: %s", error);
            string connectivity = GetJSONStringFieldValue(serverConnectivity, MessageType::agentEventServerConnectivity);
            UTL_LOG_INFO("connectivity: %s", connectivity.c_str());
            if (MessageType::offline.compare(connectivity) == 0)
            {
                // switch state to serverOffline
                UTL_LOG_INFO("Server is offline.");
                currConnState = connection->getCurrentState();
                if (currConnState == &CPluginRegistered::getInstance())
                {
                    CPluginRegistered* state = (CPluginRegistered*)currConnState;
                    state->setNewStateReason(CConnectionState::SERVER_OFFLINE);
                    connection->toggle();
                    currConnState = connection->getCurrentState();
                    if (currConnState == &CServerOffline::getInstance())
                    {
                        CServerOffline* state = (CServerOffline*)currConnState;
                        state->setLastState(CPluginRegistered::getInstance());
                    }
                }
                else if (currConnState == &CWebsocketConnected::getInstance())
                {
                    CWebsocketConnected* state = (CWebsocketConnected*)currConnState;
                    state->setNewStateReason(CConnectionState::SERVER_OFFLINE);
                    connection->toggle();
                    currConnState = connection->getCurrentState();
                    if (currConnState == &CServerOffline::getInstance())
                    {
                        CServerOffline* state = (CServerOffline*)currConnState;
                        state->setLastState(CWebsocketConnected::getInstance());
                    }
                }
            }
            else if (MessageType::online.compare(connectivity) == 0)
            {
                // switch to previous state
                UTL_LOG_INFO("Server is online.");
                currConnState = connection->getCurrentState();
                if (currConnState == &CServerOffline::getInstance())
                {
                    CServerOffline* state = (CServerOffline*)currConnState;
                    connection->toggle();
                }
            }
        }
        return;
    }
    else if (MessageType::others.compare(msgType) == 0)
    {
        return;            
    }
    if (error)  free(error);
}

void CWebSocketClient::On_close(void* c, websocketpp::connection_hdl hdl)
{
    CWebSocketClient *ptr = (CWebSocketClient*)c;
    
    UTL_LOG_INFO("The server is shutdown! Disconnect connection\n");

    ptr->m_client.close(hdl, websocketpp::close::status::normal, "Connection closed by server.");
    ptr->m_client.stop();
    ptr->m_threadstart = false;
    ptr->m_wsConnectionOpened = false;
    ptr->m_exception = "";
    ptr->m_retrySendUpdate = 0;
    ptr->m_retryWSConnection = 0;
    ptr->NotifyDisconnectCondition();
#ifdef TEST_UPDATE
    if (ptr->GetSamplePlugin()) ptr->GetSamplePlugin()->GetUpdateData()->GetNotifyPluginUpdate()->SetUpdated(false);
#endif
    currConnState = connection->getCurrentState();
    if (currConnState == &CPluginRegistered::getInstance())
    {
        CPluginRegistered* state = (CPluginRegistered*)currConnState;
        state->setNewStateReason(CPluginRegistered::WEBSOCKET_DISCONNECTED);
        connection->toggle();
    }
}

ContextPtr CWebSocketClient::On_tls_init() {
	ContextPtr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

	try {
		ctx->set_options(boost::asio::ssl::context::default_workarounds |
			boost::asio::ssl::context::no_sslv2 |
			boost::asio::ssl::context::no_sslv3 |
			boost::asio::ssl::context::single_dh_use);

		ctx->set_verify_mode(boost::asio::ssl::verify_none);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return ctx;
}

int retryTimes = 0;

#ifdef TEST_UPDATE
long updateEpoch;
static UTLTHREAD_FN_DECL NotifyUpdateThread(void* arg)
{
    CWebSocketClient *ptr = (CWebSocketClient*)arg;
    if (ptr)
    {
        if (!ptr->GetSamplePlugin())
        {
            UTL_LOG_WARN("notifyPluginUpdate data isn't avaliable. Exit NotifyUpdateThread...");
            return 0;
        }

        CUpdatePluginJson *updatePluginObj = ptr->GetSamplePlugin()->GetUpdateData()->GetNotifyPluginUpdate();
        // UTL_LOG_INFO("plugin update obj: %p", updatePluginObj);
        int retry = 0;
        while(ptr->IsWebClientWorking())
        {
            sleep(0);
            // UTL_LOG_INFO("1. retry %d, is connection opened? %d", retry, ptr->IsConnectionOpened());
            if (ptr->IsConnectionOpened())
            {
                currConnState = connection->getCurrentState();
                // UTL_LOG_INFO("connectionState: %d", currConnState->getCurrentStateReason());
                if (currConnState == &CWebsocketConnected::getInstance() || currConnState == &CResendPlugin::getInstance())
                {
                    UTL_LOG_INFO("WebsocketConnected | ResendPlugin");
                    ptr->SendNotifyPluginUpdate();
#ifdef TEST_STATES_METRICS_EVENTS
                    if (updatePluginObj && updatePluginObj->IsUpdated())
                    {
                        needCheckStates = true;
                    }
#endif
                }
                else if (currConnState == &CDeviceOffline::getInstance())
                {
                    int delay = CWebSocketClient::ExponentialRetryPause(++retryTimes);
                    UTL_LOG_INFO("DeviceOffline: sleep %d secs in the %d times.", delay, retryTimes);
                    sleep(delay);
                    connection->toggle();
                }
            }
            else
            {
                UTL_LOG_INFO("Waiting for WebSocket connection to open...%d", retry);
                sleep(CWebSocketClient::ExponentialRetryPause(retry++));
            }

            if (updatePluginObj && updatePluginObj->IsUpdated())
            {
                // UTL_LOG_INFO("loop... 3sec");
                sleep(3);
            }
        };
        ptr->ResetUpdateThreadHandle();
    }
    UTL_LOG_WARN("The thead named \"NotifyUpdateThread\" is end");
    return 0;
}

static UTLTHREAD_FN_DECL NotifyCommandThread(void* arg)
{
    CWebSocketClient *ptr = (CWebSocketClient*)arg;
    CTmatePlugin *plugin = (CTmatePlugin *) ptr->GetSamplePlugin();
    if (ptr && plugin) {
        CUpdatePluginJson *updatePluginObj = plugin->GetUpdateData()->GetNotifyPluginUpdate();
        UTL_LOG_INFO("NotifyCommandThread running");
        while (ptr->IsWebClientWorking()) {
            sleep(0); //For release CPU resource not be blocked by this thread.
            if (updatePluginObj->IsUpdated()) {
                if (needSendCmdAcks)
                {
                    SendNotifyPluginCommandAcks(ptr);
                    needSendCmdAcks = false;
                }
            }
            sleep(3);
        }
        ptr->ResetCommandThreadHandle();
        UTL_LOG_WARN("The thead named \"NotifyCommandThread\" is end");
    }
    return 0;
}

void CWebSocketClient::SendNotifyPluginUpdate()
{
    UTL_LOG_INFO("NotifyPluginUpdate()");
    if (!GetSamplePlugin())
    {
        UTL_LOG_WARN("Unknown plugin object.");
        return;
    }

    CUpdatePluginJson *updateJsonObject = GetSamplePlugin()->GetUpdateData()->GetNotifyPluginUpdate();
    if (updateJsonObject != NULL) {
        currConnState = connection->getCurrentState();
        if (currConnState == &CWebsocketConnected::getInstance() || currConnState == &CResendPlugin::getInstance())
        {
            bool minify = GetSamplePlugin()->minify;
            cJSON *updateJson = updateJsonObject->RenewUpdateJsonObject(minify);
            char *line = minify? cJSON_PrintUnformatted(updateJson) : cJSON_Print(updateJson);
            if (SendPluginNotify(this, line)) updateJsonObject->SetUpdated(true);
            // UTL_LOG_INFO("Notify plugin Update: %s", line);
            free(line);
            CWebsocketConnected* state = (CWebsocketConnected*)currConnState;
            connection->toggle();
        }
        else UTL_LOG_INFO("Not the right state to send notifyPluginUpdate...");
    }
    else
    {
        UTL_LOG_WARN("The JSON object for notifyPluginUpdate API isn't ready.");
    }
}

bool CWebSocketClient::SendPluginNotify(CWebSocketClient *ptr, const char *notify)
{
    // send notify
    if (ptr && notify)
    {
        websocketpp::lib::error_code ec;
        ptr->m_pwebclient->pclient->send(ptr->m_pwebclient->phd, notify, websocketpp::frame::opcode::TEXT, ec);
        UTL_LOG_INFO("Notify: %s", notify);

        if (ec)
        {
            UTL_LOG_ERROR("Error code: %s", ec.message().c_str());
            return false;
        }
        return true;
    }
    else return false;
}
#endif

#ifdef TEST_STATES_METRICS_EVENTS
static UTLTHREAD_FN_DECL NotifyDataThread(void* arg)
{
    CWebSocketClient *ptr = (CWebSocketClient*)arg;
    CTmatePlugin *plugin = (CTmatePlugin *) ptr->GetSamplePlugin();
    if (ptr && plugin)
    {
        CUpdatePluginJson *updatePluginObj = plugin->GetUpdateData()->GetNotifyPluginUpdate();
        UTL_LOG_INFO("NotifyDataThread running");
        int stateCount = 0;
        while (ptr->IsWebClientWorking()) {
            sleep(0); //For release CPU resource not be blocked by this thread.
            if (updatePluginObj->IsUpdated())
            {
                bitset<4> fileChanges;
                if (stateCount % 20 == 1 || needCheckStates) // check if any changes of states output files
                {
                    // Run states' scripts to see if there're changes need to be notified.
                    bitset<4> updateMask;
                    if (stateCount == 1) updateMask.set();
                    else updateMask.set().set(1,0);
#ifdef DEBUG
                    UTL_LOG_INFO("updateMask = %s", updateMask.to_string<char,std::string::traits_type,std::string::allocator_type>().c_str());
#endif
                    plugin->UpdateStates(updateMask);

                    // Check if output files changes?
                    fileChanges = plugin->IsStateFilesChanged();
                    if (fileChanges != StateUpdated::none)
                    {
                        needSendStates = true;
                    }
                    needCheckStates = false;
                }

                if (gotSigInt) // Update the latest states status and exit the program.
                {
                    SendNotifyPluginStates(ptr, fileChanges);
                }
                else if (needSendStates) // Send notify for changed states.
                {
                    SendNotifyPluginStates(ptr, fileChanges);
                    needSendStates = false;
                }
            }
            if (gotSigInt)
            {
                UTL_LOG_INFO("going to exit program...");
                ptr->SetEndWebSocket(true);
            }
            sleep(3);
            stateCount++;
            if (stateCount == INT32_MAX) stateCount = 0;
        }
        ptr->ResetDataThreadHandle();
        UTL_LOG_WARN("The thead named \"NotifyDataThread\" is end");
    }
    return 0;
}
#endif

CWebSocketClient::CWebSocketClient(string ipaddr, string port)
{
    m_endWebSocket = false;
    m_url.append("wss://");
    m_url.append(ipaddr);
    m_url.append(":");
    m_url.append(port);
    UTL_LOG_INFO("==> set url as %s", m_url.c_str());
    m_threadstart = false;
    m_wsConnectionOpened = false;
    m_exception = "";
    m_pwebclient = new CWEBCLIENTPARAM();
    m_recvlist = NULL;
    m_threadHandle.handle = -1;
    UTLCond_Init(&m_NotifyCMDcond);
    UTLMutex_Init(&m_NotifyCMDmutex);
#ifdef TEST_UPDATE
    m_receivedCommand = NULL;
    m_threadUpdateHandle.handle = -1;
    m_threadCommandHandle.handle = -1;
#endif
#ifdef TEST_STATES_METRICS_EVENTS
    m_threadDataHandle.handle = -1;
#endif
    m_plugin = NULL;
    m_retryWSConnection = 0;
    m_retrySendUpdate = 0;
}

CWebSocketClient::~CWebSocketClient()
{
    if (m_pwebclient) delete(m_pwebclient);
    m_client.stop();
    m_threadstart = false;
    UTLCond_Destroy(&m_NotifyCMDcond);
    UTLMutex_Destroy(&m_NotifyCMDmutex);
}

void CWebSocketClient::Initial()
{
    try {
        m_client.set_access_channels(websocketpp::log::alevel::all);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload|websocketpp::log::alevel::frame_header|websocketpp::log::alevel::control);
        m_client.init_asio();
        m_client.set_reuse_addr(true);

        // Register our handlers
        m_client.set_open_handler(bind(&On_open,this,placeholders::_1));
        m_client.set_fail_handler(bind(&On_fail,this,placeholders::_1));
        m_client.set_message_handler(bind(&On_message,this,placeholders::_1,placeholders::_2));
        m_client.set_close_handler(bind(&On_close, this,placeholders::_1));
        m_client.set_tls_init_handler(bind(&On_tls_init));
    } catch(const exception & e) {
        UTL_LOG_ERROR("Exception: %s\n", e.what());
    } catch(websocketpp::lib::error_code e) {
        UTL_LOG_ERROR("Exception: %s\n", e.message().c_str());
    } catch(...) {
        UTL_LOG_ERROR("Other exception\n");
    }
}

bool CWebSocketClient::StartWebClient()
{
    bool retval = true;
    int rv;
    UTLThread_attr_t threadAttrs;

    if (m_threadHandle.handle == -1) {
        m_threadstart = true;
        rv = UTLThread_AttrInit(&threadAttrs);
        if (rv < 0) {
            UTL_LOG_ERROR("VPLThread_AttrInit returned %d", rv);
            m_threadstart = retval = false;
            goto end;
        } 
        rv = UTLThread_AttrSetStackSize(&threadAttrs, UTLTHREAD_STACKSIZE_MIN + (4*1024));
        if (rv < 0) {
            UTL_LOG_ERROR("VPLThread_AttrSetStackSize returned %d", rv);
            m_threadstart = retval = false;
            goto end;
        }
        rv = UTLDetachableThread_Create(&m_threadHandle, ClientWorkThread, this, &threadAttrs, NULL);
        if (rv < 0) {
            UTL_LOG_ERROR("VPLDetachableThread_Create returned %d", rv);
            m_threadstart = retval = false;
            goto end;
        }
#ifdef TEST_UPDATE
        rv = UTLDetachableThread_Create(&m_threadUpdateHandle, NotifyUpdateThread, this, &threadAttrs, NULL);
        if (rv < 0) {
            UTL_LOG_ERROR("VPLDetachableThread_Create returned %d", rv);
            m_threadstart = retval = false;
            goto end;
        }

        rv = UTLDetachableThread_Create(&m_threadCommandHandle, NotifyCommandThread, this, &threadAttrs, NULL);
        if (rv < 0) {
            UTL_LOG_ERROR("VPLDetachableThread_Create returned %d", rv);
            m_threadstart = retval = false;
            goto end;
        }
#endif
#ifdef TEST_STATES_METRICS_EVENTS
        rv = UTLDetachableThread_Create(&m_threadDataHandle, NotifyDataThread, this, &threadAttrs, NULL);
        if (rv < 0) {
            UTL_LOG_ERROR("VPLDetachableThread_Create returned %d", rv);
            m_threadstart = retval = false;
            goto end;
        }
#endif
    }
end:
    UTLThread_AttrDestroy(&threadAttrs);
    return retval;
}

int CWebSocketClient::ExponentialRetryPause(int retryTimes)
{
    if (retryTimes == 0) return 1;
    int base = 0;
    while ((retryTimes = retryTimes/2) >= 1 && base < 6) {
        base++;
    };
    UTL_LOG_INFO ("base = %d", base);
    return (base+1)*10;
}

void CWebSocketClient::SignalHandler(int signal) {
    const char *signal_name;
    sigset_t pending;

    // Find out which signal we're handling
    switch (signal) {
        case SIGINT:
            UTL_LOG_INFO("Caught SIGINT, exiting now\n");
            gotSigInt = true;
            break;

        default:
            fprintf(stderr, "Caught wrong signal: %d\n", signal);
            return;
    }
}

bool CWebSocketClient::IsErrorMessage(const char* command)
{
    bool ret = true;
    cJSON *rcmd = cJSON_Parse(command);
    if (rcmd != NULL) {
        cJSON *rcmdparam = cJSON_GetObjectItem(rcmd, JKEY_ERROR);
        if (rcmdparam == NULL) {
            ret = false;
        }
    }
    cJSON_Delete(rcmd);

    return ret;
}

void CWebSocketClient::NotifyDisconnectCondition() 
{
    UTLCond_Signal(&m_NotifyCMDcond);
}

void CWebSocketClient::UpdateThreadHandle()
{
    m_threadHandle.handle = -1;
}
