//写邮件事务实现

#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "MG_ProtocolStruct.h"
#include "oss_define.h"

class AO_Server_RltWriteMail;

class AO_Server_ReqWriteMail : public TS_AtomOperate
{
public:
	AO_Server_ReqWriteMail();
	~AO_Server_ReqWriteMail() {}

	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Rollback();
	virtual void			Reset();

  void set_rlt_opt(AO_Server_RltWriteMail * opt){ m_rlt_opt = opt; }

private:
	Protocol::PS_MailInfo	m_kMailInfo;
	AO_Server_RltWriteMail  * m_rlt_opt;
};

class AO_Server_RltWriteMail : public TS_AtomOperate
{
public:	
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	GameLogic::LC_BackPackEntryList  hblist;
	unique_id_impl			charID;
	unique_id_impl			toCharID;
	StringType				rcname;
	StringType				Title;
	StringType				Context;
	int								send_gp;
	int								pay_gp;
	int								cost_gp;
};

class TS_Server_WriteMail : public TS_TranBase
{
public:
	enum WriteMailStep
	{
		STEP_REQ_GM_WRITE_MAIL = 0,
		STEP_RLT_GM_WRITE_MAIL,			
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_WRITE_MAIL;
	}
public:
	TS_Server_WriteMail();
	~TS_Server_WriteMail();
};