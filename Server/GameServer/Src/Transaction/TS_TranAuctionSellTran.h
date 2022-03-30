#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "MG_ProtocolStruct.h"

class AO_Server_ReqAuctionSell : public TS_AtomOperate
{
public:
	AO_Server_ReqAuctionSell();
	~AO_Server_ReqAuctionSell() {}

	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Rollback();
	virtual void			Reset();


private:
	Protocol::PS_ItemAssetEntry	m_kItemEntry;
	uint32_t					m_uiCost;
};

class AO_Server_RltAuctionSell : public TS_AtomOperate
{
public:	
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();

private:
	void _respendSuccessMsg(const Protocol::PS_Auction& kAuction);
};

class TS_Server_AuctionSellTran : public TS_TranBase
{
public:
	enum AuctionSellStep
	{
		STEP_REQ_GG_AUCTIONSELL = 0,
		STEP_RLT_GG_AUCTIONSELL,
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_AUCTION_SELL_TRAN;
	}

public:
	TS_Server_AuctionSellTran();
	~TS_Server_AuctionSellTran();
};