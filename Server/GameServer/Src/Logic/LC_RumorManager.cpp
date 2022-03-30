
#include "LC_RumorManager.h"
#include "GameServerApp.h"

#include "UT_Redis.h"
#include "LS_LuaStateFactory.h"
#include "LS_LuaState.h"
#include "boost/lexical_cast.hpp"
#include "MG_ChatProtocol.h"
#include "UT_Timer.h"
#include "LC_LogicManagerBase.h"
#include "LC_PlayerManagerBase.h"

using namespace Utility;


GameLogic::LC_RumorLogInfo* GameLogic::LC_RumorGrade::GetRumorLogInfo(int32_t iType)
{
	LC_RumorLogInfoMap::iterator it = m_kRumorLogs.find(iType);
	if (it != m_kRumorLogs.end())
	{
		return &it->second;
	}

	static GameLogic::LC_RumorManager* pkRumorManager = GameLogic::LC_RumorManager::GetSingletonPtr();
	LC_Rumor* pkRumor = pkRumorManager->GetRumorByType(iType);
	if (NULL == pkRumor)
	{
		return NULL;
	}

	m_kRumorLogs[iType].m_iGrade = m_iGrade;
	m_kRumorLogs[iType].m_iType = iType;
	m_kRumorLogs[iType].m_uiLimit = pkRumor->GetRumorLimit(m_iGrade);
	return &m_kRumorLogs[iType];
}

void GameLogic::LC_RumorGrade::Update(void)
{
	LC_RumorLogInfoMap::iterator it = m_kRumorLogs.begin();
	for (; it!=m_kRumorLogs.end(); ++it)
	{
		it->second.SyncRumor();
	}
}

int32_t GameLogic::LC_RumorGrade::SetDataToProtocolStruct(Protocol::PS_RumorLogInfos& rkStruct, bool all)
{
	int32_t iDirty = 0;
	LC_RumorLogInfoMap::iterator it = m_kRumorLogs.begin();
	for (; it!=m_kRumorLogs.end(); ++it)
	{
		if (it->second.GetDirtyFlag() || all)
		{
			Protocol::PS_RumorLogInfo psLogInfo;
			it->second.SetDataToProtoclStruct(psLogInfo, all);
			rkStruct.push_back(psLogInfo);
			if (!all)
			{
				it->second.SetDirtyFlag(false);
			}
			++iDirty;
		}
	}
	return iDirty;
}
void GameLogic::LC_RumorGrade::GetDataFromProtocolStruct(Protocol::PS_RumorLogInfos& rkStruct)
{
	Protocol::PS_RumorLogInfos::iterator it = rkStruct.begin();
	for (; it!=rkStruct.end(); ++it)
	{
		Protocol::PS_RumorLogInfo psLogInfo = *it;
		LC_RumorLogInfo* pkLogInfo = GetRumorLogInfo(psLogInfo.m_iType);
		if (pkLogInfo)
		{
			pkLogInfo->GetDataFromProtocolStruct(psLogInfo);
		}
	}
}

void GameLogic::LC_RumorManager::ReadConfigFile()
{
	StringType pkConfigFileName = RUMOR_CONFIG;

	LS_LuaState* pkLuaState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	if(!pkLuaState)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"lua system error![%s]",pkConfigFileName.c_str());
		return;
	}

	bool bLoadScript = pkLuaState->DoFile(pkConfigFileName);
	if(!bLoadScript)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"lua file[%s]error",pkConfigFileName.c_str());
		T_ASSERT_CRITICAL_S(false);
		return;
	}

	StringType kTableName = "RumorPoint";
	static bool bReload = false;

	StringType strFindValue = kTableName + ".RumorType";
	LS_LuaObject* pkLuaObject = pkLuaState->GetLuaObject(strFindValue, bReload);
	if (pkLuaObject)
	{
		for (int i = 0 ;i<pkLuaObject->GetChildCount() ;++i)
		{
			LS_LuaObject* pkLuaChild = pkLuaObject->GetChild(i);
			LS_LuaObject* pkLuaTmp = NULL;

			LC_Rumor kRumor;

			int iRumorType = 0, iTriggerType = 0, iChatChannel = 0;
			//StringType strDescription;

			pkLuaTmp = pkLuaChild->GetChild("RumorType");
			if (pkLuaTmp)
			{
				iRumorType = pkLuaTmp->ToInt();
			}
			pkLuaTmp = pkLuaChild->GetChild("TriggerType");
			if (pkLuaTmp)
			{
				iTriggerType = pkLuaTmp->ToInt();
			}
			pkLuaTmp = pkLuaChild->GetChild("ChatChannel");
			if (pkLuaTmp)
			{
				iChatChannel = pkLuaTmp->ToInt();
			}

			kRumor.SetRumorType(RumorType(iRumorType));
			kRumor.SetTriggerType(RumorTriggerType(iTriggerType));
			kRumor.SetChatChannel(iChatChannel);


			pkLuaTmp = pkLuaChild->GetChild("RecordInfo");
			if (pkLuaTmp)
			{
				int32_t iMaxL = pkLuaTmp->GetChild("MaxLocalCount") ? pkLuaTmp->GetChild("MaxLocalCount")->ToInt() : 0;
				int32_t iMaxG = pkLuaTmp->GetChild("MaxGroupCount") ? pkLuaTmp->GetChild("MaxGroupCount")->ToInt() : 0;
				int32_t iMaxU = pkLuaTmp->GetChild("MaxUnionCount") ? pkLuaTmp->GetChild("MaxUnionCount")->ToInt() : 0;
				kRumor.SetRecord(iMaxU, iMaxG, iMaxL);
			}

			int iConditionNum = 0;
			pkLuaTmp = pkLuaChild->GetChild("ConditionDescriptionNum");
			if (pkLuaTmp)
			{
				iConditionNum = pkLuaTmp->ToInt();
			}

			pkLuaTmp = pkLuaChild->GetChild("ConditionDescription");
			if (pkLuaTmp)
			{
				for (int j = 0; j != iConditionNum; ++j)
				{
					char buff[256] = {0};
					sprintf_s(buff, sizeof(buff)-1, "ConditionDescription_%i", j);

					LS_LuaObject* pkLuaCondition = pkLuaTmp->GetChild(buff);

					if(pkLuaCondition && pkLuaCondition->IsArray())
					{
						LC_RumorDataEntry kEntry;
						if (pkLuaCondition->GetChild(0)->IsArray())
						{
							for (int ii=0; ii<LC_RumorKey::RUMER_KEY_COUNT; ++ii)
							{
								kEntry.kKey[ii] = pkLuaCondition->GetChild(0)->GetChild(ii) ? pkLuaCondition->GetChild(0)->GetChild(ii)->ToInt() : 0;
							}
						}
						else
						{
							kEntry.kKey[0] = pkLuaCondition->GetChild(0)->ToInt();
						}

						kEntry.noticeID = pkLuaCondition->GetChild(1)->ToInt();
						//kEntry.sInfo = pkLuaCondition->GetChild(1)->ToCStr();

						if (pkLuaCondition->GetChildCount()>2 )
						{
							kEntry.iGrade = (pkLuaCondition->GetChild(2)->ToInt());
						}

						kRumor.AddRumerEntry(kEntry);
					}
				}
			}

			m_kRumorMap[kRumor.GetRumorType()] = (kRumor);
			m_kRumorMap[kRumor.GetRumorType()].SetUP();
		}
	}
}

GameLogic::LC_RumorManager::LC_RumorManager()
{
	m_kRumorMap.clear();
	ReadConfigFile();
	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_DULL));

	for (int i=0; i<INF_SPH_TP_MAX; ++i)
	{
		m_kRumorGrades[i].m_iGrade = i;
	}

	LC_RumorMap::iterator it = m_kRumorMap.begin();
	for (; it!=m_kRumorMap.end(); ++it)
	{
		for (int i=0; i<INF_SPH_TP_MAX; ++i)
		{
			m_kRumorGrades[i].GetRumorLogInfo(it->first);
		}
	}
}

GameLogic::LC_RumorManager::~LC_RumorManager()
{

}

GameLogic::LC_Rumor* GameLogic::LC_RumorManager::GetRumorByType( int32_t type )
{
	LC_RumorMap::iterator iter = m_kRumorMap.find(type);
	if (iter != m_kRumorMap.end())
	{
		return &iter->second;
	}
	return 0;
}
GameLogic::LC_RumorGrade* LC_RumorManager::GetRumorGrade(int32_t iGrade)
{	
	switch(iGrade)
	{
	case INF_SPH_TP_SERVER:
	case INF_SPH_TP_GROUP:
	case INF_SPH_TP_UNION:
		{
			return &m_kRumorGrades[iGrade];
		}
		break;
	default:
		break;
	}
	return NULL;
}
GameLogic::LC_RumorLogInfo* GameLogic::LC_RumorManager::GetRumorLogInfo(int32_t iType, int32_t iRumorGrade)
{
	LC_RumorGrade* pkRumorGrade = GetRumorGrade(iRumorGrade);
	if (NULL == pkRumorGrade)
	{
		return NULL;
	}
	return pkRumorGrade->GetRumorLogInfo(iType);
}

void GameLogic::LC_RumorManager::Update(float ftime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}

	for (int i=0; i<INF_SPH_TP_MAX; ++i)
	{
		LC_RumorGrade& rkGrade = m_kRumorGrades[i];
		
		rkGrade.Update();
	}

	//do logic up
	{
		Protocol::MG_SyncRumorLogsInfo syncMsg;
		int32_t iDirty = SetDataToProtocolStruct(syncMsg);
		if (iDirty)
		{
			SendMsgToChat(MGPT_SYNC_RUMOR_INFO, &syncMsg, 0, 0, 0, 0, MG_SERVER_ROUTER_FLAG_SINGLE);
		}
	}
}

void GameLogic::LC_RumorManager::OnGameServerLoaded(void)
{}

void GameLogic::LC_RumorManager::AddLog(int32_t iType, const LC_RumorLog& kLog, LC_RumorDataEntry* pkRumorDataEntry)
{
	if (NULL == pkRumorDataEntry)
	{
		return;
	}
	GameLogic::LC_Rumor* pkRumer = GetRumorByType(iType);
	if (NULL == pkRumer)
	{
		return;
	}

	GameLogic::LC_RumorGrade* pkRumorGrade = GetRumorGrade(pkRumorDataEntry->iGrade);
	if (NULL == pkRumorGrade)
	{
		return;
	}

	LC_RumorLogInfo* pkLogInfo = pkRumorGrade->GetRumorLogInfo(iType);
	if (NULL == pkLogInfo)
	{
		return;
	}

	pkLogInfo->AddLog(kLog);
}

void GameLogic::LC_RumorManager::SyncRumorLogs(LC_ServerPlayer* pkPlayer)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	Protocol::MG_SyncRumorLogsInfo syncMsg;
	SetDataToProtocolStruct(syncMsg, true);
	pkPlayer->SendMsgToClient(MGPT_SYNC_RUMOR_INFO, &syncMsg);
}

int32_t GameLogic::LC_RumorManager::SetDataToProtocolStruct(Protocol::MG_SyncRumorLogsInfo& rkStruct, bool all)
{
	int32_t iDirty = 0;
	for (int i=0; i<INF_SPH_TP_MAX; ++i)
	{
		iDirty += m_kRumorGrades[i].SetDataToProtocolStruct(rkStruct.m_kLogs[i], all);
	}
	return iDirty;
}
void GameLogic::LC_RumorManager::GetDataFromProtocolStruct(Protocol::MG_SyncRumorLogsInfo& rkStruct)
{
	for (int i=0; i<INF_SPH_TP_MAX; ++i)
	{
		m_kRumorGrades[i].GetDataFromProtocolStruct(rkStruct.m_kLogs[i]);
	}
}
