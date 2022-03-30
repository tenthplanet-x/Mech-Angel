#pragma once
#include "CR_UICommon.h"
#include "RS_GeometryBuffer.h"
#include "IG_Image.h"
#include "RS_ScreenElements.h"
#include "FT_Font.h"
#include "IG_FlashImage.h"
namespace GUISys
{
	class RS_ScreenRender;
	class IG_Material;
}

class UE_PlayerName;
class RS_ScreenElements;
class UE_RenderManager :public Memory::MM_BaseObject,public Utility::UT_Singleton<UE_RenderManager>
{
public:
	static const float HPHEIGHT;
	static const float HPWIDTH;
	UE_RenderManager();
	~UE_RenderManager();

	void	ClearAllBuffer();
	unsigned int	RenderUEScreen();//绘制人物名称，血条等信息，用ScreenElement
	void    SubmitPlayerNames( GUISys::RS_GeometryBuffer* pkBuffer );
	void    SubmitPlayerAnnotations( GUISys::RS_GeometryBuffer* pkBuffer );
	void    SubmitTitleFlash( GUISys::RS_GeometryBuffer* pkBuffer );

	bool	AddNameInfo(GUISys::RS_GeometryBuffer* pkBuffer ,
						const NiPoint3 &kPos,
						const StringType &sName,
						const StringType &sTitle,
						const StringType &sDebug,
						const NiColor &kNameColor,
						const NiColor &kTitleColor,
						const NiColor &kDebugColor,
						float fNameAlpha,
						bool bIsPlayer,
						int iStar = 0,
						int iTitleLevel = 0);

	bool	AddGuildInfo(GUISys::RS_GeometryBuffer* pkBuffer ,
						const NiPoint3 &kPos,
						const StringType &sName,
						const StringType &sTitle,
						const StringType &sDebug,
						const NiColor &kNameColor,
						const NiColor &kTitleColor,
						const NiColor &kDebugColor,
						float fNameAlpha,
						bool bIsPlayer,
						int iStar = 0);

	bool    AddAnnotate(GUISys::RS_GeometryBuffer* pkBuffer, 
						const NiPoint3 &kPos, 
						int iAnnotateMask, 
						const NiPoint3& kScreenPos, 
						float deltaHeight, 
						int iTitleID,
						bool bShowTitle,
						const std::vector<int>& iParamVec, 
						int& tttop,
						GUISys::GUIRect	&mTitleFlashRect,
						GUISys::GUIRect	&mGuildTitleFlashRect);

	bool	AddAnnotateEx( GUISys::RS_GeometryBuffer* pkBuffer, 
						int iTitleID, 
						bool bShowOtherTitle, 
						int iParamGuildWar, 
						GUISys::GUIRect &mTitleFlashRect,
						GUISys::GUIRect &mGuildTitleFlashRect, 
						UE_PlayerName* pkTargetName );

	bool	AddPlayerBloodInfo(int iLevel, const NiPoint3 &kPos,const float HPPercent, const float MPPercent, BloodType iType, int DamageElementType, bool isPlayer,bool isActivateAll,int starlevel, const mem::vector<int> &runelevel, bool runeshow, const mem::vector<int> &equiplevel,bool equipshow, int equipRLevel = 0);//将当前某玩家HP信息更新到渲染系统中。
	bool	AddMonsterBloodInfo(int iLevel, const NiPoint3 &kPos,const float HPPercent, const float MPPercent, BloodType iType, bool isRaidBoss = false, int iDropTimes = 0);//将当前怪物HP信息更新到渲染系统中。
	bool	AddMonsterSkillCastInfo(float Percent,const StringType& SkillName,const NiPoint3 &kPos);

	bool	AddNPCTaskInfo(const NiPoint3 &kPos, long lTaskState);//将当前NPC任务信息更新到渲染系统中。

	bool	AddNameInfoWithBG(GUISys::RS_GeometryBuffer* pkBuffer,
								const StringType &sName,
								const StringType &sTitle,
								const StringType &sDebug,
								const NiColor &kNameColor,
								const NiColor &kTitleColor,
								const NiColor &kDebugColor,
								float fNameAlpha,
								int iNameFrameState,
								const GUISys::GUIRect &kNameArea);
	bool	AddInteractiveBtn(GUISys::RS_GeometryBuffer* pkBuffer, int iFrameState, GUISys::GUIRect &kRect);

	void	UpdateOffset();
	void	Update(float fTime);
	void	LoadImgsets();
	NiPoint3					ConvertToScreenPos(const NiPoint3 &kPos);
	NiPoint2					To_Screen_Pos(const NiPoint3 &kLocalPos);
	void						AddScreenElements(RS_ScreenElements* pElements);

private:

	NiTimeControllerPtr		    _getNiTimeController(NiNodePtr spNode);//取出控制器
	void						_renderUEFX();//冒血等等冒字处理
	unsigned int				_renderGeometry(GUISys::RS_GeometryBuffer *pGeometryBuffer) ;

	unsigned int                _submitQuad( GUISys::RS_GeometryBuffer::QuadVector::iterator& iter, 
		const GUISys::IG_Material* &pCurImageSet) const;

	void						_updateSourceTextureArea(GUISys::IG_ImagePtr pkImg, const GUISys::GUIRect& rect);

	void						_RenderTextName(GUISys::RS_GeometryBuffer* pkBuffer, const StringType &sName, GUISys::GUIColor &kNameColor, const GUISys::GUIRect &kNameArea, GUISys::FT_Font* pFont);

private:

	/*
		UE_RenderManager管理的image创建规则：
		只有在loadImageSet中加载的image, 且保证在整个渲染流程中不会被覆盖的image, 才可以直接使用GFxGetImageFromAtlas赋值；
		否则必须调用_SafeGfxGetImageFromAtlas；
	*/
	GUISys::IG_Image*			_SafeGfxGetImageFromAtlas(const char * pName);

	void						_ClearGfxImageReferences();

private:

	RS_ScreenElementsPtr			m_spElementInstancing;
	RS_ScreenElementsPtr			m_spFontInstancing;

	GUISys::RS_GeometryBuffer*		m_pPlayerInfoBuffer;//血条，蓝条，玩家名称等等
	GUISys::RS_GeometryBuffer::QuadVector  m_kPlayerNames;//玩家名称
	GUISys::RS_GeometryBuffer::QuadVector  m_kPlayerAnnotations;
	GUISys::RS_GeometryBuffer::QuadVector  m_kTitleFlashs;

	NiPoint3						m_kBloodbkOffset;	//主角头顶血条底偏移
	NiPoint3						m_kBloodOffset;		//主角头顶血条偏移
	NiPoint3						m_kBoundOffset;		//主角头顶血条板偏移
	NiPoint3						m_kStarOffset;      //主角头顶星级偏移
	NiPoint3						m_kRuneOffset;      //主角头顶星魂偏移
	NiPoint3						m_kEquipOffset;      //主角头顶装备偏移
	NiPoint3						m_kEquipReinforceOffset;   //装备强化等级偏移
	NiPoint3						m_kBloodBkOffset;	//主角头顶血条底偏移 最下层
	NiPoint3						m_kNameOffset;		//主角名字偏移
	NiPoint3						m_kMonsterNameOffset;		//怪物名字偏移
	NiPoint3						m_kSmallBoundOffset;//小怪头顶血条底板偏移
	NiPoint3						m_kBossBoundOffset;//BOSS头顶血条底板偏移
	NiPoint3						m_kBossBloodOffset;//BOSS头顶血条偏移
	NiPoint3						m_kwildBossDropoffset;
	NiPoint3						m_kHolyFlamOffset;
	NiPoint3						m_kStarsOffset;
	NiPoint3						m_kNpcTaskOffset;
	NiPoint3						m_kNpcBloodOffset;		//怪物血条偏移
	float							m_fLevelWidth;		//主角等级宽度
	float							m_fBossLevelWidth;	//BOSS等级宽度
	float							m_fBloodWidth;
	float							m_fBloodHeight;
	float							m_fBossBloodWidth;  //Boss血量size
	float							m_fBossBloodHeight;
	float							m_fNPCTaskWidth;  //NPC任务size
	float							m_fNPCTaskHeight;
	float							m_fBloodWidthSmall;
	float							m_fBloodHeightSmall;
	float							m_fNameIconOffset;
	float							m_fTransXFactor;
	float							m_fTransXOffset;
	float							m_fTransYFactor;
	int								m_iFont;
	int								m_iNumberFont;
	int								m_iEquipReinforceFont;
	int                             m_iguildFont;
	
	GUISys::IG_ImagePtr				m_pPBloodBKImg;			//玩家血条底图
	GUISys::IG_ImagePtr				m_pBoundImg;			//玩家血条框
	GUISys::IG_ImagePtr				m_pBoundBkImg;			//玩家血条框底
	GUISys::IG_ImagePtr				m_pBoundBossImg;		//Boss血条框
	GUISys::IG_ImagePtr				m_pBoundMonsterImg;		//小怪血条框
	GUISys::IG_ImagePtr				m_pMRedImgBigDeep;		//Boss血条
	GUISys::IG_ImagePtr				m_pMRedImgBigLight;
	GUISys::IG_ImagePtr				m_pMRedImgSmallDeep;	//小怪血条
	GUISys::IG_ImagePtr				m_pMRedImgSmallLight;
	GUISys::IG_ImagePtr				m_pPGreenImgDeep;		//玩家血条绿色
	GUISys::IG_ImagePtr				m_pPGreenImgLight;
	GUISys::IG_ImagePtr				m_pPBlueImgDeep;		//玩家血条蓝色
	GUISys::IG_ImagePtr				m_pPBlueImgLight;
	GUISys::IG_ImagePtr				m_pPRedImgDeep;			//玩家血条红色
	GUISys::IG_ImagePtr				m_pPRedImgLight;		

	GUISys::IG_ImagePtr				m_pGroupLeader;
	GUISys::IG_ImagePtr				m_pGroupMember;
	GUISys::IG_ImagePtr				m_pOtherGroupMember;
	GUISys::IG_ImagePtr				m_pNameBGIconNormal;
	GUISys::IG_ImagePtr				m_pNameBGIconHover;
	GUISys::IG_ImagePtr				m_pDomainIcon;
	GUISys::IG_ImagePtr				m_pVipIcon;
	GUISys::IG_ImagePtr				m_pDailyActivityIcon;
	GUISys::IG_ImagePtr				m_pTitleIcon;
	GUISys::IG_ImagePtr				m_pTitleFlash;
	GUISys::IG_ImagePtr				m_pVillainIcon;
	GUISys::IG_ImagePtr				m_pEnemyIcon;
	GUISys::IG_ImagePtr				m_pPlatformVipIcon;
	GUISys::IG_ImagePtr				m_pPlatformVipYearIcon;
	GUISys::IG_ImagePtr				m_pGmTitleIcon;
	GUISys::IG_ImagePtr				m_pNPCToyIcon;
	GUISys::IG_ImagePtr				m_pNPCSkillPercentBKImg;
	GUISys::IG_ImagePtr				m_pNPCSkillPercentImg;

	GUISys::IG_ImagePtr				m_spTaskAcceptIcon;
	GUISys::IG_ImagePtr				m_spTaskAcceptIcon2;
	GUISys::IG_ImagePtr				m_spTaskUnFinishIcon;
	GUISys::IG_ImagePtr				m_spTaskUnFinishIcon2;
	GUISys::IG_ImagePtr				m_spTaskNoneIcon;
	GUISys::IG_ImagePtr				m_spTaskFinishIcon;
	GUISys::IG_ImagePtr				m_spTaskFinishIcon2;
	GUISys::IG_ImagePtr             m_pEquipBkImage;
	GUISys::IG_ImagePtr             m_pWildBossDropImage;

	mem::vector<GUISys::IG_ImagePtr> m_pNPCStarIcon;
	mem::vector<GUISys::IG_ImagePtr> m_pBuddyStarIconVec;
	mem::vector<GUISys::IG_ImagePtr> m_pRuneStarIconVec;
	mem::vector<GUISys::IG_ImagePtr> m_pEquipStarIconVec;
	mem::vector<GUISys::IG_ImagePtr> m_pHpBackVec;
	mem::vector<GUISys::IG_ImagePtr> m_pHpBackAdvVec;
	mem::vector<GUISys::IG_ImagePtr> m_pVipLevelIcon;
	mem::vector<GUISys::IG_ImagePtr> m_pCommanderTitleLevelIcon;




	mem::vector<RS_ScreenElements*> m_kScreenElementsArray;

	// Hack!: 保证我们的IG_IMAGE中材质生命周期，能覆盖到渲染完成之后;
	mem::map<const GUISys::IG_Image*, GUISys::IG_ImagePtr> m_kImageMaterialReferenceTempMap;
};