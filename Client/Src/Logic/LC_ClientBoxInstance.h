#ifndef _LC_CLIENT_BOX_INSTANCE_H
#define _LC_CLIENT_BOX_INSTANCE_H

#include "MG_BotProtocol.h"
#include "CF_Bot.h"
#include "CF_BotUpgrade.h"


namespace GameLogic
{
	enum BotPromptType
	{
		BOT_PROMPTTYPE_CANNOT = 0,
		BOT_PROMPTTYPE_CANACTIVE = 1,
		BOT_PROMPTTYPE_CANLEVELUP = 2
	};

	struct BotReward
	{
		int32_t itemID;
		int32_t itemCount;

		BotReward(int32_t itemID, int32_t itemCount)
			: itemID(itemID)
			, itemCount(itemCount)
		{}

	};


	class LC_ClientBotInstance : public Memory::MM_BaseObject
	{
	public:
		LC_ClientBotInstance();
		virtual ~LC_ClientBotInstance();
		DECL_RTTI_NAME( LC_ClientBuddyInstance );
	public:
		void		Init();
		void		Reuse();
		void		ParseProtocol(const BotData_Base& BotData);
		void		GetLevelData();
		StringType	GetLevelUpData();
		BotPromptType GetPrompt(LC_ClientGamePlayer* pkGamePlayer);
	
		bool		IsAvtivce() { return bIsAvtive; }
		uint16_t  GetCID() { return cID; }
		uint16_t  GetLevel() { return iLevel; }
		uint32_t  GetEfficencyByIndex(int index);
		int		   GetEfficencyItemIDByIndex(int index);
		void  SetCID(int _cID) { cID = _cID; }
		bool		IsActiveTalent();
		StringType  GetBotInfo();
		StringType  ContructTarentInfo();
	private:
		uint16_t			cID;
		bool				bIsAvtive;
		uint16_t			iLevel;
		StringType			sUpLevelConsumption;
		uint32_t			iTalentReward;
		uint16_t			iTalentEfficiency;
		int32_t				nTimeStamp;

		CF_Bot::DataEntry*	m_pkCsvData;

		typedef mem::vector<BotReward> ResouseReward;
		mem::map<int, ResouseReward> m_kLevelResouse;
		
		mem::vector<BotReward> m_kActiveConsumption;
		mem::vector<BotReward> m_kUpLevelConsumption;
	};
}

#endif 