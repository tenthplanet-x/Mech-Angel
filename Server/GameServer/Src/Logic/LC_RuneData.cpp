#include "LC_RuneData.h"
#include "LC_ServerPlayer.h"
#include "MG_GameProtocol.h"
#include "MG_TYPES.h"
#include "MG_TaskProtocol.h"

#include "CF_RuneList.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;

CRuneData::CRuneData() : m_bActiveState(false), m_nTotalRuneLevel(0), m_nTotalRuneStarLevel(0)
{
	m_runeRewardRecord.clear();
}

CRuneData::~CRuneData()
{

}

void CRuneData::Init()
{
	m_bActiveState = false;
	m_nTotalRuneLevel = 0;
	m_nTotalRuneStarLevel = 0;
	m_runeRewardRecord.clear();
}

void CRuneData::LoadData(const char_data_define::PBRuneData& data)
{
	m_runeRewardRecord.clear();

	m_nTotalRuneLevel = data.runetotallevel();
	m_nTotalRuneStarLevel = data.runetotalstarlevel();
	m_bActiveState = data.activestate();
	for(int i=0; i<data.runerecord_size(); ++i)
	{
		int runeID = data.runerecord(i).runeitemid();
		bool tag = data.runerecord(i).addreward();
		m_runeRewardRecord.insert(std::make_pair(runeID, tag));
	}
}

void CRuneData::SaveData(char_data_define::PBRuneData* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	pkData->set_runetotallevel(m_nTotalRuneLevel);
	pkData->set_runetotalstarlevel(m_nTotalRuneStarLevel);
	pkData->set_activestate(m_bActiveState);
	for(mem::map<int32_t, bool>::iterator it=m_runeRewardRecord.begin(); it!=m_runeRewardRecord.end(); ++it)
	{
		char_data_define::char_runeRecord* data = pkData->add_runerecord();
		data->set_runeitemid(it->first);
		data->set_addreward(it->second);
	}
}

int32_t CRuneData::GetTotalLevel()
{
	return m_nTotalRuneLevel;
}

void CRuneData::SetTotalLevel(int32_t level)
{
	m_nTotalRuneLevel = level;
}

int32_t CRuneData::GetTotalStarLevel()
{
	return m_nTotalRuneStarLevel;
}

void CRuneData::SetTotalStarLevel(int32_t starLevel)
{
	m_nTotalRuneStarLevel = starLevel;
}

void CRuneData::RuneReward(LC_ServerPlayer* pkPlayer, int32_t itemID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_RLT_RuneReward rltMsg;
	rltMsg.m_nRewardID = itemID;
	rltMsg.m_nResult = DoRuneReward(pkPlayer, itemID);
	pkPlayer->SendMsgToClient(MGPT_RESP_RUNE_REWARD, &rltMsg);
}

void CRuneData::SyncRuneRecord(LC_ServerPlayer* pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	MG_SyncRuneRecord sync;
	for(mem::map<int32_t, bool>::iterator it=m_runeRewardRecord.begin(); it!=m_runeRewardRecord.end(); ++it)
	{
		sync.m_nRuneRecord.insert(std::make_pair(it->first, it->second));
	}

	pkPlayer->SendMsgToClient(MGPT_SYNC_RUNERECORD, &sync);
}


bool	CRuneData::GetBuddyRuneEquipState()
{
	return m_bActiveState;
}

bool	CRuneData::SetBuddyRuneSlotState()
{
	if(m_bActiveState != true)
	{
		m_bActiveState = true;
	}

	return true;
}

bool CRuneData::UpdateRuneRewardRecord(int32_t lItemID, bool addReward, bool modifyAddReward)
{
	bool tag = false;
	//是否出现在记录集中
	mem::map<int32_t, bool>::iterator it = m_runeRewardRecord.find(lItemID);
	if(it == m_runeRewardRecord.end())
	{
		tag = true;
		m_runeRewardRecord.insert(std::make_pair(lItemID, addReward));
	}
	else if(modifyAddReward && it->second != addReward)
	{
		it->second = addReward;
	}

	return tag;
}

//------------------protected:
ResultType CRuneData::DoRuneReward(LC_ServerPlayer* pkPlayer, int32_t itemID)
{
	//参数检测
	if(NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(itemID);
	if(NULL == pkData)
	{
		return RE_ERROR_PARAM;
	}

	//是否已经领过奖励
	mem::map<int32_t, bool>::iterator it = m_runeRewardRecord.find(itemID);
	if(it == m_runeRewardRecord.end())
	{
		return RE_ERROR_PARAM;
	}
	else if(it->second)
	{
		return RE_RUNE_ITEM_ALREADY_GET;
	}
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(itemID);
	//埋点日志
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BAOJU_OPERATE, OPERATE_LOGTYPE5);
	StringType mailTitle;
	StringType mailDesc;
	ResultType res = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkData->_sReward, pkPlayer, SYS_FUNC_TP_RUNE_REWARD, itemID, mailTitle, mailDesc, false,false,false,nLogCode, nParams);
	if(RE_SUCCESS == res)
	{
		UpdateRuneRewardRecord(itemID, true, true);
	}

	return res;
}