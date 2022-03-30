#pragma once

#include "TS_TranType.h"
#include "TS_TranBase.h"

#include "TS_WriteMailTran.h"
#include "TS_GetItemAffixTran.h"
#include "TS_TranAuctionSellTran.h"
#include "TS_TranAuctionBuyTran.h"
#include "TS_PayMailTran.h"
#include "TS_CreateGuildTran.h"
#include "TS_TranDonateGuildMaterials.h"
#include "TS_GMRequest.h"
#include "TS_TranSkyArena.h"
#include "UT_MemoryPool.h"

class TS_TranPool
{
public:
	TS_TranPool();
	~TS_TranPool();

	virtual TS_TranBase*		AllocTran( uint32_t eTransactionType );
	virtual void				FreeTran( TS_TranBase* pkTransaction );

protected:
	Utility::UT_MemoryPool<TS_Server_WriteMail>					m_kWriteMailPool;
	Utility::UT_MemoryPool<TS_Server_PayForMail>				m_kPayForMailPool;
	Utility::UT_MemoryPool<TS_Server_GetItemAffixFromMail>		m_kGetItemAffixPool;
	Utility::UT_MemoryPool<TS_Server_AuctionSellTran>			m_kAuctionSellPool;
	Utility::UT_MemoryPool<TS_Server_TranAuctionBuyTran>		m_kAuctionBuyPool;
	Utility::UT_MemoryPool<TS_Server_CreateGuild>				m_kCreateGuildPool;
	Utility::UT_MemoryPool<TS_Server_DonateGuildMaterialsTran>	m_kDonateMaterailsPool;
	Utility::UT_MemoryPool<TS_Server_GM>						m_kGMPool;
	Utility::UT_MemoryPool<TS_Server_SkyArenaBattleInfo>		m_kSkyArenaPoll;
};