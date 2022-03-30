#include "LC_NoviceTutorial.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "MG_GameProtocol.h"
#include "MG_SyncProtocol.h"

#include "CF_TutorialRewards.h"

CNoviceTutorial::CNoviceTutorial()
{
	m_vNoviceTutorialRecord.clear();
	m_vGetRewardNoviceTutorial.clear();
}

CNoviceTutorial::~CNoviceTutorial()
{
}

void CNoviceTutorial::LoadData(const ::char_data_define::PBNoviceTutorial& data)
{
	m_vNoviceTutorialRecord.clear();
	m_vNoviceTutorialRecord.reserve(data.tutorialid_size());
	for(int i=0; i<data.tutorialid_size(); ++i)
	{
		m_vNoviceTutorialRecord.push_back(data.tutorialid(i));
	}

	m_vGetRewardNoviceTutorial.clear();
	m_vGetRewardNoviceTutorial.reserve(data.tutorialrewardid_size());
	for(int i=0; i<data.tutorialrewardid_size(); ++i)
	{
		m_vGetRewardNoviceTutorial.push_back(data.tutorialrewardid(i));
	}
}

void CNoviceTutorial::SaveData(::char_data_define::PBNoviceTutorial* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	for(int i=0; i<m_vNoviceTutorialRecord.size(); ++i)
	{
		pkData->add_tutorialid(m_vNoviceTutorialRecord[i]);
	}

	for(int i=0; i<m_vGetRewardNoviceTutorial.size(); ++i)
	{
		pkData->add_tutorialrewardid(m_vGetRewardNoviceTutorial[i]);
	}
}

void CNoviceTutorial::AddNoviceTutorial(LC_ServerPlayer* pkOwnerPlayer, uint32_t tutorialID)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AddNoviceTutorial r1! tutorialID:%d", tutorialID);
		return;
	}

	CF_TutorialRewards::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_TutorialRewards>(tutorialID);
	if(NULL == pkData)
	{
		const unique_id_impl& instance = pkOwnerPlayer->GetInstance();
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AddNoviceTutorial r2! tutorialID:%d, c_cid:%d, c_citizenship:%d, c_uid:%d", 
			tutorialID, instance.detail(), instance.catagory(), pkOwnerPlayer->GetUserID());
		return;
	}

	MG_RLT_AddNoviceTutorial rltMsg;
	rltMsg.m_nNoviceTutorialID = tutorialID;
	rltMsg.m_nResult = RE_SUCCESS;

	//是否重复添加
	mem::vector<uint32_t>::iterator findIt = find(m_vNoviceTutorialRecord.begin(), m_vNoviceTutorialRecord.end(), tutorialID);
	if(findIt != m_vNoviceTutorialRecord.end())
	{
		rltMsg.m_nResult = RE_NOVICE_TUTORIAL_ERROR_3;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_ADDNOVICETUTORIAL, &rltMsg);
		return;
	}

	//是否已经添加前置教程
	if(0 != pkData->_iPreID)
	{
		findIt = find(m_vNoviceTutorialRecord.begin(), m_vNoviceTutorialRecord.end(), pkData->_iPreID);
		if(findIt == m_vNoviceTutorialRecord.end())
		{
			rltMsg.m_nResult = RE_NOVICE_TUTORIAL_ERROR_4;
			pkOwnerPlayer->SendMsgToClient(MGPT_RLT_ADDNOVICETUTORIAL, &rltMsg);
			return;
		}
	}

	m_vNoviceTutorialRecord.push_back(tutorialID);
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_ADDNOVICETUTORIAL, &rltMsg);
}

void CNoviceTutorial::GetNoviceTutorialReward(LC_ServerPlayer* pkOwnerPlayer, uint32_t tutorialID)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetNoviceTutorialReward r1! tutorialID:%d", tutorialID);
		return;
	}

	CF_TutorialRewards::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_TutorialRewards>(tutorialID);
	if(NULL == pkData || pkData->_kRewards.empty())
	{
		const unique_id_impl& instance = pkOwnerPlayer->GetInstance();
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GetNoviceTutorialReward r2! tutorialID:%d, c_cid:%d, c_citizenship:%d, c_uid:%d", 
			tutorialID, instance.detail(), instance.catagory(), pkOwnerPlayer->GetUserID());
		return;
	}

	MG_RLT_GetNoviceTutorialReward rltMsg;
	rltMsg.m_nNoviceTutorialID = tutorialID;
	
	//是否重复领奖
	mem::vector<uint32_t>::iterator findIt1 = find(m_vGetRewardNoviceTutorial.begin(), m_vGetRewardNoviceTutorial.end(), tutorialID);
	if(findIt1 != m_vGetRewardNoviceTutorial.end())
	{
		rltMsg.m_nResult = RE_NOVICE_TUTORIAL_ERROR_1;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNOVICETUTORIALREWARD, &rltMsg);
		return;
	}

	//是否可以领奖
	mem::vector<uint32_t>::iterator findIt2 = find(m_vNoviceTutorialRecord.begin(), m_vNoviceTutorialRecord.end(), tutorialID);
	if(findIt2 == m_vNoviceTutorialRecord.end())
	{
		rltMsg.m_nResult = RE_NOVICE_TUTORIAL_ERROR_2;
		pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNOVICETUTORIALREWARD, &rltMsg);
		return;
	}

	//领奖
	StringType str;
	rltMsg.m_nResult = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkData->_kRewards, pkOwnerPlayer, SYS_FUNC_TP_NoviceTutorialReward, tutorialID, str, str, false);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		m_vGetRewardNoviceTutorial.push_back(tutorialID);
	}
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_GETNOVICETUTORIALREWARD, &rltMsg);
}

void CNoviceTutorial::SyncNoviceTutorialInfo(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SyncNoviceTutorialInfo r1!");
		return;
	}

	MG_SyncNoviceTutorialInfo msg;
	msg.m_vNoviceTutorialRecord.reserve(m_vNoviceTutorialRecord.size());
	msg.m_vNoviceTutorialRecord.insert(msg.m_vNoviceTutorialRecord.end(), m_vNoviceTutorialRecord.begin(), m_vNoviceTutorialRecord.end());

	msg.m_vNoviceTutorialRecord.reserve(m_vGetRewardNoviceTutorial.size());
	msg.m_vGetRewardNoviceTutorial.insert(msg.m_vGetRewardNoviceTutorial.end(), m_vGetRewardNoviceTutorial.begin(), m_vGetRewardNoviceTutorial.end());

	pkOwnerPlayer->SendMsgToClient(MGPT_SYNC_NOVICETUTORIALINFO, &msg);
}