#ifndef _LC_CLIENT_BOX_MANAGER_H
#define _LC_CLIENT_BOX_MANAGER_H

#include "LC_ClientBoxInstance.h"
#include "MG_BotProtocol.h"

namespace GameLogic 
{

	typedef mem::map<int, LC_ClientBotInstance*> BotDataMap;
	typedef mem::map<int, LC_ClientBotInstance*>::iterator BotDataMapIter;

	struct BotProfit 
	{
		uint32_t itemID;
		uint32_t itemCount;

		BotProfit(uint32_t itemID, uint32_t itemCount)
			: itemID(itemID)
			, itemCount(itemCount)
		{}

	};

	class LC_ClientBotManager : public Utility::UT_Singleton<LC_ClientBotManager>,
		public LC_DirtyFlagAble
	{
	public:
		LC_ClientBotManager();
		virtual ~LC_ClientBotManager();
		DECL_RTTI_NAME( LC_ClientBotManager );

		void		init();
		void		Reset();
		void		ParseProtocol(const MG_SC_BotDataSync& rltMsg);
		void		GetBeginFromProtocol(const MG_SC_BotDataSync_BeginTime& rltMsg);
		void		ParseProfitDataFromProtocol(const MG_SC_BotBonusCacheSync& rltMsg);
		void		ParseVipNewProfitDataFromProtocol(const MG_SC_NewVIPBonusCacheSync& rltMsg);
		void		GetDataFromProtocol(const BotData_Base& rltMsg);
		void		ConstructBenefitData();
		StringType  GetBotInfoByCID(int cID);
		StringType  GetLevelUpBotInfoByCID(int cID);
		StringType  GetAllBotInfo();
		StringType  GetProfitInfo();
		StringType  GetTarentInfo();
		BotPromptType GetPrompt(LC_ClientGamePlayer* pkGamePlayer);		//判断有没有娃娃可以升级或激活
		int			GetTotalCount() { return m_kBotDataMap.size(); }
		int			GetActiveCount();
		
		int			GetTotalRewardByIndex(int index);
		int			GetRewardItemIDByIndex(int index);
		int32_t		GetBeginTime() { return m_nBeginTime; }
		
		int32_t		GetTotalReward1() { return m_iTotalReward1; }
		int32_t		GetTotalReward2() { return m_iTotalReward2; }
		int32_t		GetTotalReward3() { return m_iTotalReward3; }
		int32_t		GetTotalReward4() { return m_iTotalReward4; }
		int32_t		GetTotalReward5() { return m_iTotalReward5; }
		int32_t		GetTotalReward6() { return m_iTotalReward6; }

		int           GetDirtyFlagRevisionID() { return LC_DirtyFlagAble::GetDirtyFlagRevisionID();}
	protected:
		BotDataMap                m_kBotDataMap;
		mem::vector<BotProfit>						m_kBotProfitData;
		int32_t                         m_nBeginTime;
		int32_t	                        m_iTotalReward1;
		int32_t	                        m_iTotalReward2;
		int32_t	                        m_iTotalReward3;
		int32_t	                        m_iTotalReward4;
		int32_t	                        m_iTotalReward5;
		int32_t	                        m_iTotalReward6;
		
		mem::vector<BotProfit>			m_kVipNewProfitData;


	};

}



#endif