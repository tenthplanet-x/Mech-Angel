
#include "CF_TranscriptionRule.h"
#include "LC_ServerPlayer.h"
#include "LC_PlayerGroupManagerBase.h"
#include "LC_ServerMapLogic_TranscriptionRuleManger.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerGuildManager.h"
#include "CF_WorldMapList.h"
using namespace GameLogic;

LC_ServerMapLogic_TranscriptionRuleManger::LC_ServerMapLogic_TranscriptionRuleManger()
{}

LC_ServerMapLogic_TranscriptionRuleManger::~LC_ServerMapLogic_TranscriptionRuleManger()
{}

CF_TranscriptionRule::DataEntry* LC_ServerMapLogic_TranscriptionRuleManger::GetRule(int resId)
{
	CF_TranscriptionRule* TranscriptionRulePtr=CF_TranscriptionRule::GetSingletonPtr();
	CF_TranscriptionRule::DataEntry* pkEntry=TranscriptionRulePtr->GetEntryPtr(resId);
	return pkEntry;
}

ResultType LC_ServerMapLogic_TranscriptionRuleManger::CheckEnter(GameLogic::LC_PlayerBase* pkPlayer,CF_TranscriptionRule::DataEntry* rule)
{
	if(rule==NULL)
		return RE_FAIL;
	LC_ServerPlayer* pkServerPlayer = static_cast<LC_ServerPlayer*>(pkPlayer);
	if (pkServerPlayer==NULL)
	{
		return RE_FAIL;
	}

	pkServerPlayer = pkServerPlayer->GetOwnerPlayer();
	if (NULL == pkServerPlayer)
	{
		return RE_FAIL;
	}

	if(!pkServerPlayer->GetReliveBeforeEnterMapTag() && pkServerPlayer->IsClientLoading())
	{
		return RE_SOMEONE_IS_LOADING_PLEASE_WAIT;
	}
	//组队限制目前不考虑
	int level = pkServerPlayer->GetLevel();
	if( rule->_iPlayerMaxLevel>0 && rule->_iPlayerMaxLevel < level)
	{
		return RE_TRANSCRIPTION_LEVEL_TO_HIGH;
	}
	else if(rule->_iPlayerMinLevel >0 && rule->_iPlayerMinLevel > level)
	{
		return RE_TRANSCRIPTION_LEVEL_TO_LOW;
	}

	//指挥官头衔
	int commanderTitle = pkServerPlayer->GetCommanderTitle();
	if(0!=rule->_iCommanderTitleLevel && commanderTitle < rule->_iCommanderTitleLevel)
	{
		return RE__TRANSCRIPTION_COMMANDER_TITLE_LIMITED;
	}

	//前置副本
	if(!rule->_kPreResIDs.empty())
	{
		LC_RaidAssert* pkRaidAsset = pkServerPlayer->GetRaidAsset();
		
		Utility::UT_SIMDataList::iterator it = rule->_kPreResIDs.begin();
		for (; it!=rule->_kPreResIDs.end(); ++it)
		{
			if(!pkRaidAsset->FindRaid((int32_t)it->ID()))
			{
				return RE_TRANSCRIPTION_PRE_NOT_COMPLETE;
			}
		}
	}
	//体力
	if(rule->_iStamina>pkServerPlayer->GetVigor())
	{
		return RE_TRANSCRIPTION_VIGOR_NOT_ENOUGH;
	}
	//战斗力
	if(pkServerPlayer->GetControllerCombatScore() < rule->_CombatScore)
	{
		return RE_TRANSCRIPTION_SCORE_NOT_ENOUGH;
	}
	//公会限制  屠龙刀刚过来的时候没有工会信息
	
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	
	CrossRealmType iCrossType = CrossRealmType_Team;
	bool bGuildCross = false;
	if(enterLeaveManager->CheckCrossRealmType(pkServerPlayer->GetID(), iCrossType))
	{
		bGuildCross = (iCrossType==CrossRealmType_GuildWar || iCrossType==CrossRealmType_GuildTransport);
	}
		
	uint64_t guildID = pkServerPlayer->GetGuildID();
	if(!bGuildCross && rule->_GuildID > 0 && guildID==INVALID_GUILD_ID )
	{
		return RE_TRANSCRIPTION_NO_GUILD_ID;
	}

	if(rule->_GuildID > 0)
	{
		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(guildID);
		if(pkGuildInfo == NULL)
		{
			return RE_ENTER_ERROR_NOT_GUILD;
		}

		if(pkGuildInfo->GetLevel() < rule->_GuildID)
		{
			return RE_GUILD_LEVEL_NOTENOUGH;
		}
	}
	
	if (!rule->_kConsumeItems.empty())
	{
		if (RE_SUCCESS!=LC_Helper::IsItemsEnough(pkServerPlayer->GetPackAsset(), rule->_kConsumeItems))
		{
			return RE_TRANSCRIPTION_ITEM_LACK;
		}
	}
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_TranscriptionRuleManger::CheckEnter(object_id_type playerID,int level,CF_TranscriptionRule::DataEntry* rule)
{
	if(rule==NULL)
		return RE_FAIL;
	if( rule->_iPlayerMaxLevel>0 && rule->_iPlayerMaxLevel < level)
	{
		return RE_TRANSCRIPTION_LEVEL_TO_HIGH;
	}
	else if(rule->_iPlayerMinLevel >0 && rule->_iPlayerMinLevel > level)
	{
		return RE_TRANSCRIPTION_LEVEL_TO_LOW;
	}
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_TranscriptionRuleManger::CheckLeave(GameLogic::LC_PlayerBase* pkPlayer,CF_TranscriptionRule::DataEntry* rule)
{
	return RE_SUCCESS;
}