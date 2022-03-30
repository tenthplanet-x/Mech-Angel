#pragma once

#include "MG_CS_Guild.h"
#include "LC_GuildAsset.h"
#include "LC_GuildMilestoneStruct.h"
#include "LC_GuildSkillInfoStruct.h"

namespace GameLogic
{
	class LC_ClientGuildManager: public Utility::UT_Singleton<LC_ClientGuildManager>,
		public Memory::MM_BaseObject
	{
	public:
		enum
		{
			UP_PAGE = -1,
			FIRST_PAGE,
			DOWN_PAGE 
		};
		LC_ClientGuildManager();
		virtual ~LC_ClientGuildManager();
		DECL_RTTI_NAME( LC_ClientGuildManager );

		// 客户端请求获取门派信息
		ResultType RequestGuildAsset();

		// 客户端请求创建门派
		ResultType RequestCreateGuild(const StringType& strGuildName,const StringType& strGuildDeclare, int iCreatType);

		// 客户端请求加入门派
		ResultType RequestApplyGuild(guild_id_type lGuildID, int iGuildLevel);
		ResultType RequestApplyPlayerGuild(unique_id_type lPlayerID);
		ResultType RequestQuickApplyGuild();

		// 确认公会申请列表
		ResultType RequestComfirmApplyGuild(int iIndex, bool bAgree);
		// 确认公会全部批准申请列表入口
		ResultType RequestComfirmApplyAllGuildEntry(bool bAgree);
		// 确认公会全部批准申请列表循环
		ResultType RequestComfirmApplyAllGuild(int iIndex, bool bAgree);

		// 屏蔽申请
		ResultType RequestForbidApply();

		// 清空申请列表
		ResultType RequestClearApplyList();

		// 客户端请求离开门派
		void RequestExitGuild();

		// 客户端请求修改宣言
		ResultType RequestModifyDeclaration(StringType& strGuildDeclaration);

		// 客户端请求任命
		ResultType RequestAppoint(unique_id_type lTargetID, const StringType& strName, long lOfficialPostion);

		// 客户端请求开除成员
		ResultType RequestKickMember(unique_id_type lTargetID);
		
		// 客户端请求解散门派
		ResultType RequestDismissGuild();

		// 客户端请求申请列表
		//ResultType RequestGuildInviteList();

		// 客户端请求邀请玩家加入门派
		ResultType RequestInviteToGuild(unique_id_type lTargetID);

		// 客户端确认门派邀请信息
		ResultType RequestConfirmInviteToGuild(guild_id_type lGuildID, unique_id_type lInvitePlayer, bool bAgree);	
		// 客户端请求升级公会
		ResultType RequestUpgradeGuild(void);
		//申请自动批准加入
		ResultType RequestAutoAddApplyList(bool bCheck, guild_id_type guildId);

		//请求招募公会成员
		void RequestRecruitGuildMember();

		//请求领取每日福利
		void RequestGetGuildReward();

		//请求查询公会排名
		void RequestGuildRankNumber();


	public:
		// 获取相应大事记信息
		LC_ClientGuildMilestoneStruct* GetGuildMilestone();		
	
	public:
		// 获取门派技能信息
		LC_ClientGuildSkillInfoStruct* GetGuildSkillInfo();

		// 请求门派技能信息
		//ResultType RequestGuildSkill();

		// 用材料研究某个技能
		ResultType RequestDonateMaterial(int64_t lMaterialNum, bool bMoney);
		
		// 学习门派技能
		ResultType RequestLearnGuildSkill();

		// 是否可以学习门派技能
		bool		CheckLearnGuildSkill(int iSkillIndex);

		// 获取搜索门派信息
		LC_GuildSearchInfo* GetGuildSearchInfo(){ return &m_kGuildSearchInfo;}
		
		// 请求搜索门派信息
		ResultType RequestSearchGuild(int iIndex, int eSortType, int eGuildSortCol, bool bAutoAdd, int iNumPerPage);
		// 
		void	   RequestUnionInfo();

		//void	   SetREQSearchFlag(bool bFlag){ m_bREQSearchGuildInfo = bFlag;}
		void	   SetReqUnionInfo(bool bFlag)  { m_bReqUnionInfo = bFlag; }
		// 选中技能编号
		void SetSelectedSkillIndex(int iSkillIndex);
		// 添加捐献材料
		ResultType AddDonateMaterial(int iPackType, int iIndex);
		// 清空所捐献材料
		void ClearDonateMaterial();
		// 获取所捐献材料
		long GetDonateMaterialID();
		void SetDonateMaterailID(int iID);
		void ClearPage(){m_iPage = 0;};
		void Reset();

	public:
		void UpdateGuildInfo(const Protocol::PS_GuildInfo& kGuildInfo);
		void UpdateGuildRankInfo(uint32_t guildRank);
		void UpdateAllGuildMemberInfo(const Protocol::PS_GuildAllMemberInfo& kGuildMemberInfo);
		void UpdateGuildMemberArenaRankInfo(std::vector<int32_t> &viRank);
		void AddGuildMemberInfo(const Protocol::PS_GuildMemberInfo &kGuildMemberInfo);
		void DelGuildMemberInfo(unique_id_type iUserID);
		void UpdateGuildMemberInfo(const Protocol::PS_GuildMemberInfo &kGuildMemberInfo);
		void UpdateDeclaration(const StringType& strDeclaration);
		void UpdatePosition(const Protocol::PS_GuildPosition &kGuildPosition);
		//void UpdateGuildSkillInfo(const Protocol::PS_GuildSkillInfo& kGuildSkillInfo);
		void UpdateSearchGuildInfo(const Protocol::PS_GuildSearchInfo& kGuildSearchInfo,uint32_t uiIndex);
		void UpdateGuildApplyList(const Protocol::PS_GuildAllApplyInfo& kAllApplyInfo);
		void AddGuildApply(const Protocol::PS_GuildApplyInfo &kApplyInfo);
		void DelGuildApply(unique_id_type iUserID);

		StringType GetGuildLevelTooltip(int iLevel);

	public:
		LC_GuildAsset*			GetGuildAsset();
		LC_GuildMemberEntry*	GetPersonalInfo();

	public:
		//int GetApplyComfirmNum(){return m_iApplyComfirmNum;}
		//void SetApplyComfirmNum(int num){ m_iApplyComfirmNum = num;}
		int GetFirstValidApplyPlayerIndex();
		int GetGuildMaxMember(int level);
	protected:
		void _updateGuildMilestone(int32_t iMilestone);

	protected:
		LC_GuildAsset					m_kGuildAsset;
		LC_ClientGuildMilestoneStruct	m_kGuildMilestone;
		LC_ClientGuildSkillInfoStruct	m_kGuildSkillInfo;
		LC_GuildSearchInfo				m_kGuildSearchInfo;
			
		int								m_iSelectedSkillIndex;		// 选中技能ID
		long							m_lDonateMaterialID;		// 捐献材料ID

		//bool							m_bREQSearchGuildInfo;
		bool							m_bReqUnionInfo;

		uint32_t						m_iPage;

		//int								m_iApplyComfirmNum;
		unique_id_type					applyPlayerId[MAX_GUILD_APPLY_NUM];
	};
};