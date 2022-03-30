#include "LC_ServerPlayer.h"
#include "MG_GameProtocol.h"
#include "CF_FlowerConfig.h"
#include "CF_FlowerSendLimitCfg.h"
#include "MG_Game2Group.h"
#include "GameServerApp.h"
#include "LC_RankManager.h"
#include "LC_ServerLogicManager.h"
#include "CF_OffLineExp.h"
#include "CF_OffLineExpRatio.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "UT_Func.h"
#include "CF_WorldMapList.h"
#include "ajson.hpp"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "MG_LoginProtocol.h"
#include "game_version.h"
#include "LC_ServerAllianceManager.h"
#include "NewUserEvent.h"
#include "oss_define.h"
#include "ACT_ActivityDataManager.h"
#include "GlobalSettings.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace GameLogic;
using namespace Protocol;
using namespace CSVFile;

void LC_ServerPlayer::SetAdult(bool value)
{
	// 这么写，只是为了应付版署
	if ( m_kUserName	 == "dev001" 
		|| m_kUserName	 == "dev002" 
		|| m_kUserName	 == "dev003" 
		|| m_kUserName	 == "dev004" 
		|| m_kUserName	 == "dev005" 
		|| m_kUserName	 == "dev006" 
		|| m_kUserName	 == "dev007" 
		|| m_kUserName	 == "dev008" 
		|| m_kUserName	 == "dev009" 
		|| m_kUserName	 == "dev010" 
		)
	{
		value = true;
	}
	if(m_Adult == value)
	{
		return;
	}
	m_Adult = value;
	if(false == value)
	{
		SetAntiWallowState(HEALTHINESS);
		SetAnitWallowOnlineTime(0);
	}
}

void LC_ServerPlayer::SetAntiWallowState(AnitWallowState state)
{
	if( m_antiWallowState != state )
	{
		m_antiWallowState = state;
		Sync_AnitWallowOnlineTime();
	}
}

void LC_ServerPlayer::Sync_AnitWallowOnlineTime()
{
	if (!IsMainController())
		return;
	MG_RLT_Anti_Wallow antiWallowMsg;
	antiWallowMsg.m_anitWallowState = m_antiWallowState;
	antiWallowMsg.m_adult = m_Adult;
	antiWallowMsg.m_onlineTime = m_anitWallowOnlineTime;
	SendMsgToClient(MGPT_RLT_ANTI_WALLOW,&antiWallowMsg);
} 

void LC_ServerPlayer::SetAnitWallowOnlineTime(int32_t value)
{
	m_anitWallowOnlineTime = value;
	if(false == m_Adult)
	{
		return;
	}
	AnitWallowState wallowStatus;
	//if (m_anitWallowOnlineTime < ANTIWALLOW_INTERVAL*3)
	int nWallowTimeout = GetGlobalSetting.nWallowTimeout_None;
	if (m_anitWallowOnlineTime < nWallowTimeout)
	{
		wallowStatus = HEALTHINESS;
	}
	//else if (m_anitWallowOnlineTime < ANTIWALLOW_INTERVAL*5)
	//{
	//	wallowStatus = WEARINESS;
	//}
	else
	{
		wallowStatus = WALLOW;
	}
	SetAntiWallowState(wallowStatus);
}

void LC_ServerPlayer::_updateAntiWallow(float fCurrentTime)
{
	int32_t intelval = (int32_t)(fCurrentTime - m_antiWallowIntervalTS);
	if( intelval > 10.0)
	{
		m_antiWallowIntervalTS = (int32_t)fCurrentTime;
		SetAnitWallowOnlineTime(m_anitWallowOnlineTime+intelval);
		m_TotalOnlineTime += intelval;
		Activity::ACT_ScheduleDataManager* pkACTDataMgr = Activity::ACT_ScheduleDataManager::GetSingletonPtr();
		if (NULL != pkACTDataMgr)
		{
			pkACTDataMgr->OnOnlineTime(this, intelval);
		}
	}
}

///////////////////////////////////////////////////////////
int32_t	LC_ServerPlayer::GetDailyCleanValue(uint32_t nType)
{
	mem::map<uint32_t, int32_t>::iterator iter = m_dailyCleanMap.find(nType);
	if(iter == m_dailyCleanMap.end())
	{
		return 0;
	}
	return iter->second;
}

void LC_ServerPlayer::SetDailyCleanValue(uint32_t nType, int32_t nValue, bool bModify)
{
	mem::map<uint32_t, int32_t>::iterator iter = m_dailyCleanMap.find(nType);
	if(iter == m_dailyCleanMap.end())
	{
		m_dailyCleanMap[nType] = nValue;
	}
	else
	{
		iter->second = nValue;
	}
	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);

	if(bModify == true)
	{
		MG_Resp_DailyClean sendResp;
		sendResp.m_type = 1;
		sendResp.m_dailyMap[nType] = nValue;
		SendMsgToClient(MGPT_SEND_DAILY_CLEAN_RESP, &sendResp);
	}
}

void LC_ServerPlayer::SyncDailyCleanInfo()
{
	if (!IsMainController())
		return;
	MG_Resp_DailyClean sendResp;
	sendResp.m_type = 0;
	sendResp.m_dailyMap.insert(m_dailyCleanMap.begin(), m_dailyCleanMap.end());
	SendMsgToClient(MGPT_SEND_DAILY_CLEAN_RESP, &sendResp);
}

void LC_ServerPlayer::SyncRewardGainInfo()
{
	//MG_Resp_RewardGain sendResp;
	//sendResp.m_type = 0;
	//sendResp.m_rewardMap.insert(m_rewardGainMap.begin(), m_rewardGainMap.end());
	//SendMsgToClient(MGPT_SEND_REWARD_GAIN_RESP, &sendResp);
}

void LC_ServerPlayer::SyncAsControleeInfo(void)
{
	MG_PB<msg_define::char_control_info> syncMsg;
	msg_define::char_control_info& rk_control = syncMsg.m_value;
	rk_control.mutable_self()->set_userid(GetControlerPeer().UserID());
	rk_control.mutable_self()->set_charid(GetControlerPeer().CharID());
	rk_control.mutable_self()->set_objectid(GetControlerPeer().ObjectID());

	msg_define::control_entry* pkentry = rk_control.add_controls();
	if (pkentry)
	{
		pkentry->set_userid(GetUserID());
		pkentry->set_charid(GetInstance());
		pkentry->set_objectid(GetID());
	}
	SendMsgToClient(MGPT_SYNC_CONTROL_INFO, &syncMsg);
}

void LC_ServerPlayer::SyncAsControlerInfo(void)
{
	MG_PB<msg_define::char_control_info> syncMsg;
	msg_define::char_control_info& rk_control = syncMsg.m_value;
	rk_control.mutable_self()->set_userid(GetUserID());
	rk_control.mutable_self()->set_charid(GetInstance());
	rk_control.mutable_self()->set_objectid(GetID());

	msg_define::control_entry* pkentry = rk_control.add_controls();
	if (pkentry)
	{
		pkentry->set_userid(GetControleePeer().UserID());
		pkentry->set_charid(GetControleePeer().CharID());
		pkentry->set_objectid(GetControleePeer().ObjectID());
	}

	SendMsgToClient(MGPT_SYNC_CONTROL_INFO, &syncMsg);
}
void LC_ServerPlayer::ClearDailyClean(uint32_t timestamp)
{
	if (LC_Helper::CheckIsSameDay(timestamp, m_dailyCleanStramp))
	{
		return;
	}

	for(mem::map<uint32_t, int32_t>::iterator iter = m_dailyCleanMap.begin(); iter != m_dailyCleanMap.end(); iter++)
	{
		iter->second = 0;
	}
	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
	SyncDailyCleanInfo();
	m_dailyCleanStramp = timestamp;
	m_PlayerGuildAssert.CheckRaidRewardTimeStamp();
}

uint64_t LC_ServerPlayer::GetRewadGainValue(uint32_t nType)
{
	mem::map<uint32_t, uint64_t>::iterator iter = m_rewardGainMap.find(nType);
	if(iter == m_rewardGainMap.end())
	{
		return 0;
	}
	return iter->second;
}

void LC_ServerPlayer::SetRewadGainValue(uint32_t nType, uint64_t nValue, bool bModify)
{
	mem::map<uint32_t, uint64_t>::iterator iter = m_rewardGainMap.find(nType);
	if(iter == m_rewardGainMap.end())
	{
		m_rewardGainMap[nType] = nValue;
	}
	else
	{
		iter->second = nValue;
	}
	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);

	if(bModify == true)
	{
		MG_Resp_RewardGain sendResp;
		sendResp.m_type = 1;
		sendResp.m_rewardMap[nType] = nValue;
		SendMsgToClient(MGPT_SEND_REWARD_GAIN_RESP, &sendResp);
	}
}

void	LC_ServerPlayer::AddSendFlowerLog(const unique_id_impl& charID, const StringType& sTargetName, int nKeyId)
{
	StringStreamType outStream;

	MG_GG_Add_UserLog reqMsg;
	reqMsg.m_Type = 0;
	/*
	//我赠送日志
	reqMsg.m_userId = nSelfId;
	outStream << "{\"uid\":" << nTargetId
		<< ",\"name\":\"" << sTargetName.c_str()
		<< "\",\"key\":" << nKeyId
		<< ",\"send\":" << 0
		<< "}";
	reqMsg.m_sLogInfo = outStream.str().c_str();
	SendMsgToGroup(MGPT_SS_RANK_ADD_USERLOG, &reqMsg, GetUserID(), GetCharID());
	*/

	//对方获得日志
	outStream.str("");
	reqMsg.m_CharID = charID;
	outStream << "{\"uid\":" << GetInstance()
		<< ",\"name\":\"" << GetOwnerCharName().c_str()
		<< "\",\"key\":" << nKeyId
		<< ",\"send\":" << 1
		<< "}";
	reqMsg.m_sLogInfo = outStream.str().c_str();
	SendMsgToGroup(MGPT_SS_RANK_ADD_USERLOG, &reqMsg, GetUserID(), GetInstance());
}

ResultType LC_ServerPlayer::SendFlowerToPlayer(const unique_id_impl& charID,const StringType& sTargetName, int32_t nOptId)
{
	CF_FlowerConfig::DataEntry* pEntry = CF_FlowerConfig::Instance()->GetEntryPtr(nOptId);
	if(pEntry == NULL || charID <= 0)
	{
		return RE_FLOWER_ID_ERROR;
	}

	LC_ServerPlayer* pTargetPlayer = NULL;
	if(charID == GetInstance())
	{
		pTargetPlayer = this;
	}
	else
	{
		LC_ServerPlayerManager* pPlayerManager = SERVER_GET_PLAYER_MANAGER();
		if(pPlayerManager != NULL)
		{
			pTargetPlayer = (LC_ServerPlayer*)pPlayerManager->FindPlayer(charID);
		}
	}

	bool bCheckCross = true;
	if(pTargetPlayer)
	{
		if(UT_ServerHelper::GetSrcServerId(this) == UT_ServerHelper::GetSrcServerId(pTargetPlayer))
		{
			bCheckCross = false;
		}
	}

	if(bCheckCross == true)
	{
		//自己在跨服中
		if(UT_ServerHelper::IsInCross(this))
		{
			return RE_CHAT_NOT_SAME_SERVER;
		}

		//对方在跨服中
		if((pTargetPlayer != NULL && UT_ServerHelper::IsInCross(pTargetPlayer) == true) ||
			(pTargetPlayer == NULL && UT_ServerHelper::IsInCross(charID) == true))
		{
			return RE_CHAT_NOT_SAME_SERVER;
		}
	}

	//vip等级校验
	if(pEntry->_lVipLvl > (long)GetVIPLevel())
	{
		return RE_FLOWER_VIP_LVL_ERROR;
	}

	CF_FlowerSendLimitCfg::DataEntry *pLimitCfg = CF_FlowerSendLimitCfg::Instance()->GetEntryPtr(pEntry->_lLimitGroup);
	if(pLimitCfg == NULL)
	{
		return RE_FLOWER_ID_ERROR;
	}

	uint32_t nSendKey = _GetDailyCleanKeyID(E_DailyClean_Flower_Send, pEntry->_lLimitGroup);
	uint32_t nExpKey = _GetDailyCleanKeyID(E_DailyClean_Flower_GetExp, pEntry->_lLimitGroup);

	//赠送次数校验
	int nSendCnt = 0;
	if(pLimitCfg->_lSendCnt >= 0)
	{
		nSendCnt =  GetDailyCleanValue(nSendKey);
		if(nSendCnt >= pLimitCfg->_lSendCnt)
		{
			return RE_FLOWER_SEND_COUNT_ERROR;
		}
	}

	//元宝扣除
	if(pEntry->_lYBCost > 0)
	{
		if(GetUnBindYuanBao().GetMoney() < pEntry->_lYBCost)
		{
			return RE_FLOWER_NO_MONEY_ERROR;
		}
		else
		{
			if(ReduceCash(pEntry->_lYBCost, CT_UNBIND_YUANBAO, SYS_FUNC_TP_RANK_FLOWER, 0) == false)
			{
				return RE_FLOWER_COST_MONEY_ERROR;
			}
		}
	}

	//金币扣除
	if(pEntry->_lJBCost > 0)
	{
		if(GetUnBindCash().GetMoney() < pEntry->_lJBCost)
		{
			return RE_FLOWER_NO_MONEY_ERROR;
		}
		else
		{
			if(ReduceCash(pEntry->_lJBCost, CT_UNBIND_CASH, SYS_FUNC_TP_RANK_FLOWER, 0) == false)
			{
				return RE_FLOWER_COST_MONEY_ERROR;
			}
		}
	}

	//获得经验
	int nExpCnt = GetDailyCleanValue(nExpKey);
	if(pLimitCfg->_lExpCnt >= 0 && nExpCnt < pLimitCfg->_lExpCnt)
	{
		//增加经验
		AddExpWithGamePlay(pEntry->_lExp, SYS_FUNC_TP_RANK_FLOWER, 0);
		//增加真气
		AddSkillLevelupPoints(pEntry->_lSp, SYS_FUNC_TP_RANK_FLOWER, 0);
		//增加次数限制
		SetDailyCleanValue(nExpKey, nExpCnt + 1);
	}

	//增加赠送次数
	if(pLimitCfg->_lSendCnt >= 0)
	{
		SetDailyCleanValue(nSendKey, nSendCnt + 1);
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, __FUNCTION__, pEntry->_lPopulate, charID, nOptId);

	AddSendFlowerLog(charID, sTargetName, nOptId);

	//记录送花
	if(m_flowerSendCnt.size() < E_FlowerSend_Tp_Max)
	{
		for(size_t i=m_flowerSendCnt.size(); i<E_FlowerSend_Tp_Max; ++i)
		{
			m_flowerSendCnt.push_back(0);
		}
	}

	if(pEntry->_lFlowerCnt > 0)
	{
		uint32_t nLastValue = m_flowerSendCnt[E_FlowerSend_Tp_SendFlower];
		m_flowerSendCnt[E_FlowerSend_Tp_SendFlower] += pEntry->_lFlowerCnt;
		SERVER_GET_RANK_MANAGER()->GainFlowerTitle(GetInstance(), GetOwnerCharName(), E_FlowerSend_Tp_SendFlower, m_flowerSendCnt[E_FlowerSend_Tp_SendFlower], nLastValue);
	}

	if(pEntry->_lEggsCnt > 0)
	{
		uint32_t nLastValue = m_flowerSendCnt[E_FlowerSend_Tp_SendEggs];
		m_flowerSendCnt[E_FlowerSend_Tp_SendEggs] += pEntry->_lEggsCnt;
		SERVER_GET_RANK_MANAGER()->GainFlowerTitle(GetInstance(), GetOwnerCharName(), E_FlowerSend_Tp_SendEggs, m_flowerSendCnt[E_FlowerSend_Tp_SendEggs], nLastValue);
	}
	return RE_SUCCESS;
}

void	LC_ServerPlayer::OnPKValueChange(int nValue)
{
	if(nValue >= DEAD_DROP_PROP)
	{
		//校验增加大恶人buff
		LC_ActorStateEntry* pkDstStateEntry = m_kControllerSkillStateMap->GetStateEntryByID(DAEREN);
		if (NULL == pkDstStateEntry)
		{
			m_kControllerSkillStateMap->AddSkillStateToStateMap(DAEREN, 0, GetID());
		}
	}

	MG_RTL_GetPKValue rtlMsg;
	rtlMsg.m_lPlayerID = GetID();
	rtlMsg.m_lPkValue = nValue;

	m_dataChangeFlag.GetBroadCastFlag().SetControllerProperty();
	SendMsgToClient(MGPT_RLT_GET_PK_VALUE,&rtlMsg);
}

///////////////////////////////////////////////////////////////////////
int32_t	LC_ServerPlayer::GainOffLineExp(int32_t nGainType)
{
	CF_OffLineExp* pOfflineExpCfg = CF_OffLineExp::GetSingletonPtr();
	CF_OffLineExpRatio* pOfflineExpRatio = CF_OffLineExpRatio::GetSingletonPtr();
	

	CF_OffLineExpRatio::DataEntry* pExpRatio = pOfflineExpRatio->GetEntryPtr(m_lLevel);
	if (NULL == pExpRatio)
	{
		return RE_OFFLINEEXP_CURR_LEVEL;
	}

	CF_OffLineExp::DataEntry* pOffLineData = pOfflineExpCfg->GetEntryPtr(nGainType);
	if (NULL == pOffLineData)
	{
		return RE_OFFLINEEXP_GAINTYPE_ERR;
	}

	if(GetVIPLevel() < (uint32_t)pOffLineData->_lVipLvl)
	{
		return RE_OFFLINEEXP_VIPLEVEL;
	}

	uint32_t nOffLineExp= GetOffLineExp();
	if(nOffLineExp <= 0)
	{
		return RE_OFFLINEEXP_EXP_NULL;
	}

	uint32_t nOffLineExpTimes = (uint32_t)((uint64_t)nOffLineExp * pOffLineData->_lGainAdd * 0.01f);	

	Utility::UT_SIMDataList items = pExpRatio->_kReward;
	int plus = (GetOfflineExpTime() / pExpRatio->_iTime) * (pOffLineData->_lGainItemAdd* 0.01f);
	for(Utility::UT_SIMDataList::iterator it=items.begin(); it!=items.end(); ++it)
	{
		int itemNum = it->ItemCount() * plus;
		it->ItemCount(itemNum);
	}

	//判断背包是否充足
	ResultType res = LC_Helper::CanAddItemEx(this, items, CIET_NONE);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	if(pOffLineData->_lCostType >= CT_BIND_CASH && pOffLineData->_lCostType < CT_CASH_COUNT)
	{
		//具体公式修改
		int nCostMoney = 0;
		if(pOffLineData->_lCostPecTp == 1)
		{
			nCostMoney = (uint32_t)Utility::Round2Int(((double)(((int64_t)nOffLineExp) * pOffLineData->_lCostPec)) / 1000000.0f);
		}
		else if(pOffLineData->_lCostPecTp == 2)
		{
			nCostMoney = (uint32_t)Utility::Round2Int(((double)(((int64_t)GetOfflineExpTime()) * pOffLineData->_lCostPec)) / 1000000.0f);
		}
		else
		{
			return RE_OFFLINEEXP_TABLE_ERR;
		}

		if(nCostMoney <= 0)
		{
			nCostMoney = 1;
		}

		if(ReduceCash(nCostMoney, (CashType)pOffLineData->_lCostType, SYS_FUNC_TP_OFFLINE, 0) == false)
		{
			return RE_OFFLINEEXP_COSTMONEY;
		}		
	}
	//离线奖励--埋点日志
	int noffTime = GetOfflineExpTime();
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(noffTime);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, OFFLINE_OPERATE, OPERATE_LOGTYPE1);

	//离线经验只给指挥官加
	AddEXP(nOffLineExpTimes, SYS_FUNC_TP_OFFLINE, 0, false, nLogCode, nParams);

	//领取道具
	for (Utility::UT_SIMDataList::const_iterator cit = items.begin(); cit != items.end(); ++cit)
	{
		UT_SIMDataInfo info = *cit;
		int32_t nItemId = info.X();
		int32_t nCount = info.Y();
		res = AddItem(nItemId, nCount,false,0,0, nLogCode, nParams);
		if(RE_SUCCESS != res)
		{
			break;
		}
	}

	SyncBackPackToClient(PACK_TYPE_BASIC);
	SetOffLineExp(0, 0);
	return RE_SUCCESS;
}

uint32_t	LC_ServerPlayer::GetOffLineExp()
{
	return m_OffLineExpData.m_nExp;
}

int32_t		LC_ServerPlayer::GetOfflineExpTime()
{
	return m_OffLineExpData.m_nOffTime;
}

void	LC_ServerPlayer::SetOffLineExp(uint32_t nValue, uint32_t nTime)
{
	m_OffLineExpData.m_nExp = nValue;
	m_OffLineExpData.m_nOffTime = nTime;

	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
}

void	LC_ServerPlayer::AddOffLineExp(uint32_t nValue, int32_t nTime)
{
	m_OffLineExpData.m_nExp += nValue;
	m_OffLineExpData.m_nOffTime += nTime;
	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
}

void LC_ServerPlayer::calculateOfflineTimeExp()
{
	int64_t nowTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(nowTime <= m_iLastOfflineTime)
	{
		return;
	}

	//如果新建的账号有离线奖励，是因为本地gameserver的时间修改了
	//因为m_iLastOfflineTime是从linux服务器上拿的时间，nowTime是gamserver获取的本地时间
	int64_t nCutTime = nowTime - m_iLastOfflineTime;
	if(nCutTime < 60 || m_iLastOfflineTime==0)	//1分钟以内不算
	{
		return;
	}

	CF_OffLineExp* pOfflineExpCfg = CF_OffLineExp::GetSingletonPtr();
	CF_OffLineExpRatio* pOfflineExpRatio = CF_OffLineExpRatio::GetSingletonPtr();

	CF_OffLineExpRatio::DataEntry* pOffLineData = pOfflineExpRatio->GetEntryPtr(m_lLevel);
	if (NULL == pOffLineData)
	{
		return;
	}

	CF_OffLineExp::DataEntryMapExternIterator iter = pOfflineExpCfg->GetIterator();
	if(iter.HasMoreElements() == false)
	{
		return;
	}
	const CF_OffLineExp::DataEntry * pOfflineExp = iter.PeekNextValuePtr();
	if(m_OffLineExpData.m_nOffTime >= (uint32_t)pOfflineExp->_lOffTimeMax)
	{
		return ;
	}

	int64_t nTmpTime = pOfflineExp->_lOffTimeMax - m_OffLineExpData.m_nOffTime;
	if(nCutTime > nTmpTime)
	{
		nCutTime = nTmpTime;
	}

	uint32_t nExpAdd = (uint32_t)Utility::Round2Int(((double)(nCutTime * pOffLineData->_lExpPec)) / 30000.0f);
	if(nExpAdd <= 0)
	{
		return;
	}

	AddOffLineExp(nExpAdd, (int32_t)nCutTime);
}
//////////////////////////////////////////////////////////////////////////////////////////
void		LC_ServerPlayer::SetMysticalShopResetCnt(uint32_t nType, uint32_t nCount)
{
	m_MySyicalShopMark.m_resetCntMark[nType] = nCount;
	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
}

uint32_t	LC_ServerPlayer::GetMysticalShopResetCnt(uint32_t nType)
{
	mem::map<uint32_t, uint32_t>::iterator iter = m_MySyicalShopMark.m_resetCntMark.find(nType);
	if(iter != m_MySyicalShopMark.m_resetCntMark.end())
	{
		return iter->second;
	}
	return 0;
}

void		LC_ServerPlayer::SetMysticalShopBuyCnt( uint32_t nType, uint32_t nCount)
{
	m_MySyicalShopMark.m_buyCntMark[nType] = nCount;
	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
}

uint32_t	LC_ServerPlayer::GetMysticalShopBuyCnt(uint32_t nType)
{
	mem::map<uint32_t, uint32_t>::iterator iter = m_MySyicalShopMark.m_buyCntMark.find(nType);
	if(iter != m_MySyicalShopMark.m_buyCntMark.end())
	{
		return iter->second;
	}
	return 0;
}

void	LC_ServerPlayer::sendMysticalShopInfo()
{
	MG_RTL_MysticalShopInfo rtlMsg;
	rtlMsg.m_nTimeStamp = m_MySyicalShopMark.m_nTimeStamp;
	rtlMsg.m_PlayerBuyMark.insert(m_MySyicalShopMark.m_buyCntMark.begin(), m_MySyicalShopMark.m_buyCntMark.end());
	rtlMsg.m_PlayerResetMark.insert(m_MySyicalShopMark.m_resetCntMark.begin(), m_MySyicalShopMark.m_resetCntMark.end());
	rtlMsg.m_ShopList.reserve(m_MySyicalShopMark.m_itemidlist.size());
	for(int i=0; i<m_MySyicalShopMark.m_itemidlist.size();++i)
	{
		rtlMsg.m_ShopList.push_back(m_MySyicalShopMark.m_itemidlist[i]);
	}
	SendMsgToClient(MGPT_RLT_MYSTICAL_SHOP_INFO, &rtlMsg);
}

uint32_t LC_ServerPlayer::CheckCanBuyItemWithVipLimit(uint32_t nCount, uint32_t nID, uint32_t nTypeClean, uint32_t nTypeVip)
{
	int32_t nVipID = m_kVipAsset.GetProfitIDByTypes(nTypeVip, nID);
	if(nVipID <= 0)
	{
		return RE_BUY_ITEM_VIP_LIMIT_BUYNUM;
	}
	int32_t nCanBuyMax = m_kVipAsset.GetProfitValue(nVipID);
	if(nCanBuyMax <= 0)
	{
		return RE_BUY_ITEM_VIP_LIMIT_BUYNUM;
	}

	uint32_t nCleanKey = _GetDailyCleanKeyID(nTypeClean, nID);
	int32_t nNowCount =  GetDailyCleanValue(nCleanKey);

	if(nNowCount >= nCanBuyMax || nNowCount + (int32_t)nCount > nCanBuyMax)
	{
		return RE_BUY_ITEM_VIP_LIMIT_HASNAX;
	}
	return RE_SUCCESS;
}

void LC_ServerPlayer::AddBuyItemWithVipLimit(uint32_t nCount, uint32_t nID, uint32_t nTypeClean, uint32_t nTypeVip)
{
	if(CheckCanBuyItemWithVipLimit(nCount, nID, nTypeClean, nTypeVip) == RE_SUCCESS)
	{
		uint32_t nCleanKey = _GetDailyCleanKeyID(nTypeClean, nID);
		int32_t nNowCount =  GetDailyCleanValue(nCleanKey);
		SetDailyCleanValue(nCleanKey, nNowCount + nCount);
	}
}

uint32_t LC_ServerPlayer::CheckPayForShop_Cash(int64_t nCash, uint32_t nCashType, uint32_t iSrcType, uint64_t iSrcID)
{
	return RE_SUCCESS;
}

uint32_t LC_ServerPlayer::CheckPayForShop_Score(int32_t nCash, uint32_t nCashType, uint32_t iSrcType, uint64_t iSrcID)
{
	if(nCash > 0)
	{
		if(nCashType == GS_SCR_TP_TRIALS && iSrcType == SYS_FUNC_TP_CITY_SHOP)
		{
			return CheckCanBuyItemWithVipLimit(nCash, LC_VIP_PROF_KEY_STORY_RONGYU);
		}
	}
	return RE_SUCCESS;
}

void LC_ServerPlayer::OnPayForShop_Score(int32_t nCash, uint32_t nCashType, uint32_t iSrcType, uint64_t iSrcID)
{
	if(nCash > 0)
	{
		if(nCashType == GS_SCR_TP_TRIALS && iSrcType == SYS_FUNC_TP_CITY_SHOP)
		{
			return AddBuyItemWithVipLimit(nCash, LC_VIP_PROF_KEY_STORY_RONGYU);
		}
	}
}

void LC_ServerPlayer::OnSubordinateConsume(int32_t iSubType, int iPayType, int32_t iValue)
{
	static Activity::ACT_ScheduleDataManager* pkScheduleDataMgr = Activity::ACT_ScheduleDataManager::GetSingletonPtr();
	pkScheduleDataMgr->OnSubordinateConsume(this, iSubType, iPayType, iValue);
}

bool LC_ServerPlayer::DealSelfGMRequest(int gmType, const StringType& strInfo)
{
	switch(gmType)
	{
	case 1:			//设置风雷宫领奖数据记录
		{
			mem::vector<StringType> strList;
			Utility::tokenize(strInfo, strList, "_");
			size_t nSize = strList.size();
			uint32_t nOpt = 0, nType = 0, nPage = 0, nStage = 0;
			__Get_Token_Value(nOpt, strList, nSize, 0, uint32_t, atoi);
			__Get_Token_Value(nType, strList, nSize, 1, uint32_t, atoi);
			__Get_Token_Value(nPage, strList, nSize, 2, uint32_t, atoi);
			__Get_Token_Value(nStage, strList, nSize, 3, uint32_t, atoi);

			if(nOpt == 0)		//清除数据
			{
				m_PlayerGuildAssert.ClearRaidReward();
			}
			else if(nOpt == 1)
			{
				if(nType == 0)
				{
					m_PlayerGuildAssert.SetRewardTimeStamp(GET_CURRENT_TIMESTAMP_IN_SECONDS());
				}
				else
				{
					m_PlayerGuildAssert.SetRewardTimeStamp(nPage);
				}
			}
			else if(nOpt == 2)
			{
				mem::map<int, std::set<int> >& rReward = m_PlayerGuildAssert.GetRaidStageReward();
				rReward[nPage].insert(nStage);
			}
			else if(nOpt == 3)
			{
				mem::map<int, std::set<int> >& rReward = m_PlayerGuildAssert.GetRaidStageReward();
				rReward[nPage].erase(nStage);
			}
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

//攻击者, 被攻击者
void	LC_ServerPlayer::SendNoticeOnPlayerAttack(uint32_t nAttackType, LC_ServerMapLogic_Place* pLogicPlace, LC_ServerPlayer* pSrcPlayer, LC_ServerPlayer* pDstPlayer, int32_t nSrcSId, int32_t nDstSId)
{
	if(pLogicPlace == NULL || pSrcPlayer == NULL || pDstPlayer == NULL)
	{
		return;
	}

	CF_WorldMapList::DataEntry* pConfig = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr(pLogicPlace->GetMapResID());
	if(pConfig == NULL || pConfig->_iKillBroadcast.empty() == true)
	{
		return;
	}

	if(nAttackType == E_AttackType_Attack)
	{
		uint32_t nNowTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		if(nNowTime >= pSrcPlayer->m_nAttackNoticeCD && pSrcPlayer->m_nAttackNoticeCD + 10 > nNowTime)
		{
			return;
		}
		pSrcPlayer->m_nAttackNoticeCD = nNowTime;
	}

	bool bSrcLocal = UT_ServerHelper::CheckLocal(nSrcSId);
	bool bDstLocal = UT_ServerHelper::CheckLocal(nDstSId);
	mem::set<size_t> noticeSet;
	
	UT_AtkNoticeList& rKillBroadCast = pConfig->_iKillBroadcast;
	size_t nSize = rKillBroadCast.size();
	for(size_t i = 0; i < nSize; i++)
	{
		UT_AtkNotice& rNotice = rKillBroadCast[i];
		if(rNotice.m_nType != nAttackType)
		{
			continue;
		}
		if(rNotice.m_nNotice <= 0)
		{
			continue;
		}
		if(rNotice.m_nAtkLocal > 0 && bSrcLocal == false)
		{
			continue;
		}
		if(rNotice.m_nBAtkLocal > 0 && bDstLocal == false)
		{
			continue;
		}
		noticeSet.insert(i);
	}

	if(noticeSet.empty() == true)
	{
		return;
	}

	int32_t nServerID = UT_ServerHelper::GetServerId();
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	noticeFmort.AddPlayerInfo(pSrcPlayer->GetInstance(), pSrcPlayer->GetOwnerCharName());
	noticeFmort.AddPlayerInfo(pDstPlayer->GetInstance(), pDstPlayer->GetOwnerCharName());
	noticeFmort.AddFlyKicks(nServerID, pSrcPlayer->GetMapLogicID(), pSrcPlayer->GetMapResID(), pSrcPlayer->GetCurrentLocation());
	noticeFmort.AddFlyKicks(nServerID, pDstPlayer->GetMapLogicID(), pDstPlayer->GetMapResID(), pDstPlayer->GetCurrentLocation());
	noticeFmort.AddSendFlower(pSrcPlayer->GetInstance());
	noticeFmort.AddSendFlower(pDstPlayer->GetInstance());
	noticeFmort.AddPathFound(nServerID, pSrcPlayer->GetMapLogicID(), pSrcPlayer->GetMapResID(), pSrcPlayer->GetCurrentLocation());
	noticeFmort.AddPathFound(nServerID, pDstPlayer->GetMapLogicID(), pDstPlayer->GetMapResID(), pDstPlayer->GetCurrentLocation());

	for(mem::set<size_t>::iterator iter = noticeSet.begin(); iter != noticeSet.end(); iter ++)
	{
		size_t nIndex = *iter;
		if(nIndex < 0 || nIndex >= nSize)
		{
			continue;
		}
		UT_AtkNotice& rNotice = rKillBroadCast[nIndex];
		noticeFmort.SetMsgID((int32_t)rNotice.m_nNotice);

		if(rNotice.m_nSend <= 0)
		{
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pSrcPlayer->GetCitizenship(), pSrcPlayer, pLogicPlace);
		}
		else
		{
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pDstPlayer->GetCitizenship(), pDstPlayer, pLogicPlace);
		}
	}
}

void	LC_ServerPlayer::SendNoticeEnterGame(uint32_t nEnterResult, bool bSwitchLine)
{
	static GameServerManager* pkSvrMgr = GetGameServerApp()->GetServerManager();
	Protocol::MG_GC_RLT_EnterGame rltEnterMsg;
	rltEnterMsg.m_lEnterGameStatus = nEnterResult;
	rltEnterMsg.m_lUserID = m_dwUserID;
	rltEnterMsg.m_lCharID = m_Instance;
	rltEnterMsg.m_bSwitchLine = bSwitchLine;
	int nBuddy = m_nBuddyTemplateId;
	rltEnterMsg.m_lCharType = this->GetCharType();
	rltEnterMsg.m_lUnionID = GetGameUnionID();
	rltEnterMsg.m_lServerID = GetGameServerID();
	rltEnterMsg.m_lServerEntryID = GetGameServerEntryID();
	rltEnterMsg.m_iNewPlayerLevel = MIN_NOT_NEWBIE_LEVEL;
	rltEnterMsg.m_lWeaponType = m_eWeaponAttribute;
	TPSTR2STDSTR(m_kCharName_gb, rltEnterMsg.m_szCharName);
	rltEnterMsg.m_strGameVersion = GAME_VERSION_STRING();
	rltEnterMsg.m_strResVersion = g_svr_version_info.custom_res_version();
	rltEnterMsg.m_uiTimestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	rltEnterMsg.m_uiOpenTime = (uint32_t)pkSvrMgr->GetServerStartTime();
	rltEnterMsg.m_uiConflateTime = (uint32_t)pkSvrMgr->GetServerConflateTime();
	rltEnterMsg.m_uiLastOffLineTime = m_iLastOfflineTime;
	SendMsgToClient(MGPT_GC_RLT_ENTER_GAME, &rltEnterMsg);

	if (GetLoginCount() == 1)
	{
		oss_login_step(GetCitizenship(), GetUserName(), EVENT_SERVER_GC_RLT_ENTER_GAME, GetPlayerPlatformType(),  nEnterResult);
	}
}
