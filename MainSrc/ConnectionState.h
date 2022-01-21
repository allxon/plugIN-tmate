#ifndef _CONNECTIONSTATE_H_
#define _CONNECTIONSTATE_H_

#include "Connection.h"

class CConnection;

class CConnectionState {
public:
    enum Reason
    {
        PLUGIN_REGISTERED = 0,
        DEVICE_OFFLINE,
        SERVER_OFFLINE,
        SERVER_ONLINE,
        WEBSOCKET_DISCONNECTED,
        AGENT_DISABLED,
        AGENT_ALIVE,
        WEBSOCKET_CONNECTED,
        RESEND_PLUGIN,
        ERROR,
        NONE = -1
    };

    void setNewStateReason(Reason reason) { mReason = reason; }
    Reason getCurrentStateReason() { return mReason; }
    virtual ~CConnectionState() {}
    virtual void enter(CConnection* connection) = 0;
    virtual void toggle(CConnection* connection) = 0;
    virtual void exit(CConnection* connection) = 0;

protected:
    Reason mReason;
};

#endif