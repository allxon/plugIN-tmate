#ifndef _TMATEPLUGIN_H_
#define _TMATEPLUGIN_H_

#include <bitset>
#include "PluginSample.h"
#include "../Util/include/AlertsPluginJson.h"

#define SAMPLE_TMATE_PLUGIN      "TmatePlugin"
#define PLUGINS_PATH                "/opt/allxon/plugIN/"
#define SCRIPTS_STATES_PATH         "scripts/states/"
#define SCRIPTS_COMMANDS_PATH       "scripts/commands/"


class StateUpdated
{
public:
    const static std::bitset<4> status;
    const static std::bitset<4> version;
    const static std::bitset<4> web;
    const static std::bitset<4> ssh;
    const static std::bitset<4> none;
};

class TmateStates
{
public:
    const static std::string status;
    const static std::string version;
    const static std::string web;
    const static std::string ssh;
};

class TmateCommands
{
public:
    const static std::string install;
    const static std::string start;
    const static std::string stop;
    const static std::string uninstall;
    const static std::string password;
};

class CTmatePlugin : public CPluginSample
{
public:
    CTmatePlugin();
    CTmatePlugin(CPluginSampleConfig *sampleConfig);
    CTmatePlugin(std::string apiVersion, bool minify, std::string appGUID, std::string accessKey);
    ~CTmatePlugin();

    CUpdatePluginJson *SetNotifyPluginUpdate();
    bool AcceptReceivedCommand(cJSON *commandJson);
    std::string ExecuteReceivedCommand(cJSON *commandJson, cJSON *cmdAck);
    void UpdateStates(std::bitset<4> updateMask);
    cJSON *AddStatusState();
    cJSON *AddVersionState();
    cJSON *AddWebLinkState();
    cJSON *AddSshLinkState();
    std::bitset<4> IsStateFilesChanged();

    std::string m_pluginPath;
    std::string m_statusOutput;
    std::string m_versionOutput;
    std::string m_webOutput;
    std::string m_sshOutput;

protected:
    void Init();
    static std::string ReadOutput(const std::string outputName);
    static bool RunStatesScript(std::string scriptCmd);
    static cJSON *GetStatesOutput(std::string pluginPath, std::string stateName);
    static void RunPluginScriptCmd(const std::string scriptCmd, const std::string cmdParam, bool &result);
    static void RunPluginScriptCmdOutput(const std::string scriptCmd, const std::string cmdParam, bool &result, std::string &output);
    static std::string RandomString(int len);
    static int FileIsModified(const char *path, time_t oldMTime, time_t &newMTime);

private:
    time_t statusTime;
    time_t versionTime;
    time_t webTime;
    time_t sshTime;
};

#endif
