#include "LC_NonpareilReturn.h"
#include "LC_ServerPlayer.h"
#include "LC_Helper.h"
#include "GameServerManager.h"
#include "GameServerApp.h"
#include "UT_Timer.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;

void CNonpareilReturn::Clear()
{
	m_NonpareilReturnInfo.clear();
}

CNonpareilReturn::CNonpareilReturn()
{
	Clear();
}

CNonpareilReturn::~CNonpareilReturn()
{
	Clear();
}

void CNonpareilReturn::SaveData(char_data_define::PBNonpareilReturnInfo* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	for(uint32_t i=0; i<m_NonpareilReturnInfo.size(); ++i)
	{
		::char_data_define::PBNonpareilReturnData* pkInfo = pkData->add_nonpareilreturndata();
		if(NULL != pkInfo)
		{
			pkInfo->set_nonpareilid(m_NonpareilReturnInfo[i].key);
			pkInfo->set_getreward(m_NonpareilReturnInfo[i].value);
		}
	}
}

void CNonpareilReturn::LoadData(const char_data_define::PBNonpareilReturnInfo& data)
{
	Clear();
	m_NonpareilReturnInfo.reserve(data.nonpareilreturndata_size());
	for(int i=0; i<data.nonpareilreturndata_size(); ++i)
	{
		KeyValue_Base_Bool info;
		info.key = data.nonpareilreturndata(i).nonpareilid();
		info.value = data.nonpareilreturndata(i).getreward();

		m_NonpareilReturnInfo.push_back(info);
	}
}

void CNonpareilReturn::BuyNonpareilItem(LC_ServerPlayer* pkOwnerPlayer, int32_t id)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_BuyNonpareilItem rltMsg;
	rltMsg.m_nID = id;
	rltMsg.m_nResult = RE_SUCCESS;

	CF_NonpareilItem::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_NonpareilItem>(id);
	if(NULL == pkData)
	{
		rltMsg.m_nResult = RE_ERROR_PARAM;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_BUYNONPAREILITEM, &rltMsg);
		return;
	}

	//检测openLevel是否开启 pkData->_openLevelID
	bool tag = pkOwnerPlayer->GetOpenAbilityAsset().IsAbilityOpened(pkData->_openLevelID);
	if(!tag)
	{
		rltMsg.m_nResult = RE_NONPAREILERROR_6;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_BUYNONPAREILITEM, &rltMsg);
		return;
	}

	//是否购买过
	for(uint32_t i=0; i<m_NonpareilReturnInfo.size(); ++i)
	{
		if(m_NonpareilReturnInfo[i].key == id)
		{
			rltMsg.m_nResult = RE_NONPAREILERROR_1;
			pkOwnerPlayer->SendMsgToClient(MGPT_RLT_BUYNONPAREILITEM, &rltMsg);
			return;
		}
	}

	//货币是否足够
	for(uint32_t i=0; i<pkData->_cost.size(); ++i)
	{			
		if(!pkOwnerPlayer->IsCashEnough(pkData->_cost[i].ParamA(), pkData->_cost[i].ID()))
		{
			rltMsg.m_nResult = RE_NONPAREILERROR_2;
			pkOwnerPlayer->SendMsgToClient(MGPT_RLT_BUYNONPAREILITEM, &rltMsg);
			return;
		}
	}

	//极品白拿道具
	vector<int> nParams; 
	nParams.clear();
	nParams.push_back(id);
	int nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY4_OPERATE, OPERATE_LOGTYPE1);

	//发道具
	StringType mailTitle;
	StringType mailDesc;
	rltMsg.m_nResult = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkData->_BuyItem, pkOwnerPlayer, SYS_FUNC_TP_NONPAREIL_ITEM, id, mailTitle, mailDesc, false, false,false,nLogCode,nParams);
	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_BUYNONPAREILITEM, &rltMsg);
		return;
	}

	//扣钱
	for(uint32_t i=0; i<pkData->_cost.size(); ++i)
	{		
		pkOwnerPlayer->ReduceCash(pkData->_cost[i].ParamA(), (CashType)pkData->_cost[i].ID(), SYS_FUNC_TP_NONPAREIL_ITEM, id, nLogCode,nParams);
	}

	//增加购买记录
	KeyValue_Base_Bool data;
	data.key = id;
	data.value = false;
	m_NonpareilReturnInfo.push_back(data);

	rltMsg.m_nResult = RE_SUCCESS;
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_BUYNONPAREILITEM, &rltMsg);
}

void CNonpareilReturn::GetNonpareilReturn(LC_ServerPlayer* pkOwnerPlayer, int32_t id)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_GetNonpareilReturn rltMsg;
	rltMsg.m_nID = id;
	rltMsg.m_nResult = RE_FAIL;
	
	CF_NonpareilItem::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_NonpareilItem>(id);
	if(NULL == pkData)
	{
		rltMsg.m_nResult = RE_ERROR_PARAM;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNONPAREILRETURN, &rltMsg);
		return;
	}

	//是否购买过道具 或者 是否已经领取了返利
	rltMsg.m_nResult = RE_NONPAREILERROR_4;
	for(uint32_t i=0; i<m_NonpareilReturnInfo.size(); ++i)
	{
		if(m_NonpareilReturnInfo[i].key == id)
		{
			rltMsg.m_nResult = m_NonpareilReturnInfo[i].value==false ? RE_SUCCESS : RE_NONPAREILERROR_5;
			break;
		}
	}

	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNONPAREILRETURN, &rltMsg);
		return;
	}

	//是否到了领取返利的时间
	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	uint32_t startTime = (uint32_t)pkServerMgr->GetServerStartTime();
	if(startTime < 0)
	{
		rltMsg.m_nResult = RE_ERROR_PARAM;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNONPAREILRETURN, &rltMsg);
		return;
	}

	uint32_t timestamp = (uint32_t)time(NULL);
	int32_t startDay = LC_Helper::GetDeltaDay(startTime, timestamp)+1;
	if(startDay < pkData->_Time)
	{
		rltMsg.m_nResult = RE_NONPAREILERROR_3;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNONPAREILRETURN, &rltMsg);
		return;
	}

	//极品白拿返利道具
	vector<int> nParams; 
	nParams.clear();
	nParams.push_back(id);
	int nLogCode = LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY27_OPERATE, OPERATE_LOGTYPE1);

	//领取返利
	StringType mailTitle;
	StringType mailDesc;
	rltMsg.m_nResult = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkData->_returnCost, pkOwnerPlayer, SYS_FUNC_TP_NONPAREIL_RETURN_ITEM, id, mailTitle, mailDesc, false, false,false,nLogCode,nParams);
	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNONPAREILRETURN, &rltMsg);
		return;
	}

	//记录已经领取返利
	for(uint32_t i=0; i<m_NonpareilReturnInfo.size(); ++i)
	{
		if(m_NonpareilReturnInfo[i].key == id)
		{
			m_NonpareilReturnInfo[i].value = true;
			break;
		}
	}

	rltMsg.m_nResult = RE_SUCCESS;
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNONPAREILRETURN, &rltMsg);
}

void CNonpareilReturn::SyncNonpareilRecordAsset(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_SyncNonpareilReturnInfo syncMsg;
	syncMsg.m_NonpareilReturnInfo.reserve(m_NonpareilReturnInfo.size());
	for(uint32_t i=0; i<m_NonpareilReturnInfo.size(); ++i)
	{
		syncMsg.m_NonpareilReturnInfo.push_back(m_NonpareilReturnInfo[i]);
	}
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCNONPAREILRETURNINFO, &syncMsg);
}