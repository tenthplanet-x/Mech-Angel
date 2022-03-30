#include "ClientPCH.h"

#include "LC_ClientGuildProfitManager.h"
#include "LC_ClientLogicManager.h"
#include "UT_FileManagerBase.h"
#include "SC_ClientScriptInterface.h"


using namespace GameLogic;


LC_ClientGuildProfitManager::LC_ClientGuildProfitManager()
: NoDrawedNormalProfit(0)
, NoDrawedRareProfit(0)
{

}

LC_ClientGuildProfitManager::~LC_ClientGuildProfitManager()
{

}

void LC_ClientGuildProfitManager::Init()
{
	::CreateDirectoryW(L"guildprofit" , NULL );

	Reset();
}

void LC_ClientGuildProfitManager::Unit()
{
	Reset();
}

void LC_ClientGuildProfitManager::Reset()
{
	ReUse();
}

void LC_ClientGuildProfitManager::ReUse()
{
	m_mDetailNormalMap.clear();
	m_mDetailRareMap.clear();
	m_vProfitNormalList.clear();
	m_vProfitRareList.clear();
	NoDrawedNormalProfit = 0;
	NoDrawedRareProfit = 0;
	m_uiID = 0;
}

void LC_ClientGuildProfitManager::InitData(const StringType& str)
{
	TStringVector kVector = UT_BaseStringUtility::SplitString(str, '@');
	for (int k = 0; k < kVector.size(); ++k)
	{
		if (k == 0)
			SetRecord(m_mDetailNormalMap, kVector[k]);
		else if (k == 1)
			SetRecord(m_mDetailRareMap, kVector[k]);
		else
			break;
	}
}

void LC_ClientGuildProfitManager::SetRecord(mem::map<uint16_t, ProfitInfo>& list, const StringType& str)
{
	TStringVector detail = UT_BaseStringUtility::SplitString(str, '#');
	for (int i = 0; i < detail.size(); ++i)
	{
		TStringVector info = UT_BaseStringUtility::SplitString(detail[i], '|');
		if (info.size() == 2)
		{
			uint16_t id = atoi(info[0].c_str());
			const StringType& reward = info[1];

			mem::map<uint16_t, ProfitInfo>::iterator iter = list.find(id);
			if (iter != list.end())
			{
				iter->second.m_sRewardList = reward;
			}
		}
	}
}

uint16_t LC_ClientGuildProfitManager::GetProfitIDByKey(int key, bool bNormal)
{
	if (bNormal)
	{
		if (key >= m_vProfitNormalList.size())
			return 0;
		return m_vProfitNormalList[key]->m_iProfitID;
	}
	else
	{
		if (key >= m_vProfitRareList.size())
			return 0;
		return m_vProfitRareList[key]->m_iProfitID;
	}
}

int32_t LC_ClientGuildProfitManager::GetItemIDByKey(int key, bool bNormal)
{
	if (bNormal && key < m_vProfitNormalList.size())
	{
		return m_vProfitNormalList[key]->m_iItemID;
		
	}
	else if (key < m_vProfitRareList.size())
	{
		return m_vProfitRareList[key]->m_iItemID;
	}
	return 0;
}

StringType LC_ClientGuildProfitManager::GetCharNameByKey(int key, bool bNormal)
{
	if (bNormal && key < m_vProfitNormalList.size())
	{
		return m_vProfitNormalList[key]->m_sCharName;

	}
	else if (key < m_vProfitRareList.size())
	{
		return m_vProfitRareList[key]->m_sCharName;
	}
	return "";
}

int LC_ClientGuildProfitManager::GetPresentTypeByKey(int key, bool bNormal)
{
	if (bNormal && key < m_vProfitNormalList.size())
	{
		return m_vProfitNormalList[key]->m_iPresentType;
	}
	else if (key < m_vProfitRareList.size())
	{
		return m_vProfitRareList[key]->m_iPresentType;
	}
	return 0;
}

int32_t LC_ClientGuildProfitManager::GetInvaildTimeByKey(int key, bool bNormal)
{
	if (bNormal && key < m_vProfitNormalList.size())
	{
		return m_vProfitNormalList[key]->m_iInvaildTime;

	}
	else if (key < m_vProfitRareList.size())
	{
		return m_vProfitRareList[key]->m_iInvaildTime;
	}
	return 0;
}

bool LC_ClientGuildProfitManager::GetProfitIsDrawedByKey(int key, bool bNormal)
{
	if (bNormal && key < m_vProfitNormalList.size())
	{
		return m_vProfitNormalList[key]->m_bIsDrawed;
	}
	else if (key < m_vProfitRareList.size())
	{
		return m_vProfitRareList[key]->m_bIsDrawed;
	}
	return false;
}

StringType LC_ClientGuildProfitManager::GetProfitRecordByKey(int key, bool bNormal)
{
	if (bNormal && key < m_vProfitNormalList.size())
	{
		return m_vProfitNormalList[key]->m_sRewardList;
	}
	else if (key < m_vProfitRareList.size())
	{
		return m_vProfitRareList[key]->m_sRewardList;
	}
	return "";
}

void LC_ClientGuildProfitManager::SyncProfitRecord(const std::map<uint16_t,PS_PersonGuildPresentInfo>& info)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	std::map<uint16_t,PS_PersonGuildPresentInfo>::const_iterator itor = info.begin();
	for (; itor != info.end(); ++itor)
	{
		int igrade = pkInterface->GetMadeItemGrade(itor->second.m_nItemID);
		if (igrade <= 3)
		{
			ProfitInfo& profit = m_mDetailNormalMap[itor->first];
			profit.m_iProfitID = itor->first;
			profit.m_iItemID = itor->second.m_nItemID;
			profit.m_sCharName = STDSTR2TPSTR(itor->second.m_nCharName);
			profit.m_iPresentType = itor->second.m_nPresentType;
			profit.m_bIsDrawed = itor->second.m_nRewardState;
			profit.m_iInvaildTime = itor->second.m_nInvaildTime;

			if (!itor->second.m_nRewardState)
				NoDrawedNormalProfit += 1;
			m_vProfitNormalList.push_back(&profit);
		}
		else
		{
			ProfitInfo& profit = m_mDetailRareMap[itor->first];
			profit.m_iProfitID = itor->first;
			profit.m_iItemID = itor->second.m_nItemID;
			profit.m_sCharName = STDSTR2TPSTR(itor->second.m_nCharName);
			profit.m_iPresentType = itor->second.m_nPresentType;
			profit.m_bIsDrawed = itor->second.m_nRewardState;
			profit.m_iInvaildTime = itor->second.m_nInvaildTime;

			if (!itor->second.m_nRewardState)
				NoDrawedRareProfit += 1;
			m_vProfitRareList.push_back(&profit);
		}
	}
}

bool LC_ClientGuildProfitManager::AddProfitRecord(const MG_AddGuildPresentInfo& Info)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	int igrade = pkInterface->GetMadeItemGrade(Info.m_nData.m_nItemID);
	if (igrade <= 3)
	{
		ProfitInfo& profit = m_mDetailNormalMap[Info.m_nID];
		profit.m_iProfitID = Info.m_nID;
		profit.m_iItemID = Info.m_nData.m_nItemID;
		profit.m_sCharName = STDSTR2TPSTR(Info.m_nData.m_nCharName);
		profit.m_iPresentType = Info.m_nData.m_nPresentType;
		profit.m_bIsDrawed = Info.m_nData.m_nRewardState;
		profit.m_iInvaildTime = Info.m_nData.m_nInvaildTime;

		NoDrawedNormalProfit += 1;
		m_vProfitNormalList.push_back(&profit);
	}
	else
	{
		ProfitInfo& profit = m_mDetailRareMap[Info.m_nID];
		profit.m_iProfitID = Info.m_nID;
		profit.m_iItemID = Info.m_nData.m_nItemID;
		profit.m_sCharName = STDSTR2TPSTR(Info.m_nData.m_nCharName);
		profit.m_iPresentType = Info.m_nData.m_nPresentType;
		profit.m_bIsDrawed = Info.m_nData.m_nRewardState;
		profit.m_iInvaildTime = Info.m_nData.m_nInvaildTime;

		NoDrawedRareProfit += 1;
		m_vProfitRareList.push_back(&profit);
	}

	SortProfitRecord(igrade <= 3, igrade > 3);
	return igrade <= 3;
}

bool LC_ClientGuildProfitManager::AddProfitRecord(const std::map<uint16_t,std::string>& info)
{
	std::map<uint16_t, std::string>::const_iterator itor = info.begin();
	bool bNormalChange = false;
	bool bRareChange = false;
	for (; itor != info.end(); ++itor)
	{
		mem::map<uint16_t, ProfitInfo>::iterator iter = m_mDetailNormalMap.find(itor->first);
		if (iter != m_mDetailNormalMap.end())
		{
			iter->second.m_bIsDrawed = true;
			iter->second.m_sRewardList = STDSTR2TPSTR(itor->second);
			bNormalChange = true;
			NoDrawedNormalProfit -= 1;
		}
		else
		{
			mem::map<uint16_t, ProfitInfo>::iterator it = m_mDetailRareMap.find(itor->first);
			if (it != m_mDetailRareMap.end())
			{
				it->second.m_bIsDrawed = true;
				it->second.m_sRewardList = STDSTR2TPSTR(itor->second);
				bRareChange = true;
				NoDrawedRareProfit -= 1;
			}
		}
	}

	SortProfitRecord(bNormalChange, bRareChange);
	return bNormalChange;
}

bool LC_ClientGuildProfitManager::DelProfitRecord(uint16_t id)
{
	bool bNormalChange = false;
	mem::map<uint16_t, ProfitInfo>::iterator iter = m_mDetailNormalMap.find(id);
	if (iter != m_mDetailNormalMap.end())
	{
		if (!iter->second.m_bIsDrawed)
		{
			NoDrawedNormalProfit -= 1;
		}
		m_mDetailNormalMap.erase(iter);
		bNormalChange = true;
		for (mem::vector<ProfitInfo*>::iterator iter = m_vProfitNormalList.begin(); iter != m_vProfitNormalList.end(); ++iter)
		{
			if ((*iter)->m_iProfitID == id)
			{
				m_vProfitNormalList.erase(iter);
				break;
			}
		}
	}
	else
	{
		mem::map<uint16_t, ProfitInfo>::iterator itor = m_mDetailRareMap.find(id);
		if (itor != m_mDetailRareMap.end())
		{
			if (!itor ->second.m_bIsDrawed)
			{
				NoDrawedRareProfit -= 1;
			}
			m_mDetailRareMap.erase(iter);
			bNormalChange = false;
			for (mem::vector<ProfitInfo*>::iterator iter = m_vProfitRareList.begin(); iter != m_vProfitRareList.end(); ++iter)
			{
				if ((*iter)->m_iProfitID == id)
				{
					m_vProfitRareList.erase(iter);
					break;
				}
			}
		}
	}
	SortProfitRecord(bNormalChange, !bNormalChange);
	return bNormalChange;
}

void LC_ClientGuildProfitManager::SortProfitRecord(bool bNormal, bool bRare)
{
	int num = GetGlobalSetting.ClientMaxPresentNum;
	if (bNormal)
	{
		if (m_vProfitNormalList.size() < num)
			num = m_vProfitNormalList.size();
		std::sort(m_vProfitNormalList.begin(), m_vProfitNormalList.end(),  _sortGuildProfit);
	}
	if (bRare)
	{
		if (m_vProfitRareList.size() < num)
			num = m_vProfitRareList.size();
		std::sort(m_vProfitRareList.begin(), m_vProfitRareList.end(),  _sortGuildProfit);
	}
}

int LC_ClientGuildProfitManager::GetNoDrawedCount(bool bNormal)
{
	if (bNormal)
		return NoDrawedNormalProfit;

	return NoDrawedRareProfit;
}

StringType LC_ClientGuildProfitManager::_getSaveString()
{
	StringStreamType sStream;
	float itime = GET_PROCESS_TIME;
	for (int i = 0; i < m_vProfitNormalList.size(); ++i)
	{
		ProfitInfo* info = m_vProfitNormalList[i];
		if (info->m_iInvaildTime > itime && info->m_sRewardList != "")
		{
			sStream << info->m_iProfitID << "|" << info->m_sRewardList << "#";
		}
	}
	sStream << "@";
	for (int i = 0; i < m_vProfitRareList.size(); ++i)
	{
		ProfitInfo* info = m_vProfitRareList[i];
		if (info->m_iInvaildTime > itime && info->m_sRewardList != "")
		{
			sStream << info->m_iProfitID << "|" << info->m_sRewardList << "#";
		}
	}
	sStream << "@";
	return sStream.str();
}

void LC_ClientGuildProfitManager::LoadFile(unique_id_type uiID)
{
	if(m_uiID > 0)
		return;

	m_uiID = uiID;

	char szFileName[64] = {0};
	sprintf(szFileName,"%s/%lld.data", STR_GUILDPROFIT_DIR, m_uiID);

	FILE* f = fopen(szFileName, "rb");
	if (NULL == f)
	{
		DWORD errorValue = GetLastError();
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "LC_ClientGuildProfitManager::LoadFile Error=%d", errorValue);
		return;
	}

	fseek(f, 0, SEEK_END);
	unsigned int sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	StringType sVecContent;
	sVecContent.resize(sz);
	fread(&sVecContent[0],sz, 1, f);

	fclose(f);

	InitData(sVecContent);
}

void LC_ClientGuildProfitManager::SaveFile()
{
	if(m_uiID == 0)
		return;
	
	const StringType& sContent = _getSaveString();

	char szFileName[64] = {0};
	sprintf(szFileName, "%s/%I64d.data", STR_GUILDPROFIT_DIR, m_uiID);
	
	FILE* f = fopen(szFileName, "wb+");
	if(!f)
	{
		DWORD errorValue = GetLastError();
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "LC_ClientGuildProfitManager::_saveFile Error=%d", errorValue);
		return;
	}

	fwrite(sContent.c_str(),sContent.length(), 1, f);
	fclose(f);
}

bool GameLogic::LC_ClientGuildProfitManager::_sortGuildProfit(const ProfitInfo* v1, const ProfitInfo* v2)
{
	if(v1->m_bIsDrawed == v2->m_bIsDrawed)
		return v1->m_iInvaildTime < v2->m_iInvaildTime;

	return !v1->m_bIsDrawed;
}
