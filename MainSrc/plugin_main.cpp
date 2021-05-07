#include "Log.h"
#include <dirent.h>
#include <typeinfo>
#include <csignal>
#include "Connection.h"
#include "ConcreteStates.h"
#include "WebSocketClient.h"
#include "../Util/include/Utl_Log.h"
#include "../Util/include/UpdatePluginJson.h"
#include "../Util/include/PluginException.h"
#include "../Util/include/CommandAcksPluginJson.h"
#include "../Plugins/TmatePlugin.h"

using namespace std;

#define DEBUG
// #define TEST_WEBSOCKET_ONLY
#define LAUNCH_AGENT
#define BDM_AGENT "BDM_Agent"
#define PID_FILE "/var/run/tmatePlugin.pid"

CConnection* connection;
CConnectionState* currConnState;

int retryConnectWebsocket = 0;

void registerSigHandler()
{
    struct sigaction sa;

    sa.sa_handler = &CWebSocketClient::SignalHandler;
    sa.sa_flags = SA_RESTART;
    sigfillset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error: cannot handle SIGINT"); // Should not happen
    }
}

int fdlock;
int getLock(void)
{
  struct flock fl;

  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 1;

  if((fdlock = open(PID_FILE, O_WRONLY|O_CREAT, 0666)) == -1)
    return 0;

  if(fcntl(fdlock, F_SETLK, &fl) == -1)
    return 0;

  return 1;
}

string getCurrentDate()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[32];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y",timeinfo);

    return string(buffer);
}

int getProcIdByName(string procName)
{
    int pid = -1;

    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != NULL)
    {
        // Enumerate all entries in directory until process found
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp)))
        {
            // Skip non-numeric entries
            int id = atoi(dirp->d_name);
            if (id > 0)
            {
                // Read contents of virtual /proc/{pid}/cmdline file
                string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
                ifstream cmdFile(cmdPath.c_str());
                string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(pos + 1);
                    // Compare against requested process name
                    if (procName == cmdLine)
                        pid = id;
                }
            }
        }
    }
    closedir(dp);

    return pid;
}

#ifdef LAUNCH_AGENT
bool agentLaunchedByPlugin = false;
int launchAgent()
{
    int psId = getProcIdByName(BDM_AGENT);
    UTL_LOG_INFO("1. Agent psId: %d", psId);
    currConnState = connection->getCurrentState();
    if (currConnState == &CInit::getInstance())
    {
        CInit* state = (CInit*)currConnState;
        state->setNewStateReason(psId > 0? CInit::AGENT_ALIVE : CInit::AGENT_DISABLED);
        connection->toggle();
    }
    else if (currConnState == &CWebsocketDisconnected::getInstance())
    {
        CWebsocketDisconnected* state = (CWebsocketDisconnected*)currConnState;
        if (psId <= 0) state->setNewStateReason(psId > 0? CInit::AGENT_ALIVE : CInit::AGENT_DISABLED);
        connection->toggle();
    }
    if (psId == -1)
    {
        agentLaunchedByPlugin = true;
        system("BDM_Agent -d -o &"); // TODO: add checking the status value of system call for Agent.
        psId = getProcIdByName(BDM_AGENT);
        UTL_LOG_INFO("2. Agent psId: %d", psId);
        currConnState = connection->getCurrentState();
        if (currConnState == &CAgentDisabled::getInstance())
        {
            CAgentDisabled* state = (CAgentDisabled*)currConnState;
            state->setNewStateReason(psId > 0? CAgentDisabled::AGENT_ALIVE : CAgentDisabled::AGENT_DISABLED);
            connection->toggle();
        }
    }
    return psId;
}
#endif

#ifdef TEST_WEBSOCKET_ONLY
void On_open(void* c, websocketpp::connection_hdl hdl)
{
    WebClient *ptr = (WebClient *)c;
    UTL_LOG_INFO("Websocket connection established.");
}

void On_fail(void* c, websocketpp::connection_hdl hdl)
{
    UTL_LOG_INFO("on_fail");
    WebClient *ptr = (WebClient *)c;
    ptr->get_alog().write(websocketpp::log::alevel::app, "Connection Failed");
    ptr->close(hdl, websocketpp::close::status::normal, "Connection closed by server.");
    ptr->stop();
}

void On_close(void* c, websocketpp::connection_hdl hdl)
{
    WebClient *ptr = (WebClient *)c;
    
    UTL_LOG_INFO("The server is shutdown! Disconnect connection\n");
    ptr->close(hdl, websocketpp::close::status::normal, "Connection closed by server.");
    ptr->stop();
}
#endif

int main(int argc, char **argv) 
{
    Log start; // start Logging

    if(!getLock()) // Check single instance app
    {
        UTL_LOG_INFO("Process already running!\n", stderr);
        return 1;
    }

    registerSigHandler(); // register to handle signal

    connection = new CConnection(); // Create connection FSM
    // UTL_LOG_INFO("connection state = %s", typeid(connection->getCurrentState()).name());
#ifdef _ARM_PLATFORM_
    CWebSocketClient::ClearPluginState();
#endif

    UTL_LOG_INFO("argc = %d, argv[1] = %s", argc, argv[1]);
    if (argc > 1 && strcmp(argv[1], "test") == 0)
    {
        // Test...
#ifdef TEST_WEBSOCKET_ONLY
        string url = "ws://127.0.0.1:5566";
        UTL_LOG_INFO("url: %s", url.c_str());
        WebClient client;
        try {
            UTL_LOG_INFO("==> set_access_channels <==");
            client.set_access_channels(websocketpp::log::alevel::none);
            UTL_LOG_INFO("==> clear_access_channels <==");
            client.clear_access_channels(websocketpp::log::alevel::frame_payload|websocketpp::log::alevel::frame_header|websocketpp::log::alevel::control|websocketpp::log::alevel::all);
            UTL_LOG_INFO("==> init_asio <==");
            client.init_asio();
            // Register out handlers
#ifdef  _ARM_PLATFORM_
            UTL_LOG_INFO("==> set_open_handler On_open <==");
            client.set_open_handler(bind(&On_open, &client, ::_1));
            UTL_LOG_INFO("==> set_fail_handler On_fail <==");
            client.set_open_handler(bind(&On_fail, &client, ::_1));
            UTL_LOG_INFO("==> set_close_handler On_close <==");
            client.set_close_handler(bind(&On_close, &client, ::_1));
#else
            client.set_open_handler(bind(&On_open, &client, placeholders::_1));
            client.set_fail_handler(bind(&On_fail, &client, placeholders::_1));
            client.set_close_handler(bind(&On_close, &client, placeholders::_1));
#endif
            // the event loop starts
            websocketpp::lib::error_code ec;
            UTL_LOG_INFO("==> get_connection <==");
            WebClient::connection_ptr con = client.get_connection(url, ec);
            UTL_LOG_INFO("==> connect <==");
            client.connect(con);
            // Start the ASIO io_service run loop
            UTL_LOG_INFO("==> run <==");
            client.run();
            while (1)
            {
                sleep(30);
            };
            UTL_LOG_INFO("==> end <==");
        } catch (const std::exception & e) {
            printf("Exception: %s\n", e.what());
        } catch (websocketpp::lib::error_code e) {
            printf("Exception: %s\n", e.message().c_str());
        } catch (...) {
            printf("Other exception\n");
        }
#endif
    }
    else
    {
        if (argc == 1)
        {
            UTL_LOG_WARN("Please provide a plugin config file, e.g. \'device_plugin plugin_config.json\'.");
            return 0;
        }
        else if (argc > 2)
        {
            UTL_LOG_WARN("Wrong arguments. Usage: device_plugin [config file]");
            return 0;
        }

        UTLMutex_t mutex;
        UTLCond_t maincond;

        int rv = UTLCond_Init(&maincond);
        if (rv != 0) 
        {
            UTL_LOG_ERROR("%s failed: %d", "UTLCond_Init", rv);
            return 0;
        }
        rv = UTLMutex_Init(&mutex);
        if (rv != 0) 
        {
            UTL_LOG_ERROR("%s failed: %d", "UTLMutex_Init", rv);
            return 0;
        }

CONNECT_WEBSOCKET:
#ifdef LAUNCH_AGENT
        int agentRet = -1;
        int retryLaunchingCount = 0;
        do {
            agentRet = launchAgent();
            UTL_LOG_INFO("agent psId = %d", agentRet);
            sleep(CWebSocketClient::ExponentialRetryPause(++retryLaunchingCount));
        } while (agentRet == -1);
        retryLaunchingCount = 0;
        currConnState = connection->getCurrentState();
        if (currConnState == &CAgentDisabled::getInstance())
        {
            CAgentDisabled* state = (CAgentDisabled*)currConnState;
            state->setNewStateReason(CConnectionState::ERROR);
            connection->toggle();
            goto EXIT;
        }

        UTL_LOG_INFO("Agent psId: %d", getProcIdByName(BDM_AGENT));
        if (agentLaunchedByPlugin)
        {
            agentLaunchedByPlugin = false;
            UTL_LOG_INFO("wait 90 secs for websocket server ready...");
            sleep(90);
        }
#endif
#ifdef DEBUG
        UTL_LOG_INFO("new wsclientobj");
#endif
        CWebSocketClient *wsclientobj = new CWebSocketClient();
        wsclientobj->Initial();
#if defined(TEST_UPDATE) || defined(TEST_LOCAL_COMMANDS)
        if (argv[1] && strlen(argv[1]) > 0)
        {
            CPluginSampleConfig *config = new CPluginSampleConfig(argv[1]);
            if (config)
            {
                string sampleName = config->GetSampleName();
                UTL_LOG_INFO("sample name = %s", sampleName.c_str());
                if (sampleName.compare(SAMPLE_TMATE_PLUGIN) == 0)
                {
                    CTmatePlugin *tmatePlugin = new CTmatePlugin(config);
                    if (tmatePlugin->SetNotifyPluginUpdate()) wsclientobj->SetSamplePlugin(tmatePlugin);
                }
                else
                {
                    UTL_LOG_INFO("Unknown sample config. Terminate this plugin.");
                    goto EXIT;
                }
            }
        }
#endif
#ifdef DEBUG
        UTL_LOG_INFO("start work, update... threads.");
#endif
        wsclientobj->StartWebClient();
        do {
            // UTL_LOG_INFO("wait for wsclientobj alive...");
            sleep(10);
        } while(wsclientobj->WebClientIsAlive() == false && wsclientobj->GetException().compare("invalid state") != 0);
#ifdef DEBUG
        if (wsclientobj->WebClientIsAlive()) UTL_LOG_INFO("wsclientobj is alive.");
        else UTL_LOG_INFO(wsclientobj->GetException().c_str());
#endif

        while (wsclientobj && !wsclientobj->IsEndWebSocket())
        {
            if(wsclientobj->WebClientIsAlive() == false) {
                UTL_LOG_WARN("Web socket connection is broken, retry connection!");
#ifdef _ARM_PLATFORM_
                CWebSocketClient::ClearPluginState();
#endif
                if (wsclientobj)
                {
                    delete(wsclientobj);
                    wsclientobj = NULL;
                }
                currConnState = connection->getCurrentState();
                if (currConnState == &CPluginRegistered::getInstance())
                {
                    CPluginRegistered* state = (CPluginRegistered*)currConnState;
                    state->setNewStateReason(CConnectionState::WEBSOCKET_DISCONNECTED);
                    connection->toggle();
                }

                sleep(CWebSocketClient::ExponentialRetryPause(++retryConnectWebsocket));
                goto CONNECT_WEBSOCKET;
            }
            else
            {
// #ifdef DEBUG
//                 UTL_LOG_INFO("WebClientIsAlive: %d", wsclientobj->WebClientIsAlive());
// #endif
                sleep(10);
            }
        }

        UTLMutex_Destroy(&mutex);
        UTLCond_Destroy(&maincond);

        if (wsclientobj)
        {
            websocketpp::lib::error_code ec;
            wsclientobj->m_client.close(wsclientobj->m_pwebclient->phd, websocketpp::close::status::normal, "Connection closed by client.");
            if (ec)
            {
                UTL_LOG_ERROR("Error code: %s", ec.message().c_str());
            }
            while (true)
            {
                sleep(5);
                if (wsclientobj->GetThreadHandle() > -1 && wsclientobj->GetUpdateThreadHandle() > -1 &&
                    wsclientobj->GetCommandThreadHandle() > -1 && wsclientobj-> GetDateThreadHandle() > -1 &&
                    wsclientobj->WebClientIsAlive())
                {
                    UTL_LOG_INFO("%d, %ld, %ld, %ld, %ld", wsclientobj->WebClientIsAlive(), wsclientobj->GetThreadHandle(),
                    wsclientobj->GetUpdateThreadHandle(), wsclientobj->GetCommandThreadHandle(), wsclientobj->GetDateThreadHandle());
                    sleep(1);
                }
                else break;
            }
            delete wsclientobj;
            wsclientobj = NULL;
        }
    }

EXIT:
#ifdef _ARM_PLATFORM_
    CWebSocketClient::ClearPluginState();
#endif
    if (connection) delete connection;
    UTL_LOG_INFO("plugIN is terminated.");
    return 0;
}
