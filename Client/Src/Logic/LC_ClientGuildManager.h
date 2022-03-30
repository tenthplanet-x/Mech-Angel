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

		// �ͻ��������ȡ������Ϣ
		ResultType RequestGuildAsset();

		// �ͻ������󴴽�����
		ResultType RequestCreateGuild(const StringType& strGuildName,const StringType& strGuildDeclare, int iCreatType);

		// �ͻ��������������
		ResultType RequestApplyGuild(guild_id_type lGuildID, int iGuildLevel);
		ResultType RequestApplyPlayerGuild(unique_id_type lPlayerID);
		ResultType RequestQuickApplyGuild();

		// ȷ�Ϲ��������б�
		ResultType RequestComfirmApplyGuild(int iIndex, bool bAgree);
		// ȷ�Ϲ���ȫ����׼�����б����
		ResultType RequestComfirmApplyAllGuildEntry(bool bAgree);
		// ȷ�Ϲ���ȫ����׼�����б�ѭ��
		ResultType RequestComfirmApplyAllGuild(int iIndex, bool bAgree);

		// ��������
		ResultType RequestForbidApply();

		// ��������б�
		ResultType RequestClearApplyList();

		// �ͻ��������뿪����
		void RequestExitGuild();

		// �ͻ��������޸�����
		ResultType RequestModifyDeclaration(StringType& strGuildDeclaration);

		// �ͻ�����������
		ResultType RequestAppoint(unique_id_type lTargetID, const StringType& strName, long lOfficialPostion);

		// �ͻ������󿪳���Ա
		ResultType RequestKickMember(unique_id_type lTargetID);
		
		// �ͻ��������ɢ����
		ResultType RequestDismissGuild();

		// �ͻ������������б�
		//ResultType RequestGuildInviteList();

		// �ͻ�������������Ҽ�������
		ResultType RequestInviteToGuild(unique_id_type lTargetID);

		// �ͻ���ȷ������������Ϣ
		ResultType RequestConfirmInviteToGuild(guild_id_type lGuildID, unique_id_type lInvitePlayer, bool bAgree);	
		// �ͻ���������������
		ResultType RequestUpgradeGuild(void);
		//�����Զ���׼����
		ResultType RequestAutoAddApplyList(bool bCheck, guild_id_type guildId);

		//������ļ�����Ա
		void RequestRecruitGuildMember();

		//������ȡÿ�ո���
		void RequestGetGuildReward();

		//�����ѯ��������
		void RequestGuildRankNumber();


	public:
		// ��ȡ��Ӧ���¼���Ϣ
		LC_ClientGuildMilestoneStruct* GetGuildMilestone();		
	
	public:
		// ��ȡ���ɼ�����Ϣ
		LC_ClientGuildSkillInfoStruct* GetGuildSkillInfo();

		// �������ɼ�����Ϣ
		//ResultType RequestGuildSkill();

		// �ò����о�ĳ������
		ResultType RequestDonateMaterial(int64_t lMaterialNum, bool bMoney);
		
		// ѧϰ���ɼ���
		ResultType RequestLearnGuildSkill();

		// �Ƿ����ѧϰ���ɼ���
		bool		CheckLearnGuildSkill(int iSkillIndex);

		// ��ȡ����������Ϣ
		LC_GuildSearchInfo* GetGuildSearchInfo(){ return &m_kGuildSearchInfo;}
		
		// ��������������Ϣ
		ResultType RequestSearchGuild(int iIndex, int eSortType, int eGuildSortCol, bool bAutoAdd, int iNumPerPage);
		// 
		void	   RequestUnionInfo();

		//void	   SetREQSearchFlag(bool bFlag){ m_bREQSearchGuildInfo = bFlag;}
		void	   SetReqUnionInfo(bool bFlag)  { m_bReqUnionInfo = bFlag; }
		// ѡ�м��ܱ��
		void SetSelectedSkillIndex(int iSkillIndex);
		// ��Ӿ��ײ���
		ResultType AddDonateMaterial(int iPackType, int iIndex);
		// ��������ײ���
		void ClearDonateMaterial();
		// ��ȡ�����ײ���
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
			
		int								m_iSelectedSkillIndex;		// ѡ�м���ID
		long							m_lDonateMaterialID;		// ���ײ���ID

		//bool							m_bREQSearchGuildInfo;
		bool							m_bReqUnionInfo;

		uint32_t						m_iPage;

		//int								m_iApplyComfirmNum;
		unique_id_type					applyPlayerId[MAX_GUILD_APPLY_NUM];
	};
};