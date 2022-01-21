#ifndef _CONCRETESTATES_H_
#define _CONCRETESTATES_H_

#include "Log.h"
#include "Connection.h"
#include <typeinfo>

class CInit : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CInit() { mReason = CInit::NONE;/*UTL_LOG_INFO("CInit()");*/ }
    CInit(const CInit& other);
    CInit& operator=(const CInit& other);
};

class CAgentAlive : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CAgentAlive() { mReason = CAgentAlive::NONE;/*UTL_LOG_INFO("CAgentAlive()");*/ }
    CAgentAlive(const CAgentAlive& other);
    CAgentAlive& operator=(const CAgentAlive& other);
};

class CAgentDisabled : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CAgentDisabled() { mReason = CAgentDisabled::NONE;/*UTL_LOG_INFO("CAgentDisabled()");*/ }
    CAgentDisabled(const CAgentDisabled& other);
    CAgentDisabled& operator=(const CAgentDisabled& other);
};

class CWebsocketConnected : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CWebsocketConnected() { mReason = CWebsocketConnected::NONE;/*UTL_LOG_INFO("CWebsocketConnected()");*/ }
    CWebsocketConnected(const CWebsocketConnected& other);
    CWebsocketConnected& operator=(const CWebsocketConnected& other);
};

class CDeviceOffline : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();
    // void setRetryTimes(int retryTimes) { mRetryTimes = retryTimes; }

private:
    CDeviceOffline() { mReason = CDeviceOffline::NONE;/*UTL_LOG_INFO("CDeviceOffline()");*/ }
    CDeviceOffline(const CDeviceOffline& other);
    CDeviceOffline& operator=(const CDeviceOffline& other);
    // int mRetryTimes;
};

class CWebsocketDisconnected : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();
    // void setRetryTimes(int retryTimes) { mRetryTimes = retryTimes; }

private:
    CWebsocketDisconnected() { mReason = CWebsocketDisconnected::NONE;/*UTL_LOG_INFO("CWebsocketDisconnected()");*/ }
    CWebsocketDisconnected(const CWebsocketDisconnected& other);
    CWebsocketDisconnected& operator=(const CWebsocketDisconnected& other);
    // int mRetryTimes;
};

class CPluginRegistered : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CPluginRegistered() { mReason = CPluginRegistered::NONE;/*UTL_LOG_INFO("CPluginRegistered()");*/ }
    CPluginRegistered(const CPluginRegistered& other);
    CPluginRegistered& operator=(const CPluginRegistered& other);
};

class CResendPlugin : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CResendPlugin() { mReason = CResendPlugin::NONE;/*UTL_LOG_INFO("CResendPlugin()");*/ }
    CResendPlugin(const CResendPlugin& other);
    CResendPlugin& operator=(const CResendPlugin& other);
};

class CServerOffline : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();
    void setLastState(CConnectionState& lastState) { mLastState = &lastState; }

private:
    CServerOffline() { mReason = CServerOffline::NONE;/*UTL_LOG_INFO("CServerOffline()");*/ }
    CServerOffline(const CServerOffline& other);
    CServerOffline& operator=(const CServerOffline& other);
    CConnectionState* mLastState = NULL;
};

class CExit : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CExit() { mReason = CExit::NONE;/*UTL_LOG_INFO("CExit()");*/ }
    CExit(const CExit& other);
    CExit& operator=(const CExit& other);
};
#endif