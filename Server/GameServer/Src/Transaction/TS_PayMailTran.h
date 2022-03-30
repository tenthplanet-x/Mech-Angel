//付费取信事务

#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"

// 请求付费
class AO_Server_ReqPayMail : public TS_AtomOperate
{
public:
	AO_Server_ReqPayMail();
	~AO_Server_ReqPayMail() {}

	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Rollback();
	virtual void			Reset();
private:
	uint64_t				m_uiMailID;
	int64_t					m_llPayPrice;
};

// 修改邮件状态
class AO_Server_RltModifyMailStatus : public TS_AtomOperate
{
public:	
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Reset();


private:
	uint64_t m_uiMailID;
};

class TS_Server_PayForMail: public TS_TranBase
{
public:
	enum WriteMailStep
	{
		STEP_REQ_GM_MODIFY_MAIL_STATUS = 0,
		STEP_RLT_GM_MODIFY_MAIL_STATUS,			
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_PAYFORMAIL;
	}
public:
	TS_Server_PayForMail();
	~TS_Server_PayForMail();
};
