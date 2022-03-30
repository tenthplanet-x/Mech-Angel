#pragma once

#include "MG_TYPES.h"
#include "MG_CS_Alliance.h"
#include "MG_Game2Guild.h"

using namespace GameLogic;

namespace GameLogic 
{
	class LC_ClientAlliance : public Memory::MM_BaseObject
	{
	public:
		LC_ClientAlliance();
		virtual ~LC_ClientAlliance();
		DECL_RTTI_NAME(LC_ClientAlliance)

		void ReUse();

		int GetCount();
		StringType GetServerName(int iIndex);
		int GetServerID(int iIndex);

		StringType GetGuildName(int32_t serverId);
		StringType GetGuildLeader(int32_t serverId);
		StringType GetWinGuildName();

		int GetBestGuildID() { return m_iGainServerID; };

		//请求边境地图信息
		void ReqAllianceInfo();
		//请求联盟信息
		void ReqAllianceUnitInfo();
		void ReqUnitReward();
		void RltUnitReward(uint16_t m_type, const std::map<uint32_t,uint64_t>& rwd_map);
		//请求领取奖励(王旗)
		void ReqDrawUnitRwd(int iType);
		void RltEnterAlliance(int retType);
		bool IsCanDrawWQRwd();
		//请求进入某一个边境地图
		void ReqEnterAlliance(int serverID , int transferAreaID);
		void RltAllianceInfo(int groupID, Protocol::UnitServerNodeList& li);
		void RltAllianceUnitInfo(Protocol::MG_Resp_Alliance_Unit_Info &msg);
		//请求边境屠龙刀信息
		void ReqAllianceWinInfo();
		void RltAllianceWinInfo(Protocol::MG_SS_GUILD_WAR_WIN_INFO& msg);

		//通天战场相关
		bool HasFactionReward();

		class ServerNode 
		{
		public:
			int serverID;
			StringType serverName;
		};
		typedef std::vector<ServerNode> CCServerNodeList;

		typedef std::map<int32_t,ST_GUILD_WIN_Node> GuildWinfoList;

	private:

		int m_groupID;
		CCServerNodeList m_serverList;

		GuildWinfoList m_GuildInfoList;
		ST_GUILD_WIN_Node m_winInfo;

		uint64_t m_iWQGainTime;
		uint32_t m_iGainServerID;

		//通天战场全服buff奖励
		uint64_t  m_iFactionWinTime;
		uint32_t  m_iFactionDefServer;
		int32_t   m_iFactionDefStatus;

		std::map<uint32_t,uint64_t> m_UnitRwdMap;
	};
}