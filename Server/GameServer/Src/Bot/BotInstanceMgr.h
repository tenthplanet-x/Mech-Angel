#ifndef __BOT_INSTANCE_MGR_H__
#define __BOT_INSTANCE_MGR_H__

#pragma once

//#include "LC_DirtyFlag.h"
//#include "LC_Define.h"
#include "SystemError.h"

#include "char_data.pb.h"
#include "CF_Bot.h"

//using namespace CSVFile;
class LC_ServerPlayer;
namespace Bot
{
	class BotInstance;
	class BotInstanceMgr
	{
	public:
		BotInstanceMgr();
		~BotInstanceMgr();

		void ReUse();
		void Init(object_id_type nOwnerPlayerId);
		void Clear();

		int Activate();
		bool IsActivated();
		// GameLogic
		int ActivateBot(int32_t nBotResId, bool bForce = false);
		int UpgradeLevel(int32_t nBotResId, bool bForce = false);

		void ResetBeginTimes(uint32_t time);
		uint32_t GetBeginTime();

		uint32_t GetTimer_Normal(int index);
		void ResetTimer_Normal(int index, uint32_t time);

		void Enable(uint32_t time);

		int GetCount();
		BotInstance* GetBotInstance( int32_t nBotResId );
		object_id_type GetOwnerPlayerId();
		LC_ServerPlayer* GetOwnerPlayer();
		// DBLogic
		BotInstance* CreateBotInstance( CSVFile::CF_Bot::DataEntry* pResData );
		//void SaveAll(char_data_define::bot_detail_data & detail_data);
		//void LoadAll(const char_data_define::bot_detail_data & detail_data);
		void SyncAllBotData();
		void SyncBotBaseData();
		void SyncBotBonusData();
		const mem::map<int32_t, BotInstance*>& GetRawData();
		int Update( float fDelta, float fCurrent );
		int TryFetchBonus();
		int FetchNormalBonus( int nSeconds, int nType );
	private:
		void RecoverAllData();
		void ReUse_BotInstance();
		BotInstance* AddBot_Impl(CSVFile::CF_Bot::DataEntry* pResData);
		int RemoveBot( BotInstance* pInstance);
		int GenBotTypeData();
		int TryUpdate_Normal(  uint32_t nNow, uint32_t nMaxTimer, int nType );
		int TryUpdate_Talent(  uint32_t nNow, uint32_t nMaxTimer );
		int OnUpdate_Normal( int nCount, int nType );
		int OnUpdate_Talent( int nCount );
		int DoFetchBonus( LC_ServerPlayer* pOwner, ResultType& resType );
		int ActivateDefaultBot();
		void TryRecoverActivate();
		void InitNormalBonusTime( uint32_t nTime, bool bForce = false );
		void ResetBonusTimes( uint32_t nBeginTime );
		void DoResetBonusTimes( uint32_t nBeginTime, uint32_t& nOrigTime, uint32_t nTimer );
		int CheckUpdate( uint32_t nNow );
		int CheckUpdateBonus( uint32_t nBeginTime );
		int Update_Modified( float fDelta, float fCurrent );
		uint32_t GetNormalBonusTimerByType( int nType );
	private:
		void ModifyBonusCache_Simple( uint32_t nItemId, int32_t nCount );
		void ModifyBonusCache( uint32_t nItemId, int32_t nCount );
		void ModifyBonusCache_Talent( uint32_t nItemId, int32_t nCount );
		void ClearBonusCache();
		void SyncBonusCache();
		void SyncTimeStamp();
		int DoFetchBonus_Simple( LC_ServerPlayer* pOwner );
		int DoFetchBonus_Normal( LC_ServerPlayer* pOwner );
		int DoFetchBonus_Talent( LC_ServerPlayer* pOwner );
	private:
		object_id_type m_nOwnerPlayerId;
		mem::map<int32_t, BotInstance*> m_ppBotInstance;
		mem::map<uint32_t, uint32_t> m_bonusCache_Simple;
		mem::map<uint32_t, uint32_t> m_bonusCache;
		mem::map<uint32_t, uint32_t> m_bonusCache_Talent;
		uint32_t m_nBeginTime;//第一个挂机娃娃被激活的时间
		uint32_t m_nTimer_Normal[eNormalBonus_Count];
		uint32_t m_nTimer_Talent;
		uint32_t m_nLastBonusTimer;
		friend class BotInstance;
		friend class LC_ServerPlayer;
	};
}

using namespace Bot;

#endif //__ACT_ACTIVITY_ASSET_H__
