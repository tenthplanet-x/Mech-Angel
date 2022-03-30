#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "MG_DatabaseStruct.h"
#include "LC_Define.h"
#include "UT_ComplexData.h"

class AO_Server_ReqAuctionBuy : public TS_AtomOperate
{
public:
	AO_Server_ReqAuctionBuy();
	~AO_Server_ReqAuctionBuy() {}

	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Rollback();
	virtual void			Reset();

private:
	Protocol::PS_ItemAssetEntry	m_kItemEntry;
	unique_id_impl		m_kAuctionUID;
	Utility::LC_CashMap	m_kCashs;
};

class AO_Server_RltAuctionBuy : public TS_AtomOperate
{
public:	
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Reset();
	void                    BuySuccess(Protocol::MG_BODY* pkMsg, uint32_t uiID);

	unique_id_impl		m_kAuctionUID;
	Utility::LC_CashMap	m_kCashs;
};

//!!!!!!! 需要增加超时设计
class TS_Server_TranAuctionBuyTran : public TS_TranBase
{
public:
	enum AuctionBuyStep
	{
		STEP_REQ_GG_AUCTIONBUY = 0,
		STEP_RLT_GG_AUCTIONBUY,
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_AUCTION_BUY_TRAN;
	}

public:
	TS_Server_TranAuctionBuyTran();
	~TS_Server_TranAuctionBuyTran();
};