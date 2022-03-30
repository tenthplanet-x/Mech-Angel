

#include "TS_TranPool.h"

using namespace Utility;

//-------------------------------------------------
TS_TranPool::TS_TranPool()
{
	m_kWriteMailPool.Init(64, 5, "TS_TranPool.m_kWriteMailPool");
	m_kPayForMailPool.Init(64, 5, "TS_TranPool.m_kPayForMailPool");
	m_kGetItemAffixPool.Init(64, 5, "TS_TranPool.m_kGetItemAffixPool");
	m_kAuctionSellPool.Init(64, 5, "TS_TranPool.m_kAuctionSellPool");
	m_kAuctionBuyPool.Init(64, 5, "TS_TranPool.m_kAuctionBuyPool");
	m_kCreateGuildPool.Init(64, 5, "TS_TranPool.m_kCreateGuildPool");
	m_kDonateMaterailsPool.Init(64, 5, "TS_TranPool.m_kDonateMaterailsPool");
	m_kGMPool.Init(64, 5, "TS_TranPool.m_kGMPool");
	m_kSkyArenaPoll.Init(64, 5, "TS_TranPool.m_kSkyArenaPoll");
}
//-------------------------------------------------
TS_TranPool::~TS_TranPool()
{
}
//-------------------------------------------------
TS_TranBase* TS_TranPool::AllocTran(uint32_t eTransactionType)
{
	TS_TranBase* pkTran = NULL;
	switch (eTransactionType)
	{
	case TRAN_TYPE_WRITE_MAIL:
		pkTran = m_kWriteMailPool.Alloc();
		break;
	case TRAN_TYPE_PAYFORMAIL:
		pkTran = m_kPayForMailPool.Alloc();
		break;
	case TRAN_TYPE_GET_ITEM_AFFIX:
		pkTran = m_kGetItemAffixPool.Alloc();
		break;
	case TRAN_TYPE_AUCTION_SELL_TRAN:
		pkTran = m_kAuctionSellPool.Alloc();
		break;
	case TRAN_TYPE_AUCTION_BUY_TRAN:
		pkTran = m_kAuctionBuyPool.Alloc();
		break;
	case TRAN_TYPE_CREATE_GUILD:
		pkTran = m_kCreateGuildPool.Alloc();
		break;
	case TRAN_TYPE_DONATE_MATERIALS:
		pkTran = m_kDonateMaterailsPool.Alloc();
		break;
	case TRAN_TYPE_GM:
		pkTran = m_kGMPool.Alloc();
		break;
	case TRAN_TYPE_SKY_ARENA:
		pkTran = m_kSkyArenaPoll.Alloc();
		break;
	default:
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"AllocTransaction unkown type", eTransactionType);		
	}

	if(pkTran)
		pkTran->Reset();

	return pkTran;
}
//-------------------------------------------------
void TS_TranPool::FreeTran(TS_TranBase* pkTran)
{
	if(NULL == pkTran)
		return;
	
	pkTran->Reset();

	switch ( pkTran->GetType() )
	{
	case TRAN_TYPE_WRITE_MAIL:
		m_kWriteMailPool.Free( static_cast < TS_Server_WriteMail* > ( pkTran ) );
		break;
	case TRAN_TYPE_PAYFORMAIL:
		m_kPayForMailPool.Free( static_cast < TS_Server_PayForMail* >( pkTran ) );
		break;
	case TRAN_TYPE_GET_ITEM_AFFIX:
		m_kGetItemAffixPool.Free( static_cast < TS_Server_GetItemAffixFromMail* > ( pkTran ) );
		break;
	case TRAN_TYPE_AUCTION_SELL_TRAN:
		m_kAuctionSellPool.Free( static_cast < TS_Server_AuctionSellTran* > ( pkTran ) );
		break;
	case TRAN_TYPE_AUCTION_BUY_TRAN:
		m_kAuctionBuyPool.Free( static_cast < TS_Server_TranAuctionBuyTran* > ( pkTran ) );
		break;
	case TRAN_TYPE_CREATE_GUILD:
		m_kCreateGuildPool.Free( static_cast < TS_Server_CreateGuild* > ( pkTran ) );
		break;
	case TRAN_TYPE_DONATE_MATERIALS:
		m_kDonateMaterailsPool.Free( static_cast < TS_Server_DonateGuildMaterialsTran* > ( pkTran ) );
		break;
	case TRAN_TYPE_GM:
		m_kGMPool.Free(static_cast < TS_Server_GM* > ( pkTran ));
		break;
	case TRAN_TYPE_SKY_ARENA:
		m_kSkyArenaPoll.Free(static_cast < TS_Server_SkyArenaBattleInfo* > ( pkTran ));
		break;
	default:
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"FreeTransaction unkown type", pkTran->GetType() );		
	}
}