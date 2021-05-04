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
    CInit() { /*UTL_LOG_INFO("CInit()")*/; }
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
    CAgentAlive() { /*UTL_LOG_INFO("CAgentAlive()")*/; }
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
    CAgentDisabled() { /*UTL_LOG_INFO("CAgentDisabled()")*/; }
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
    CWebsocketConnected() { /*UTL_LOG_INFO("CWebsocketConnected()")*/; }
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
    CDeviceOffline() { /*UTL_LOG_INFO("CDeviceOffline()")*/; }
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
    CWebsocketDisconnected() { /*UTL_LOG_INFO("CWebsocketDisconnected()")*/; }
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
    CPluginRegistered() { /*UTL_LOG_INFO("CPluginRegistered()")*/; }
    CPluginRegistered(const CPluginRegistered& other);
    CPluginRegistered& operator=(const CPluginRegistered& other);
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
    CServerOffline() { /*UTL_LOG_INFO("CServerOffline()")*/; }
    CServerOffline(const CServerOffline& other);
    CServerOffline& operator=(const CServerOffline& other);
    CConnectionState* mLastState;
};

class CExit : public CConnectionState
{
public:
    void enter(CConnection* connection);
    void toggle(CConnection* connection);
    void exit(CConnection* connection);
    static CConnectionState& getInstance();

private:
    CExit() { /*UTL_LOG_INFO("CExit()")*/; }
    CExit(const CExit& other);
    CExit& operator=(const CExit& other);
};
#endif