#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "MG_ProtocolStruct.h"

class AO_Server_ReqSkyArenaBattleInfo : public TS_AtomOperate
{
public:
	AO_Server_ReqSkyArenaBattleInfo();
	~AO_Server_ReqSkyArenaBattleInfo() {}

	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Rollback();
	virtual void			Reset();


private:
	Protocol::PS_ItemAssetEntry	m_kItemEntry;
	uint32_t					m_uiCost;
};

class AO_Server_RltSkyArenaBattleInfo : public TS_AtomOperate
{
public:	
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();

private:
	void _respendSuccessMsg(Protocol::MG_BODY* pkMsg);
};

class TS_Server_SkyArenaBattleInfo : public TS_TranBase
{
public:
	enum SkyArenaBattleInfoStep
	{
		STEP_REQ_GG_SkyArenaBattleInfo = 0,
		STEP_RLT_GG_SkyArenaBattleInfo,
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_SKY_ARENA;
	}

public:
	TS_Server_SkyArenaBattleInfo();
	~TS_Server_SkyArenaBattleInfo();
};