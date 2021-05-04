#include "Log.h"
#include "Connection.h"
#include "ConcreteStates.h"
#include <typeinfo>

CConnection::CConnection()
{
    mCurrentState = &CInit::getInstance();
}

void CConnection::setState(CConnectionState& newState)
{
    UTL_LOG_INFO("Current state: %s, new state: %s", typeid(this).name(), typeid(newState).name());
    mCurrentState->exit(this);
    mCurrentState = &newState;
    mCurrentState->enter(this);
}

void CConnection::toggle()
{
    mCurrentState->toggle(this);
}