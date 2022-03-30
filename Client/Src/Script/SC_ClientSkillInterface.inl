#include "CF_GuildRaid.h"
#include "LC_GuildRaidDataManager.h"
#include "CF_WildPrinces.h"
#include "CF_GuildClash.h"
#include "CF_UIOpenLevel.h"
#include "CF_ShopList.h"
#include "CF_CommonStatReplevy.h"
#include "LC_AchievementManager.h"
#include "CF_SystemTitleList.h"
#include "LC_TitleManager.h"
#include "CF_FunctionPreview.h"
#include "CF_SevenDaysLoginReward.h"
#include "CF_GameStoryRaidList.h"
#include "CF_TreasurePanel.h"
#include "CF_FlowerConfig.h"
#include "CF_FlowerSendLimitCfg.h"
#include "CF_FlowerGiftCfg.h"
#include "CF_SecretSkillClient.h"
#include "CF_OffLineExp.h"
#include "CF_MysticalShop.h"
#include "CF_MysticalShopVip.h"
#include "CF_MysticalShopReset.h"
#include "CF_GodSuiteTask.h"
#include "CF_QQGirls.h"
#include "CF_SecretTreasureChests.h"
#include "CF_SecretTreasureSchedule.h"
#include "CF_SecretTreasureShop.h"
#include "CF_SecretTreasureReward.h"
#include "CF_FashionSuit.h"
#include "CF_Fashion.h"
#include "CF_ArenaSection.h"
#include "CF_ArenaTimes.h"
#include "CF_StarsRune.h"
#include "CF_GemReinforceLevel.h"
#include "CF_GemExp.h"
#include "CF_ACTMonopoly.h"
#include "CF_ACTMonopolyA.h"
#include "CF_ACTLottery.h"
#include "CF_DailyActivity.h"
#include "CF_SignInDailyReward.h"
#include "CF_FirstDailyCharge.h"
#include "CF_BuddyGrowGift.h"
#include "CF_OffLineExpRatio.h"
#include "CF_CollectionActivation.h"
#include "CF_CollectionReward.h"
#include "CF_BossFirstKill.h"
#include "CF_EndlessTower.h"
#include "CF_Biography.h"
#include "CF_BiographyTask.h"
#include "CF_HolyDeed.h"
#include "CF_SevenDaysTask.h"
#include "CF_SevenDaysTotalReward.h"
#include "CF_CompositionFormula.h"
#include "CF_InvestmentCardTask.h"
#include "CF_InvestmentCard.h"
#include "CF_BossInvade.h"
#include "CF_BossInvadeRankReward.h"
#include "CF_VipNew.h"
#include "CF_RecommendHero.h"
#include "CF_ItemList.h"
#include "CF_ACTShopA.h"
#include "CF_TestAccPointRewards.h"
#include "CF_TestRankReward.h"
#include "CF_CommanderTitle.h"
#include "CF_HeadFrame.h"
#include "CF_SpecialTraining.h"
#include "CF_GloryExpedition.h"
#include "CF_BuddyRuneRecommend.h"
#include "CF_YiJiTuXiStageReward.h"
#include "CF_YiJiXunBaoStageReward.h"
#include "CF_RotaryTableReward.h"
#include "CF_CommanderCombatPK.h"
#include "CF_NonpareilItem.h"
#include "CF_BestGuildGuildRankReward.h"
#include "CF_BestGuildPersonalRankReward.h"
#include "CF_BestGuild.h"
#include "CF_RegularBoss.h"
#include "CF_PetIslandStageReward.h"
#include "CF_EscortReward.h"
#include "CF_BestGuildGoal.h"
#include "CF_BestGuildChapterID.h"
#include "CF_NewAchievementReward.h"
#include "CF_YiJiTuXiRankRewards.h"
#include "CF_EntrustTask.h"
#include "CF_GuildTower.h"
#include "CF_TestRankGuildReward.h" 

//////野外首领
StringType SC_ClientSkillInterface::GetWildPrincesData(int iDataStringType, int iMapResID, int iStage)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	StringStreamType sStream("");
	StringType strError = "";

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	LC_ClientGameStoryManager* pkGameStoryMgr = pkGamePlayer->GetGameStoryManager();
	if (!pkGameStoryMgr)
		return strError;

	switch( iDataStringType)
	{
	case DataStringType_WildPrinces_BossList:
		{
			CF_WildPrinces::DataEntryMapExternIterator kIter = CF_WildPrinces::GetSingleton().GetIterator();
			for( ; kIter.HasMoreElements(); kIter.MoveNext() )
			{
				const CF_WildPrinces::DataEntry* pkEntry = kIter.PeekNextValuePtr();
				sStream << pkEntry->_lMapResID << "#" << pkEntry->_lCharType << "#" << pkEntry->_lType << "#" <<pkEntry->_ID << "#";

				CSVFile::CF_CharType::DataEntry* pkCharTypeData = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_CharType>(pkEntry->_lCharType);
				if (NULL == pkCharTypeData)
					sStream << "##";
				else
					sStream << pkCharTypeData->_kCharName << "#" << pkCharTypeData->_lLevel << "#";

				sStream << pkEntry->_kLocation << "#";
				sStream << pkEntry->_kRewardDesc << "#";
				sStream << pkEntry->_kRefreshTime << "#";
				sStream << pkEntry->_kCameraPara << "#";
				sStream << pkEntry->_kRefreshCron << "#";
				sStream << pkEntry->_kTips << "#";

				CSVFile::CF_WorldMapList::DataEntry* pkWorldMapData = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_WorldMapList>(pkEntry->_lMapResID);
				if (NULL == pkWorldMapData)
					sStream << "0#";
				else
					sStream << pkWorldMapData->_iNeedLevel << "#";

				sStream << pkEntry->_kBossIcon << "#";

				if (NULL == pkCharTypeData)
					sStream << "0#";
				else
					sStream << pkCharTypeData->_lCombatScore << "#";

				sStream << pkEntry->_GameStoryID << "#";
				sStream << pkEntry->_kTitle1 << "#";
				sStream << pkEntry->_kReward1 << "#";
				sStream << pkEntry->_kTitle2 << "#";
				sStream << pkEntry->_kReward2 << "#";
				sStream << pkEntry->_kTitle3 << "#";
				sStream << pkEntry->_kReward3 << "#";
				sStream << pkEntry->_kTitle4 << "#";
				sStream << pkEntry->_kReward4 << "#";

				sStream << pkEntry->_iCommonType << "#";
				sStream << pkEntry->_iStoryGroupID << "#";
				sStream << "|";
			}
		}
		break;
	case DataStringType_WildPrinces_Boss_Info:
		{
			CF_WildPrinces::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WildPrinces>(iMapResID);
			if (NULL == pkData)
				return strError;

			CSVFile::CF_CharType::DataEntry* pkCharTypeData = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_CharType>(pkData->_lCharType);
			if (NULL == pkCharTypeData)
				sStream << "##";
			else
				sStream << pkCharTypeData->_kCharName << "#" << pkCharTypeData->_lLevel << "#";

			sStream << pkData->_lType << "#";
			sStream << pkData->_kLocation << "#";
			sStream << pkData->_kRewardDesc << "#";
			sStream << pkData->_kRefreshTime << "#";
			sStream << pkData->_kCameraPara << "#";
			sStream << pkData->_kRefreshCron << "#";
		}
		break;
	case DataStringType_WildPrinces_MapStateList:
		{
			GameStoryInfo* pkGameStoryInfo = pkGameStoryMgr->GetGameStoryInfo(iMapResID);
			if (!pkGameStoryInfo)
				return strError;

			sStream << pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos.size() << "#";
			for (int i = 0; i < pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos.size(); i++)
			{
				if (pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos[i].m_kKeyInfos.empty())
				{
					sStream << "0#-1###";
				}
				else 
				{
					sStream << pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos[i].m_kKeyInfos[0].m_iResID << "#";
					if (pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos[i].m_kKeyInfos[0].m_uiCharID == INVALID_ACTOR_ID)
					{
						sStream << "0###";
					}
					else
					{
						sStream << "1" << "#";
						sStream << pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos[i].m_kKeyInfos[0].m_szCharName << "#";
						//sStream << "(" << pkInterface->GetTimeString(pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos[i].m_kKeyInfos[0].m_uiTimestamp, 2) << ")" << "#";
						sStream << pkGameStoryInfo->m_kBatchInfo.m_kAreaInfos[i].m_kKeyInfos[0].m_uiTimestamp << "#";
					}
				}
			}
		}
		break;
	case DataStringType_WildPrinces_TimeStamp:
		{
			GameStoryInfo* pkGameStoryInfo = pkGameStoryMgr->GetGameStoryInfo(iMapResID);
			if (!pkGameStoryInfo)
				return "0";

			int32_t iTime = pkGameStoryInfo->GetNextRefreshTime();

			sStream << iTime;
		}
		break;
	}

	return sStream.str();
}

//试炼点数商店
StringType _getDataString_TrialShopList( int iDataStringType, int iID, int iIndex )
{
	StringType strError("");
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	LC_ClientShopCity* pkShopCity = LC_ClientShopCity::GetSingletonPtr();
	if (!pkShopCity)
		return strError;
	ShopCityEntryVector& entryList = pkShopCity->GetShopCityEntryVector(LSCT_STORY);
	if (entryList.empty())
		return strError;

	StringStreamType sStream("");

	mem::multimap<int, StringType> mSortedData;
	int totalCount = entryList.size();

	for (ShopCityEntryVectorItr iter = entryList.begin(); iter != entryList.end(); ++iter)
	{
		CF_ShopList::DataEntry* pkData = (*iter).pkData;
		StringStreamType sTemp("");
		Utility::LC_ComMap::iterator it;
		Utility::LC_ComMap& kScores = pkData->_gsScorePrice;
		it = kScores.begin();
		int price = 0;
		if (it->first!= iID)
		{
			continue;
		}

		sTemp << pkData->_iItemID << "_";

		for (; it!=kScores.end(); ++it)
		{
			if (it->first==iID)
			{
				price += it->second;
			}
		}
		sTemp << price << "_";
		

		Utility::LC_ComMap& kCounts = pkData->_sCountLimits;
		it= kCounts.begin();
		int counts = 0;
		for (; it!=kCounts.end(); ++it)
		{
			if (it->first==0)
			{
				counts += it->second;
			}
		}
		sTemp << counts << "_";

		sTemp << pkGamePlayer->GetCommonShopAsset().GetAvailableCount(pkData->_iID) << "_";
		sTemp << pkData->_iID << "_";
		sTemp << pkData->_iLevelRequired << "_";
		sTemp << pkData->_iLevelOpen << "_";
		mSortedData.insert(std::make_pair(pkData->_iRank*10, sTemp.str()));
	}

	sStream << totalCount << "#";

	mem::multimap<int, StringType>::iterator it = mSortedData.begin();
	for (; it != mSortedData.end(); ++it)
	{
		sStream << it->second << "#";
	}

	return sStream.str();
}

//折扣商店
StringType _getDataString_DiscountShopList( int iDataStringType, int iID, int iIndex )
{
	StringType strError("");
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	LC_ClientShopCity* pkShopCity = LC_ClientShopCity::GetSingletonPtr();
	if (!pkShopCity)
		return strError;
	ShopCityEntryVector& entryList = pkShopCity->GetShopCityEntryVector(iID);
	if (entryList.empty())
		return strError;

	mem::vector<StringType> mSortedData;
	int totalCount = 0;
	int commonCount = 0;

	int platform = pkGamePlayer->GetPlayerPlatformType();
	uint32_t iCurTime = pkGamePlayer->GetServerTimeVal();
	uint32_t iServerOpenTime = pkGamePlayer->GetServerOpenTime();
	uint32_t iServerConflateTime = pkGamePlayer->GetServerConflateTime();
	StringStreamType sStream("");
	for (ShopCityEntryVectorItr iter = entryList.begin(); iter != entryList.end(); ++iter)
	{
		CF_ShopList::DataEntry* pkData = (*iter).pkData;

		if (!pkData->_PlatformType.empty() && pkData->_PlatformType.find(platform) == pkData->_PlatformType.end())
		{
			continue;
		}
		if (!pkData->_ExceptPlatformType.empty() && pkData->_ExceptPlatformType.find(platform) != pkData->_ExceptPlatformType.end())
		{
			continue;
		}
		bool bValid = LC_Helper::IsCommTimeViable(iCurTime,iServerOpenTime,iServerConflateTime,pkData->_beginTime.GetTimeStramp(), pkData->_endTime.GetTimeStramp(), pkData->_coldSTime, pkData->_beginSTime, pkData->_endSTime, pkData->_coldMTime, pkData->_beginMTime, pkData->_endMTime);
		if (!bValid)
		{
			continue;
		}
		uint32_t iBeginTime = pkData->_beginTime.GetTimeStramp();
		uint32_t iEndTime = pkData->_endTime.GetTimeStramp();
		uint32_t iSBeginTime = pkData->_beginSTime.GetTimeStramp(iServerOpenTime);
		uint32_t iSEndTime = pkData->_endSTime.GetTimeStramp(iServerOpenTime);
		uint32_t iSColdTime = pkData->_coldSTime.GetTimeStramp(iServerOpenTime);
		bool bsTimeFlag = iCurTime >= iSBeginTime && iCurTime <= iSEndTime && iCurTime >= iSColdTime;
		if (bsTimeFlag)//在开服时间戳成立的情况下使用开服时间戳
		{
			iBeginTime = iSBeginTime;
			iEndTime = iSEndTime;
		}
		iSBeginTime = pkData->_beginMTime.GetTimeStramp(iServerConflateTime);
		iSEndTime = pkData->_endMTime.GetTimeStramp(iServerConflateTime);
		iSColdTime = pkData->_coldMTime.GetTimeStramp(iServerConflateTime);
		bsTimeFlag = iCurTime >= iSBeginTime && iCurTime <= iSEndTime && iCurTime >= iSColdTime;
		if (bsTimeFlag)//在合服时间戳成立的情况下使用合服时间戳
		{
			iBeginTime = iSBeginTime;
			iEndTime = iSEndTime;
		}
		StringStreamType sTemp("");
		sTemp << pkData->_iID << "#";
		sTemp << pkData->_iItemID << "#";
		sTemp << pkData->_iCount << "#";
		sTemp << pkData->_iRank << "#";
		sTemp << LC_Helper::GetComMapValue(pkData->_OriginalPriceList, CT_UNBIND_YUANBAO) << "#";
		sTemp << LC_Helper::GetComMapValue(pkData->_ActualPriceList, CT_UNBIND_YUANBAO)  << "#";
		sTemp << iBeginTime << "#";
		sTemp << iEndTime << "#";

		Utility::LC_ComMap& kCounts = pkData->_sCountLimits;
		Utility::LC_ComMap::iterator it;

		int iType = 0;
		int iValue = 0;
		int iAvailableCount = 0;
		it= kCounts.begin();
		if (it!=kCounts.end())
		{
			if(it->first >= E_CountLimitType_G_Forever)
				//全服限购
				iAvailableCount = LC_ClientShopManager::GetSingletonPtr()->GetCommonShopAsset()->GetAvailableCount(pkData->_iID);
			else
				iAvailableCount = pkGamePlayer->GetCommonShopAsset().GetAvailableCount(pkData->_iID);

			iType = it->first;
			iValue = it->second;
		}

		sTemp << iAvailableCount << "#" << iType << "#" << iValue << "#";

		mSortedData.push_back(sTemp.str());

		if(pkData->_iRank != 1)
			commonCount++;

		totalCount++;
	}

	sStream << totalCount << "|";
	sStream << commonCount << "|";

	mem::vector<StringType>::iterator it = mSortedData.begin();
	for (; it != mSortedData.end(); ++it)
	{
		sStream << (*it) << "|";
	}

	return sStream.str();
}

//任务奖励物品
StringType _getDataString_TaskRewardList(int iDataStringType, int iID, int iIndex)
{
	StringType strError = "";
	StringStreamType sStream("");

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (!pkTaskManager)
		return strError;
	LC_ClientTask *pkTask = (LC_ClientTask*)pkTaskManager->GetTask(iID);
	if (!pkTask)
		return strError;
	
	Utility::UT_SIMDataList kItems;
	pkTask->GetSIMDataInfoList(pkGamePlayer, kItems);
	size_t iSize = kItems.size();
	sStream << iSize << "#";
	for (size_t i = 0; i < iSize; i++)
	{
		Utility::UT_SIMDataInfo& kInfo = kItems[i];
		sStream << kInfo.ID() << "_" << kInfo.ParamA() << "#";
	}

	return sStream.str();
}

////雷达地图左侧按钮开启信息
StringType _getDataString_UIOpenLevel( int iDataStringType, int iID, int iIndex )
{

	CF_UIOpenLevel::DataEntryMapExternIterator kIter = CF_UIOpenLevel::GetSingleton().GetIterator();
	StringStreamType sStream("");
	while(kIter.HasMoreElements())
	{
		CF_UIOpenLevel::DataEntry* pkData = kIter.PeekNextValuePtr();
		sStream << pkData->_iID << "|";
		sStream << pkData->_kKeyID << "|";
		sStream << pkData->_iPosition << "|";
		sStream << pkData->_kPath << "|";
		sStream << pkData->_kDesc << "|";
		sStream << pkData->_iOpenLevel << "|";
		sStream << pkData->_iPreTaskID << "|";
		sStream << pkData->_bShowInOtherServer << "|";
		sStream << pkData->_iDInTutorial << "|";
		sStream << pkData->_iPlatForm << "|";
		sStream << pkData->_iTxSubPlatForm << "|";
		sStream << pkData->_iUnlockLevel<< "|";
		sStream << pkData->_iUnlockTaskID<< "|";
		sStream << pkData->_iExceptPlatForm << "|";
		sStream << pkData->_SpecialTrainingID << "#";
		kIter.MoveNext();
	}

	return sStream.str();
}

////成就系统
StringType SC_ClientSkillInterface::GetAchievementData(int iDataStringType, int iClassID, int iSubClassID)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	StringStreamType sStream("");
	StringType strError = "";

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;

	switch(iDataStringType)
	{
	case DataStringType_Achievement_TitleList:
		{
			int iIndex = iClassID;
			int iNum = iSubClassID;
			return LC_TitleManager::GetSingleton().GetTitleListString(iIndex, iNum);
		}
		break;
	case DataStringType_Achievement_TitleStateList:
		{
			LC_TitleAsset* pkTitleAsset = pkGamePlayer->GetTitleAsset();
			if(NULL == pkTitleAsset)
				return EMPTY_STRING;

			int iIndex = iClassID;
			int iNum = iSubClassID;

			int iSize = LC_TitleManager::GetSingleton().GetTitleCount();
			if(iSize <= iIndex)
				return EMPTY_STRING;

			int iCount = iNum;
			if(iSize - iIndex < iNum)
				iCount = iSize - iIndex;
			sStream << iCount << "#";
			int iEnd = iIndex + iCount;
			for (int i = iIndex; i < iEnd; i++)
			{
				int iID = LC_TitleManager::GetSingleton().GetTitleIDByIndex(i);
				if (pkTitleAsset->IsHoldSystemTitle(iID))
					sStream << "1";
				else
					sStream << "0";
				sStream << "#";
			}
		}
		break;
	case DataStringType_Achievement_TitleCount:
		{
			sStream << LC_TitleManager::GetSingleton().GetTitleCount();
		}
		break;
	case DataStringType_Achievement_CurrentTitle:
		{
			sStream << pkGamePlayer->GetTitleAsset()->GetCurrentTitle();
		}
		break;
	case DataStringType_Achievement_TitleTips:
		{
			int iTitleID = iClassID;
			CF_SystemTitleList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SystemTitleList>(iTitleID);

			if(pkData)
			{
				int curTime = pkGamePlayer->GetServerTimeVal();
				int titleTime = 0;
				LC_TitleAsset* pAsset = pkGamePlayer->GetTitleAsset();
				if (pAsset)
				{
					titleTime = pAsset->GetPeriodTitleCountTime(iTitleID);
				}
				int countTime = titleTime-curTime;

				if (!pAsset->IsHoldSystemTitle(iTitleID))
				{
					countTime = 0;
				}
				sStream << pkData->_kTitleName << "#"<<Utility::Int2String(countTime)<<"#";
				sStream << pkData->_kCondition << "#";

				StringType kAtt = "";
				int iCount = 0;
				if(pkData->_lMaxHP > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_MAX_HP) + ":" + Utility::Int2String(pkData->_lMaxHP) + ";";
				}

				if(pkData->_HPReg > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_HP_RECOVER) + ":" + Utility::Int2String(pkData->_HPReg) + ";";
				}

				if(pkData->_lMaxPHA > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_MAX_ATK) + ":" + Utility::Int2String(pkData->_lMaxPHA) + ";";
				}

				if(pkData->_lATKRateAdd > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_ATK_RATE) + ":" + Utility::Int2String(pkData->_lATKRateAdd) + ";";
				}
					
				if(pkData->_lCirRATE > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_CIR_RATE) + ":" + Utility::Int2String(pkData->_lCirRATE) + ";";
				}

				if(pkData->_lCirDAM > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_CIR_DAM) + ":" + Utility::Int2String(pkData->_lCirDAM) + ";";
				}

				if(pkData->_lPHD > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_DEF) + ":" + Utility::Int2String(pkData->_lPHD) + ";";
				}

				if(pkData->_lDogRATE > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_DOG_RATE) + ":" + Utility::Int2String(pkData->_lDogRATE) + ";";
				}

				if(pkData->_lCirDefAdd > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_CIR_DEF) + ":" + Utility::Int2String(pkData->_lCirDefAdd) + ";";
				}

				if(pkData->_lMoveSPEED > 0)
				{
					iCount++;
					kAtt += Utility::Int2String(ATT_MOVE_SPEED) + ":" + Utility::Int2String(pkData->_lMoveSPEED) + ";";
				}

				kAtt = Utility::Int2String(iCount) + ";" + kAtt;

				sStream << kAtt << "#";
				sStream	<< pkData->_lFightScore << "#";
				sStream << pkData->_iPeriod << "#";
				sStream << pkData->_kRes << "#";
				sStream << pkData->_sEffect << "#";
			}
		}
		break;
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCommonStatRewardList(int iDataStringType, int iType, int iKey)
{
	StringType strError = "";
	LC_CommonStatEntry* pkCmnStatEntry = NULL;

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	LC_CommonStatManager* pkCmnMgr = LC_CommonStatManager::GetSingletonPtr();
	if (!pkCmnMgr)
		return strError;
	LC_CommonStatRewardDataEntry* pkRewardDataEntry = pkCmnMgr->GetCmnRewardDataEntry(iType, iKey);
	if (!pkRewardDataEntry)
		return strError;

	const Utility::UT_SIMDataList *pkItems = pkRewardDataEntry->m_kRewardItems.GetAllItemList();
	int iCount = 0;
	StringType sContent = "";
	WeaponType eWeaponType = pkGamePlayer->GetWeaponAttribute();
	if (iDataStringType == DataStringType_CommonStatRewardList_Confirm)
	{
		Utility::UT_SIMDataList::const_iterator it = pkItems->begin();
		for (; it!=pkItems->end(); ++it)
		{
			int32_t WItemSection = it->ItemSection();
			if (!it->ItemProbability() && (WItemSection == eWeaponType || WItemSection == WT_NONE || WItemSection == WT_HAND ))
			{
				iCount++;
				sContent += Utility::Int2String(it->ID()) + "_" + Utility::Int2String(it->ParamA()) + "_" + Utility::Int2String(it->ItemExistenceFunc()) + "_" + Utility::Int2String(it->ItemReinfore()) + "_" + Utility::Int2String(it->ItemBindMode() > 0) + "#";
			}
		}
	}
	else if (iDataStringType == DataStringType_CommonStatRewardList_Random)
	{
		Utility::UT_SIMDataList::const_iterator it = pkItems->begin();
		for (; it!=pkItems->end(); ++it)
		{
			int32_t WItemSection = it->ItemSection();
			if (it->ItemProbability() && (WItemSection == eWeaponType || WItemSection == WT_NONE || WItemSection == WT_HAND ))
			{
				iCount++;
				sContent += Utility::Int2String(it->ID()) + "_" + Utility::Int2String(it->ParamA()) + "_" + Utility::Int2String(it->ItemExistenceFunc()) + "_" + Utility::Int2String(it->ItemReinfore()) + "#";
			}
		}
	}
	else if (iDataStringType == DataStringType_CommonStatRewardList_MainTask)
	{
		Utility::UT_SIMDataList::const_iterator it = pkItems->begin();
		for (; it!=pkItems->end(); ++it)
		{
			iCount++;
			sContent += Utility::Int2String(it->ID()) + "_" + Utility::Int2String(it->ParamA()) + "_" + Utility::Int2String(it->ItemExistenceFunc()) + "_" + Utility::Int2String(it->ItemReinfore()) + "#";
		}
	}
	if (iCount > 0)
		sContent = Utility::Int2String(iCount) + "#" + sContent;
	return sContent;
}

StringType _getDataString_Subordinage_Profit(int iDataStringType, int iType, int iKey)
{
	StringType strError = "";

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;

	const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(iType, iKey);
	if (NULL == entry)
	{
		return strError;
	}
	LC_ItemsReward items = entry->_kProfitItems;

	const Utility::UT_SIMDataList * pkitems = items.GetAllItemList();

	int iCount = 0;
	StringType sContent = "";
	WeaponType eWeaponType = pkGamePlayer->GetWeaponAttribute();
	if (iDataStringType == DataStringType_Subordinage_Profit)
	{
		Utility::UT_SIMDataList::const_iterator it = pkitems->begin();
		for (; it!=pkitems->end(); ++it)
		{
			int32_t WItemSection = it->ItemSection();
			if (!it->ItemProbability() && (WItemSection == eWeaponType || WItemSection == WT_NONE || WItemSection == WT_HAND ))
			{
				iCount++;
				sContent += Utility::Int2String(it->ID()) + "_" + Utility::Int2String(it->ParamA()) + "_" + Utility::Int2String(it->ItemExistenceFunc()) + "_" + Utility::Int2String(it->ItemReinfore()) + "#";
			}
		}
	}
	if (iCount > 0)
		sContent = Utility::Int2String(iCount) + "#" + sContent;
	return sContent;
}
////成就系统
StringType _getDataString_FunctionPreview(int iDataStringType, int iID, int iIndex)
{
	StringStreamType sStream("");
	CF_SevenDaysLoginReward::DataEntryMapExternIterator kIter = CF_SevenDaysLoginReward::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_SevenDaysLoginReward::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iD << "|";
		sStream << pkEntry->_iItemID << "|";
		sStream << pkEntry->_lCharID << "|";
		sStream << pkEntry->_CID << "|";
		sStream << pkEntry->_CameraDist << "|";
		Utility::UT_SIMDataList::const_iterator iter = pkEntry->_Position.begin();
		if(iter != pkEntry->_Position.end())
			sStream << iter->IID() << "|" << iter->ParamA() << "|" << iter->ParamB() << "|";
		else
			sStream << "0|0|0|";
		sStream << pkEntry->_sModelIcon << "|";
		sStream << pkEntry->_Angle << "|";
		sStream << pkEntry->_bShow << "#";
	}
	/*CF_FunctionPreview::DataEntryMapExternIterator kIter = CF_FunctionPreview::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_FunctionPreview::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		sStream << Int2String(pkEntry->_iLevel) << "|";
		sStream << Int2String(pkEntry->_iTaskID) << "|";
		sStream << Int2String(pkEntry->_iItemID) << "|";
		sStream << pkEntry->_sDescription << "|";
		sStream << pkEntry->_name << "|";
		sStream << Int2String(pkEntry->_CID) << "|";
		sStream << pkEntry->_CameraDist << "|";
		Utility::UT_SIMDataList::const_iterator iter = pkEntry->_Position.begin();
		if(iter != pkEntry->_Position.end())
			sStream << iter->IID() << "|" << iter->ParamA() << "|" << iter->ParamB() << "|";
		else
			sStream << "0|0|0|";
		sStream << pkEntry->_iD << "|" << pkEntry->_sModelIcon << "#";
	}*/
	return sStream.str();
}

StringType _getDataString_ShopList_EntryInfo(int iDataStringType, int iID, int iIndex)
{
	StringType strError("");
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	CF_ShopList::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_ShopList>(iID);
	if (!pkEntry)
		return strError;

	int iCostType=0;
	int64_t iActualPrice=0;
	LC_Helper::GetComMapOnlyPrice(pkEntry->_ActualPriceList,iCostType,iActualPrice);

	int iOriginalCostType=0;
	int64_t iOriginalPrice=0;
	LC_Helper::GetComMapOnlyPrice(pkEntry->_OriginalPriceList, iOriginalCostType,iOriginalPrice);

	StringStreamType sStream("");
	//需要的属性依次往后加
	sStream << pkEntry->_iShopType << "#";
	sStream << pkEntry->_iItemID << "#";
	sStream << pkEntry->_iBind << "#";
	sStream << pkEntry->_iVIPLevelRequired << "#";
	sStream << iCostType << "#";
	sStream << iActualPrice << "#";
	sStream << iOriginalCostType << "#";
	sStream << iOriginalPrice << "#";
	sStream << pkEntry->_iCount << "#";
	sStream << pkGamePlayer->GetCommonShopAsset().GetAvailableCount(iID)<< "#";

	int iValid = 0;
	uint64_t iCurTime = (uint64_t)pkGamePlayer->GetServerTimeVal();
	uint64_t iServerOpenTime = (uint64_t)LC_Helper::GetDayTimeStamp(pkGamePlayer->GetServerOpenTime(), 0, 0, 0);
	uint64_t iServerConflateTime = (uint64_t)LC_Helper::GetDayTimeStamp(pkGamePlayer->GetServerConflateTime(), 0, 0, 0);
	iValid = LC_Helper::IsCommTimeViable(iCurTime,iServerOpenTime,iServerConflateTime,pkEntry->_beginTime.GetTimeStramp(), pkEntry->_endTime.GetTimeStramp(), pkEntry->_coldSTime, pkEntry->_beginSTime, pkEntry->_endSTime, pkEntry->_coldMTime, pkEntry->_beginMTime, pkEntry->_endMTime) ? 1 : 0;
	sStream << iValid << "#" << pkEntry->_endSTime.GetTimeStramp(iServerOpenTime) - iCurTime<<"#";

	Utility::LC_ComMap& kCounts = pkEntry->_sCountLimits;
	Utility::LC_ComMap::iterator it;
	it= kCounts.begin();
	if (it!=kCounts.end())
	{
		sStream << it->first <<"#"<<it->second<<"#";
	}
	else
	{
		sStream << "0#0#";
	}

	return sStream.str();
}

StringType _getDataString_GameStoryRaidList_Info(int iDataStringType, int iID, int iIndex)
{
	StringStreamType sStream("");
	StringType strError = "";

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	LC_ClientGameStoryManager* pkGameStoryMgr = pkGamePlayer->GetGameStoryManager();
	if (!pkGameStoryMgr)
		return strError;

	switch( iDataStringType)
	{
	case DataStringType_GameStoryRaidList_BossInfo:
		{
			sStream << CF_GameStoryRaidList::GetSingleton().GetEntryCount() << "|";
			CF_GameStoryRaidList::DataEntryMapExternIterator kIter = CF_GameStoryRaidList::GetSingleton().GetIterator();
			for( ; kIter.HasMoreElements(); kIter.MoveNext() )
			{
				
				const CF_GameStoryRaidList::DataEntry* pkEntry = kIter.PeekNextValuePtr();
				if(pkEntry->_lBelongStoryID == 5)
				{
					sStream << pkEntry->_iID << "#" << pkEntry->_kName << "#";
					const CF_CharType::DataEntry *pkCharTypeEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(pkEntry->_lCharType);
					if(NULL != pkCharTypeEntry)
						sStream << pkCharTypeEntry->_lLevel << "#";
					else
						sStream << "#";
					sStream << pkEntry->_lCharType << "#" << pkEntry->_NeedLevel << "#";

					sStream << pkEntry->_kCameraPara << "#" << pkEntry->_Reward << "#" << pkEntry->_CombatScore << "#";
					sStream	<< pkEntry->_PreMapResID << "#" << pkEntry->_PreMapResID2 << "#" << pkEntry->_MapResID << "#" << pkEntry->_kTips << "#" << pkEntry->_kBossIcon << "|";
			
				}
			}
		}
		break;
	}

	return sStream.str();
}


StringType _getDataString_WorldMapList_LimitInfo(int iDataStringType, int iID, int iIndex)
{
	StringStreamType sStream("");
	StringType strError = "";

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;

	switch( iDataStringType)
	{
	case DataStringType_WorldMapList_LimitInfo:
		{
			CF_WorldMapList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iID);
			if(NULL != pkEntry)
			{
				sStream << pkEntry->_iNeedLevel << "#" << pkEntry->_iVIPLevel << "#";
			}
			else
			{
				sStream << "##";
			}

			CF_TranscriptionRule::DataEntry *pkTrans = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(iID);
			int bEnough = 1;//0表示缺乏道具，1表示道具足够
			int iOutNeedCount = 0;
			int iOutNeedItemID = 0;
			if(NULL != pkTrans)
			{
				Utility::UT_SIMDataList	&ConsumeItems = pkTrans->_kConsumeItems;
				for(int i=0; i<ConsumeItems.size(); ++i)
				{
					int iItemID = ConsumeItems[i].ID();
					CF_ItemList::DataEntry *pkItem = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
					if(NULL != pkItem)
					{
						int iCurCount = pkGamePlayer->GetCanCostItemCount(iItemID);
						int iNeedCount = ConsumeItems[i].ItemCount();
						iOutNeedCount = iNeedCount;
						iOutNeedItemID = iItemID;
						sStream << pkItem->_kName << "_" << iCurCount << "_" << iNeedCount << "_";
						if(iCurCount < iNeedCount)  // 道具不足
						{
							bEnough = 0;
						}
					}
				}
				sStream << "#";
			}
			else
			{
				sStream << "#";
			}
			sStream << bEnough << "#";
			sStream << iOutNeedCount << "#";
			sStream << iOutNeedItemID << "#";
		}
		break;
	}

	return sStream.str();
}

StringType _getDataString_SendFlower_PanelInfo(int iDataStringType, int iID, int iIndex)
{
	StringStreamType sStream("");
	CF_FlowerConfig::DataEntryMapExternIterator kIter = CF_FlowerConfig::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_FlowerConfig::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_id > 0)
		{
			sStream << pkEntry->_id  << "|";
			sStream << pkEntry->_sDescrip << "|";
			sStream << pkEntry->_lVipLvl << "|";
			sStream << pkEntry->_lYBCost << "|";
			sStream << pkEntry ->_lJBCost << "|";
			sStream << pkEntry->_lSp << "|";
			sStream << pkEntry->_lPopulate << "|";
			CF_FlowerSendLimitCfg::DataEntry *pkItem = SafeGetCSVFileDataEntryPtr<CF_FlowerSendLimitCfg>(pkEntry->_lLimitGroup);
			sStream << pkItem->_lSendCnt<< "#";
		}
	}
	return sStream.str();	
}

StringType _getDataString_PopularReward_Info(int iDataStringType, int iID, int iIndex)
{
	StringStreamType sStream("");
	CF_FlowerGiftCfg::DataEntryMapExternIterator kIter = CF_FlowerGiftCfg::GetSingleton().GetIterator();
	for( int i=0; i<10 && kIter.HasMoreElements(); ++i, kIter.MoveNext())
	{
		const CF_FlowerGiftCfg::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_id > 0)
		{
			sStream << pkEntry->_MoneyCnt << "|";
		}
	}
	return sStream.str();	
}


//可拆分礼包开启信息
StringType _getDataString_DisassembleUseInfo( int iDataStringType, int iID, int iIndex )
{
	StringStreamType sStream("");

	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iID, false, false);
	if(NULL != pkItemInfo)
	{
		sStream << pkItemInfo->_kDisassembleUseCost << "|";
		SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
		if (pkClientInterface)
		{
			SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
			if (pkInterface)
				sStream << pkInterface->GetItemDisassembleUseFuncResult(iID);
		}
	}
	return sStream.str();
}

StringType _getDataString_MulSelectGiftInfo( int iDataStringType, int iID, int iIndex )
{
	StringStreamType sStream("");

	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iID, false, false);
	if(NULL != pkItemInfo)
	{
		if(NULL != pkItemInfo && !pkItemInfo->_kDisassembleUseFuncIDs.empty())
		{
			CF_ItemDisassembleUseFunc::DataEntry *pkItemDisassembleUseFunc = SafeGetCSVFileDataEntryPtr<CF_ItemDisassembleUseFunc>(pkItemInfo->_kDisassembleUseFuncIDs.front().ID());
			if(NULL != pkItemDisassembleUseFunc)
			{
				Utility::UT_SIMDataList &ItemList = pkItemDisassembleUseFunc->_lMustDropItems;
				//StringType sContent = "";
				for(Utility::UT_SIMDataList::const_iterator it = ItemList.begin(); it != ItemList.end(); ++it)
				{
					if (it->ItemChoice() == iIndex )
					{
						sStream << Utility::Int2String(it->ID()) << "#" << Utility::Int2String(it->ParamA()) << "#" <<Utility::Int2String(it->ItemBindMode() > 0)<<"#"<<Utility::Int2String(it->ItemChoice())<<"#"<<Utility::Int2String(it->ItemReinfore())<<"#"<<Utility::Int2String(it->ItemExistenceFunc())<<"#";
					}
				}				
			}
		}
	}
	return sStream.str();
}
//--离线经验配置信息
StringType _getDataString_OffLineExpInfo( int iDataStringType, int iID, int iIndex )
{
	CF_OffLineExp::DataEntryMapExternIterator kIter = CF_OffLineExp::GetSingleton().GetIterator();
	StringStreamType sStream("");
	while(kIter.HasMoreElements())
	{
		CF_OffLineExp::DataEntry* pkData = kIter.PeekNextValuePtr();
		sStream << pkData->_iID << "|";
		sStream << pkData->_sDescrip << "|";
		sStream << pkData->_lVipLvl << "|";
		sStream << pkData->_lCostType << "|";
		sStream << pkData->_lCostPecTp << "|";
		sStream << pkData->_lCostPec << "|";
		sStream << pkData->_lOffTimeMax << "|";
		sStream << pkData->_lGainAdd << "|";
		sStream << pkData->_lGainItemAdd << "|";
		sStream << "#";
		kIter.MoveNext();
	}

	return sStream.str();
}

//--离线奖励倍率
StringType _getDataString_OffLineRwdRatio( int iDataStringType, int iID, int iIndex )
{
	CF_OffLineExpRatio::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_OffLineExpRatio>(iID);
	if(pkEntry != NULL)
	{
		StringStreamType sStream("");
		sStream << pkEntry->_iTime << '#';
		Utility::UT_SIMDataList::const_iterator cit = pkEntry->_kReward.begin();
		for(; cit != pkEntry->_kReward.end(); ++cit)
		{
			sStream << cit->ID() << '@' << cit->ParamA() << '|';
		}

		return sStream.str();
	}
	
	return "";
}

//--地图分流相关信息获取
StringType _getDataString_NewBeeMapInfo( int iDataStringType, int iID, int iIndex )
{
	CF_WorldMapList::DataEntryMapExternIterator kIter = CF_WorldMapList::GetSingleton().GetIterator();
	StringStreamType sStream("");
	while(kIter.HasMoreElements())
	{
		CF_WorldMapList::DataEntry* pkData = kIter.PeekNextValuePtr();
		if ((pkData->_iTributaryMapCount > 1 && pkData->_AutoCreate == 1) || pkData->_bShowLine)
		{
			sStream << pkData->_iID << "|";
			sStream << pkData->_iMapLogicType << "|";
			sStream << pkData->_iTributaryMapCount << "|";
			sStream << pkData->_iTributaryPlayerCount << "|";
			sStream << "#";
		}
		kIter.MoveNext();
	}

	return sStream.str();
}

//橙色装备兑换商店
StringType _getDataString_ExchangeShopList( int iDataStringType, int iID, int iIndex )
{
	StringType strError("");
	StringStreamType sStream("");
	LC_ClientShopCity* pkShopCity = LC_ClientShopCity::GetSingletonPtr();
	if (!pkShopCity)
		return strError;
	ShopCityEntryVector& entryList = pkShopCity->GetShopCityEntryVector(iID);
	if (entryList.empty())
		return strError;
	mem::vector<StringType> mSortedData;
	int totalCount = entryList.size();

	for (ShopCityEntryVectorItr iter = entryList.begin(); iter != entryList.end(); ++iter)
	{
		CF_ShopList::DataEntry* pkData = (*iter).pkData;
		StringStreamType sTemp("");
		sTemp << pkData->_iID << "#";
		sTemp << pkData->_iItemID << "#";
		sTemp << pkData->_iCount << "#";

		//这里最多显示两种
		int PriceType[2] = {0,0};
		int PriceValue[2] = {0,0};

		//std::map<int ,int> k;
		const Utility::LC_ComMap& kPirce = pkData->_ActualPriceList;
		Utility::LC_ComMap::const_iterator it;
		int i = 0;
		for(it = kPirce.begin(); i < 2 && it != kPirce.end(); ++i, ++it)
		{
			PriceType[i] = it->first;
			PriceValue[i] = (int)it->second;
		}

		for (int i = 0; i < 2; ++i)
		{
			sTemp << PriceType[i] << "#" << PriceValue[i] << "#";
		}
		mSortedData.push_back(sTemp.str());
	}

	sStream << totalCount << "|";

	mem::vector<StringType>::iterator it = mSortedData.begin();
	for (; it != mSortedData.end(); ++it)
	{
		sStream << (*it) << "|";
	}

	return sStream.str();
}

//绝版礼包界面
StringType _getDataString_LimitGiftBag( int iDataStringType, int iID, int iIndex )
{
	StringType strError("");
	StringStreamType sStream("");

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return strError;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return strError;
	LC_ClientShopCity* pkShopCity = LC_ClientShopCity::GetSingletonPtr();
	if (!pkShopCity)
		return strError;
	ShopCityEntryVector& entryList = pkShopCity->GetShopCityEntryVector(LSCT_LIMITGIFTBAG);
	if (entryList.empty())
		return strError;
	mem::vector<StringType> mSortedData;
	int totalCount = entryList.size();

	for (ShopCityEntryVectorItr iter = entryList.begin(); iter != entryList.end(); ++iter)
	{
		CF_ShopList::DataEntry* pkData = (*iter).pkData;

		int Price=0;
		int CostType=0;
		mem::map<int32_t, int64_t>::iterator itPrice=pkData->_ActualPriceList.begin();
		if (itPrice != pkData->_ActualPriceList.end())
		{
			CostType	= itPrice->first;
			Price		= itPrice->second;
		}

		StringStreamType sTemp("");
		sTemp << pkData->_iID << "#";
		sTemp << pkData->_iItemID << "#";
		sTemp << pkData->_iCount << "#";
		sTemp << LC_Helper::GetComMapValue(pkData->_OriginalPriceList,CT_PENTACLE) << "#";
		sTemp << Price << "#";
		sTemp << pkGamePlayer->GetCommonShopAsset().GetAvailableCount(pkData->_iID) << "#";
		sTemp << CostType << "#";
		mSortedData.push_back(sTemp.str());
	}

	sStream << totalCount << "|";

	mem::vector<StringType>::iterator it = mSortedData.begin();
	for (; it != mSortedData.end(); ++it)
	{
		sStream << (*it) << "|";
	}

	return sStream.str();
}

StringType _getDataString_CharAttribute(int iDataStringType, int iID, int iIndex)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (!pkLogicManager)
		return "";

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return "";

	int iAtk = 0;
	int iAtkRate = 0;
	int iCir = 0;
	int iDef = 0;
	int iDogge = 0;
	int iCirDef = 0;
	int iHp = 0;
	int iDmgRate = 0;
	int iDmgDerate = 0;
	int iCirDam = 0;

	
	CF_CharAttributeFunc::DataEntry *pkAttrFunc = SafeGetCSVFileDataEntryPtr<CF_CharAttributeFunc>(iID);
	if(pkAttrFunc)
	{
		iAtk += (int)pkAttrFunc->_Atk;
		iAtkRate += (int)pkAttrFunc->_ATKRate;
		iCir += (int)pkAttrFunc->_Cir;
		iDef += (int)pkAttrFunc->_Def;
		iDogge += (int)pkAttrFunc->_Dogge;
		iCirDef += (int)pkAttrFunc->_CirDef;
		iHp += (int)pkAttrFunc->_HP;
		iDmgRate += (int)pkAttrFunc->_DmgRate;
		iDmgDerate += (int)pkAttrFunc->_DmgDerate;
		iCirDam += (int)pkAttrFunc->_CirDam;
	}				
	
	StringStreamType sStream("");
	sStream << ATT_MAX_ATK << "#" << iAtk << "|" << ATT_ATK_RATE << "#" << iAtkRate << "|" << ATT_CIR_RATE << "#" << iCir << "|";
	sStream << ATT_DEF << "#" << iDef << "|" << ATT_DOG_RATE << "#" << iDogge << "|" << ATT_CIR_DEF << "#" << iCirDef << "|";
	sStream << ATT_MAX_HP << "#" << iHp << "|" << ATT_DMG_RATE << "#" << iDmgRate << "|" << ATT_DMG_DERATE << "#" << iDmgDerate << "|";
	sStream << ATT_CIR_DAM << "#" << iCirDam << "|";
	return sStream.str();
}

StringType _getDataString_SubordinateSpecialAttribInfo(int iDataType, int iID, int iIndex)
{
	int result = 0;
	CF_SubRelateAttri::DataEntryMapExternIterator kIter = CF_SubRelateAttri::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_SubRelateAttri::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		TypeNumList::const_iterator it = pkEntry->_kActiveConds.begin();
		if(it != pkEntry->_kActiveConds.end())
		{
			if(iID == it->getType() && iIndex >= it->getNum())
			{
				result = pkEntry->_iAttrPercent;
			}
		}
	}

	return Int2String(result);
}



StringType _getDataString_SubordinateExpireForbid(int iDataType, int iID, int iIndex)
{
	const LC_SubordinateDataEntry *entry = SafeGetSubDataEntry(iID, iIndex);
	if(entry)
	{
		return Int2String(entry->_iExpireForbid);
	}

	return "";
}

StringType _getDataString_RaidExpInfo(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");

	CF_GameStoryRaidList::DataEntryMapExternIterator kIter = CF_GameStoryRaidList::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_GameStoryRaidList::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_lBelongStoryID == 210)
		{
			sStream << pkEntry ->_kName << "#" ;
			sStream << pkEntry ->_kBossIcon << "#" ;
			sStream << pkEntry ->_PreMapResID << "#";
			sStream << pkEntry ->_CombatScore << "#";
			sStream << pkEntry ->_lCharType << "#";
			sStream << pkEntry ->_Reward << "#";
			sStream << pkEntry ->_kTips << "#";
			sStream << pkEntry ->_MapResID << "#";

			sStream << pkEntry ->_NeedLevel << "#";
		
			sStream << "|";
		}
		
	}

	return sStream.str();

}
StringType _getDataString_RaidSingleSole(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");
	CF_GameStoryRaidList::DataEntryMapExternIterator kIter = CF_GameStoryRaidList::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_GameStoryRaidList::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(   (pkEntry ->_lBelongStoryID >= 38 && pkEntry ->_lBelongStoryID <= 40)
			|| (pkEntry ->_lBelongStoryID >= 101 && pkEntry ->_lBelongStoryID <= 110))
		{
			sStream << pkEntry ->_kName << "#" ;
			sStream << pkEntry ->_MapResID << "#";
			sStream << pkEntry ->_kBossIcon << "#" ;
			sStream << pkEntry ->_PreMapResID << "#";
			sStream << pkEntry ->_CombatScore << "#";
			sStream << pkEntry ->_NeedLevel << "#";
			sStream << pkEntry ->_lCharType << "#";
			sStream << pkEntry ->_Reward << "#";
			sStream << pkEntry ->_kTips << "#";
			sStream << pkEntry ->_lBelongStoryID << "#";
			CSVFile::CF_TranscriptionRule::DataEntry *Logic = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_TranscriptionRule>(pkEntry->_MapResID);
			sStream << Logic ->_lFirstLightTaskID << "#";
			sStream << pkEntry ->_NameImage << "#" ;

			sStream << "|";
		}

	}
	return sStream.str();

}

StringType _getDataString_RaidSingleSub(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream;
	StringType sItem;
	sItem.reserve(64);

	CF_GameStoryRaidList::DataEntryMapExternIterator kIter = CF_GameStoryRaidList::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_GameStoryRaidList::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(IS_RAIDSINGLE_SUB(pkEntry ->_lBelongStoryID))
		{
			sStream << pkEntry ->_kName << "#" ;
			sStream << pkEntry ->_MapResID << "#";
			sStream << pkEntry ->_kBossIcon << "#" ;
			sStream << pkEntry ->_PreMapResID << "#";
			sStream << pkEntry ->_CombatScore << "#";
			sStream << pkEntry ->_NeedLevel << "#";
			sStream << pkEntry ->_lCharType << "#";
			sStream << pkEntry ->_Reward << "#";
			sStream << pkEntry ->_kTips << "#";
			sStream << pkEntry ->_lBelongStoryID << "#";

			CSVFile::CF_TranscriptionRule::DataEntry *Logic = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_TranscriptionRule>(pkEntry->_MapResID);
			T_ASSERT_CRITICAL_S(Logic != NULL);
			
			sStream << Logic->_lFirstLightTaskID << "#";

			sItem.clear();
			FormatDataIFs2String(Logic->_Reward_Quick, sItem);
			sStream << sItem << "#" << pkEntry ->_NameImage << "#" ;
			
			sStream << "|";
		}

	}
	return sStream.str();

}

StringType _getDataString_RailyActivity(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");
	CF_DailyActivity::DataEntryMapExternIterator kIter = CF_DailyActivity::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_DailyActivity::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iLevel << "#";
		sStream << pkEntry->_lLiveness << "#";
		sStream << pkEntry->_sGiftInfos << "#";
		sStream << pkEntry->_sActivityRule << "#";

		sStream << "|";
	}
	return sStream.str();

}

StringType _getDataString_RailyDignReward(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");
	CF_SignInDailyReward::DataEntryMapExternIterator kIter = CF_SignInDailyReward::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_SignInDailyReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iDate << "#";
		if (iMapResID == SIGNIN_TYPE_1)
			sStream << pkEntry->_Reward << "#";
		else if(iMapResID == SIGNIN_TYPE_2)
			sStream << pkEntry->_RewardMiniLauncher << "#";


		sStream << "|";
	}
	return sStream.str();

}

StringType _getDataString_RailyCharge(int iDataStringType, int iMapResID, int iStage)
{

	StringStreamType sStream("");
	CF_FirstDailyCharge::DataEntryMapExternIterator kIter = CF_FirstDailyCharge::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_FirstDailyCharge::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iDay << "#";
		sStream << pkEntry->_bType << "#";
		sStream << pkEntry->_sItem << "#";
		sStream << pkEntry->_sShopItem << "#";

		LC_ComMap::const_iterator itrOrg = pkEntry->_sOriginalPriceList.begin();
		if(itrOrg == pkEntry->_sOriginalPriceList.end())
			sStream << "0#0#";
		else
			sStream << itrOrg->first << "#" << itrOrg->second << "#";

		LC_ComMap::const_iterator itrActual = pkEntry->_sActualPriceList.begin();
		if(itrActual == pkEntry->_sActualPriceList.end())
			sStream << "0#0#";
		else
			sStream << itrActual->first << "#" << itrActual->second << "#";

		sStream << pkEntry->_iTime << "#";
		sStream << pkEntry->_iID << "#";
		sStream << "|";
	}
	return sStream.str();

}

StringType _getDataString_BuddyUpgradCost(int iDataStringType, int cID, int iStage)
{
	StringType sCost;

	CF_Buddy::DataEntry  *pkEntry = CF_Buddy::GetSingleton().GetEntryPtr(cID);
	if (pkEntry != NULL)
	{
		long shopID = pkEntry->_kFireShopIDs;
		if (shopID > 0 )
		{
			CashType cashType = CT_PENTACLE;
			CF_ShopList::DataEntry  *pkEntry = CF_ShopList::GetSingleton().GetEntryPtr(shopID);
			if(pkEntry != NULL)
			{
				int64_t iCost = LC_Helper::GetComMapValue(pkEntry->_ActualPriceList, cashType);
				if(iCost == 0)
				{
					cashType = CT_UNBIND_YUANBAO;
					iCost = LC_Helper::GetComMapValue(pkEntry->_ActualPriceList, cashType);
				}
				
				if(iCost > 0)
					UT_BaseStringUtility::format(sCost, "%d#%d", cashType, iCost);
			}
		}
	}

	return sCost;

}

StringType _getDataString_BuddyUpgradData(int iDataStringType, int cID, int iStage)
{

	StringStreamType sStream("");
	CF_BuddyGrowGift::DataEntryMapExternIterator kIter = CF_BuddyGrowGift::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BuddyGrowGift::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_cID << "#";
		sStream << pkEntry->_iDay << "#";
		
		Utility::UT_SIMDataList::const_iterator it = pkEntry->_kReward.begin();
		for (; it!=pkEntry->_kReward.end(); ++it)
		{
			sStream << it->ID() <<"#";
			sStream << it->ItemCount() <<"#";
		}


		sStream << "|";
	}
	return sStream.str();

}

StringType _getDataString_Collection(int iDataStringType, int cID, int iStage)
{

	StringStreamType sStream("");
	CF_CollectionReward::DataEntryMapExternIterator kIter = CF_CollectionReward::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_CollectionReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->id << "#";
		sStream << pkEntry->SuitID << "#";
		sStream << pkEntry->Type << "#";

		Utility::UT_SIMDataList::const_iterator it = pkEntry->CollectionID.begin();
		sStream << pkEntry->CollectionID.size() << "#";
		for (; it!=pkEntry->CollectionID.end(); ++it)
		{
			sStream << it->ID() <<";";

		}
		sStream << "#";

		sStream << pkEntry->CollectionStarLevel << "#";
		Utility::UT_SIMDataList::const_iterator it2 = pkEntry->RewardList.begin();
		sStream << pkEntry->RewardList.size() << "#";
		for (; it2!=pkEntry->RewardList.end(); ++it2)
		{
			sStream << it2->ID() <<":";
			sStream << it2->ItemCount() <<";";
		}
		sStream << "#";

		sStream << pkEntry->SortID << "#";
		sStream << pkEntry->Name << "#";
		sStream << pkEntry->Condition << "#";

		sStream << "|";
	}
	return sStream.str();

}

StringType _getDataString_BossFirstKill(int iDataStringType, int cID, int iStage)
{
	StringStreamType sStream;
	StringType sItem;
	sItem.reserve(64);

	CF_BossFirstKill::DataEntryMapExternIterator kIter = CF_BossFirstKill::GetSingleton().GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_BossFirstKill::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_CharTypeID <<"#";
		sStream << pkEntry->_kReward.size() << "#";

		sItem.clear();
		FormatDataIFs2String(pkEntry->_kReward, sItem);
		sStream << sItem << "#";
		
		sItem.clear();
		FormatDataIFs2String(pkEntry->_kServerReward, sItem);
		sStream << sItem << "#";

		sStream << pkEntry->_iLinkID <<"#";
		sStream << pkEntry->_kBossIcon <<"#";

		sStream << "|";
	}
	return sStream.str();
}

StringType _getDataString_EndlessTower(int iDataStringType, int cID, int iStage)
{
	StringStreamType sStream("");
	CF_EndlessTower::DataEntryMapExternIterator kIter = CF_EndlessTower::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_EndlessTower::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iGameStoryID <<"#";
		sStream << pkEntry->_PreStoryID <<"#";
		sStream << pkEntry->_iType <<"#";
		sStream << pkEntry->_ChallengeCount <<"#";
		sStream << pkEntry->_iCount <<"#";
		sStream << pkEntry->_sActivityBack <<"#";
		sStream << pkEntry->_sActivityText <<"#";

		sStream << "|";

	}
	return sStream.str();
}

StringType _getDataString_Biography(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_Biography::DataEntryMapExternIterator kIter = CF_Biography::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_Biography::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iBiographyID <<"#";
		sStream << pkEntry->_kName <<"#";
		sStream << (int)pkEntry->_iBuddyID <<"#";
		sStream << pkEntry->_bBuddyActivate <<"#";
		sStream << pkEntry->_iActivateTaskID <<"#";
		sStream << pkEntry->_iPreBiographyID <<"#";
		sStream << pkEntry->_sBiographyIcon <<"#";
		Utility::UT_SIMDataList::const_iterator Chapter = pkEntry->_iBiographyTaskID.begin();
		for (; Chapter != pkEntry->_iBiographyTaskID.end(); ++Chapter)
		{
			sStream << Chapter->ID() << ";";
		}
		sStream << "#";
		sStream << pkEntry->_kStatisticsTips <<"#";

		sStream << "|";
	}
	return sStream.str();
}

StringType _getDataString_BuddyBiography(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_BiographyTask::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_BiographyTask>(ID);
	if (pkEntry)
	{
		sStream << pkEntry->_lID <<"#";
		sStream << pkEntry->_kName <<"#";
		Utility::UT_SIMDataList::const_iterator taskIter =  pkEntry->_sTaskID.begin();
		for (; taskIter !=  pkEntry->_sTaskID.end(); ++taskIter)
		{
			sStream << taskIter->ID() << ";";
		}
		sStream << "#";
		Utility::UT_SIMDataList::const_iterator achTaskIter =  pkEntry->_sAchievementID.begin();
		for (; achTaskIter !=  pkEntry->_sAchievementID.end(); ++achTaskIter)
		{
			sStream << achTaskIter->ID() << ";";
		}
		sStream << "#";
		Utility::UT_SIMDataList::const_iterator rewardIter =  pkEntry->_kReward.begin();
		for (; rewardIter !=  pkEntry->_kReward.end(); ++rewardIter)
		{
			sStream << rewardIter->ID() << ";";
		}
		sStream << "#";
		
		sStream << pkEntry->_sRewardDescribe <<"#";
		sStream << pkEntry->_sRewardIcon <<"#";
		sStream << pkEntry->_sButtonIcon <<"#";

	}
	return sStream.str();
}

StringType _getDataString_Holydeed(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_HolyDeed::DataEntryMapExternIterator kIter = CF_HolyDeed::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_HolyDeed::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iItemID <<"#";
		sStream << pkEntry->_sModelIcon <<"#";
		sStream << (int)pkEntry->_cID <<"#";
		sStream << pkEntry->_sAchievePath <<"#";
		sStream << pkEntry->_kDetailDescript <<"#";
		sStream << "|";
	}
	return sStream.str();
}
StringType _getDataString_SevenDaysExperiment(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_SevenDaysTask::DataEntryMapExternIterator kIter = CF_SevenDaysTask::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_SevenDaysTask::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_lID <<"#";
		StringType sStr;
		FormatDataIFs2String(pkEntry->_sAchievementID, sStr);
		sStream << sStr <<"#";
		sStream << pkEntry->_sEndID <<"#";
		sStream << "|";
	}
	sStream << "%";
	CF_SevenDaysTotalReward::DataEntryMapExternIterator lIter = CF_SevenDaysTotalReward::GetSingleton().GetIterator();
	for (; lIter.HasMoreElements(); lIter.MoveNext())
	{
		CF_SevenDaysTotalReward::DataEntry *pkEntry = lIter.PeekNextValuePtr();
		sStream << pkEntry->_lID << "#";
		sStream << pkEntry->_iAchievementPoint << "#";
		StringType sStr;
		FormatDataIFs2String(pkEntry->_sGiftInfos, sStr);
		sStream << sStr << "#";
		sStream << pkEntry->_sDescription << "#";
		sStream << "|";
	}
	return sStream.str();
}

StringType _getDataString_EquipUpgrade(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_CompositionFormula::DataEntryMapExternIterator kIter = CF_CompositionFormula::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_CompositionFormula::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_lYangItemID << "|";
		StringType sStr;
		FormatDataIFs2String(pkEntry->_kNeedMaterials, sStr);
		sStream << sStr << "#";
	}
	return sStream.str();
}

StringType _getDataString_BattlePassCard(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_InvestmentCardTask::DataEntryMapExternIterator kIter = CF_InvestmentCardTask::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_InvestmentCardTask::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		StringType sStr1;
		sStream << Int2String(pkEntry->_iID) << "|" << Int2String(pkEntry->_iType) << "|" << Int2String(pkEntry->_logicVal) << "|" << Int2String(pkEntry->_bReview) << "|" << pkEntry->_bPreview << "|";
		FormatDataIFs2String(pkEntry->_kFreeItems, sStr1);
		sStream << sStr1 << "|";
		StringType sStr2;
		FormatDataIFs2String(pkEntry->_kHighItems, sStr2);
		sStream << sStr2 << "#";
	}
	return sStream.str();
}

StringType _getDataString_BattlePassCardRewardShow(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_InvestmentCard::DataEntryMapExternIterator kIter = CF_InvestmentCard::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_InvestmentCard::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << Int2String(pkEntry->_iType) << "|" << Int2String(pkEntry->_iSchedule) << "|" << FormatComMap2String(pkEntry->_kPrices,1) << "|" << Long2String(pkEntry->_lCharType) << "|" << Float2String(pkEntry->_Angle) << "|"
				<< Float2String(pkEntry->_fCameraDist) << "|" << Long2String(pkEntry->_iLinkID) << "|" << pkEntry->_sBigRewardDescription << "|" << pkEntry->_sBigRewardName << "|" << pkEntry->_sActivityDescription << "|"
				<< pkEntry->_sTitle << "|";
		StringType sStr1;
		FormatDataIFs2String(pkEntry->_Position, sStr1);
		sStream << sStr1 << "|";
		StringType sStr2;
		FormatDataIFs2String(pkEntry->_kFreeItems, sStr2);
		sStream << sStr2 << "|";
		StringType sStr3;
		FormatDataIFs2String(pkEntry->_kHighItems, sStr3);
		sStream << sStr3 << "|";
		sStream << pkEntry->_SubType << "#";
	}
	return sStream.str();
}

StringType _getDataString_BossInvade(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_BossInvade::DataEntryMapExternIterator kIter = CF_BossInvade::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_BossInvade::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iStoryID << "|" ;
		sStream << pkEntry->_lCharType << "|" ;
		sStream << pkEntry->_sDamageUpBuddy << "|" ;
		sStream << pkEntry->_sSkills << "|" ;
		sStream << pkEntry->_sBossDescription << "|" ;
		sStream << "#";
	}
	return sStream.str();
}

StringType _getDataString_VipNewTaskByStage(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_VipNew::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_VipNew>(iStage);
	if (pkEntry)
	{
		mem::vector<UT_SIMDataInfo>::iterator it = pkEntry->_sUpgradeTask.begin();
		while (it != pkEntry->_sUpgradeTask.end())
		{
			sStream<<it->GetTaskID()<<";";
			++it;
		}
		sStream<<"|";
		it = pkEntry->_sLinkID.begin();
		while (it != pkEntry->_sLinkID.end())
		{
			sStream<<it->GetTaskID()<<";";
			++it;
		}
	}
	return sStream.str();
}

StringType _getDataString_VipNewRewardByStage(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_VipNew::DataEntryMapExternIterator kIter = CF_VipNew::GetSingleton().GetIterator();

	CF_VipNew::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_VipNew>(iStage);
	if (pkEntry)
	{
		 mem::vector<UT_SIMDataInfo>::iterator it = pkEntry->_sUpgradeReward.begin();
		 while (it != pkEntry->_sUpgradeReward.end())
		 {
			sStream<<it->GetItemId()<<","<<it->GetItemNum()<<";";
			 ++it;
		 }
		sStream<< "|"
			   << pkEntry->_iReward1<<";"<< pkEntry->_iReward2<<";"<< pkEntry->_iReward3<<";"<< pkEntry->_iReward4<<";"<< pkEntry->_iReward5<<";"
			   << pkEntry->_iReward6<<";"<< pkEntry->_iReward7<<";"<< pkEntry->_iReward8<<";"<< "|"
			   << pkEntry->_iEfficiency1<<";"<< pkEntry->_iEfficiency2<<";"<< pkEntry->_iEfficiency3<<";"<< pkEntry->_iEfficiency4<<";"<< pkEntry->_iEfficiency5<<";"
			   << pkEntry->_iEfficiency6<<";"<< pkEntry->_iEfficiency7<<";"<< pkEntry->_iEfficiency8<<";";
	}
	return sStream.str();
}

StringType _getDataString_CommanderTitleRewardById(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");	
	CF_CommanderTitle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CommanderTitle>(ID);
	if (pkData)
	{
		mem::vector<UT_SIMDataInfo>::iterator it = pkData->_sReward.begin();
		while (it !=  pkData->_sReward.end())
		{
			sStream<<it->GetItemId()<<","<<it->GetItemNum()<<";";
			++it;
		}
	}
	return sStream.str();
}

StringType _getDataString_SpecialTrainingRewardById(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");	
	CF_SpecialTraining::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SpecialTraining>(ID);
	if (pkData)
	{
		sStream	<<pkData->_name<<"|";
		if (pkData->_reward.size()>0)
		{
			sStream<<pkData->_reward[0].GetItemId()<<"|";
			sStream<<pkData->_reward[0].GetItemNum()<<"|";
		}
		else
		{
			sStream<<0<<"|";
			sStream<<0<<"|";
		}
		sStream<<pkData->_lCharID<<"|";
		sStream	<<pkData->_sModelIcon<<"|";
		sStream	<<pkData->_sTipsIcon<<"|";
		sStream <<pkData->_Angle<<"|";
		sStream <<pkData->_sDesIcon;
	}
	return sStream.str();
}

StringType _getDataString_CharTypePositionInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_CharType::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(ID);
	if (pkData)
	{
		const mem::vector<int16_t>& pos = pkData->_Position;
		if(pos.size() >= 3)
		{
			sStream << pos[0] << '|'
					<< pos[1] << '|'
					<< pos[2] << '|';
		}
		else
		{
			sStream << "0|0|0|";
		}
		
		sStream << pkData->_CameraDist;

	}
	return sStream.str();
}

StringType _getDataString_BossInvadeRankReward(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_BossInvadeRankReward::DataEntryMapExternIterator kIter = CF_BossInvadeRankReward::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_BossInvadeRankReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iStoryID << "|" ;
		sStream << pkEntry->_iRankStartIndex << "|" ;
		sStream << pkEntry->_iRankEndIndex << "|" ;
		sStream << pkEntry->_kReward << "|" ;
		sStream << "#";
	}
	return sStream.str();
}

StringType _getDataString_GuildBossBefallReward(int iDataStringType, int ID, int iStage)
{	
	StringStreamType sStream("");
	CF_YiZhongJiangLinRankReward::DataEntryMapExternIterator kIter = CF_YiZhongJiangLinRankReward::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_YiZhongJiangLinRankReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iType << "|" ;
		sStream << pkEntry->_iRankStartIndex << "|" ;
		sStream << pkEntry->_iRankEndIndex << "|" ;
		sStream << pkEntry->_kReward << "|" ;
		sStream << "#";
	}
	return sStream.str();
}

StringType _getDataString_RaidBossSpeciesStoryID(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_GameStory::DataEntryMapExternIterator kIter = CF_GameStory::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_GameStory::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_lGameStoryType == GAMESTORY_TYPE_Species)
			sStream << pkEntry->_iID << "|";
	}
	return sStream.str();
}

StringType _getDataString_GuildBossBefallStoryID(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_GameStory::DataEntryMapExternIterator kIter = CF_GameStory::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_GameStory::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_lGameStoryType == GAMESTORY_TYPE_GUILDBOSSBEFALL)
			sStream << pkEntry->_iID << "|";
	}
	return sStream.str();
}


StringType _getDataString_RecommendHeroByID(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_RecommendHero::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_RecommendHero>(ID);
	if(pkEntry)
	{
		sStream<<pkEntry->_sBuddy1<<"|"<<pkEntry->_sBuddy2;
	}
	
	return sStream.str();
}


StringType _getDataString_GetACTShopAAvatarInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_ACTShopA::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_ACTShopA>(ID);
	if(pkEntry)
	{
		sStream <<pkEntry->_CharTypeID1<<"#"<<pkEntry->_Position1;
	}

	return sStream.str();
}

StringType _getDataString_RaidBossPersonalInfo(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");

	CF_GameStoryRaidList::DataEntryMapExternIterator kIter = CF_GameStoryRaidList::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_GameStoryRaidList::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_lBelongStoryID == iMapResID)
		{
			sStream << pkEntry ->_kName << "#" ;
			sStream << pkEntry ->_kBossIcon << "#" ;
			sStream << pkEntry ->_PreMapResID << "#";
			sStream << pkEntry ->_CombatScore << "#";
			sStream << pkEntry ->_lCharType << "#";
			sStream << pkEntry ->_Reward << "#";
			sStream << pkEntry ->_kTips << "#";
			sStream << pkEntry ->_MapResID << "#";
			CF_TranscriptionRule::DataEntry *pkMissionEntry  = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(pkEntry ->_MapResID);
			if (pkMissionEntry)
				sStream << pkMissionEntry ->_iPlayerMinLevel << "#" << pkMissionEntry->_iCommanderTitleLevel;
			else
				sStream << "0#0";
			sStream << "|";
		}
	}
	return sStream.str();
}

StringType _getDataString_RaidBossPublicRewardInfo(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");
	CF_RegularBoss::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_RegularBoss>(iStage);
	if (pkEntry)
	{
		StringType s1;
		FormatDataIFs2String(pkEntry->_kReward1, s1);
		sStream << s1 << "#";
		StringType s2;
		FormatDataIFs2String(pkEntry->_kReward2, s2);
		sStream << s2 << "#";
		StringType s3;
		FormatDataIFs2String(pkEntry->_kReward3, s3);
		sStream << s3 << "#";
		StringType s4;
		FormatDataIFs2String(pkEntry->_kReward4, s4);
		sStream << s4 << "#";
		sStream << pkEntry->_iGroupID << "#";
	}
	return sStream.str();
}

StringType _getDataString_AnswerActivityPointReward(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");
	CF_TestAccPointRewards::DataEntryMapExternIterator kIter = CF_TestAccPointRewards::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_TestAccPointRewards::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_accountPoint << "|" ;
		StringType s;
		FormatDataIFs2String(pkEntry->_rewards, s);
		sStream << s << "#";
	}
	return sStream.str();
}

StringType _getDataString_AnswerActivityRankRewards(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");
	CF_TestRankReward::DataEntryMapExternIterator kIter = CF_TestRankReward::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_TestRankReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iRankStartIndex << "|" ;
		sStream << pkEntry->_iRankEndIndex << "|" ;
		sStream << pkEntry->_kReward << "|" ;
		sStream << "#";
	}
	return sStream.str();
}
StringType _getDataString_AnswerActivityGuildRankRewards(int iDataStringType, int iMapResID, int iStage){
	StringStreamType sStream("");
	CF_TestRankGuildReward::DataEntryMapExternIterator kIter = CF_TestRankGuildReward::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_TestRankGuildReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iRankStartIndex << "|" ;
		sStream << pkEntry->_iRankEndIndex << "|" ;
		sStream << pkEntry->_kReward << "|" ;
		sStream << "#";
	}
	return sStream.str();
}

StringType _getDataString_YijituxiRankRewards(int iDataStringType, int iMapResID, int iStage)
{
	StringStreamType sStream("");
	CF_YiJiTuXiRankRewards::DataEntryMapExternIterator kIter = CF_YiJiTuXiRankRewards::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_YiJiTuXiRankRewards::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iRankStartIndex << "|" ;
		sStream << pkEntry->_iRankEndIndex << "|" ;
		sStream << pkEntry->_kReward << "|" ;
		sStream << "#";
	}
	return sStream.str();
}

StringType _getDataString_PersonalHeadSettingInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_HeadFrame::DataEntryMapExternIterator kIter = CF_HeadFrame::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_HeadFrame::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_id << "|";
		sStream << pkEntry->_sModelIcon << "|";
		sStream << pkEntry->_sAchievePath << "|";
		sStream << pkEntry->_sName << "#";
	}
	return sStream.str();
}

StringType _getDataString_GloryExpeditionReward(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_GloryExpedition::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_GloryExpedition>(ID);
	if (pkEntry)
	{
		sStream << pkEntry->_sIcon << "|";
		sStream << pkEntry->_iStarNumber1 << "#"<<pkEntry->_kReward1[0].GetItemId()<< "#"<<pkEntry->_kReward1[0].GetItemNum()<<"|";
		sStream << pkEntry->_iStarNumber2 << "#"<<pkEntry->_kReward2[0].GetItemId()<< "#"<<pkEntry->_kReward2[0].GetItemNum()<<"|";
		sStream << pkEntry->_iStarNumber3<< "#"<<pkEntry->_kReward3[0].GetItemId()<< "#"<<pkEntry->_kReward3[0].GetItemNum()<<"|";

		mem::vector<int>::const_iterator iter = pkEntry->_iTranscriptionRuleID.begin();
		for( ; iter!=pkEntry->_iTranscriptionRuleID.end(); ++iter)
		{
			sStream << *iter << ';';
		}
	}
	return sStream.str();
}

static void MergeRewards(mem::map<int,int>& RewardMap,Utility::UT_SIMDataList& RewardArr)
{
	for (int i=0;i<RewardArr.size();i++)
	{
		UT_SIMDataInfo& pInfo=RewardArr[i];
		int iItemID=pInfo.GetItemId();
		int iItemNum=pInfo.GetItemNum();
		mem::map<int,int>::iterator it=RewardMap.find(iItemID);
		if(it!=RewardMap.end())
		{
			it->second+=iItemNum;
		}
		else
		{
			RewardMap.insert(std::pair<int,int>(iItemID,iItemNum));
		}
	}
}

StringType _getDataString_GloryExpeditionMissionInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	
	CF_TranscriptionRule::DataEntry *pkMissionEntry  = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(ID);
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if(NULL == pkPlayer)
		return "";
	int iCommanderLevel=pkPlayer->GetCommanderLevel();
	CF_EXPList::DataEntry * pkExpEntry=SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);
	if (pkMissionEntry)
	{
		sStream<<pkMissionEntry->_iIsSpecial<<"|";//是否时特殊关卡
		sStream<<pkMissionEntry->_CombatMinScore<<"|";//推荐战力
		sStream<<pkMissionEntry->_iPlayerMinLevel<<"|";//等级下限
		//
		sStream<<pkMissionEntry->_Consume_Quick_Money[0].GetItemNum()<<"|";//扫荡消耗物品数量

		//首通奖励
		for (int i=0;i<pkMissionEntry->_lFirstLightReward.size();i++)
		{
			UT_SIMDataInfo& pInfo=pkMissionEntry->_lFirstLightReward[i];
			sStream<<pInfo.GetItemId()<<":"<<pInfo.GetItemNum()<<";";
		}
		sStream<<"|";

		//把三个星级的奖励合并
		mem::map<int,int> Rewards;
		if (iStage>=1)
		{
			MergeRewards(Rewards,pkMissionEntry->_kItem1);
		}
		if (iStage>=2)
		{
			MergeRewards(Rewards,pkMissionEntry->_kItem2);
		}
		if (iStage>=3)
		{
			MergeRewards(Rewards,pkMissionEntry->_kItem3);
		}
		
		mem::map<int,int>::iterator it=Rewards.begin();
		while(it!= Rewards.end())
		{
			sStream<<it->first<<":"<<it->second<<";";
			++it;
		}
		//额外增加buddy经验
		if (pkExpEntry)
		{
			int RewardId=10000030;
			int effect[3]={pkMissionEntry->_BuddyExp1,pkMissionEntry->_BuddyExp2,pkMissionEntry->_BuddyExp3};
			int baseValue=pkExpEntry->_lBaseBuddyXP;
			int iValue=0;
			for (int i=0;i<3;i++)
			{
				iValue+=effect[i]*baseValue;
				
			}
			if (iValue>0)
			{
				sStream<<RewardId<<":"<<iValue<<";";
			}
		}
		sStream<<"|";
		sStream<<pkMissionEntry->_Consume_Quick_Money[0].GetItemId()<<"|";//扫荡消耗物品ID

		//扫荡奖励
		for (int i=0;i<pkMissionEntry->_Reward_Quick.size();i++)
		{
			UT_SIMDataInfo& pInfo=pkMissionEntry->_Reward_Quick[i];
			sStream<<pInfo.GetItemId()<<":"<<pInfo.GetItemNum()<<";";
		}
		
		if (pkExpEntry)
		{
			int RewardId[3]={10000030,10000031,10000032};
			int effect[3]={pkMissionEntry->_BuddyExp_Quick,pkMissionEntry->_ControllerExp_Quick,pkMissionEntry->_Money_Quick};
			int baseValue[3]={pkExpEntry->_lBaseBuddyXP,pkExpEntry->_lBaseXP,pkExpEntry->_lBaseCash};
			for (int i=0;i<3;i++)
			{
				int iValue=effect[i]*baseValue[i];
				if (iValue>0)
				{
					sStream<<RewardId[i]<<":"<<iValue<<";";
				}
			}
		}
		
		sStream<<"|";

		//特殊关卡奖励
		for (int i=0;i<pkMissionEntry->_SpecialReward.size();i++)
		{
			UT_SIMDataInfo& pInfo=pkMissionEntry->_SpecialReward[i];
			sStream<<pInfo.GetItemId()<<":"<<pInfo.GetItemNum()<<";";
		}
		sStream<<"|";
		sStream<<pkMissionEntry->_Times_Quick;
	}
	return sStream.str();
}

StringType _getDataString_RecommendRuneInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	
	CF_BuddyRuneRecommend::DataEntryMapExternIterator kIter = CF_BuddyRuneRecommend::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_BuddyRuneRecommend::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_ID << "|" << (int)pkEntry->_cID << "|" << pkEntry->_sRecommendName << "|" << pkEntry->_sRuneID << "#";
	}
	return sStream.str();
}

StringType _getDataString_GloryExpeditionMissionIndex(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	int iMissionIdx=1;
	int iChapterId=1;
	CF_TranscriptionRule::DataEntry *pkMissionEntry  = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(ID);
	if (pkMissionEntry)
	{
		iChapterId=pkMissionEntry->_chapterID;
		CF_GloryExpedition::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_GloryExpedition>(iChapterId);
		if (pkEntry)
		{
			mem::vector<int>::const_iterator iter = pkEntry->_iTranscriptionRuleID.begin();
			for(int i=0; i<pkEntry->_iTranscriptionRuleID.size(); ++i)
			{
				if (pkEntry->_iTranscriptionRuleID[i]==ID)
				{
					iMissionIdx=i+1;
					break;
				}
			}
		}
	}
	sStream << iChapterId << "#" << iMissionIdx << "#0";
	return sStream.str();
}

StringType _getDataString_GloryExpeditionRaidInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_TranscriptionRule::DataEntry *pkMissionEntry  = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(ID);
	if (pkMissionEntry)
	{

		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
		LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
		if(NULL == pkPlayer)
			return "";
		int iCommanderLevel=pkPlayer->GetCommanderLevel();
		CF_EXPList::DataEntry * pkExpEntry=SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);
		int baseValue=0;
		if (pkExpEntry)
		{
			baseValue=pkExpEntry->_lBaseBuddyXP;
		}
		int buddyExpItemId=10000030;
		int effect[3]={pkMissionEntry->_BuddyExp1,pkMissionEntry->_BuddyExp2,pkMissionEntry->_BuddyExp3};
		for (int i=0;i<pkMissionEntry->_kItem1.size();i++)
		{
			UT_SIMDataInfo& pInfo=pkMissionEntry->_kItem1[i];
			sStream<<pInfo.GetItemId()<<":"<<pInfo.GetItemNum()<<";";
		}
		int iValue=effect[0]*baseValue;
		if (iValue>0)
		{
			sStream<<buddyExpItemId<<":"<<iValue<<";";
		}
		sStream<<"|";
		for (int i=0;i<pkMissionEntry->_kItem2.size();i++)
		{
			UT_SIMDataInfo& pInfo=pkMissionEntry->_kItem2[i];
			sStream<<pInfo.GetItemId()<<":"<<pInfo.GetItemNum()<<";";
		}
		iValue=effect[1]*baseValue;
		if (iValue>0)
		{
			sStream<<buddyExpItemId<<":"<<iValue<<";";
		}
		sStream<<"|";
		for (int i=0;i<pkMissionEntry->_kItem3.size();i++)
		{
			UT_SIMDataInfo& pInfo=pkMissionEntry->_kItem3[i];
			sStream<<pInfo.GetItemId()<<":"<<pInfo.GetItemNum()<<";";
		}
		iValue=effect[2]*baseValue;
		if (iValue>0)
		{
			sStream<<buddyExpItemId<<":"<<iValue<<";";
		}
		sStream<<"|";
		sStream<<30<<"|";
		sStream<<60<<"|";
		sStream<<pkMissionEntry->_iExistTime<<"|";
	}
	CF_WorldMapList::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(ID);
	if(pkEntry)
	{
		sStream<<pkEntry->_kName;
	}
	return sStream.str();
}

StringType _getDataString_RaidGloryNextMission(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	int iChapterId=1;
	int iNextMissionId=0;
	int iLevelLimit=1;
	bool bInCurChapter=false;//下一关是否在当前章节
	CF_TranscriptionRule::DataEntry *pkMissionEntry  = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(ID);
	if (pkMissionEntry)
	{
		iChapterId=pkMissionEntry->_chapterID;
		CF_GloryExpedition::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_GloryExpedition>(iChapterId);
		if (pkEntry)
		{
			const mem::vector<int>& kvect = pkEntry->_iTranscriptionRuleID;

			for (int i=0;i<kvect.size();i++)
			{
				int iTransID = kvect[i];
				if (bInCurChapter)
				{
					iNextMissionId = iTransID;
					break;
				}
				if (iTransID==ID)
				{
					if (i == kvect.size()-1)
					{
						iChapterId++;
						break;
					}
					else
					{
						bInCurChapter=true;
					}
				}
			}
		}
	}
	if (!bInCurChapter)
	{
		CF_GloryExpedition::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_GloryExpedition>(iChapterId);
		if (pkEntry && !pkEntry->_iTranscriptionRuleID.empty())
		{
			iNextMissionId = pkEntry->_iTranscriptionRuleID[0];
		}
	}
	CF_TranscriptionRule::DataEntry *pkNextMission  = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(iNextMissionId);
	if (pkNextMission)
	{
		iLevelLimit=pkNextMission->_iPlayerMinLevel;
	}
	sStream <<iNextMissionId<<"#"<<iLevelLimit;
	return sStream.str();
}

StringType _getDataString_YijiTuxiInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_YiJiTuXiStageReward::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_YiJiTuXiStageReward>(ID);
	if (pkEntry)
	{
		sStream<<pkEntry->_KillCount<<'|';
		for (int i=0;i<pkEntry->_Reward.size();i++)
		{
			Utility::UT_SIMDataInfo& pInfo=pkEntry->_Reward[i];
			sStream<<pInfo.GetItemId()<<':'<<pInfo.GetItemNum()<<';';
		}
		sStream << '|';
	}
	return sStream.str();
}

StringType _getDataString_YijiXunbaoInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_YiJiXunBaoStageReward::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_YiJiXunBaoStageReward>(ID);
	if (pkEntry)
	{
		sStream<<pkEntry->_KillCount<<'|';
		for (int i=0;i<pkEntry->_Reward.size();i++)
		{
			Utility::UT_SIMDataInfo& pInfo=pkEntry->_Reward[i];
			sStream<<pInfo.GetItemId()<<':'<<pInfo.GetItemNum()<<';';
		}
		sStream << '|';
	}
	return sStream.str();
}

StringType _getDataString_GetRotaryReward(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_RotaryTableReward::DataEntryMapExternIterator iter = CF_RotaryTableReward::GetSingletonPtr()->GetIterator();

	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_RotaryTableReward::DataEntry* pkEntry=iter.PeekNextValuePtr();
		Utility::UT_SIMDataInfo& pInfo=pkEntry->_RotaryTableReward[0];
		sStream<<pInfo.GetItemId()<<':'<<pInfo.GetItemNum()<<';';
	}

	return sStream.str();

}

StringType _getDataString_CombatCompareInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_CommanderCombatPK::DataEntryMapExternIterator kIter = CF_CommanderCombatPK::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_CommanderCombatPK::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_ID << "|" << pkEntry->_IconPath << "|" << pkEntry->_NamePath << "|" << pkEntry->_fID << "|" << pkEntry->_linkID << "|" << pkEntry->_kKeyID << "#";
	}
	return sStream.str();
}

StringType _getDataString_NonpareilItemInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	SerializeDataFile<CF_NonpareilItem>(sStream, '#', '|');
	return sStream.str();
}

StringType _getDataString_BestGuildRankReward(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	if (ID == 0)
	{
		CF_BestGuildGuildRankReward::DataEntryMapExternIterator kIter = CF_BestGuildGuildRankReward::GetSingleton().GetIterator();
		for (;kIter.HasMoreElements();kIter.MoveNext())
		{
			CF_BestGuildGuildRankReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
			if (pkEntry->_iType == iStage)
			{
				sStream << pkEntry->_iRankStartIndex<< '#' <<pkEntry->_iRankEndIndex << '#'
						<< pkEntry->_kReward1 << '#' << pkEntry->_kReward2 << '#'
						<< pkEntry->_kReward3 << '#' << pkEntry->_kReward4 << '|';
			}
		}
	}
	else if(ID == 1)
	{
		CF_BestGuildPersonalRankReward::DataEntryMapExternIterator kIter = CF_BestGuildPersonalRankReward::GetSingleton().GetIterator();
		for (;kIter.HasMoreElements();kIter.MoveNext())
		{
			CF_BestGuildPersonalRankReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
			if (pkEntry->_iType == iStage)
				sStream << pkEntry->_iRankStartIndex<< '#' <<pkEntry->_iRankEndIndex << '#' << pkEntry->_kReward << '|';
		}
	}
	return sStream.str();
}

StringType _getDataString_BestGuildInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_BestGuild::DataEntryMapExternIterator kIter = CF_BestGuild::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_BestGuild::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iShowSort<< '#' <<pkEntry->_iGamestory << '|';
	}
	return sStream.str();
}

StringType _getDataString_ToyIslandRewardInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");
	CF_PetIslandStageReward::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_PetIslandStageReward>(ID);
	if (pkEntry)
	{
		sStream<<pkEntry->_KillCount<<'|';
		for (int i=0;i<pkEntry->_Reward.size();i++)
		{
			Utility::UT_SIMDataInfo& pInfo=pkEntry->_Reward[i];
			sStream<<pInfo.GetItemId()<<':'<<pInfo.GetItemNum()<<';';
		}
		sStream << '|';
	}
	return sStream.str();
}

StringType _getDataString_ConvoyRewards(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if(NULL == pkPlayer)
		return "";

	int iCommanderLevel=pkPlayer->GetCommanderLevel();
	CF_EXPList::DataEntry * pkExpEntry=SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);
	if (NULL == pkExpEntry)
	{
		return "";
	}

	CF_EscortReward::DataEntryMapExternIterator kIter = CF_EscortReward::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_EscortReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		mem::map<int,int> Rewards;
		Rewards[10000031]=pkEntry->_iExp*pkExpEntry->_lBaseXP;
		Rewards[10000032]=pkEntry->_iCash*pkExpEntry->_lBaseCash;
		Rewards[10000030]=pkEntry->_iBuddyExp*pkExpEntry->_lBaseBuddyXP;
		MergeRewards(Rewards,pkEntry->_sRewardItem1);


		sStream << pkEntry->_iNpcID<<"#";

		mem::map<int,int>::iterator it=Rewards.begin();
		while(it!= Rewards.end())
		{
			sStream<<it->first<<":"<<it->second<<";";
			++it;
		}

		sStream << '|';
	}
	return sStream.str();
}

//iStage : 1RobSuccess 2 ConveyFail 3 ConveySuccess
StringType _getDataString_ConvoyRewardsById(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream("");

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if(NULL == pkPlayer)
		return "";

	int iCommanderLevel=pkPlayer->GetCommanderLevel();
	CF_EscortReward::DataEntry * pkEntry=SafeGetCSVFileDataEntryPtr<CF_EscortReward>(ID);
	CF_EXPList::DataEntry * pkExpEntry=SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);

	if (pkEntry&&pkExpEntry)
	{
		mem::map<int,int> Rewards;
	

		float percent=1.f;
		if (iStage==1)
		{
			percent=0.3f;
			MergeRewards(Rewards,pkEntry->_sRewardItem3);
		}
		else if(iStage==2)
		{
			percent=0.7f;
			MergeRewards(Rewards,pkEntry->_sRewardItem2);
		}
		else
		{
			percent=1.0f;
			MergeRewards(Rewards,pkEntry->_sRewardItem1);
		}
	

		mem::map<int,int>::iterator it=Rewards.begin();
		while(it!= Rewards.end())
		{
			sStream<<it->first<<":"<<it->second<<";";
			++it;
		}
		Rewards.clear();

		Rewards[10000031]=pkEntry->_iExp*pkExpEntry->_lBaseXP;
		Rewards[10000032]=pkEntry->_iCash*pkExpEntry->_lBaseCash;
		Rewards[10000030]=pkEntry->_iBuddyExp*pkExpEntry->_lBaseBuddyXP;

		it=Rewards.begin();
		while(it!= Rewards.end())
		{
			sStream<<it->first<<":"<<(it->second*percent)<<";";
			++it;
		}
	}
		
	return sStream.str();
}

StringType _getDataString_BestGuildGoalInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_BestGuildGoal::DataEntryMapExternIterator kIter = CF_BestGuildGoal::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_BestGuildGoal::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_lID << '|' << pkEntry->_iGamestory << '|'<< pkEntry->_sName << '|' << pkEntry->_sInfo << '|' << pkEntry->_Count << '|' << pkEntry->_iLinkID << '#';
	}
	return sStream.str();
}

StringType _getDataString_NewAchievementRewardInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_NewAchievementReward::DataEntryMapExternIterator kIter = CF_NewAchievementReward::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_NewAchievementReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iID << '|' << pkEntry->_chapterName << '|' << pkEntry->_rewardNum << '|';

		Utility::UT_SIMDataList::const_iterator iter = pkEntry->_chapterReward.begin();
		if(iter != pkEntry->_chapterReward.end())
			sStream << iter->IID() << "|" << iter->ParamA() << "|" << iter->ParamB() << "|";
		else
			sStream << "0|0|0|";

		sStream << pkEntry->_lCharID << '|' << pkEntry->_CID << '|'<< pkEntry->_CameraDist << '|' ;

		Utility::UT_SIMDataList::const_iterator itor = pkEntry->_Position.begin();
		if(itor != pkEntry->_Position.end())
			sStream << itor->IID() << "|" << itor->ParamA() << "|" << itor->ParamB() << "|";
		else
			sStream << "0|0|0|";

		sStream << pkEntry->_Angle << '|' << pkEntry->_sModelIcon << '|' << pkEntry->_sDesIcon << '#';
	}
	return sStream.str();
}

StringType _getDataString_EntrustTaskInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(ID);
	if (pkEntry)
	{
		sStream<< pkEntry->_iTaskID <<"|";
		sStream<< pkEntry->_iStar <<"|";
		sStream<< pkEntry->_sName <<"|";
		sStream<< pkEntry->_sMonsterIcon <<"|";
		sStream<< pkEntry->_iTime <<"|";
		sStream<< pkEntry->_iType <<"|";
		sStream<< pkEntry->_sCondition <<"|";
		sStream<< pkEntry->_sConsume <<"|";
		sStream<< pkEntry->_kCurrentPrices <<"|";
		sStream<< pkEntry->_sDesc <<"|";
		mem::vector<UT_SIMDataInfo>::iterator it=pkEntry->_kReward.begin();
		while(it!= pkEntry->_kReward.end())
		{
			sStream<<it->GetItemId()<<":"<<it->GetItemNum()<<";";
			++it;
		}
		sStream <<"|";
	}
	return sStream.str();
}

StringType _getDataString_GuildBossActivityInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_GuildTower::DataEntryMapExternIterator kIter = CF_GuildTower::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_GuildTower::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_lMapResID << '@' <<  pkEntry->_PreMapResID << '@' <<  pkEntry->_lCharType << '@' <<  pkEntry->_GameStoryID << '@';

		StringType sStr;
		FormatDataIFs2String(pkEntry->_consumeItem, sStr);
		sStream << sStr << '@';

		sStream << pkEntry->_kBossIcon << '@' << pkEntry->_iRankRewardType << '@'<< pkEntry->_sJoinReward << '@' << pkEntry->_sTotalReward << '@' << pkEntry->_sSkills << '@' << pkEntry->_sBossDescription << '#';
	} 
	return sStream.str();
}

StringType _getDataString_GuildBossBefallInfo(int iDataStringType, int ID, int iStage)
{
	StringStreamType sStream;
	CF_YiZhongJiangLinBossInformation::DataEntryMapExternIterator iter = CF_YiZhongJiangLinBossInformation::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_YiZhongJiangLinBossInformation::DataEntry* pkEntry = iter.PeekNextValuePtr();	
		sStream<<pkEntry->_iStoryID<<"|"<<pkEntry->_lCharType<<"|"<<pkEntry->_sSkills<<"|"<<pkEntry->_sBossDescription<<"|" << pkEntry->_sLocation << "#";
	}
	return sStream.str();
}

////datastring
StringType SC_ClientSkillInterface::GetDataString(int iDataStringType, int iID, int iIndex)
{
	switch(iDataStringType)
	{
	case DataStringType_UIOpenLevel:
		return _getDataString_UIOpenLevel(iDataStringType, iID, iIndex);

	case DataStringType_TrialShopList:
		return _getDataString_TrialShopList(iDataStringType, iID, iIndex);

	case DataStringType_DiscountShopList:
		return _getDataString_DiscountShopList(iDataStringType, iID, iIndex);

	case DataStringType_TaskListRewardList:
		return _getDataString_TaskRewardList(iDataStringType, iID, iIndex);

	case DataStringType_FunctionPreview:
		return _getDataString_FunctionPreview(iDataStringType, iID, iIndex);

	case DataStringType_ShopList_EntryInfo:
		return _getDataString_ShopList_EntryInfo(iDataStringType, iID, iIndex);

	case DataStringType_GameStoryRaidList_BossInfo:
		return _getDataString_GameStoryRaidList_Info(iDataStringType, iID, iIndex);

	case DataStringType_WorldMapList_LimitInfo:
		return _getDataString_WorldMapList_LimitInfo(iDataStringType, iID, iIndex);

	case DataStringType_SendFlower_PanelInfo:
		return _getDataString_SendFlower_PanelInfo(iDataStringType, iID, iIndex);

	case DataStringType_PopularReward_Info:
		return _getDataString_PopularReward_Info(iDataStringType, iID, iIndex);

	case DataStringType_DisassembleUse_Info:
		return _getDataString_DisassembleUseInfo(iDataStringType, iID, iIndex);

	case DataStringType_MulSelectGift_Info:
		return _getDataString_MulSelectGiftInfo(iDataStringType, iID, iIndex);

	case DataStringType_OffLineExp_Info:
		return _getDataString_OffLineExpInfo(iDataStringType, iID, iIndex);

	case DataStringType_OffLineRwdRatio_Info:
		return _getDataString_OffLineRwdRatio(iDataStringType, iID, iIndex);

	case DataStringType_NewBeeMap_Info:
		return _getDataString_NewBeeMapInfo(iDataStringType, iID, iIndex);

	case DataStringType_ExchangeShop:
		return _getDataString_ExchangeShopList(iDataStringType, iID, iIndex);

	case DataStringType_Subordinage_Profit:
		return _getDataString_Subordinage_Profit(iDataStringType, iID, iIndex);

	case DataStringType_LimitGiftBag:
		return _getDataString_LimitGiftBag(iDataStringType, iID, iIndex);

	case DataStringType_CharAttribute:
		return _getDataString_CharAttribute(iDataStringType, iID, iIndex);

	case DataStringType_SubordinateSpecialAttribInfo:
		return _getDataString_SubordinateSpecialAttribInfo(iDataStringType, iID, iIndex);

	case DataStringType_SubordinateExpireForbid:
		return _getDataString_SubordinateExpireForbid(iDataStringType, iID, iIndex);

	case DataStringType_RaidExpInfo:
		return _getDataString_RaidExpInfo(iDataStringType,iID,iIndex);

	case DataStringType_RaidSingleSole:
		return _getDataString_RaidSingleSole(iDataStringType,iID,iIndex);

	case DataStringType_RaidSingleSub:
		return _getDataString_RaidSingleSub(iDataStringType,iID,iIndex);

	case DataStringType_RaidBossSpeciesStoryID:
		return _getDataString_RaidBossSpeciesStoryID(iDataStringType,iID,iIndex);
	case DataStringType_GuildBossBefallStoryID:
		return _getDataString_GuildBossBefallStoryID(iDataStringType,iID,iIndex);
	case DataStringType_RaidBossPersonal:
		return _getDataString_RaidBossPersonalInfo(iDataStringType,iID,iIndex);

	case DataStringType_RaidBossPublic:
		return _getDataString_RaidBossPublicRewardInfo(iDataStringType,iID,iIndex);

	case DataStringType_RaidGloryNextMissionId:
		return _getDataString_RaidGloryNextMission(iDataStringType,iID,iIndex);

	case DataStringType_YijiTuxiInfo:
		return _getDataString_YijiTuxiInfo(iDataStringType,iID,iIndex);

	case DataStringType_RailyActivity:
		return _getDataString_RailyActivity(iDataStringType,iID,iIndex);

	case DataStringType_RailySignReward:
		return _getDataString_RailyDignReward(iDataStringType,iID,iIndex);

	case DataStringType_RailyCharge:
		return _getDataString_RailyCharge(iDataStringType,iID,iIndex);

	case DataStringType_BuddyUpgradCost:
		return _getDataString_BuddyUpgradCost(iDataStringType,iID,iIndex);

	case DataStringType_BuddyUpgradData:
		return _getDataString_BuddyUpgradData(iDataStringType,iID,iIndex);

	case DataStringType_Collection:
		return _getDataString_Collection(iDataStringType,iID,iIndex);

	case DataStringType_BossFirstKill:
		return _getDataString_BossFirstKill(iDataStringType,iID,iIndex);

	case DataStringType_EndlessTower:
		return _getDataString_EndlessTower(iDataStringType,iID,iIndex);

	case DataStringType_Biography:
		return _getDataString_Biography(iDataStringType,iID,iIndex);

	case DataStringType_BuddyBiography:
		return _getDataString_BuddyBiography(iDataStringType,iID,iIndex);

	case DataStringType_Holydeed:
		return _getDataString_Holydeed(iDataStringType,iID,iIndex);

	case DataStringType_SevenDaysExperiment:
		return _getDataString_SevenDaysExperiment(iDataStringType,iID,iIndex);

	case DataStringType_EquipUpgrade:
		return _getDataString_EquipUpgrade(iDataStringType,iID,iIndex);

	case DataStringType_BattlePassCard:
		return _getDataString_BattlePassCard(iDataStringType,iID,iIndex);

	case DataStringType_BattlePassCardRewardShow:
		return _getDataString_BattlePassCardRewardShow(iDataStringType,iID,iIndex);

	case DataStringType_BossInvade:
		return _getDataString_BossInvade(iDataStringType,iID,iIndex);

	case DataStringType_BossInvadeRankReward:
		return _getDataString_BossInvadeRankReward(iDataStringType,iID,iIndex);

	case DataStringType_GuildBossBefallReward:
		return _getDataString_GuildBossBefallReward(iDataStringType,iID,iIndex);

	case DataStringType_VipNew:
		return _getDataString_VipNewRewardByStage(iDataStringType,iID,iIndex);

	case DataStringType_VipNewTask:
		return _getDataString_VipNewTaskByStage(iDataStringType,iID,iIndex);

	case DataStringType_RecommendHeroByID:
		return _getDataString_RecommendHeroByID(iDataStringType,iID,iIndex);

	case DataStringType_ACTShopAAvatarInfo:
		return _getDataString_GetACTShopAAvatarInfo(iDataStringType,iID,iIndex);

	case DataStringType_AnswerActivityPointReward:
		return _getDataString_AnswerActivityPointReward(iDataStringType,iID,iIndex);

	case DataStringType_AnswerActivityRankRewards:
		return _getDataString_AnswerActivityRankRewards(iDataStringType,iID,iIndex);

	case DataStringType_AnswerActivityGuildRankRewards:
		return  _getDataString_AnswerActivityGuildRankRewards(iDataStringType,iID,iIndex) ;

	case DataStringType_YijituxiRankRewards:
		return _getDataString_YijituxiRankRewards(iDataStringType,iID,iIndex);

	case DataStringType_CommanderTitleRewards:
		return _getDataString_CommanderTitleRewardById(iDataStringType,iID,iIndex);

	case DataStringType_SpecialTrainingRewardInfo:
		return _getDataString_SpecialTrainingRewardById(iDataStringType,iID,iIndex);

	case DataStringType_CharTypePosition:
		return _getDataString_CharTypePositionInfo(iDataStringType,iID,iIndex);

	case DataStringType_PersonalHeadSetting:
		return _getDataString_PersonalHeadSettingInfo(iDataStringType,iID,iIndex);

	case DataStringType_GloryExpeditionReward:
		return _getDataString_GloryExpeditionReward(iDataStringType,iID,iIndex);

	case DataStringType_GloryExpeditionMissionInfo:
		return _getDataString_GloryExpeditionMissionInfo(iDataStringType,iID,iIndex);

	case DataStringType_RecommendRuneInfo:
		return _getDataString_RecommendRuneInfo(iDataStringType,iID,iIndex);

	case DataStringType_GloryExpeditionMissionIndex:
		return _getDataString_GloryExpeditionMissionIndex(iDataStringType,iID,iIndex);

	case DataStringType_GloryExpeditionRaidInfo:
		return _getDataString_GloryExpeditionRaidInfo(iDataStringType,iID,iIndex);

	case DataStringType_YijiXunbaoInfo:
		return _getDataString_YijiXunbaoInfo(iDataStringType,iID,iIndex);

	case DataStringType_RotaryTableReward:
		return _getDataString_GetRotaryReward(iDataStringType,iID,iIndex);

	case DataStringType_CombatCompareInfo:
		return _getDataString_CombatCompareInfo(iDataStringType,iID,iIndex);

	case DataStringType_NonpareilItemInfo:
		return _getDataString_NonpareilItemInfo(iDataStringType,iID,iIndex);

	case DataStringType_BestGuildRankReward:
		return _getDataString_BestGuildRankReward(iDataStringType,iID,iIndex);

	case DataStringType_BestGuildInfo:
		return _getDataString_BestGuildInfo(iDataStringType,iID,iIndex);

	case DataStringType_ToyIslandRewardInfo:
		return _getDataString_ToyIslandRewardInfo(iDataStringType,iID,iIndex);

	case DataStringType_EscortRewards:
		return _getDataString_ConvoyRewards(iDataStringType,iID,iIndex);

	case DataStringType_BestGuildGoalInfo:
		return _getDataString_BestGuildGoalInfo(iDataStringType,iID,iIndex);

	case DataStringType_EscortRewardsById:
		return _getDataString_ConvoyRewardsById(iDataStringType,iID,iIndex);

	case DataStringType_NewAchievementRewardInfo:
		return _getDataString_NewAchievementRewardInfo(iDataStringType,iID,iIndex);

	case DataStringType_EntrustTaskInfo:
		return _getDataString_EntrustTaskInfo(iDataStringType,iID,iIndex);

	case DataStringType_GuildBossActivityInfo:
		return _getDataString_GuildBossActivityInfo(iDataStringType,iID,iIndex);

	case DataStringType_GuildBossBefallInfo:
		return _getDataString_GuildBossBefallInfo(iDataStringType,iID,iIndex);


	default:
		return "";
	}
}
