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
	unsigned int	RenderUEScreen();//�����������ƣ�Ѫ������Ϣ����ScreenElement
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

	bool	AddPlayerBloodInfo(int iLevel, const NiPoint3 &kPos,const float HPPercent, const float MPPercent, BloodType iType, int DamageElementType, bool isPlayer,bool isActivateAll,int starlevel, const mem::vector<int> &runelevel, bool runeshow, const mem::vector<int> &equiplevel,bool equipshow, int equipRLevel = 0);//����ǰĳ���HP��Ϣ���µ���Ⱦϵͳ�С�
	bool	AddMonsterBloodInfo(int iLevel, const NiPoint3 &kPos,const float HPPercent, const float MPPercent, BloodType iType, bool isRaidBoss = false, int iDropTimes = 0);//����ǰ����HP��Ϣ���µ���Ⱦϵͳ�С�
	bool	AddMonsterSkillCastInfo(float Percent,const StringType& SkillName,const NiPoint3 &kPos);

	bool	AddNPCTaskInfo(const NiPoint3 &kPos, long lTaskState);//����ǰNPC������Ϣ���µ���Ⱦϵͳ�С�

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

	NiTimeControllerPtr		    _getNiTimeController(NiNodePtr spNode);//ȡ��������
	void						_renderUEFX();//ðѪ�ȵ�ð�ִ���
	unsigned int				_renderGeometry(GUISys::RS_GeometryBuffer *pGeometryBuffer) ;

	unsigned int                _submitQuad( GUISys::RS_GeometryBuffer::QuadVector::iterator& iter, 
		const GUISys::IG_Material* &pCurImageSet) const;

	void						_updateSourceTextureArea(GUISys::IG_ImagePtr pkImg, const GUISys::GUIRect& rect);

	void						_RenderTextName(GUISys::RS_GeometryBuffer* pkBuffer, const StringType &sName, GUISys::GUIColor &kNameColor, const GUISys::GUIRect &kNameArea, GUISys::FT_Font* pFont);

private:

	/*
		UE_RenderManager�����image��������
		ֻ����loadImageSet�м��ص�image, �ұ�֤��������Ⱦ�����в��ᱻ���ǵ�image, �ſ���ֱ��ʹ��GFxGetImageFromAtlas��ֵ��
		����������_SafeGfxGetImageFromAtlas��
	*/
	GUISys::IG_Image*			_SafeGfxGetImageFromAtlas(const char * pName);

	void						_ClearGfxImageReferences();

private:

	RS_ScreenElementsPtr			m_spElementInstancing;
	RS_ScreenElementsPtr			m_spFontInstancing;

	GUISys::RS_GeometryBuffer*		m_pPlayerInfoBuffer;//Ѫ����������������Ƶȵ�
	GUISys::RS_GeometryBuffer::QuadVector  m_kPlayerNames;//�������
	GUISys::RS_GeometryBuffer::QuadVector  m_kPlayerAnnotations;
	GUISys::RS_GeometryBuffer::QuadVector  m_kTitleFlashs;

	NiPoint3						m_kBloodbkOffset;	//����ͷ��Ѫ����ƫ��
	NiPoint3						m_kBloodOffset;		//����ͷ��Ѫ��ƫ��
	NiPoint3						m_kBoundOffset;		//����ͷ��Ѫ����ƫ��
	NiPoint3						m_kStarOffset;      //����ͷ���Ǽ�ƫ��
	NiPoint3						m_kRuneOffset;      //����ͷ���ǻ�ƫ��
	NiPoint3						m_kEquipOffset;      //����ͷ��װ��ƫ��
	NiPoint3						m_kEquipReinforceOffset;   //װ��ǿ���ȼ�ƫ��
	NiPoint3						m_kBloodBkOffset;	//����ͷ��Ѫ����ƫ�� ���²�
	NiPoint3						m_kNameOffset;		//��������ƫ��
	NiPoint3						m_kMonsterNameOffset;		//��������ƫ��
	NiPoint3						m_kSmallBoundOffset;//С��ͷ��Ѫ���װ�ƫ��
	NiPoint3						m_kBossBoundOffset;//BOSSͷ��Ѫ���װ�ƫ��
	NiPoint3						m_kBossBloodOffset;//BOSSͷ��Ѫ��ƫ��
	NiPoint3						m_kwildBossDropoffset;
	NiPoint3						m_kHolyFlamOffset;
	NiPoint3						m_kStarsOffset;
	NiPoint3						m_kNpcTaskOffset;
	NiPoint3						m_kNpcBloodOffset;		//����Ѫ��ƫ��
	float							m_fLevelWidth;		//���ǵȼ����
	float							m_fBossLevelWidth;	//BOSS�ȼ����
	float							m_fBloodWidth;
	float							m_fBloodHeight;
	float							m_fBossBloodWidth;  //BossѪ��size
	float							m_fBossBloodHeight;
	float							m_fNPCTaskWidth;  //NPC����size
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
	
	GUISys::IG_ImagePtr				m_pPBloodBKImg;			//���Ѫ����ͼ
	GUISys::IG_ImagePtr				m_pBoundImg;			//���Ѫ����
	GUISys::IG_ImagePtr				m_pBoundBkImg;			//���Ѫ�����
	GUISys::IG_ImagePtr				m_pBoundBossImg;		//BossѪ����
	GUISys::IG_ImagePtr				m_pBoundMonsterImg;		//С��Ѫ����
	GUISys::IG_ImagePtr				m_pMRedImgBigDeep;		//BossѪ��
	GUISys::IG_ImagePtr				m_pMRedImgBigLight;
	GUISys::IG_ImagePtr				m_pMRedImgSmallDeep;	//С��Ѫ��
	GUISys::IG_ImagePtr				m_pMRedImgSmallLight;
	GUISys::IG_ImagePtr				m_pPGreenImgDeep;		//���Ѫ����ɫ
	GUISys::IG_ImagePtr				m_pPGreenImgLight;
	GUISys::IG_ImagePtr				m_pPBlueImgDeep;		//���Ѫ����ɫ
	GUISys::IG_ImagePtr				m_pPBlueImgLight;
	GUISys::IG_ImagePtr				m_pPRedImgDeep;			//���Ѫ����ɫ
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

	// Hack!: ��֤���ǵ�IG_IMAGE�в����������ڣ��ܸ��ǵ���Ⱦ���֮��;
	mem::map<const GUISys::IG_Image*, GUISys::IG_ImagePtr> m_kImageMaterialReferenceTempMap;
};