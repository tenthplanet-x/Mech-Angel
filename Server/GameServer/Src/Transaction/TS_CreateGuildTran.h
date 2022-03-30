//¥¥Ω®√≈≈…

#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "LC_Money.h"

class AO_Server_ReqCreateGuild : public TS_AtomOperate
{
public:
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Rollback();
	virtual void			Reset();

private:
	MoneyType				m_llCash;
	StringType				m_strGuildName;
};

class AO_Server_RltCreateGuild : public TS_AtomOperate
{
public:	
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
};

class TS_Server_CreateGuild : public TS_TranBase
{
public:
	enum CreateGuildStep
	{
		STEP_REQ_GG_CRAETE_GUILD = 0,
		STEP_RLT_GG_CREATE_GUILD,			
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_CREATE_GUILD;
	}
public:
	TS_Server_CreateGuild();
	~TS_Server_CreateGuild();
};