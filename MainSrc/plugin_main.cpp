#include "Log.h"
#include <dirent.h>
#include <typeinfo>
#include <csignal>
#include "Connection.h"
#include "ConcreteStates.h"
#include "WebSocketClient.h"
#include "../Util/include/Utl_Log.h"
#include "../PluginSDK/UpdatePluginJson.h"
#include "../PluginSDK/PluginException.h"
#include "../PluginSDK/CommandAcksPluginJson.h"
#include "../Plugins/TmatePlugin.h"

using namespace std;

#define BDM_AGENT "BDM_Agent"
#define PID_FILE "/var/run/tmatePlugin.pid"

extern bool gotSigInt;
CConnection* connection;
CConnectionState* currConnState;

int retryConnectWebsocket = 0;

void registerSigHandler()
{
    struct sigaction sa;

    sa.sa_handler = &CWebSocketClient::SignalHandler;
    sa.sa_flags = SA_RESTART;
    sigfillset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Error: cannot handle SIGINT"); // Should not happen
    }
}

int getLock(void)
{
    struct flock fl;
    int fdlock;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 1;

    if((fdlock = open(PID_FILE, O_WRONLY|O_CREAT, 0666)) == -1) return 0;

    if(fcntl(fdlock, F_SETLK, &fl) == -1) return 0;

    return 1;
}

string getCurrentDate()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[32];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y", timeinfo);

    return string(buffer);
}

int main(int argc, char **argv)
{
    Log start; // start Logging
    UTL_LOG_INFO("BUILD_INFO: %s", BUILD_INFO);

    if(!getLock()) // Check single instance app
    {
        UTL_LOG_INFO("Process already running!\n", stderr);
        return 1;
    }

    registerSigHandler(); // register to handle signal

    connection = new CConnection(); // Create connection FSM
    // UTL_LOG_INFO("connection state = %s", typeid(connection->getCurrentState()).name());

    UTL_LOG_INFO("argc = %d, argv[1] = %s", argc, argv[1]);
    if (argc > 1 && strcmp(argv[1], "test") == 0)
    {
        // Test...
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
        currConnState = connection->getCurrentState();
        CAgentDisabled* state = (CAgentDisabled*)currConnState;
        state->setNewStateReason(CAgentDisabled::AGENT_ALIVE);
        connection->toggle();
#ifdef DEBUG
        UTL_LOG_INFO("new wsclientobj");
#endif
        CWebSocketClient *wsclientobj = new CWebSocketClient();
        wsclientobj->Initial();
#ifdef TEST_UPDATE
        // Init plugIN config
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
        // Start WebSocketClient connection to Agent thread, update, data and commandAcks threads.
        wsclientobj->StartWebClient();
        do {
            // UTL_LOG_INFO("wait for wsclientobj alive...");
            // Wait until the WebSocketClient connection to Agent is finished to build.
            sleep(10);
        } while(!wsclientobj->IsWebClientWorking() && wsclientobj->GetException().compare("invalid state") != 0);
#ifdef DEBUG
        if (wsclientobj->IsWebClientWorking()) UTL_LOG_INFO("wsclientobj is alive.");
        else UTL_LOG_INFO(wsclientobj->GetException().c_str());
#endif

        while (wsclientobj && !wsclientobj->IsEndWebSocket())
        {
            // Leave this while loop after received ctrl-c signal
            if (gotSigInt)
            {
                UTL_LOG_INFO("exit program.");
                wsclientobj->SetEndWebSocket(true);
            }

            if(wsclientobj->IsWebClientWorking())
            {
// #ifdef DEBUG
//                 UTL_LOG_INFO("IsWebClientWorking: %d", wsclientobj->IsWebClientWorking());
// #endif
                sleep(10);
            }
            else
            {
                UTL_LOG_WARN("Web socket connection is broken, retry connection!");
                // Switch to WEBSOCKET_DISCONNECTED connection-state
                currConnState = connection->getCurrentState();
                if (currConnState == &CPluginRegistered::getInstance())
                {
                    CPluginRegistered* state = (CPluginRegistered*)currConnState;
                    state->setNewStateReason(CConnectionState::WEBSOCKET_DISCONNECTED);
                    connection->toggle();
                }
                // Re-create WebSocketClient object for connection to Agent
                if (wsclientobj)
                {
                    delete(wsclientobj);
                    wsclientobj = NULL;
                }
                sleep(CWebSocketClient::ExponentialRetryPause(++retryConnectWebsocket));
                goto CONNECT_WEBSOCKET;
            }
        }

        UTLMutex_Destroy(&mutex);
        UTLCond_Destroy(&maincond);

        // Delete WebSocketClient object after received ctrl-c signal.
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
                    wsclientobj->IsWebClientWorking())
                {
                    UTL_LOG_INFO("%d, %ld, %ld, %ld, %ld", wsclientobj->IsWebClientWorking(), wsclientobj->GetThreadHandle(),
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
    if (connection) delete connection;
    UTL_LOG_INFO("plugIN is terminated.");
    return 0;
}
