#include "ClientPCH.h"
#include "LC_ClientGuildHomeMgr.h"
#include "LC_ClientLogicManager.h"
#include "UT_LogicInfoMessage.h"
#include "LC_ClientGamePlayer.h"
#include "MG_CS_GuildHome.h"
#include "NW_Helper.h"
#include "SystemError.h"
#include "MG_CS_QuanThanhTemple.h"
#include "LC_PackStruct.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;

GameLogic::LC_ClientGuildHomeMgr::LC_ClientGuildHomeMgr()
{
	ClearDirtyFlag();
	for(uint32_t i = 0; i < MAX_WAREHOUSE_PACK_COUNT; ++ i)
		m_uiGuildWareHouseVersion[i] = 0;
	for(uint32_t i = 0; i < MAX_THIEF; ++ i)
		m_iThiefCount[i] = 0;
	m_bIsInThief = false;
	m_fRecordTime = 0.0f;
	m_iGuildContribute = 0;
	m_iEXP = 0;
	m_uiQueryKillThiefTime = 0;
}

GameLogic::LC_ClientGuildHomeMgr::~LC_ClientGuildHomeMgr()
{

}

void GameLogic::LC_ClientGuildHomeMgr::RecvEnterGuildHome()
{

}

void GameLogic::LC_ClientGuildHomeMgr::RequestUpgradeNPC( uint32_t lNPCType )
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkPlayer)
	{	
		return;
	}

	MG_REQ_UpgradeNPC	kUpgradeNPCMsg;
	kUpgradeNPCMsg.m_lNPCType  = lNPCType;
	kUpgradeNPCMsg.m_lPlayerID = pkPlayer->GetID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UPGRADE_NPC, &kUpgradeNPCMsg);
	if(bCreateMsg)
		pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void GameLogic::LC_ClientGuildHomeMgr::RecvUpgradeNPC()
{

}

void GameLogic::LC_ClientGuildHomeMgr::RequestPutIntoGuildWarehouse(int iPackType, int iEntryIndex,int iWareHouseIndex )
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkClientPlayer = pkLogicManager->GetOperationPlayer();
	if(!pkClientPlayer)
	{	
		return;
	}
	LC_PackAsset* pkPackAsset = pkClientPlayer->GetPackAsset();
	if (!pkPackAsset)
	{
		return;
	}

	LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry(iPackType,iEntryIndex);
	if ( pkEntry && pkEntry->GetValid() )
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		MG_REQ_PUTWAREHOSE kReqMsg;
		kReqMsg.m_iPackType = iPackType;
		kReqMsg.m_iEntryIndex = iEntryIndex;
		kReqMsg.m_uiWareHouseIndex = iWareHouseIndex;

		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PUTWAREHOUSE, &kReqMsg);
		if (bCreateMsg)
		{
			pkClientPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}
	else
	{
		SendLogicInfoMessage(RE_FAILED_PUTGUILDWAREHOUSE_GOODS_NOT_EXIST);
	}
}

void GameLogic::LC_ClientGuildHomeMgr::RequestGetGuildWareHouseInfo( int iGuildID )
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkClientPlayer = pkLogicManager->GetOperationPlayer();
	if(!pkClientPlayer)
	{	
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_GUILDWAREHOUSE kReqMsg;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETGUILDWAREHOUSEINFO, &kReqMsg);
	if (bCreateMsg)
	{
		pkClientPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void GameLogic::LC_ClientGuildHomeMgr::AddDirtyFlagRevisionID( long lRevisionID )
{
	if(lRevisionID>=0 && lRevisionID<FLAG_MAX_NUM)
	m_DirtyFlag[lRevisionID]++;
}

long GameLogic::LC_ClientGuildHomeMgr::GetDirtyFlagRevisionID( long lRevisionID)
{
	if (lRevisionID>=0 && lRevisionID<FLAG_MAX_NUM)
	{
		return m_DirtyFlag[lRevisionID];
	}
	return -1;
}

void GameLogic::LC_ClientGuildHomeMgr::ClearDirtyFlag()
{
	for (int i=0;i != FLAG_MAX_NUM;++i)
	{
		m_DirtyFlag[i] = -1;
	}
}

void GameLogic::LC_ClientGuildHomeMgr::_getGuildWareHousePackFromProtocol( const Protocol::PS_WarehousePack& kPack )
{
	m_kGuildWareHouse.Reset();
	m_kGuildWareHouse.GetFromProtocol(kPack);
}

void GameLogic::LC_ClientGuildHomeMgr::GetGuildWareHouseFromProtocol( const Protocol::MG_RLT_GUILDWAREHOUSE& rltMsg )
{
	_getGuildWareHousePackFromProtocol(rltMsg.m_stWareHouse);
	for(uint32_t i = 0; i < MAX_WAREHOUSE_PACK_COUNT; ++ i)
	{
		m_uiGuildWareHouseVersion[i] = rltMsg.m_uiWareHouseVersion[i];
	}
}

void GameLogic::LC_ClientGuildHomeMgr::GetKillThiefStoryCount( const Protocol::MG_RLT_CHANGE_THIEF_RECORD& rltMsg )
{
	m_iThiefCount[FLYTHIEF] = rltMsg.m_iFlyThiefNum;
	m_iThiefCount[FIRETHIEF] = rltMsg.m_iFireThiefNum;
	m_iThiefCount[KILLEDTHIEF] = rltMsg.m_iTotalKillThiefNum;
	m_iEXP = rltMsg.m_iEXP;
	m_iGuildContribute = rltMsg.m_iGuildContribute;
}

void GameLogic::LC_ClientGuildHomeMgr::RequestGetPrizeOfKillThief()
{

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkClientPlayer = pkLogicManager->GetOperationPlayer();
	if(!pkClientPlayer)
	{	
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_PRIZE_OF_KILLTHIEF, 0);
	if (bCreateMsg)
	{
		pkClientPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

long GameLogic::LC_ClientGuildHomeMgr::GetThiefCount( int iThiefType )
{
	if (iThiefType >= FLYTHIEF && iThiefType < MAX_THIEF)
	{
		return m_iThiefCount[iThiefType];
	}
	return 0;
}

bool GameLogic::LC_ClientGuildHomeMgr::GetIsInThief()
{
	 return m_bIsInThief; 
}

void GameLogic::LC_ClientGuildHomeMgr::SetIsInThief( bool val )
{
	 m_bIsInThief = val;
}

void GameLogic::LC_ClientGuildHomeMgr::RequestOpenKillThief()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkClientPlayer = pkLogicManager->GetOperationPlayer();
	if(!pkClientPlayer)
	{	
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_OPEN_KILLTHIEF, 0);
	if (bCreateMsg)
	{
		pkClientPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void GameLogic::LC_ClientGuildHomeMgr::RequestGetKillThiefInfo()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkClientPlayer = pkLogicManager->GetOperationPlayer();
	if(!pkClientPlayer)
	{	
		return;
	}
	uint32_t time = timeGetTime();
	if (time - m_uiQueryKillThiefTime < 2000)
	{
		SendLogicInfoMessage(RE_FAILED_GET_KILL_THIEF_INFO_TOO_FAST_FUNC);
		return;
	}
	else
	{
		m_uiQueryKillThiefTime = time;
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_KILLTHIEF_INFO, 0);
		if (bCreateMsg)
		{
			pkClientPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}
}


bool GameLogic::GuildWareHousePack::IsExistItem( long lItemTypeID )
{
	for(int i = 0; i < GetPackCapacity(); ++i)
	{
		const LC_BackPackEntry& kEntry = m_aData[i];

		if (kEntry.GetValid())
		{
			if (lItemTypeID == kEntry.GetTypeID())
			{
				return true;
			}			
		}
	}

	return false;
}

void GameLogic::GuildWareHousePack::Reset()
{
	for(int i = 0; i < GetPackCapacity(); ++i)
	{
		LC_BackPackEntry& kEntry = m_aData[i];
		kEntry.Reset();
	}
	m_iSize = 0;
}

void GameLogic::GuildWareHousePack::GetFromProtocol( const Protocol::PS_WarehousePack& kPack )
{
	for (std::size_t i = 0; i < kPack.m_aData.size(); ++i)
	{
		m_aData[kPack.m_aData[i].m_Index].GetFromProtocol(kPack.m_aData[i].m_Data);
	}
	m_iSize = kPack.m_iSize;
}

void GameLogic::GuildWareHousePack::SetToProtocol( Protocol::PS_WarehousePack& kPack )
{
	for (int32_t i = 0; i < GetPackCapacity(); ++i)
	{
		if(m_aData[i].IsModify())
		{
			Protocol::PS_ItemAssetIndexEntry entry;
			entry.m_Index = i;
			m_aData[i].SetToProtocol(entry.m_Data);
			kPack.m_aData.push_back(entry);
			m_aData[i].ClearModify();
		}
	}
}

GameLogic::GuildWareHousePack::GuildWareHousePack()
{
	Reset();
}

LC_BackPackEntry* GameLogic::GuildWareHousePack::GetBackPackEntry( int iEntryIndex )
{
	if (iEntryIndex < m_iSize && iEntryIndex >= 0)
	{
		return &m_aData[iEntryIndex];
	}
	return 0;
}
