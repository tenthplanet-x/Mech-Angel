#ifndef __BOT_INSTANCE_H__
#define __BOT_INSTANCE_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"
#include "CF_SystemTitleList.h"
#include "CF_LevelUp.h"
#include "CF_Bot.h"
#include "CF_BotUpgrade.h"

#include "char_data.pb.h"

#define			DEFAULT_BOT_LEVEL		1

class LC_ServerPlayer;

//namespace GameLogic
namespace Bot
{
class BotInstanceMgr;
class BotInstance : public Memory::MM_BaseObject
{
public:
	BotInstance(object_id_type nOwnerPlayerId, BotInstanceMgr* pMgr);
	virtual ~BotInstance();
public:
	void InitTemplateData(const CSVFile::CF_Bot::DataEntry* pData, bool bInit);
	void Clear(void){}
	void InitGameData();

	LC_ServerPlayer* GetOwnerPlayer();

	void SyncAllDataToClient();

	int GetQuality();
	void OnCreate();
	// Data fetch
	int32_t GetTimeStamp();
	uint32_t GetTemplateId();
	int16_t GetLevel();
	CSVFile::CF_Bot::DataEntry* GetConfig();
	int UpgradeLevel();
private:
	void OnActivate();
	// Create logic
	void OnLoadSkillData();
	void Save(char_data_define::bot_single_instance_data& data);
	void Load(const char_data_define::bot_single_instance_data& data);
private:
	// 因为所有的check and reducing都在Manager做了，那边有完整的上下文，所以，这里的UpgradeStar仅仅就是个"Do"
	int DoUpgrade();
	int DoNormalReward( LC_ServerPlayer* pOwner,  BotInstanceMgr* pMgr, int nCount, bool bSimple = false, int nType = -1 );
	int DoTalentReward( LC_ServerPlayer* pOwner, BotInstanceMgr* pMgr, int nCount );
	int GetNormalRewardByType( int32_t nType, uint32_t& nItemId, uint32_t& nCount, CSVFile::CF_BotUpgrade::DataEntry* pCfg  );
private:
	void OnLevelData( int nOrigLevel, int nLevel, bool bInit);
private:
	object_id_type m_nOwnerPlayerId;
	CSVFile::CF_Bot::DataEntry* m_pConfig;
	uint32_t m_nTimeStamp;
	int32_t m_nLevel;
	BotInstanceMgr* m_pInstanceMgr;
	friend class BotInstanceMgr;
};
}

using namespace Bot;

#endif
