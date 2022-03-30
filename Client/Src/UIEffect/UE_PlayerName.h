//描述玩家名字

#pragma once

#include "UE_UiEffectBase.h"

class LC_ClientGamePlayer;
class LC_ClientWorldManager;
class LC_ClientLogicManager;

namespace GameLogic
{
	class LC_ActorBase;
	class LC_PlayerBase;
}
class UE_PlayerName : public UE_UiEffectBase					
{
public:	
	UE_PlayerName(UI_EFFECT_TYPE eType);
	~UE_PlayerName();

	virtual bool Init();
	virtual void SetPendingParam(long Param1 , long Param2,  float fParam3 = 0.0f  );	

	void CalCharName( LC_ClientGamePlayer* pkGamePlayer, LC_ClientWorldManager* pkWorldManager, LC_ClientLogicManager* pkLogicManager, UT_Vec3Float& kNameColor);

	virtual void Show(float lifeTime = 0.0f);
	virtual void Hide();
	virtual bool Update(float fCurrentTime);
	void		 ChangePlayerTitle();
	void		 ChangeNameFrameState(int iState);
	void		 SetInteractiveBtnVisible(bool bVisible);
	virtual void RequestRedraw();
	UT_Vec3Float	ComputeActorNameColor(LC_ClientGamePlayer* pkWatcher, LC_ActorBase* pkTarget, int iPowerGuildID = -1, LC_PlayerBase* pkPlayer = NULL, bool isInTeam = false);
	void		 ResetTime(bool kFlag = false)
	{
		if ( kFlag ) 
		{
			m_fPlaceHolderCurLifeTime = -1.0f;
		}
		else
		{
			if ( m_placeHolderTime > 0.0f )
			{
				m_fPlaceHolderCurLifeTime = GET_PROCESS_TIME + m_placeHolderTime;
			}
			else
			{
				m_fPlaceHolderCurLifeTime = GET_PROCESS_TIME + g_settings.UE_PLACEHOLDER_NAME_LIFE_TIME;
			}
		}
	}
	void		SetPlaceHoldeTime(float lifeTime)
	{
		m_placeHolderTime = lifeTime;
	}

	void		 RedrawPlaceHolderNameRect();
	bool		 GetLocalNameRect(long lTargetID, GUISys::GUIRect& kRetNameRect);
	void		 SetNameRect(const GUISys::GUIRect& kNameRect);

	void		 SetMouseType(bool bValue){m_bMouseType = bValue;}
	bool		 GetMouseType(){return m_bMouseType;}

	UT_Vec3Int	 GetScreentPos();
	int			 GetFlashIndex(float fTime,int iCount,int iInterval);
	int			 GetGuildFlashIndex(float fTime,int iCount,int iInterval);
	enum NAMEFRAMESTATE
	{
		NFS_NORMAL	= 1,
		NFS_HOVER	= 2,
	};
	enum ANNOTATION_BIT
	{
		ATBIT_NONE					= 0,
		ATBIT_GROUP					= 0,
		ATBIT_TEAM_LEAD				= 1,
		ATBIT_HOLY_FLAM_HOLDER		= 2,
		ATBIT_VIP					= 3,
		ATBIT_VORPAL_BLADE_HOLDER	= 4,
		ATBIT_COOLDOWN				= 5,
		ATBIT_DOMAIN_HOLDER			= 6,
		ATBIT_STARS					= 7,
		ATBIT_TITLE					= 8,
		ATBIT_OTHER_GROUP			= 9,
		ATBIT_VILLAIN				= 10,
		ATBIT_ENEMY					= 11,
		ATBIT_PLATFORM_VIP			= 12,
		ATBIT_PLATFORM_VIP_YEAR		= 13,
		ATBIT_PLATFORM_VIP_HIGH		= 14,
		ATBIT_GUILD_WAR_TITLE_INFO	= 15,
		ATBIT_GM_TITLE_INFO			= 16,
		ATBIT_DAILY_ACTIVITY_INFO	= 17,
		ATBIT_TOYPET	= 18,
		
		ATBIT_MAX,
	};
	enum ANNOTATION
	{
		AT_NONE					= 0,
		AT_GROUP				= 1 << ATBIT_GROUP,
		AT_TEAM_LEAD			= 1 << ATBIT_TEAM_LEAD,
		AT_HOLY_FLAM_HOLDER		= 1 << ATBIT_HOLY_FLAM_HOLDER,	//圣火令持有者
		AT_VIP					= 1 << ATBIT_VIP,
		AT_VORPAL_BLADE_HOLDER	= 1 << ATBIT_VORPAL_BLADE_HOLDER,	//屠龙刀
		AT_COOLDOWN				= 1 << ATBIT_COOLDOWN,	//倒计时
		AT_DOMAIN_HOLDER		= 1 << ATBIT_DOMAIN_HOLDER,	//势力争夺战旗帜
		AT_STARS				= 1 << ATBIT_STARS,	//星星
		AT_TITLE				= 1 << ATBIT_TITLE,	//称号
		AT_OTHER_GROUP			= 1 << ATBIT_OTHER_GROUP,	//其他队伍
		AT_VILLAIN				= 1 << ATBIT_VILLAIN,	//大恶人
		AT_ENEMY				= 1 << ATBIT_ENEMY,		//仇人
		AT_PLATFORM_VIP			= 1 << ATBIT_PLATFORM_VIP,	//黄钻
		AT_PLATFORM_VIP_YEAR	= 1 << ATBIT_PLATFORM_VIP_YEAR,	//黄钻年卡
		AT_PLATFORM_VIP_HIGH	= 1 << ATBIT_PLATFORM_VIP_HIGH,	//豪华帮黄钻
		AT_GUILD_WAR_TITLE_INFO	= 1 << ATBIT_GUILD_WAR_TITLE_INFO,	//帮会头衔相关
		AT_GM_TITLE_INFO		= 1 << ATBIT_GM_TITLE_INFO,	//GM指令抬头
		AT_DAILY_ACTIVITY_INFO	= 1 << ATBIT_DAILY_ACTIVITY_INFO,	//头衔
		AT_TOYPET			= 1 << ATBIT_TOYPET, // 玩具
	};
	enum GUILDWARTITLE_INFO
	{
		GWTI_NONE					= 0,//
		GWTI_GUILD_WAR_TITLE		= 1,//武林盟主
		GWTI_GLOBAL_GUILD_WAR_TITLE	= 2,//武林至尊
	};

private:
	UT_Vec3Float _computeActorNameColor(GameLogic::LC_PlayerBase* pkWatcher, GameLogic::LC_ActorBase* pkTarget, GameLogic::LC_PlayerBase* pkPlayer, bool isInTeam = false);
	StringType					m_kDebugName;
	NiColor						m_kDebugColor;
	StringType					m_kTitleName;
	NiColor						m_kTitleColor;
	StringType					m_kCharName;
	NiColor						m_kCharColor;
	StringType					m_kGuildName;
	NiColor						m_kGuildColor;
	NiPoint3					m_kPos;
	NiPoint3                    m_kScreenPos;
	GameLogic::LC_ActorBase*	m_pkActor;
	GameLogic::LC_PlayerBase*	m_pkPlayer;
	bool                        m_bCanAttack;
	bool                        m_bShowTitle;
	float                       m_fRichTextOffset;
	float						m_placeHolderTime;
	float						m_fPlaceHolderCurLifeTime;
	float						m_fLastTime;
	int							m_kNameTop;
	
	int                         m_nAnnotationMask;
	int							m_nNameFrameState;
	int							m_nTitleID;
	std::vector<int>			m_nParamVec;
	bool						m_bShowInteractiveBtn;

	GUISys::RS_GeometryBuffer*  m_pkFontBuffer;
	GUISys::RS_GeometryBuffer*  m_pkAnnotateBuffer;
	GUISys::RS_GeometryBuffer*  m_pkTitleFlashBuffer;
	bool						m_bNeedRedraw;

	bool						m_bMouseType;

	bool						m_bShowOtherTitle;
	int							m_iLastFlashPic;
	int							m_iLastFlashPicTime;
	GUISys::GUIRect				m_kTitleFlashRect;

	int							m_iGuildLastFlashPic;
	int							m_iGuildLastFlashPicTime;
	GUISys::GUIRect				m_kGuildTitleFlashRect;
};