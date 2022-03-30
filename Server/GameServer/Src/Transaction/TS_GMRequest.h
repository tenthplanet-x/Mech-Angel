#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "redis.pb.h"

////////////////////////////////////////////////////////////////////////
class TS_Server_GM;
class AO_Server_GMStep : public TS_AtomOperate
{
public:
	AO_Server_GMStep(void): m_pkTrans(NULL) {}

	inline void SetTrans(TS_Server_GM* pkTans) { m_pkTrans = pkTans; }
	virtual void RespondRltMsg();
protected:
	TS_Server_GM*	m_pkTrans;
};

////////////////////////////////////////////////////////////////////////
class TS_Server_GM : public TS_TranBase
{
public:
	TS_Server_GM(void):m_msg_id(0){}
	virtual ~TS_Server_GM();

	virtual uint32_t GetType() { return TRAN_TYPE_GM; }
	inline void SetChannelReq(int64_t msg_id, const redis_define::gm_msg_req& req)
	{
		m_msg_id = msg_id;
		m_req = req;
	}

	void AddStep(AO_Server_GMStep* pkOp)
	{
		if (NULL == pkOp)
		{
			return;
		}
		m_kStepTranMap[m_step++] = pkOp;
		pkOp->SetTrans(this);
	}

	virtual void Reset();
	
	inline void set_rslt(const std::string& rslt) {m_rslt = rslt;}
	inline void set_data(const std::string& data) {m_data = data;}
	inline int64_t get_msgid() { return m_msg_id; }
	void Response(uint32_t rslt);
protected:
	int64_t m_msg_id;
	redis_define::gm_msg_req m_req;
	std::string	m_rslt;
	std::string m_data;
	int m_step;
};

////////////////////////////////////////////////////////////////////////
class AO_Server_QueryGuildRlt : public AO_Server_GMStep
{
public:
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
};

////////////////////////////////////////////////////////////////////////
class AO_Server_RandomGuildNameRlt : public AO_Server_GMStep
{
public:
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
};

////////////////////////////////////////////////////////////////////////
class AO_Server_CommGMRlt : public AO_Server_GMStep
{
public:
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
};
