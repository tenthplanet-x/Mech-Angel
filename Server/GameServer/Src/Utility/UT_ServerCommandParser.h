//√¸¡ÓΩ‚ Õ
#ifndef _UT_SERVER_COMMAND_PARSER_H
#define _UT_SERVER_COMMAND_PARSER_H

#include "LC_CommandParserBase.h"
#include "UT_CmdLine.h"

class TS_Server_GM;
class UT_ServerCommandParser : public Utility::LC_CommandParserBase
{
public:
	UT_ServerCommandParser();
	virtual ~UT_ServerCommandParser(){};
	virtual void ParseCommand( NetWork::NW_NetSessionBase* pkSession , const Utility::UT_CommandLine& rkCommand, uint32_t uiPri, int64_t msg_id, redis_define::gm_msg_req* preq);

	TS_Server_GM* GetGMTrans(void);
	TS_Server_GM* GetGuildGMTrans(void);
	TS_Server_GM* GetRandomGuildNameGMTrans(void);
	TS_Server_GM* GetCommGMTrans(void);
};


#endif
