#pragma once
#include "LC_CommandParserBase.h"
#include "UT_CmdLine.h"

class UT_DebugCommandParser : public Utility::LC_CommandParserBase
{
public:
	UT_DebugCommandParser();
	virtual ~UT_DebugCommandParser(){}

	//virtual void ParseCommand( const Utility::UT_CommandLine& rkCommand );
	virtual void ParseCommand(  NetWork::NW_NetSessionBase* pkSession , const Utility::UT_CommandLine& rkCommand, uint32_t uiPri);

protected:
	typedef void (*CmdProcessFunc)( const Utility::UT_CommandLine& rkCommand );
	typedef mem::map< StringType , CmdProcessFunc > CmdParseFuncMap;
	CmdParseFuncMap m_kParsers;
};