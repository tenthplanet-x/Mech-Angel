#include"LC_ServerPlayerGuild.h"
#include "CF_BestGuildRobReward.h"
#include "LC_Helper.h"
#include "GlobalSettings.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerGoalAsset.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;
static const int snMaxCount = 3;

LC_ServerPlayerGuild::LC_ServerPlayerGuild() 
{
	ReUse();
}	

LC_ServerPlayerGuild::~LC_ServerPlayerGuild() 
{
}

void LC_ServerPlayerGuild::ReUse()
{
	m_ReceivedVec.clear();
}

ResultType LC_ServerPlayerGuild::PreIsAllowChallenge(LC_ServerPlayer*pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	if(m_FreeNum <= 0 && m_PaidNum <= 0)
	{
		return RE_SOLO_DULE_CHALLENGE_TIME_OUT;
	}

	//挑战者是否有公会
	uint64_t guildID = pkPlayer->GetGuildID();
	if(0 == guildID)
	{
		return RE_BESTGUILD_ERROR_GUILDID_INVALID;
	}
	if(GetGlobalSetting.ChallengeCostItemId == 0)
		return RE_SUCCESS;
	return LC_Helper::CanDeleteItem(pkPlayer->GetPackAsset(), GetGlobalSetting.ChallengeCostItemId, GetGlobalSetting.ChallengeCostItemNum);
}

bool LC_ServerPlayerGuild::ReduceRobScoreChallengeNumAndItem(LC_ServerPlayer*pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return false;
	}

	ResultType res = PreIsAllowChallenge(pkPlayer);
	if(RE_SUCCESS != res)
	{
		return false;
	}
	if(GetGlobalSetting.ChallengeCostItemId != 0)
		pkPlayer->DeleteItem(GetGlobalSetting.ChallengeCostItemId, GetGlobalSetting.ChallengeCostItemNum);
	if(m_FreeNum > 0)
		m_FreeNum--;
	else
		m_PaidNum--;

	GoalEvent kGoalEvent;
	kGoalEvent.SetGoalEvent(pkPlayer->GetMapResID());
	pkPlayer->DoGoalEvent(LC_GOAL_TYPE_SNATCH_ACCOUNT_POINT, &kGoalEvent);

	return true;
}


void LC_ServerPlayerGuild::PackGuildRobScoreReceivedInfo(GuildRobScoreReceivedInfo& info)
{
	for (uint32_t i = 0; i < m_ReceivedVec.size();++i)
		info.m_ReceivedVec.push_back(m_ReceivedVec[i]);
	info.m_WonNum = m_WonNum;
	
}
void LC_ServerPlayerGuild::PackGuildRobScoreInfo(GuildRobScoreInfo& info)
{
	info.m_FreeNum = m_FreeNum;
	info.m_PayingNum = m_PayingNum;
	info.m_PaidNum = m_PaidNum;
}
void LC_ServerPlayerGuild::PackGuildResMsg(MG_Resp_GuildFightRobScore_Opt &msg)
{
	PackGuildRobScoreReceivedInfo(msg.m_ReceivedInfo);
	PackGuildRobScoreInfo(msg.m_RobScoreInfo);
}

ResultType LC_ServerPlayerGuild::BuyRobNum(LC_ServerPlayer*pkPlayer,int nBuyNum)
{
	if(nBuyNum <= 0)
		return RE_FAIL;
	if(m_PayingNum + nBuyNum > GetGlobalSetting.MaxBuyRobNum)
		return RE_MAX_BUY_ROB_NUM;
	// 需要消耗的钻石数量
	uint32_t needNum =  0;
	for(int i = m_PayingNum; i < m_PayingNum + nBuyNum ; i++)
	{
		needNum += GetGlobalSetting.BuyRobNumFirst;
		needNum += GetGlobalSetting.BuyRobNumPlus*(i);
	}
	
	
	//check 钻石是否足够
	if (!pkPlayer->IsCashEnough(needNum, CT_UNBIND_YUANBAO))
		return RE_UNBIND_YUANBAO_NOT_ENOUGH;
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(nBuyNum);
	nParams.push_back(m_PayingNum);
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE10);
	pkPlayer->ReduceCash(needNum, CT_UNBIND_YUANBAO, SYS_FUNC_TP_GUILDFIGHTROBSCOREBUYNUM, 0, nLogCode, nParams);
	m_PayingNum += nBuyNum;
	m_PaidNum += nBuyNum;
	return RE_SUCCESS;
}

void LC_ServerPlayerGuild::PlayerGuildDaily(LC_ServerPlayer*pkPlayer)
{
	m_PayingNum = 0;
	m_FreeNum = GetGlobalSetting.FreeRobNum;
	MG_Resp_GuildFightRobScore_Opt sendmsg;
	PackGuildResMsg(sendmsg);
	pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_ROB_SCORE, &sendmsg);
}

ResultType LC_ServerPlayerGuild::ReceiveRewardByWonNum(LC_ServerPlayer*pkPlayer,int nWonNum)
{
	if(nWonNum > m_WonNum)
		return RE_FAIL;
	for(uint32_t i = 0; i < m_ReceivedVec.size(); ++i)
		if(m_ReceivedVec[i] == nWonNum)
			return RE_FAIL;
	bool bFindFlag = false;
	StringType strReward;
	int nMaxNum = 0;
	CF_BestGuildRobReward::DataEntryMapExternIterator kIt = CF_BestGuildRobReward::GetSingletonPtr()->GetIterator();
	for(; kIt.HasMoreElements(); kIt.MoveNext())
	{
		CF_BestGuildRobReward::DataEntry* pkData = kIt.PeekNextValuePtr();
		if(/*pkData &&*/ pkData->_iWinTimes == nWonNum)
		{
			bFindFlag = true;
			strReward = pkData->_kReward;
			nMaxNum = nWonNum;
			break;
		}
	}
	if(!bFindFlag)
		return RE_FAIL;
	//背包能否装得下返还的所有道具
	ResultType res = LC_Helper::CanAddItemByStrReward(pkPlayer, strReward, CIET_NONE);
	if(RE_SUCCESS != res)
		return res;
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(nWonNum);
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN18_OPERATE, OPERATE_LOGTYPE9);
	LC_Helper::AddItemByString(pkPlayer->GetPackAsset(),strReward,CIET_NONE, true, nLogCode, nParams);
	//m_ReceivedVec.push_back(nWonNum);
	PushReceivedData(pkPlayer, nWonNum);
	int maxCount = CF_BestGuildRobReward::GetSingletonPtr()->GetEntryCount();
	if(maxCount == m_ReceivedVec.size())
	{
		if(m_WonNum > nMaxNum)
			m_WonNum = m_WonNum - nMaxNum;
		else
		 m_WonNum = 0;
		m_ReceivedVec.clear();
	}
	return RE_SUCCESS;
}

void LC_ServerPlayerGuild::SaveGuildInfo(LC_ServerPlayer* pkPlayer, char_data_define::DbGuildFightInfo& info)
{
	const unique_id_impl& instance = pkPlayer->GetInstance();
	info.mutable_single_info()->set_m_freenum(m_FreeNum);
	info.mutable_single_info()->set_m_payingnum(m_PayingNum);
	info.mutable_single_info()->set_m_paidnum(m_PaidNum);
	info.mutable_single_info()->set_m_wonnum(m_WonNum);
	int32_t nSize = m_ReceivedVec.size();
	if(nSize > snMaxCount )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerPlayerGuild::SaveGuildInfo SizeError:%u, c_cid:%d, c_citizenship:%d, c_uid:%d", 
			nSize, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		nSize = snMaxCount;
	}
	for (int i = 0; i < nSize; ++i)
		info.mutable_single_info()->add_m_receivedvec(m_ReceivedVec[i]);
}
void LC_ServerPlayerGuild::LoadGuildInfo(LC_ServerPlayer* pkPlayer, const::char_data_define::DbGuildFightInfo& info)
{
	const unique_id_impl& instance = pkPlayer->GetInstance();
	m_FreeNum = info.single_info().m_freenum();
	m_PayingNum = info.single_info().m_payingnum();
	m_PaidNum = info.single_info().m_paidnum();
	m_WonNum = info.single_info().m_wonnum();
	int32_t nSize = info.single_info().m_receivedvec_size();
	if ( nSize > snMaxCount )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerPlayerGuild::LoadGuildInfo SizeError: %u, c_cid:%d, c_citizenship:%d, c_uid:%d", 
			nSize, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		nSize = snMaxCount;
	}
	for (int i = 0; i < nSize; ++i)
	{
		//m_ReceivedVec.push_back(info.single_info().m_receivedvec(i));
		PushReceivedData(pkPlayer, info.single_info().m_receivedvec(i));
	}
}

bool LC_ServerPlayerGuild::PushReceivedData(LC_ServerPlayer* pkPlayer, int32_t nData )
{
	if (m_ReceivedVec.size() >= snMaxCount )
	{	
		const unique_id_impl& instance = pkPlayer->GetInstance();
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerPlayerGuild::PushReceivedData SizeError:%u, c_cid:%d, c_citizenship:%d, c_uid:%d",
			m_ReceivedVec.size(), instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return false;
	}

	m_ReceivedVec.push_back(nData);

	return true;
}

