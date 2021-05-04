#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "ConnectionState.h"

class CConnection
{
public:
    CConnection();
    inline CConnectionState* getCurrentState() const { return mCurrentState; }
    void toggle();
    void setState(CConnectionState& newState);

private:
    CConnectionState* mCurrentState;
};

#endif