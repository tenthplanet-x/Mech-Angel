//实现邮件取物品附件事务过程

#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "MG_ProtocolStruct.h"

class AO_Server_ReqGetItemAffix : public TS_AtomOperate
{
public:
	AO_Server_ReqGetItemAffix(void):m_iPackType(PACK_TYPE_BASIC), m_iPackIndex(-1){}

	virtual emTranOperate Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);	

	virtual void Rollback();
	virtual void Reset();

	int32_t m_iPackType;
	int32_t m_iPackIndex;
};

class AO_Server_RltGetItemAffix : public TS_AtomOperate
{
public:
	AO_Server_RltGetItemAffix()
		:m_uiMailID(INVALID_MAIL_ID), m_uiTranID(0), m_ushIndex(0), m_iPackType(PACK_TYPE_BASIC), m_iPackIndex(-1) 
	{};

	virtual emTranOperate Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void Rollback();
	virtual void RespondRltMsg();
	virtual void Reset();

private:
	uint64_t			m_uiMailID;
	uint32_t			m_uiTranID;
	uint16_t			m_ushIndex;
	Protocol::PS_ItemAssetEntry	m_kEntry;
	int32_t				m_iPackType;
	int32_t				m_iPackIndex;
};

class TS_Server_GetItemAffixFromMail : public TS_TranBase
{
public:
	enum WriteMailStep
	{
		STEP_REQ_GM_GET_ITEM_AFFIX = 0,
		STEP_RLT_GM_GET_ITEM_AFFIX,			
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_GET_ITEM_AFFIX;
	}

public:
	TS_Server_GetItemAffixFromMail();
	~TS_Server_GetItemAffixFromMail();
};