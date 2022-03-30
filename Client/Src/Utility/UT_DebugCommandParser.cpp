#include "ClientPCH.h"
#include "UT_DebugCommandParser.h"
#include "MG_DebugProtocol.h"
#include "LC_CommandParserBase.h"

using namespace Utility;
using namespace Protocol;


UT_DebugCommandParser::UT_DebugCommandParser()
: LC_CommandParserBase("ClientDebugScript.lua")
{
}

//void UT_DebugCommandParser::ParseCommand( const Utility::UT_CommandLine& rkCommand )
//{
//	const StringType kCommandName = rkCommand.GetCommandName();
//}
void UT_DebugCommandParser::ParseCommand( NetWork::NW_NetSessionBase* pkSession , const Utility::UT_CommandLine& rkCommand, uint32_t uiPri)
{
	LC_CommandParserBase::ParseCommand(rkCommand, uiPri);
}