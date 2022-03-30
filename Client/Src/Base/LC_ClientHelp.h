#pragma once;
#include "LC_Helper.h"
#include "LC_HelperDiff.h"

namespace GameLogic
{
	class LC_ClientHelp : public LC_Helper
	{
	public:
		static int		CanEnterMap(int iMapResID, LC_ClientGamePlayer* pkPlayer, int& iRetValue);
		static int		GetDailyCleanKeyID(uint32_t nType, uint32_t nID) { return nType * 1000 + nID; };
		static bool		CheckCronTimeStamp(const StringType& cronStr, int timeLen, uint32_t& uiBeginTime, uint32_t& uiEndTime);

		//获取以武器类型为前缀的资源名
		static StringType	GetWeaponedResName(GameLogic::WeaponType eWeaponType, const char * pResName, bool bDefaultAsHand = false);

		static std::string FilterGBKPlayerName(const std::string& inputStr);

		static bool IsCrossServerPlayer(LC_PlayerBase* pkPlayer);	// 该玩家是否在跨服
	};


	class LC_ClientHelpDiff : public GameLogic::LC_HelperDiff
	{
	public:
		virtual bool CrossSafe(int32_t iIndex, int32_t sServerID);

		virtual float CalMasterBuffState(int32_t serverID , int64_t iScore, int32_t worldLevel);

		virtual bool IsCrossPlayer(LC_PlayerBase* pkPlayer);
	};
}