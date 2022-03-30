#include "LC_ServerMapLogicBase.h"
#include "LC_ServerMap.h"

#include "LC_TranscriptionDataType.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerNPC.h"

#include "LC_ServerDuelFlow.h"
#include "MG_Common.h"
#include "LC_ServerMap.h"
#include "UT_GameServerLogManager.h"
#include "CF_WorldMapList.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerMapLogic_ActorManger.h"
#include "PF_ServerPathManager.h"
#include "LC_AutoFitNPCManager.h"
#include "LC_StoryLogic_Manager.h"
#include "ConfigManager.h"
#include "CF_TransferArea.h"
#include "UT_ServerHelper.h"

#include "GlobalSettings.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace GameLogic;
using namespace Protocol;
using namespace CSVFile;

void GenRandomPosition(const Utility::UT_Vec3Int& pkReferencePos, mem::vector<Utility::UT_Vec3Int>& posVec, int count, int resId)
{

	PF_ServerPathManager* pathmanager = PF_ServerPathManager::GetSingletonPtr();
	if (pathmanager == NULL)
	{
		return;
	}
	int addcount = 0;
	for (int iIndex = 0; addcount < count && iIndex < 100; ++iIndex)
	{
		UT_Vec3Int pos = pkReferencePos;
		Utility::UT_Vec3Int ret = Utility::UT_MathBase::MakeSpiralMatrixPosition(iIndex);
		pos.x += (ret.x * 110);
		pos.y += (ret.y * 110);
		if (resId >= 0)
		{
			bool bFindResult = pathmanager->IsPosValid(resId, pos);
			if (bFindResult)
			{
				posVec.push_back(pos);
				++addcount;
			}
		}
		else
		{
			posVec.push_back(pos);
			++addcount;
		}
	}
	while (addcount < count)
	{
		posVec.push_back(pkReferencePos);
		++addcount;
	}
}


//--------------------------------------------------------------
LC_ServerMapLogicBase::LC_ServerMapLogicBase()
	: LC_MapLogicBase()
	,	m_bActive(false)
	,	m_offlinetime(0)
	,   m_RecordPk(true)
	,   m_fLastUpdateInfoTime(0.0f)
	,	m_PlayerRespawnMap()
{
}
//--------------------------------------------------------------
LC_ServerMapLogicBase::~LC_ServerMapLogicBase(void)
{

}
//--------------------------------------------------------------
void LC_ServerMapLogicBase::Update(float fCurrentTime, float fDeltaTime)
{
	if (m_pkMap)
	{
		m_pkMap->Update(fCurrentTime, fDeltaTime);

		if (fCurrentTime > m_fLastUpdateInfoTime + 300.0f)
		{
			m_fLastUpdateInfoTime = fCurrentTime;
			int32_t iPlayerSize = m_pkMap->GetAllPlayerSize();

			if (iPlayerSize > 0)
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogicBase::Update map logic[%d] mapResID[%d] player[%d]", m_iMapLogicID, GetMapResID(), iPlayerSize);
		}
	}
}

void LC_ServerMapLogicBase::Init()
{
	LC_MapLogicBase::Init();
	m_bActive = false;
	m_RecordPk = true;
	m_fLastUpdateInfoTime = 0.0f;
	m_PlayerRespawnMap.clear();
}

void LC_ServerMapLogicBase::UnInit()
{
	m_bActive = false;
	m_RecordPk = true;
	m_fLastUpdateInfoTime = 0.0f;
	m_PlayerRespawnMap.clear();
	LC_MapLogicBase::UnInit();
}

//--------------------------------------------------------------
UT_Vec3Int LC_ServerMapLogicBase::ComputeEnterLocation(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, const Utility::UT_Vec3Int& rkTargetLocation)
{
	if (rkTargetLocation!=UT_Vec3Int(0, 0, 0) && (NULL==m_pkMap || m_pkMap->TestLocationValid(rkTargetLocation.x, rkTargetLocation.y)))
	{
		return rkTargetLocation;
	}
	UT_Vec3Int det(0, 0, 0);
	UT_ServerHelper::GetRandomLocationXYZ(det,GetMapResID(),pkPlayer->GetCitizenship(),UT_ServerHelper::IsInCross((LC_ServerPlayer*)pkPlayer));
	return det;
}
//---------------------------------------------------------------------------------
ResultType LC_ServerMapLogicBase::RequestChangeLocation(LC_PlayerBase* pkPlayer, LC_MapInOutType eInOutType, const UT_Vec3Int& rkTargetLocation)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;

	if (!CheckPlayerRespawn(pkServerPlayer->GetID(), eInOutType))
	{
		return RE_FAIL;
	}
	CF_WorldMapList::DataEntry* pkMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(GetMapResID());
	if (NULL == pkMapEntry)
	{
		return RE_FAIL;
	}

	//原地复活
	if (eInOutType == LMIOT_CURRENT_RESPAWN)
	{
		LC_ServerPlayer* pOwner = pkServerPlayer->GetOwnerPlayer();
		if (pOwner == NULL)
		{
			return RE_FAIL;
		}
		bool bRecoverMode = pkServerPlayer->GetOwnerReliveFlag_Recover();
		bool bIsNewbeePlayer = (pkServerPlayer->GetLevel() < MIN_NOT_NEWBIE_LEVEL);
		if (!bIsNewbeePlayer)
		{
			//检查消耗品
			if (pkServerPlayer->GetBuddyReliveFlag() && !bRecoverMode )
			{
				if ( pkServerPlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_BASIC, GetGlobalSetting.nBuddyReliveId) < GetGlobalSetting.nBuddyReliveCount)
				{
					pkServerPlayer->Send_SC_BuddyReply(0,RE_BUDDY_ERROR_RESPAWN_LESS_ITEM,0,0);
					return RE_FAIL;
				}
			}

			LC_ActorAttributeMap* pkAttributeMap = pkServerPlayer->GetFinalAttributeMap();
			if (!pkAttributeMap)
			{
				return RE_FAIL;
			}

			//普通地图原地复活HP、MP回复30%
			attr_value_type lHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
			int lMP = (int)(pkAttributeMap->GetAttribute(ATT_MAX_MP));

			pkServerPlayer->Respawn(lHP, lMP);

			if (GetGlobalSetting.nBuddyReliveCount > 0 && pkServerPlayer->GetBuddyReliveFlag() && !bRecoverMode)
			{
				int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, RELIVE_OPERATE, OPERATE_LOGTYPE1);
				PackOperateInfoList objOPList;
				PackOperateInfo objData;
				objData.Reset();
				objData.m_iOPType = OPT_DELETE_ITEM_BY_ID;
				objData.m_bOPEntry   = false;
				objData.m_lItemID    = GetGlobalSetting.nBuddyReliveId;
				objData.m_lCount     = GetGlobalSetting.nBuddyReliveCount;
				objOPList.push_back(objData);

				LC_Helper::DoItemOPs(pkServerPlayer, objOPList, SYS_FUNC_TP_RESPAWN, pkServerPlayer->GetID(), true, true, nLogCode);
			}

			//发送网络应答
			MG_RLT_Respawn rltMsg;
			rltMsg.m_lPlayerID = pkPlayer->GetID();
			rltMsg.m_iRespawnType = MGPT_REQ_RESPAWN_ON_CURRENT_LOCATION;
			rltMsg.m_iHP = lHP;
			rltMsg.m_iMP = lMP;
			rltMsg.m_iReduceExp = 0;//UT_MathBase::MinInt(iReduceExp,lPlayerEXP);
			rltMsg.m_iReduceBindCash = 0;//iReduceBindCash;

			//pkServerPlayer->SendMsgToClient(MGPT_RLT_RESPAWN, &rltMsg);
			pkServerPlayer->BroadCastToArea(MGPT_RLT_RESPAWN, &rltMsg);
		}
		else
		{
			LC_ActorAttributeMap* pkAttributeMap = pkPlayer->GetFinalAttributeMap();
			if (!pkAttributeMap)
				return RE_FAIL;

			//满血满魔
			attr_value_type lHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
			int lMP = (int)(pkAttributeMap->GetAttribute(ATT_MAX_MP));
			pkPlayer->Respawn(lHP, lMP);

			//发送网络应答
			MG_RLT_Respawn rltMsg;
			rltMsg.m_lPlayerID = pkPlayer->GetID();
			rltMsg.m_iRespawnType = MGPT_REQ_RESPAWN_ON_CURRENT_LOCATION;
			rltMsg.m_iHP = lHP;
			rltMsg.m_iMP = lMP;
			rltMsg.m_iReduceExp = 0;

			//pkServerPlayer->SendMsgToClient(MGPT_RLT_RESPAWN, &rltMsg);
			pkServerPlayer->BroadCastToArea(MGPT_RLT_RESPAWN, &rltMsg);
		}
		if (pkMapEntry->_iRespawnSafeTime > 0)
		{
			pkServerPlayer->ActiveSkillState(BAOHU_BUFF, pkMapEntry->_iRespawnSafeTime, 0);
		}
	}
	else if (eInOutType == LMIOT_SAFE_RESPAWN)
	{
		bool bIsNewbeePlayer = (pkServerPlayer->GetLevel() < MIN_NOT_NEWBIE_LEVEL);
		if (!bIsNewbeePlayer)
		{
			LC_ActorAttributeMap* pkAttributeMap = pkPlayer->GetFinalAttributeMap();
			if (!pkAttributeMap)
			{
				return RE_FAIL;
			}
			double hpRate = pkMapEntry->_iSafeRespawnHP * 0.0001f;
			if (pkPlayer->GetAutoSafeRelive())
				hpRate = 1.0000f;
			attr_value_type lHP = (attr_value_type)(pkAttributeMap->GetAttribute(ATT_MAX_HP) * hpRate);
			int lMP = (int)(pkAttributeMap->GetAttribute(ATT_MAX_MP) * 0.3);

			pkPlayer->Respawn(lHP, lMP);

			//返回复活请求
			MG_RLT_Respawn rltMsg;
			rltMsg.m_lPlayerID		= pkPlayer->GetID();
			rltMsg.m_iRespawnType	= MGPT_REQ_RESPAWN_ON_SAFE_LOCATION;
			rltMsg.m_iHP			= lHP;
			rltMsg.m_iMP			= lMP;
			rltMsg.m_iReduceExp = 0;//UT_MathBase::MinInt(iReduceExp,lPlayerEXP);
			rltMsg.m_iReduceBindCash = 0;//iReduceBindCash;

			//pkServerPlayer->SendMsgToClient(MGPT_RLT_RESPAWN, &rltMsg);
			pkServerPlayer->BroadCastToArea(MGPT_RLT_RESPAWN, &rltMsg);
		}
		else
		{
			//清空复活惩罚
			/*pkPlayer->SetRespawnPunishStartTime(0);
			pkPlayer->SetRespawnPunishLevel(0);*/

			LC_ActorAttributeMap* pkAttributeMap = pkPlayer->GetFinalAttributeMap();
			if (!pkAttributeMap)
			{
				return RE_FAIL;
			}

			//新手复活满HP、MP
			attr_value_type	lHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
			int	lMP = (int)(pkAttributeMap->GetAttribute(ATT_MAX_MP));

			pkPlayer->Respawn(lHP, lMP);

			//返回复活请求
			MG_RLT_Respawn rltMsg;
			rltMsg.m_lPlayerID		= pkPlayer->GetID();
			rltMsg.m_iRespawnType	= MGPT_REQ_RESPAWN_ON_SAFE_LOCATION;
			rltMsg.m_iHP			= lHP;
			rltMsg.m_iMP			= lMP;
			rltMsg.m_iReduceExp		= 0;
			//pkServerPlayer->SendMsgToClient(MGPT_RLT_RESPAWN, &rltMsg);
			pkServerPlayer->BroadCastToArea(MGPT_RLT_RESPAWN, &rltMsg);
		}
		if (pkMapEntry->_iRespawnSafeTime > 0)
		{
			pkServerPlayer->ActiveSkillState(BAOHU_BUFF, pkMapEntry->_iRespawnSafeTime, 0);
		}
	}

	return LC_MapLogicBase::RequestChangeLocation(pkPlayer, eInOutType, rkTargetLocation);
}
//--------------------------------------------------------------------------------
ResultType LC_ServerMapLogicBase::RequestEnterMap(LC_PlayerBase* pkPlayer, LC_MapInOutType eInOutType, const UT_Vec3Int& rkTargetLocation)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
	if(NULL == pkServerPlayer)
	{
		return RE_FAIL;
	}

	//打开玩家数据存储功能
	pkServerPlayer->SetDBSaveMaskFlag(LPDST_CHAR_MAX_COUNT, true);
	//切换玩家在世界中的位置
	int32_t nMapLogicId = GetMapLogicID();
	UT_Vec3Int enterPos = rkTargetLocation;
	if (pkServerPlayer->IsClone())
	{
		nMapLogicId = pkServerPlayer->GetAsAControlee().m_MapLogicID;
		enterPos = pkServerPlayer->GetAsAControlee().m_InitPos;
		pkServerPlayer->SetCurrentLocation(enterPos);
	}
	pkServerPlayer->AllocateBuddy(nMapLogicId);
	pkServerPlayer->ChangeMap(nMapLogicId, GetMapResID(), GetMapLogicType(), enterPos);
	pkServerPlayer->PostEnterMap(nMapLogicId, GetMapResID(), GetMapLogicType());

	return RE_SUCCESS;
}
//--------------------------------------------------------------------------------
ResultType LC_ServerMapLogicBase::RequestLeaveMap(LC_PlayerBase* pkPlayer, LC_MapInOutType eInOutType, CSVFile::CF_WorldMapList::DataEntry* pkDataEntry)
{
	return RE_SUCCESS;
}

void LC_ServerMapLogicBase::NPCDead(LC_ServerNPC* pkServerNPC)
{
	if (pkServerNPC == NULL)
	{
		return;
	}
	//经验奖励
	LC_Helper::RewardExpOnNPCDead(pkServerNPC, pkServerNPC->GetNPCOwnerList().GetOwnerIDVector());
	//好友度奖励
	//LC_Helper::RewardFriendShipOnNPCDead(pkServerNPC,pkServerNPC->GetNPCOwnerList().GetOwnerIDVector());
}
//------------------------------------------------------------------------------
void LC_ServerMapLogicBase::DropItemsOnNPCDead(LC_ServerNPC* pkServerNPC)
{
}
//----------------------------------------------------------------
void LC_ServerMapLogicBase::PlayerBeKilled(LC_ServerPlayer* pkServerPlayer, object_id_type lSrcActorID)
{
	UpdatePlayerRespawn(pkServerPlayer->GetID());
}

bool LC_ServerMapLogicBase::RestartLogic()
{
	return true;
}

//--------------------------------------------------------------
void LC_ServerMapLogicBase::PlayerChangeLogicArea(LC_ServerPlayer* player, int prearea, int curarea)
{
	return;
}
void LC_ServerMapLogicBase::UpdatePlayerRespawn(object_id_type playerid)
{
	mem::map<int, PlayerRespawnData>::iterator it = m_PlayerRespawnMap.find(playerid);
	if (it == m_PlayerRespawnMap.end())
	{
		PlayerRespawnData& temp = m_PlayerRespawnMap[playerid];
		temp.m_PlayerId = playerid;
		temp.m_DeadTime = 1;
		temp.m_CurRespawnTime = 5;
		temp.m_SafeRespawnTime = 0;
		temp.m_TimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	}
	else
	{
		++it->second.m_DeadTime;
		it->second.m_TimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	}
}

PlayerRespawnData* LC_ServerMapLogicBase::GetPlayerRespawn(object_id_type playerid)
{
	mem::map<int, PlayerRespawnData>::iterator it = m_PlayerRespawnMap.find(playerid);
	return (it != m_PlayerRespawnMap.end()) ? &(it->second) : NULL;
}

void LC_ServerMapLogicBase::GetRespawnTime(object_id_type playerid, int& currespawntime, int& saferespawntime)
{
	PlayerRespawnData* thedata = GetPlayerRespawn(playerid);
	if (thedata == NULL)
	{
		return;
	}
	time_t span = GET_CURRENT_TIMESTAMP_IN_SECONDS() - thedata->m_TimeStamp;
	if (span >= thedata->m_CurRespawnTime)
	{
		currespawntime = 0;
	}
	else
	{
		currespawntime = (int)(thedata->m_CurRespawnTime - span);
	}

	if (span >= thedata->m_SafeRespawnTime)
	{
		saferespawntime = 0;
	}
	else
	{
		saferespawntime = (int)(thedata->m_SafeRespawnTime - span);
	}
}

bool LC_ServerMapLogicBase::CheckPlayerRespawn(object_id_type playerid, GameLogic::LC_MapInOutType eInOutType)
{
	//原地复活
	if (eInOutType == LMIOT_CURRENT_RESPAWN)
	{
		return true;
	}
	else
	{
		PlayerRespawnData* thedata = GetPlayerRespawn(playerid);
		if (thedata == NULL)
		{
			return true;
		}
		else
		{
			time_t span = GET_CURRENT_TIMESTAMP_IN_SECONDS() - thedata->m_TimeStamp;
			return span > thedata->m_SafeRespawnTime ? true :false;
		}
	}
}

void LC_ServerMapLogicBase::SetActive(bool flag)
{ 
	m_bActive = flag;
}

void LC_ServerMapLogicBase::BroadcastMsgToClient(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody)
{
	LC_ServerMap* pkServerMap = GetServerMap();
	if (pkServerMap == NULL)
	{
		return;
	}
	pkServerMap->BroadCastToAll(usMsgType, pkMsgBody);
}

void LC_ServerMapLogicBase::BroadcastMsgToServerClient(int32_t nServerId, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody)
{
	LC_ServerMap* pkServerMap = GetServerMap();
	if (pkServerMap == NULL)
	{
		return;
	}

	LC_SimPlayerFilter kFilter;
	kFilter.SetNation(nServerId, false);
	pkServerMap->BroadCastToServer(usMsgType, pkMsgBody, &kFilter);
}

bool LC_ServerMapLogicBase::BroadCastGameEventToClient(TranscriptionEventType pkSendType, int64_t lEventValue1, int64_t lEventValue2, int64_t lEventValue3, int64_t lEventValue4, int64_t lEventValue5, int64_t lEventValue6)
{
	LC_ServerMap* pkServerMap = GetServerMap();

	if (pkServerMap)
	{
		MG_PB<msg_define::game_event> EventMsg;
		msg_define::game_event& msg_val = EventMsg.m_value;
		msg_val.set_event_type(GLET_TRANSCRIPTION_EVENT);
		msg_val.add_event_param(pkSendType);
		msg_val.add_event_param(lEventValue1);
		msg_val.add_event_param(lEventValue2);
		msg_val.add_event_param(lEventValue3);
		msg_val.add_event_param(lEventValue4);
		msg_val.add_event_param(lEventValue5);
		msg_val.add_event_param(lEventValue6);
		pkServerMap->BroadCastToAll(MGPT_RLT_GAME_EVENT, &EventMsg);		
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------
bool LC_ServerMapLogicBase::NotifyGameEventToClient(LC_ServerPlayer* pkServerPlayer, GameLogic::TranscriptionEventType pkSendType, int64_t lEventValue1, int64_t lEventValue2, int64_t lEventValue3, int64_t lEventValue4, int64_t lEventValue5, int64_t lEventValue6)
{
	if (NULL == pkServerPlayer)
	{
		return false;
	}
	MG_PB<msg_define::game_event> EventMsg;
	msg_define::game_event& msg_val = EventMsg.m_value;
	msg_val.set_event_type(GLET_TRANSCRIPTION_EVENT);
	msg_val.add_event_param(pkSendType);
	msg_val.add_event_param(lEventValue1);
	msg_val.add_event_param(lEventValue2);
	msg_val.add_event_param(lEventValue3);
	msg_val.add_event_param(lEventValue4);
	msg_val.add_event_param(lEventValue5);
	msg_val.add_event_param(lEventValue6);
	pkServerPlayer->SendMsgToClient(MGPT_RLT_GAME_EVENT, &EventMsg);	
	return true;
}
