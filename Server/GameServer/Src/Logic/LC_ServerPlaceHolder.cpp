#include "LC_ServerPlaceHolder.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ItemFactoryBase.h"
#include "CF_GuildWarBuff.h"
#include "oss_define.h"
#include "NW_Helper.h"
#include "GameServerApp.h"
#include "LC_RumorManager.h"
#include "GlobalSettings.h"
#include "UT_ServerHelper.h"
#include "CF_WorldMapList.h"
#include "UT_OperateLog.h"
using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
using namespace CSVFile;
using namespace OPERATELOG;

#define	ROLL_ABORT_POINT_ABANDON -1

//-----------------------------------------------------
LC_ServerPlaceHolder::LC_ServerPlaceHolder(int protected_time): LC_PlaceHolderBase(),m_MapID(0),m_protected_time(protected_time),m_nNpcId(0)
{
}
//-----------------------------------------------------
LC_ServerPlaceHolder::~LC_ServerPlaceHolder()
{

}
//-------------------------------------------
bool LC_ServerPlaceHolder::Empty()
{
	return LC_Reward::RT_NONE == GetReward().m_eType;
}
//-------------------------------------------
void LC_ServerPlaceHolder::Update( float fCurrentTime,float fDeltaTime )
{
	_updateItemsTimeout(fCurrentTime);
}
//-------------------------------------------
void AddResult(Protocol::MG_RLT_PickUpPlaceHolder& msg, uint32_t id, uint32_t rel)
{
	ResultInfo info;
	info.m_lID = id;
	info.m_Result = rel;
	msg.m_Results.push_back(info);
}
//-------------------------------------------
void LC_ServerPlaceHolder::PickupGold(LC_ServerPlayer* pkPlayer , Protocol::MG_RLT_PickUpPlaceHolder& msg)
{
	//oss_role_pickup(pkPlayer, m_Reward.GetGoldCount());
	//捡取金币--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(pkPlayer->GetMapResID());
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, 0);
	if( pkPlayer->AddCash(m_Reward.GetGoldCount(), CT_UNBIND_CASH, SYS_FUNC_TP_PICKUP, 0, 0,true, NULL, nLogCode, nParams) )
	{
		AchievementEvent kEvent;
		kEvent.SetNum(m_Reward.GetGoldCount());
		pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_PICKCASH, kEvent);

		m_Reward.Clear();
		AddResult(msg, GetID(),RE_CHEST_PICKUP_SUCCESS);
	}
	else
	{
		AddResult(msg, GetID(),RE_CHEST_MONEY_FULL);
	}
}

/*
void LC_ServerPlaceHolder::PickupSP(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg)
{
  pkPlayer->AddSkillLevelupPoints(m_Reward.GetSPCount(), SYS_FUNC_TP_PICKUP, 0);
  m_Reward.Clear();
  AddResult(msg, GetID(),RE_CHEST_PICKUP_SUCCESS);
}
*/

void LC_ServerPlaceHolder::PickupBossJP(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg)
{
	int count = m_Reward.GetBossJP();
	int32_t maxCount = GetGlobalSetting.BossJPCollectCount;	
	if(-1 != maxCount)
	{
		//检测今日新增精魄是否到达上限
		int32_t addMaxCount = maxCount - pkPlayer->GetCollectBossJP();
		if(addMaxCount <= 0)
		{
			m_Reward.Clear();
			AddResult(msg, GetID(),RE_BOSSJP_BE_LIMITED);
			return;
		}
		count = count < addMaxCount ? count : addMaxCount;
	}
	//捡取BOOJP--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(pkPlayer->GetMapResID());
	//nParams.push_back(GetNPCID());
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, 0);
	if( pkPlayer->AddCash(count, CT_BOSS_JP, SYS_FUNC_TP_PICKUP, 0,0,true,NULL,nLogCode, nParams) )
	{
		pkPlayer->AddCollectBossJP(count);
		m_Reward.Clear();
		AddResult(msg, GetID(),RE_CHEST_PICKUP_SUCCESS);			
	}
	else
	{
		AddResult(msg, GetID(),RE_CHEST_21);
	}
}

void LC_ServerPlaceHolder::PickupUnbindyuanbao(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg)
{
	//捡取--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(pkPlayer->GetMapResID());
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, 0);
	if( pkPlayer->AddCash(m_Reward.GetUnbindyuanbaoCount(), CT_UNBIND_YUANBAO, SYS_FUNC_TP_PICKUP, 0,0,true, NULL, nLogCode, nParams) )
	{
		m_Reward.Clear();
		AddResult(msg, GetID(),RE_CHEST_PICKUP_SUCCESS);
	}
	else
	{
		AddResult(msg, GetID(),RE_CHEST_UNBINDYUANBAO);
	}
}

void LC_ServerPlaceHolder::PickupRuneCream(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg)
{
	//捡取--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(pkPlayer->GetMapResID());
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, 0);
	if( pkPlayer->AddCash(m_Reward.GetRuneCreamCount(), CT_RUNE_CREAM, SYS_FUNC_TP_PICKUP, 0, 0, true, NULL, nLogCode,nParams) )
	{
		m_Reward.Clear();
		AddResult(msg, GetID(),RE_CHEST_PICKUP_SUCCESS);
	}
	else
	{
		AddResult(msg, GetID(),RE_CHEST_22);
	}
}

void LC_ServerPlaceHolder::PickupEffect(LC_ServerPlayer* pkPlayer, Protocol::MG_RLT_PickUpPlaceHolder& msg)
{
	int effect_id = m_Reward.GetEffectID();
	CF_GuildWarBuff::DataEntry * entry = SafeGetCSVFileDataEntryPtr<CF_GuildWarBuff>(effect_id,false);

	if(entry && pkPlayer)
	{
		switch(entry->_Type)
		{
			case 1:
			{
				CF_SkillStateList::DataEntry * sentry = SafeGetCSVFileDataEntryPtr<CF_SkillStateList>(entry->_State);
				if (sentry)
				{
					pkPlayer->ActiveSkillState(entry->_State,entry->_StateTime,pkPlayer->GetID(),sentry->_fStateConstParameter1,sentry->_fStateConstParameter2,sentry->_fStateConstParameter3,sentry->_fStateConstParameter4,sentry->_fStateConstParameter5);
				}
				break;
			}
			case 2:
			{
				pkPlayer->AddHP(entry->_Value);
				break;
			}
			case 3:
			{
				pkPlayer->AddMP(entry->_Value);
				break;
			}
			case 4:
			{
				pkPlayer->AddStamina(entry->_Value);
				break;
			}
			default:
				break;
		}

	}

	m_Reward.Clear();
	AddResult(msg, GetID(),RE_CHEST_PICKUP_SUCCESS);
}

//-------------------------------------------
void LC_ServerPlaceHolder::RequestPickupItem(LC_ServerPlayer* pkPlayer , Protocol::MG_RLT_PickUpPlaceHolder& msg, bool checkCD)
{
	IF_A_NA_PLAYER(pkPlayer)
		return;

	if (!pkPlayer->IsInGame())
	{
		return;
	}

	if(WALLOW == pkPlayer->GetAntiWallowState())
	{
		AddResult(msg, GetID(),RE_CHEST_CANT_PICK_WHEN_WALLOW);
		return;
	}

	LC_ServerPlayer* pCurrentPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pCurrentPlayer == NULL)
		return;

	int Distance = 560;
	if(pkPlayer->GetKillNPCExpFlag())
	{
		Distance = GetGlobalSetting.PickUpItemDistance;
	}
	else
	{
		if(pCurrentPlayer->GetMapLogicType() == MT_EIGHTEEN_BRONZE_MAN)
		{//当前在拾取副本中的掉落
			Distance = GetGlobalSetting.PickUpItemDistance;
		}
		else if( m_Reward.m_eType == LC_Reward::RT_GOLD)
		{
			Distance = (int)pkPlayer->GetFinalAttributeMap()->GetAttribute(ATT_PICK_CRASH_RNG);
		}
		/*
		else if( m_Reward.m_eType == LC_Reward::RT_SP)
		{
			//因为客户端要做一个真气的假的运动动画，所以服务器拾取真气求的范围要大，这样表现才会正确
			Distance = DROP_REAL_PICK_SP_RNG;
		}
		*/
		else if( m_Reward.m_eType == LC_Reward::RT_BOSS_JP)
		{
			//因为客户端要做一个真气的假的运动动画，所以服务器拾取真气求的范围要大，这样表现才会正确
			Distance = DROP_REAL_PICK_SP_RNG;
		}
		else if( m_Reward.m_eType == LC_Reward::RT_EFFECT)
		{
			Distance = DROP_AUTO_PICK_EFFECT;
		}
		else if(m_Reward.m_eType == LC_Reward::RT_UNBINDYUANBAO || m_Reward.m_eType == LC_Reward::RT_RUNE_CREAM)
		{
			//钻石的拾取范围
			Distance = (int)pkPlayer->GetFinalAttributeMap()->GetAttribute(ATT_PICK_CRASH_RNG);
		}
	}
	
	int iDistance = UT_MathBase::LineLengthXYInt(GetCurrentLocation(),pCurrentPlayer->GetCurrentLocation());
	if(iDistance > Distance)
	{
		AddResult(msg, GetID(), RE_CHEST_TOO_FAR_TO_PICK);
		return;
	}
	
	if(Empty())
	{
		AddResult(msg, GetID(), RE_CHEST_22);
		return;
	}

	object_id_type lPlayerID = pkPlayer->GetID();

	//是否是宝箱开启者
	if(!IsOwner(lPlayerID))
	{
		AddResult(msg, GetID(), RE_CHEST_BOX_PROTECTED);
		return;
	}

	bool ret = true;
	if(GameLogic::LC_Reward::RT_GOLD == m_Reward.m_eType)
	{
		PickupGold(pkPlayer,msg);
	}
	/*
	if(GameLogic::LC_Reward::RT_SP == m_Reward.m_eType)
	{
		PickupSP(pkPlayer,msg);
	}
	*/
	if(GameLogic::LC_Reward::RT_BOSS_JP == m_Reward.m_eType)
	{
		PickupBossJP(pkPlayer,msg);
	}
	else if(GameLogic::LC_Reward::RT_EFFECT == m_Reward.m_eType)
	{
		PickupEffect(pkPlayer,msg);
	}
	else if(GameLogic::LC_Reward::RT_UNBINDYUANBAO == m_Reward.m_eType)
	{
		PickupUnbindyuanbao(pkPlayer,msg);
	}
	else if(GameLogic::LC_Reward::RT_RUNE_CREAM == m_Reward.m_eType)
	{
		PickupRuneCream(pkPlayer,msg);
	}
	else if(GameLogic::LC_Reward::RT_ITEM == m_Reward.m_eType)
	{
		ret = _pickUpItem(pkPlayer,msg, checkCD);
	}
}
//-------------------------------------------
bool LC_ServerPlaceHolder::_pickUpItem(LC_ServerPlayer* pkPlayer , Protocol::MG_RLT_PickUpPlaceHolder& msg, bool checkCD)
{
	LC_PackAsset* pkBackPack = pkPlayer->GetPackAsset();
	if(pkBackPack)
	{
		int nRet = _performAddToPackage(pkPlayer->GetID(),msg, checkCD);
		if(RE_CHEST_PICKUP_SUCCESS == nRet)
		{
			m_Reward.Clear();
			return true;
		}
	}
	return false;
}
//-------------------------------------------
void LC_ServerPlaceHolder::_sendMsgToPlayer(object_id_type lPlayerID,int32_t iMsgType,Protocol::MG_BODY* pkMsgBody)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer)
	{
		pkPlayer->SendMsgToClient(iMsgType, pkMsgBody);
	}
}
//-------------------------------------------
LC_ServerPlayer* LC_ServerPlaceHolder::_findPlayer(object_id_type lPlayerID)
	{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lPlayerID);
	return pkNetPlayer;	
}
//-------------------------------------------
void LC_ServerPlaceHolder::_updateItemsTimeout(float fTime)
{
	if( m_Private == false)
	{
		if(fTime - m_fLifeStartTime > (float)m_protected_time)
		{
			this->m_vOwnerList.clear();
		}
	}
}
//-------------------------------------------
int LC_ServerPlaceHolder::_performAddToPackage(object_id_type lPlayer , Protocol::MG_RLT_PickUpPlaceHolder& msg, bool checkCD)
{
	int nRet = PR_PICK_OK;

	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayer);
	if (NULL == pkPlayer)
		return  PR_PICK_FAIL;

	LC_PackAsset* pkBackPack = NULL;
	pkBackPack = pkPlayer->GetPackAsset();

	if(pkBackPack)
	{
		ResultType iResult;

		int lTypeID = m_Reward.m_kContent.GetBackPackValueByType(LPSFET_TYPE_ID);
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		if (NULL == pkItemFactory)
			return  PR_PICK_FAIL;

		LC_ItemBase* pkItem = pkItemFactory->RequestItem(lTypeID);
		if (pkItem == NULL)
			return  PR_PICK_FAIL;

		CF_ItemList::DataEntry* pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lTypeID);
		if (pkItemData == NULL)
			return  PR_PICK_FAIL;

		if(checkCD)
		{
			float fCurrentTime = GET_PROCESS_TIME;
			if (fCurrentTime < pkPlayer->GetLastPickItemTime() + GetGlobalSetting.nPickUpItemCDTime)
			{
				return PR_PICK_FAIL;
			}
			pkPlayer->SetLastPickItemTime(fCurrentTime);
		}
		
		//捡取--埋点日志
		vector<int> nParams;
		nParams.clear();
		nParams.push_back(pkPlayer->GetMapResID());
		int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, 0);

		int nPack = GetWhichPackAutoPut(lTypeID);
		PackOperateInfoList objOPList;

		PackOperateInfo objData;
		objData.Reset();

		objData.m_iOPType = OPT_ADD_ITEM;
		objData.m_bOPEntry = true;
		objData.m_bMerger  = true;
		objData.m_iPackType  = nPack;
		objData.m_kPackEntry.Clone(GetReward().m_kContent);
		objOPList.push_back(objData);
		
		iResult = pkBackPack->ExecutePackOP(objOPList,true, NULL, nLogCode,nParams);
		if (iResult != RE_SUCCESS)
		{
			//临时修改以后提示需要规划
			if (iResult == RE_ITEM_OWN_UNIQUE)
				AddResult(msg, GetID(),RE_ITEM_OWN_UNIQUE);
			else
				nRet = PR_BACKPACK_FULL;
			//break;
		}
		else
		{
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
			PackOPRecordSet * record_set = pkBackPack->GetRecordSet();
			pkPlayer->HandlePackOPRecord(record_set, SYS_FUNC_TP_PICKUP, 0);
		}
	}
	
	if (nRet == PR_BACKPACK_FULL)
	{
		const unique_id_impl& playerInstance = pkPlayer->GetInstance();
		if(pkPlayer->GetKillNPCExpPercent()-0.00001>1.0)
		{
			LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
			if(NULL == pkOwnerPlayer)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_performAddToPackage serverid:%d, c_id:%d, c_citizenship:%d, c_uid:%d r1", 
					GetGameServerID(), playerInstance.detail(), playerInstance.catagory(), pkPlayer->GetUserID());
				return nRet;
			}

			//激活了绝版皮肤，背包不够，地图的_bAutoMail字段配1，发邮件
			int mapResID = pkPlayer->GetMapResID();
			CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapResID);
			if(NULL==pkData || 1!=pkData->_bAutoMail)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_performAddToPackage serverid:%d, c_id:%d, c_citizenship:%d, c_uid:%d, mapResID:%d r2", GetGameServerID(), playerInstance.detail(), playerInstance.catagory(), pkPlayer->GetUserID(), mapResID);
				return nRet;
			}

			UT_ServerHelper::__ItemCnt item;
			item.m_nId = m_Reward.m_kContent.GetBackPackValueByType(LPSFET_TYPE_ID);
			item.m_nCnt = 1;

			UT_ServerHelper::LC_ItemCntList rewards;
			rewards.push_back(item);

			UT_ServerHelper::SendToMail("85;", "86;", pkOwnerPlayer->GetInstance(), pkOwnerPlayer->GetOwnerCharName(), 0, rewards);
			AddResult(msg, GetID(),RE_ITEM_PACKAGE_FULL_SEND_MAIL);
			nRet = RE_ITEM_PACKAGE_FULL_SEND_MAIL;
			m_Reward.m_eType = LC_Reward::RT_NONE;
		}
		else if(pkPlayer->GetMapLogicType() == MT_EIGHTEEN_BRONZE_MAN)
		{
			//发送邮件
			LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
			if(NULL == pkOwnerPlayer)
				return nRet;
			
			UT_ServerHelper::__ItemCnt item;
			item.m_nId = m_Reward.m_kContent.GetBackPackValueByType(LPSFET_TYPE_ID);
			item.m_nCnt = 1;

			UT_ServerHelper::LC_ItemCntList rewards;
			rewards.push_back(item);

			UT_ServerHelper::SendToMail("87;", "88;", pkOwnerPlayer->GetInstance(), pkOwnerPlayer->GetOwnerCharName(), 0, rewards);
			AddResult(msg, GetID(),RE_ITEM_PACKAGE_FULL_SEND_MAIL);
			nRet = RE_ITEM_PACKAGE_FULL_SEND_MAIL;
			m_Reward.m_eType = LC_Reward::RT_NONE;
		}
		else
		{
			AddResult(msg, GetID(),RE_CHEST_FULL);
		}
	}

	if (nRet == PR_PICK_OK)
	{
		AddResult(msg, GetID(),RE_CHEST_PICKUP_SUCCESS);
		m_Reward.m_eType = LC_Reward::RT_NONE;
	}
	return nRet;
}

//-------------------------------------------
bool LC_ServerPlaceHolder::_isDistanceAndStateValidToHold(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
		return false;

	if(HEALTHINESS != pkPlayer->GetAntiWallowState())
		return false;

	if(pkPlayer->GetMapLogicID() != GetMapLogicID())
		return false;

	int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(),GetCurrentLocation());

	if (iDistance >= CHEST_MAX_DIST_TO_HOLD)
		return false;
	else
		return true;
}
//-------------------------------------------
bool LC_ServerPlaceHolder::_isDistanceAndStateValidToRoll(object_id_type lPlayerID,int lWeaponType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
		return false;
	
	if(HEALTHINESS != pkPlayer->GetAntiWallowState())
		return false;
	
	if(pkPlayer->GetMapLogicID() != GetMapLogicID())
		return false;
	
	int iDistance = 0;
	int lMapLogicType = GetMapLogicType();

	if(lMapLogicType >= MT_TRANSCRIPTION_BEGIN && lMapLogicType <= MT_TRANSCRIPTION_END)
		iDistance = 0;
	else
		iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(),GetCurrentLocation());

	if (iDistance >= CHEST_MAX_DIST_TO_ROLL)
		return false;
	
	if(lWeaponType != WT_NONE)
	{
		if(pkPlayer->GetWeaponAttribute() != lWeaponType)
			return false;
	}

	return true;
}

