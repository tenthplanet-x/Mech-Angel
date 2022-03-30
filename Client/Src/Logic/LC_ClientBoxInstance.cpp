#include "ClientPCH.h"
#include "LC_ClientBoxInstance.h"
//#include "GlobalSettings.h"

using namespace GameLogic;

LC_ClientBotInstance::LC_ClientBotInstance()
{
	iTalentReward = 0;
	iTalentEfficiency = 0;
	Reuse();
}

LC_ClientBotInstance::~LC_ClientBotInstance()
{
	Reuse();
}


void LC_ClientBotInstance::Init()
{
	CF_Bot::DataEntry *pkEntry = CF_Bot::GetSingleton().GetEntryPtr(cID);
	if ( pkEntry != NULL )
	{
		bIsAvtive = false;
		m_pkCsvData = pkEntry;
		iLevel = 1;

		m_kActiveConsumption.clear();
		if(!pkEntry->_sConsumption.empty())
		{
			UT_SSIMDataList kItem;
			ParseFmtString2DataIFs(pkEntry->_sConsumption, kItem);

			for(UT_SSIMDataList::const_iterator iter = kItem.begin(); iter!=kItem.end(); ++iter)
			{
				const UT_SIMDataSimpleInfo<int32_t>& kItemInfo = *iter;
				BotReward rwd(kItemInfo.IID(), kItemInfo.ParamA());
				m_kActiveConsumption.push_back(rwd);
			}
		}
	}


	GetLevelData();
}

void LC_ClientBotInstance::Reuse()
{
	cID = 0;
	bIsAvtive = false;
	iLevel = 1;
	sUpLevelConsumption = "";
	nTimeStamp = 0;
	m_pkCsvData = NULL;

	m_kLevelResouse.clear();
	m_kActiveConsumption.clear();
	m_kUpLevelConsumption.clear();
}


void	LC_ClientBotInstance::ParseProtocol(const BotData_Base& BotData)
{
	nTimeStamp = BotData.m_nTimeStamp;
	iLevel = BotData.m_nLevel;
	bIsAvtive = true;
	CF_BotUpgrade::DataEntryMapExternIterator kIter = CF_BotUpgrade::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BotUpgrade::DataEntry *pkUpEntry = kIter.PeekNextValuePtr();
		if ( pkUpEntry->_cID == cID && pkUpEntry->_iLevel == iLevel )
		{
			sUpLevelConsumption = pkUpEntry->_sConsumption;

			m_kUpLevelConsumption.clear();
			if(!sUpLevelConsumption.empty())
			{
				UT_SSIMDataList kItem;
				ParseFmtString2DataIFs(sUpLevelConsumption, kItem);

				for(UT_SSIMDataList::const_iterator iter = kItem.begin(); iter!=kItem.end(); ++iter)
				{
					const UT_SIMDataSimpleInfo<int32_t>& kItemInfo = *iter;
					BotReward rwd(kItemInfo.IID(), kItemInfo.ParamA());
					m_kUpLevelConsumption.push_back(rwd);
				}
			}

			break;
		}
	}
}

void	LC_ClientBotInstance::GetLevelData()
{
	CF_BotUpgrade::DataEntryMapExternIterator kIter = CF_BotUpgrade::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BotUpgrade::DataEntry *pkUpEntry = kIter.PeekNextValuePtr();
		if ( pkUpEntry->_cID == cID )
		{
			mem::map<int, ResouseReward>::iterator Iter = m_kLevelResouse.find(pkUpEntry->_iLevel);
			if (Iter == m_kLevelResouse.end())
			{
				ResouseReward kResouseReward;
				BotReward reward1(pkUpEntry->_iReward1, pkUpEntry->_iEfficiency1);
				kResouseReward.push_back(reward1);
				BotReward reward2(pkUpEntry->_iReward2, pkUpEntry->_iEfficiency2);
				kResouseReward.push_back(reward2);
				BotReward reward3(pkUpEntry->_iReward3, pkUpEntry->_iEfficiency3);
				kResouseReward.push_back(reward3);
				BotReward reward4(pkUpEntry->_iReward4, pkUpEntry->_iEfficiency4);
				kResouseReward.push_back(reward4);
				BotReward reward5(pkUpEntry->_iReward5, pkUpEntry->_iEfficiency5);
				kResouseReward.push_back(reward5);
				BotReward reward6(pkUpEntry->_iReward6, pkUpEntry->_iEfficiency6);
				kResouseReward.push_back(reward6);
				BotReward reward7(pkUpEntry->_iReward7, pkUpEntry->_iEfficiency7);
				kResouseReward.push_back(reward7);
				BotReward reward8(pkUpEntry->_iReward8, pkUpEntry->_iEfficiency8);
				kResouseReward.push_back(reward8);
				BotReward reward9(pkUpEntry->_iReward9, pkUpEntry->_iEfficiency9);
				kResouseReward.push_back(reward9);
				BotReward reward10(pkUpEntry->_iReward10, pkUpEntry->_iEfficiency10);
				kResouseReward.push_back(reward10);


				m_kLevelResouse[pkUpEntry->_iLevel] = kResouseReward;
			}
		}
	}
}

StringType LC_ClientBotInstance::GetBotInfo()
{
	T_ASSERT_CRITICAL_S(m_pkCsvData != NULL);

	StringStreamType sStream;
	sStream << cID << "#";
	sStream << bIsAvtive << "#";
	sStream << m_pkCsvData->_sName << "#";
	sStream << Int2String(m_pkCsvData->_iQuality) << "#";
	sStream << iLevel << "#";
	sStream << m_pkCsvData->_sModelIcon << "#";
	sStream << m_pkCsvData->_sConsumption << "#";
	sStream << sUpLevelConsumption << "#";
	sStream << ContructTarentInfo() << "#";
	sStream << nTimeStamp << "#";
	sStream << m_pkCsvData->_sRequirement <<"#";
	sStream << m_pkCsvData->_charType << "#";
	mem::map<int,ResouseReward>::const_iterator levelIter = m_kLevelResouse.find(iLevel);
	if (levelIter != m_kLevelResouse.end())
	{
		int i = 0;
		ResouseReward kResouseReward = levelIter->second;
		for (mem::vector<BotReward>::iterator Iter = kResouseReward.begin(); Iter != kResouseReward.end(); ++Iter)
		{
			if (Iter->itemCount > 0)
			{
				sStream << Int2String(i) << ":" << Int2String(Iter->itemID) << ":" << Int2String(Iter->itemCount) << ";";
			}
			i++;
		}
	}
	sStream << "#";
	sStream << m_pkCsvData->_iMaxLevel << "#";
	sStream << m_pkCsvData->_iTalentMaxLevel << "#";
	return sStream.str();
}

StringType LC_ClientBotInstance::GetLevelUpData()
{
	StringStreamType sStream("");
	mem::map<int,ResouseReward>::const_iterator levelIter = m_kLevelResouse.find(iLevel+1);
	if (levelIter != m_kLevelResouse.end())
	{
		ResouseReward kResouseReward = levelIter->second;
		for (mem::vector<BotReward>::iterator Iter = kResouseReward.begin(); Iter != kResouseReward.end(); ++Iter)
		{
			if (Iter->itemCount > 0)
			{
				sStream << Iter->itemID << ":" << Iter->itemCount << ";";
			}
		}
	}
	else
		sStream << "0;0;0;";

	return sStream.str();
}

BotPromptType LC_ClientBotInstance::GetPrompt(LC_ClientGamePlayer* pkGamePlayer)
{
	T_ASSERT_CRITICAL_S(m_pkCsvData != NULL);
	
	if(iLevel == m_pkCsvData->_iMaxLevel)
		return BOT_PROMPTTYPE_CANNOT;

	mem::vector<BotReward>* kVec = NULL;
	if(bIsAvtive)
		kVec = &m_kUpLevelConsumption;
	else
		kVec = &m_kActiveConsumption;

	mem::vector<BotReward>::const_iterator iter = kVec->begin();
	for( ; iter!=kVec->end(); ++iter)
	{
		const BotReward& kRwd = *iter;
		if(pkGamePlayer->GetCanCostItemCount(kRwd.itemID) < kRwd.itemCount)
			return BOT_PROMPTTYPE_CANNOT;
	}

	return bIsAvtive ? BOT_PROMPTTYPE_CANLEVELUP : BOT_PROMPTTYPE_CANACTIVE;
}

uint32_t LC_ClientBotInstance::GetEfficencyByIndex(int index)
{
	mem::map<int,ResouseReward>::const_iterator levelIter = m_kLevelResouse.find(iLevel);
	if (levelIter != m_kLevelResouse.end())
	{
		ResouseReward kResouseReward = levelIter->second;
		if (index >= 0 && index < kResouseReward.size())
		{
			return kResouseReward[index].itemCount;
		}
	}
	return 0;
}

int LC_ClientBotInstance::GetEfficencyItemIDByIndex(int index)
{
	mem::map<int,ResouseReward>::const_iterator levelIter = m_kLevelResouse.begin();
	if (levelIter != m_kLevelResouse.end())
	{
		ResouseReward kResouseReward = levelIter->second;
		if (index >= 0 && index < kResouseReward.size())
		{
			return kResouseReward[index].itemID;
		}
	}
	return 0;
}


bool LC_ClientBotInstance::IsActiveTalent()
{
	CF_BotUpgrade::DataEntryMapExternIterator kIter = CF_BotUpgrade::GetSingletonPtr()->GetIterator();
	StringStreamType sStream("");
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BotUpgrade::DataEntry *pkUpEntry = kIter.PeekNextValuePtr();
		if ( pkUpEntry->_cID == cID && pkUpEntry->_iLevel == iLevel)
		{
			if (pkUpEntry->_iTalentLevel > 0)
				return true;
			else
				return false;
		}
	}
	return false;
}

StringType LC_ClientBotInstance::ContructTarentInfo()
{
	CF_BotUpgrade::DataEntryMapExternIterator kIter = CF_BotUpgrade::GetSingletonPtr()->GetIterator();
	StringStreamType sStream("");
	int tarentLevel = 0;
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BotUpgrade::DataEntry *pkUpEntry = kIter.PeekNextValuePtr();
		if ( pkUpEntry->_cID == cID && pkUpEntry->_iLevel >= iLevel)
		{
			if (bIsAvtive )
			{
				if (pkUpEntry->_iLevel == iLevel)
				{
					sStream << pkUpEntry->_sTalentDes << ":";
					sStream << pkUpEntry->_iLevel << ":";
					sStream << pkUpEntry->_iTalentLevel << ":";
					sStream << pkUpEntry->_cID << ";";
					tarentLevel = pkUpEntry->_iTalentLevel;
				}
				if (pkUpEntry->_iTalentLevel > tarentLevel)
				{
					sStream << pkUpEntry->_sTalentDes << ":";
					sStream << pkUpEntry->_iLevel << ":";
					sStream << pkUpEntry->_iTalentLevel << ":";
					sStream << pkUpEntry->_cID << ";";
					break;
				}
			}
			else
			{
				if (pkUpEntry->_iLevel >= iLevel && pkUpEntry->_iTalentLevel > 0)
				{
					sStream << ";"; // 与上面一致
					sStream << pkUpEntry->_sTalentDes << ":";
					sStream << pkUpEntry->_iLevel << ":";
					sStream << pkUpEntry->_iTalentLevel << ":";
					sStream << pkUpEntry->_cID << ";";
					break;
				}
			}
		}
	}
	return sStream.str();
}




























