#pragma once

#include "UT_Log.h"

class UT_ClientLogRemoteConsole : public Utility::UT_LogStreamRemoteConsole
{
public:
	UT_ClientLogRemoteConsole(void);
	virtual ~UT_ClientLogRemoteConsole(void);

	virtual void LogMessage(const StringType& rkMessage);
};
