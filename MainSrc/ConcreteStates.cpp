#include "ConcreteStates.h"


/**
 * Init state of plugin connections
 */
void CInit::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(this).name());
}

void CInit::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case AGENT_ALIVE:
            connection->setState(CAgentAlive::getInstance());
            break;

        case AGENT_DISABLED:
            connection->setState(CAgentDisabled::getInstance());
            break;
    }
}

void CInit::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CInit::getInstance()
{
    static CInit singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * AgentAlive state of plugin connections
 */
void CAgentAlive::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(getInstance()).name());
}

void CAgentAlive::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case WEBSOCKET_CONNECTED:
            connection->setState(CWebsocketConnected::getInstance());
            break;

        case WEBSOCKET_DISCONNECTED:
            connection->setState(CWebsocketDisconnected::getInstance());
            break;
    }
}

void CAgentAlive::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CAgentAlive::getInstance()
{
    static CAgentAlive singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * AgentDisabled state of plugin connections
 */
void CAgentDisabled::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(getInstance()).name());
}

void CAgentDisabled::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case ERROR:
            connection->setState(CExit::getInstance());
            break;
        case AGENT_ALIVE:
            connection->setState(CAgentAlive::getInstance());
            break;
    }
}

void CAgentDisabled::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CAgentDisabled::getInstance()
{
    static CAgentDisabled singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * WebsocketConnected state of plugin connections
 */
void CWebsocketConnected::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(this).name());
}

void CWebsocketConnected::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case SERVER_OFFLINE:
            connection->setState(CServerOffline::getInstance());
            break;

        case WEBSOCKET_DISCONNECTED:
            connection->setState(CWebsocketDisconnected::getInstance());
            break;
        
        default:
            connection->setState(CPluginRegistered::getInstance());
    }
}

void CWebsocketConnected::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CWebsocketConnected::getInstance()
{
    static CWebsocketConnected singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * DeviceOffline state of plugin connections
 */
void CDeviceOffline::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(getInstance()).name());
}

void CDeviceOffline::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case WEBSOCKET_DISCONNECTED:
            connection->setState(CWebsocketDisconnected::getInstance());
            break;

        default:
            connection->setState(CWebsocketConnected::getInstance());
    }
}

void CDeviceOffline::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CDeviceOffline::getInstance()
{
    static CDeviceOffline singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * WebsocketDisconnected state of plugin connections
 */
void CWebsocketDisconnected::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(this).name());
}

void CWebsocketDisconnected::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case AGENT_DISABLED:
            connection->setState(CAgentDisabled::getInstance());
            break;

        default:
            connection->setState(CAgentAlive::getInstance());
    }
}

void CWebsocketDisconnected::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CWebsocketDisconnected::getInstance()
{
    static CWebsocketDisconnected singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * PluginRegistered state of plugin connections
 */
void CPluginRegistered::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(this).name());
}

void CPluginRegistered::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case DEVICE_OFFLINE:
            connection->setState(CDeviceOffline::getInstance());
            break;

        case SERVER_OFFLINE:
            connection->setState(CServerOffline::getInstance());
            break;

        case WEBSOCKET_DISCONNECTED:
            connection->setState(CWebsocketDisconnected::getInstance());
            break;

        case AGENT_DISABLED:
            connection->setState(CAgentDisabled::getInstance());
            break;

        case RESEND_PLUGIN:
            connection->setState(CWebsocketConnected::getInstance());

        case ERROR:
            break;
    }
    mReason = PLUGIN_REGISTERED;
}

void CPluginRegistered::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CPluginRegistered::getInstance()
{
    static CPluginRegistered singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * ResendPlugin state of plugin connections
 */
void CResendPlugin::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(this).name());
}

void CResendPlugin::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    switch (mReason)
    {
        case PLUGIN_REGISTERED:
            connection->setState(CPluginRegistered::getInstance());
            break;

        case ERROR:
            break;
    }
    mReason = RESEND_PLUGIN;
}

void CResendPlugin::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CResendPlugin::getInstance()
{
    static CResendPlugin singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * CServerOffline state of plugin connections
 */
void CServerOffline::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(this).name());
}

void CServerOffline::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    if (mLastState == &CPluginRegistered::getInstance())
    {
        connection->setState(CWebsocketConnected::getInstance());
    }
    else if (mLastState == &CWebsocketConnected::getInstance())
    {
        connection->setState(CWebsocketConnected::getInstance());
    }
}

void CServerOffline::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(this).name());
}

CConnectionState& CServerOffline::getInstance()
{
    static CServerOffline singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}

/**
 * Exit state of plugin connections
 */
void CExit::enter(CConnection* connection)
{
    UTL_LOG_INFO("enter of %s", typeid(this).name());
}

void CExit::toggle(CConnection* connection)
{
    UTL_LOG_INFO("toggle of %s", typeid(this).name());
    // connection->setState(CExit::getInstance());
}

void CExit::exit(CConnection* connection)
{
    UTL_LOG_INFO("exit of %s", typeid(getInstance()).name());
}

CConnectionState& CExit::getInstance()
{
    static CExit singleton;
    // UTL_LOG_INFO("Instance of state: %s", typeid(singleton).name());
    return singleton;
}
