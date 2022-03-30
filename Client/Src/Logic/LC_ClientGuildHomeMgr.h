#ifndef _LC_CLIENT_GUILD_HOME_MGR_H_
#define _LC_CLIENT_GUILD_HOME_MGR_H_

#include "LC_ItemBase.h"
#include "MG_CS_QuanThanhTemple.h"

namespace	GameLogic
{
	
	class GuildWareHousePack
	{
	public:
		GuildWareHousePack();
		DECL_RTTI_NAME( GuildWareHousePack);
		LC_Money			m_kUnBindCash;
		int					m_iSize;
		LC_BackPackEntry	m_aData[GUILDWAREHOUSE_PACK_ROW_NUM*GUILDWAREHOUSE_PACK_COL_NUM];

		
		int GetPackCapacity(){ return GUILDWAREHOUSE_PACK_ROW_NUM*GUILDWAREHOUSE_PACK_COL_NUM; }
		int GetPackSize(){return m_iSize;};
		LC_BackPackEntry* GetBackPackEntry(int iEntryIndex);

		bool IsExistItem(long lItemTypeID) ;
		void Reset();

		void GetFromProtocol(const Protocol::PS_WarehousePack& kPack);
		void SetToProtocol(Protocol::PS_WarehousePack& kPack);
	};

	class LC_ClientGuildHomeMgr : public Utility::UT_Singleton<LC_ClientGuildHomeMgr>,
		public Memory::MM_BaseObject
	{
	public:
		
		enum Flag
		{
			 GUILDHOME_WAREHOUSE,
			 GUILDHOME_KILLTHIEF_COUNT,
			 GUILDHOME_KILLTHIEF_STORY,
			 GUILDHOME_KILLTHIEF_RECORDTIME,
			 FLAG_MAX_NUM
		};

		enum ThiefCount
		{
			FLYTHIEF,
			FIRETHIEF,
			KILLEDTHIEF,
			MAX_THIEF
		};
		
		LC_ClientGuildHomeMgr();
		~LC_ClientGuildHomeMgr();
		DECL_RTTI_NAME( LC_ClientGuildHomeMgr);
		//门派家园
		//Req
		void	RequestUpgradeNPC( uint32_t lNPCID );//请求升级NPC
		void	RequestPutIntoGuildWarehouse(int iPackType,int iEntryIndex,int iWareHouseIndex);
		void	RequestGetGuildWareHouseInfo(int iGuildID);
		void	RequestGetPrizeOfKillThief();
		void	RequestOpenKillThief();
		void	RequestGetKillThiefInfo();
		//Recv
		void	RecvEnterGuildHome();
		void	RecvUpgradeNPC();
		
		void	AddDirtyFlagRevisionID(long lRevisionID);
		long	GetDirtyFlagRevisionID(long lRevisionID);
		void	ClearDirtyFlag();
		void	GetGuildWareHouseFromProtocol(const Protocol::MG_RLT_GUILDWAREHOUSE& rltMsg);
		void	GetKillThiefStoryCount(const Protocol::MG_RLT_CHANGE_THIEF_RECORD& rltMsg);
		GuildWareHousePack*	GetGuildWareHousePack(){return &m_kGuildWareHouse;};
		long	GetThiefCount(int iThiefType);
		bool	GetIsInThief();
		void	SetIsInThief(bool val);

		float	GetRecordTime(){ return m_fRecordTime; }
		void	SetRecordTime(float val) { m_fRecordTime = val; }
		int		GetGuildContribute(){ return m_iGuildContribute; }
		int64_t	GetEXP(){ return m_iEXP; }
	private:
		void	_getGuildWareHousePackFromProtocol( const Protocol::PS_WarehousePack& kPack );

		long				m_DirtyFlag[FLAG_MAX_NUM];
		GuildWareHousePack	m_kGuildWareHouse;
		uint32_t			m_uiGuildWareHouseVersion[MAX_WAREHOUSE_PACK_COUNT];
		int					m_iThiefCount[MAX_THIEF];
		bool				m_bIsInThief;
		float				m_fRecordTime;
		int64_t			m_iEXP;
		int					m_iGuildContribute;
		uint32_t			m_uiQueryKillThiefTime;
	};
};

#endif