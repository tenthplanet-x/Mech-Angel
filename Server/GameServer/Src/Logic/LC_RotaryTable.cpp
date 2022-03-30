#include "LC_RotaryTable.h"
#include "LC_ServerPlayer.h"

#include "CF_RotaryTable.h"
#include "CF_RotaryTableReward.h"

#include "UT_OperateLog.h"
#include "UT_ServerHelper.h"
#include "GlobalSettings.h"

using namespace OPERATELOG;
CRotaryTable::CRotaryTable() : 
	m_nRotaryTableEndTime(0), m_nRotaryTableCDStartTime(0), m_nCurrentRTRewardIndex(0), m_nRotaryTableVaildTimes(0), m_bSkipAnimation(false)
{
	m_nRotaryTableRewardIndex.clear();
}

CRotaryTable::~CRotaryTable()
{

}

void CRotaryTable::LoadData(const char_data_define::PB_RotaryTable& data)
{
	m_nRotaryTableVaildTimes = data.rotarytablevaildtimes();
	m_nRotaryTableRewardIndex.reserve(data.rotarytablerewardindex_size());
	for(int i=0; i<data.rotarytablerewardindex_size(); ++i)
	{
		m_nRotaryTableRewardIndex.push_back(data.rotarytablerewardindex(i));
	}
	m_bSkipAnimation = data.skipanimation();
}

void CRotaryTable::SaveData(char_data_define::PB_RotaryTable* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	pkData->set_rotarytablevaildtimes(m_nRotaryTableVaildTimes);
	for(int i=0; i<m_nRotaryTableRewardIndex.size(); ++i)
	{
		pkData->add_rotarytablerewardindex(m_nRotaryTableRewardIndex[i]);
	}
	pkData->set_skipanimation(m_bSkipAnimation);
}

void CRotaryTable::InitRotaryTableData(int32_t time)
{
	//计算幸运转盘活动的结束时间	
	m_nRotaryTableEndTime = time+(GetGlobalSetting.RotaryTableVaildDays-1)*ONE_DAY_SECONDS;
}

void CRotaryTable::GetRotaryTableReward(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_GetRotaryTableReward rltMsg;
	rltMsg.m_nResult = RE_SUCCESS;

	//活动请求是否有效
	rltMsg.m_nResult = CheckRotaryTableVaild();
	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETROTARYTABLEREWARD, &rltMsg);
		return;
	}

	//是否有抽奖次数
	int32_t vaildTimes = GetRotaryTableVaildTimes();
	if(vaildTimes < 1)
	{
		rltMsg.m_nResult = RE_FAIL;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETROTARYTABLEREWARD, &rltMsg);
		return;
	}

	//是否已领取所有奖励
	CF_RotaryTable* pkCSVList = CF_RotaryTable::GetSingletonPtr();
	int32_t count = (int32_t)pkCSVList->GetEntryCount();
	int32_t totalTimes = GetRotaryTableTimes();
	if(totalTimes >= count)
	{
		rltMsg.m_nResult = RE_FAIL;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETROTARYTABLEREWARD, &rltMsg);
		return;
	}

	//随机奖励
	int32_t rewardIndex = 0;
	rltMsg.m_nResult = RandomRotaryTableRewardIndex(totalTimes+1, rewardIndex);
	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETROTARYTABLEREWARD, &rltMsg);
		return;
	}

	//策划配表错误
	CF_RotaryTableReward::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RotaryTableReward>(rewardIndex);
	if(NULL==pkData)
	{
		rltMsg.m_nResult = RE_FAIL;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETROTARYTABLEREWARD, &rltMsg);
		return;
	}

	//幸运转盘，奖励不能全放进背包，则不能转准盘
	rltMsg.m_nResult = LC_Helper::CanAddItemEx(pkOwnerPlayer, pkData->_RotaryTableReward, CIET_NONE);
	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETROTARYTABLEREWARD, &rltMsg);
		return;
	}

	SetCurrentRotaryTableRewardIndex(rewardIndex);	
	
	if(m_bSkipAnimation)
	{
		UpdateRotaryTableReward(pkOwnerPlayer, true);
	}
	else
	{
		int64_t currentTime = GET_CURRENT_TIMESTAMP_IN_MILISECONDS();
		SetRotaryTableCDStartTime(currentTime);
	}

	rltMsg.m_nRewardIndex = rewardIndex;
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETROTARYTABLEREWARD, &rltMsg);
}

void CRotaryTable::SetRotaryTableSkipAnimation(LC_ServerPlayer* pkOwnerPlayer, bool flag)
{
	if(m_bSkipAnimation==flag || NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	m_bSkipAnimation = flag;
	if(m_bSkipAnimation)
	{
		UpdateRotaryTableReward(pkOwnerPlayer, true);
	}
	SyncRotaryTableSkipAnimation(pkOwnerPlayer);
}

void CRotaryTable::UpdateRotaryTableReward(LC_ServerPlayer* pkOwnerPlayer, bool force)
{
	if(0==m_nCurrentRTRewardIndex || NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	if(false==force && 0==m_nRotaryTableCDStartTime)
	{
		return;
	}

	bool tag = force;
	if(!force)
	{
		int64_t currentTime = GET_CURRENT_TIMESTAMP_IN_MILISECONDS();
		int64_t totalTime = GetGlobalSetting.RotaryTableRegularTime*1000+GetGlobalSetting.RotaryTableUnitTime*m_nCurrentRTRewardIndex*1000;
		tag = currentTime-m_nRotaryTableCDStartTime >= totalTime;
	}

	if(tag)
	{
		CF_RotaryTableReward::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RotaryTableReward>(m_nCurrentRTRewardIndex);
		if(NULL != pkData)
		{
			//幸运转盘
			vector<int> nParams;
			nParams.clear();
			nParams.push_back(GetRotaryTableTimes()+1);
			int nLogCode= LC_Helper::SerializeOperateLogCode(ACTIVITY_LOGTYPE, ACTIVITY3_OPERATE, OPERATE_LOGTYPE1);
			ResultType res = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkData->_RotaryTableReward, pkOwnerPlayer, SYS_FUNC_TP_ROTARYTABLE_REWARD, m_nCurrentRTRewardIndex, "65;", "66;", true,false, false,nLogCode,nParams);
			if(RE_SUCCESS == res)
			{
				//扣次数，通知客户端已发奖
				UpdateRotaryTableVaildTimes(pkOwnerPlayer, -1);
				UpdateRotaryTableRewardIndex(pkOwnerPlayer, m_nCurrentRTRewardIndex);
			}
		}

		SetRotaryTableCDStartTime(0);
	}
}

void CRotaryTable::SyncAllRotaryTableInfo(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	SyncRotaryTableVaildTimes(pkOwnerPlayer);
	SyncRotaryTableRewardRecord(pkOwnerPlayer);
	SyncRotaryTableEndTime(pkOwnerPlayer);
	SyncRotaryTableSkipAnimation(pkOwnerPlayer);
}

void CRotaryTable::SetRotaryTableCDStartTime(int64_t time)
{
	if(0 == time)
	{
		SetCurrentRotaryTableRewardIndex(0);
	}
	else
	{
		int64_t totalTime = GetGlobalSetting.RotaryTableRegularTime*1000+GetGlobalSetting.RotaryTableUnitTime*m_nCurrentRTRewardIndex*1000;
		int64_t endTime = m_nRotaryTableEndTime*1000.0;
		if(endTime-time < totalTime)
		{
			time = m_nRotaryTableEndTime-totalTime;
		}
	}

	m_nRotaryTableCDStartTime = time;
}

void CRotaryTable::UpdateRotaryTableVaildTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t value)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	if(value > 0 && RE_SUCCESS!=CheckRotaryTableVaild())
	{
		return;
	}

	m_nRotaryTableVaildTimes += value;
	if(m_nRotaryTableVaildTimes < 0)
	{
		m_nRotaryTableVaildTimes = 0;
	}
	SyncRotaryTableVaildTimes(pkOwnerPlayer);
}
///////////////////////////////
void CRotaryTable::SyncRotaryTableVaildTimes(LC_ServerPlayer* pkOwnerPlayer)
{
	MG_SyncRotaryTableVaildTimes msg;
	msg.m_nVaildTimes = m_nRotaryTableVaildTimes;
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCROTARYTABLEVAILDTIMES, &msg);
}

void CRotaryTable::SyncRotaryTableRewardRecord(LC_ServerPlayer* pkOwnerPlayer)
{
	MG_SyncRotaryTableRewardRecord msg;
	msg.m_nRewardIndexs.reserve(m_nRotaryTableRewardIndex.size());
	for(int i=0; i<m_nRotaryTableRewardIndex.size(); ++i)
	{
		msg.m_nRewardIndexs.push_back(m_nRotaryTableRewardIndex[i]);
	}

	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCROTARYTABLEREWARDRECORD, &msg);
}

void CRotaryTable::SyncRotaryTableEndTime(LC_ServerPlayer* pkOwnerPlayer)
{
	MG_SyncRotaryTableEndTime msg;
	msg.m_nRotaryTableEndTime = m_nRotaryTableEndTime;
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCROTARYTABLEENDTIME, &msg);
}

void CRotaryTable::SyncRotaryTableSkipAnimation(LC_ServerPlayer* pkOwnerPlayer)
{
	MG_SyncRotaryTableSkipAnimation msg;
	msg.m_bSkipAnimation = m_bSkipAnimation;
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCROTARYTABLESKIPANIMATION, &msg);
}

ResultType CRotaryTable::CheckRotaryTableVaild()
{
	int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	return m_nRotaryTableEndTime>currentTime ? RE_SUCCESS : RE_ACTIVITY_NOT_VALID;
}

int32_t CRotaryTable::GetRotaryTableVaildTimes()
{
	return m_nRotaryTableVaildTimes;
}

int32_t CRotaryTable::GetRotaryTableTimes()
{
	return m_nRotaryTableRewardIndex.size();
}

ResultType CRotaryTable::RandomRotaryTableRewardIndex(int32_t times, int32_t& rewardIndex)
{
	CF_RotaryTable::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RotaryTable>(times);
	if(NULL == pkData)
	{
		return RE_FAIL;
	}

	mem::vector<int> vaildIndex;
	for(Utility::UT_SIMDataList::iterator it=pkData->_RewardIndex.begin(); it!=pkData->_RewardIndex.end(); ++it)
	{
		int index = it->IID();
		bool tag = CheckRotaryTableRewardIndexVaild(index);
		if(tag)
		{
			vaildIndex.push_back(index);
		}
	}

	int vaildIndexSize = vaildIndex.size();
	if(vaildIndexSize == 0)
	{
		return RE_FAIL;
	}
	else if(vaildIndexSize > 1)
	{
		std::random_shuffle(vaildIndex.begin(), vaildIndex.end());
	}

	rewardIndex = vaildIndex[0];
	return RE_SUCCESS;
}

void CRotaryTable::UpdateRotaryTableRewardIndex(LC_ServerPlayer* pkOwnerPlayer, int32_t rewardIndex)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	mem::vector<int32_t>::iterator findIt = find(m_nRotaryTableRewardIndex.begin(), m_nRotaryTableRewardIndex.end(), rewardIndex);
	if(findIt == m_nRotaryTableRewardIndex.end())
	{
		m_nRotaryTableRewardIndex.push_back(rewardIndex);
		SyncRotaryTableRewardRecord(pkOwnerPlayer);
	}
}

bool CRotaryTable::CheckRotaryTableRewardIndexVaild(int32_t rewardIndex)
{
	mem::vector<int32_t>::iterator findIt = find(m_nRotaryTableRewardIndex.begin(), m_nRotaryTableRewardIndex.end(), rewardIndex);
	return findIt==m_nRotaryTableRewardIndex.end() ? true : false;
}

void CRotaryTable::SetCurrentRotaryTableRewardIndex(int32_t rewardIndex)
{
	m_nCurrentRTRewardIndex = rewardIndex;
}
