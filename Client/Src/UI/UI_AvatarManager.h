//-----------------------------------------------------------------------------
/** AvatarԪ�ع����� **/
//-----------------------------------------------------------------------------
#pragma once

#include "UI_AvatarDefine.h"
#include "MM_BaseObject.h"
#include "MG_MessageHandlerInterface.h"

//-----------------------------------------------------------------------------
class GD_ClientActorModel;
//-----------------------------------------------------------------------------
// AvatarElement
//-----------------------------------------------------------------------------
struct AvatarElement : public Memory::MM_BaseObject
{
	int	iSceneMapIndex;
	int iRenderClickIndex;
	int8_t iLife;
	GD_ClientActorModel* pkActorModel;
};
typedef AvatarElement* AvatarElementPtr;
//-----------------------------------------------------------------------------
struct AvatarBullet : public Memory::MM_BaseObject
{
	int iIndex;
	float fStartTime;
	float fFlightTime;
	int iFlyingFxId;
	NiPoint3 kStartLocation;
	AvatarElementKeyId uiKeyId;
	NiNodePtr pkFxDummyNode;
	StringType sBulletExplodeFx;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// UIAvatarManager
//-----------------------------------------------------------------------------
class UI_AvatarManager : public MG_MessageHandlerInterface,
						public Memory::MM_BaseObject,
						public Utility::UT_Singleton<UI_AvatarManager>
{
public:

	//-----------------------------
	// �����ģʽö��
	// ����ö��ֵ�޸ģ�lua��ҲҪ��Ӧ�޸ġ�
	//-----------------------------
	enum CameraMode
	{
		CM_HEAD						= 0,	// ͷ��
		CM_FULLBODY					= 1,	// ȫ��
		CM_VEHICLE					= 2,	// ���
		CM_TEAM_FULLBODY			= 3,	// ���ȫ��
		//CM_FULLBODY_WITHOUT_VE	HICLE,
		CM_VIEWINFO					= 4,	//�鿴����,
		CM_ARROW					= 5,	//����
		CM_ARENA_BATTLE_ACTOR		= 6,	//��������ս���ȫ��+����
		CM_DEVAEYE					= 7,	//����
		CM_WILD_PRINCES_BOSS		= 8,	//Ұ������Bossȫ��
		CM_ESC_UI_CHAR				= 9,	//ESC�����ɫȫ��+����+����
		CM_HEAD2					= 10,	//ͷ��2
		CM_RUNESCAPE_FULLBODY		= 11,	//�������ȫ��
		CM_CATAPULTS				= 12,	//���ǳ�
		CM_HOLYFLAM					= 13,	//ʥ����
		CM_TEAMRAID_BOSS			= 14,	//��Ӹ���boss
		CM_GREAT_KUNGFU				= 15,	//��ѧ
		CM_RUNESCAPE_VORPAL_BLADE	= 16,	//������������
		CM_FRIEND_TIPS				= 17,	//����Tipsȫ��
		CM_VEHICLEFIGHTING			= 18,
		CM_GREAT_KUNGFU_TIPS		= 19,	//ǩ������Tips
		CM_VIP_RAID					= 20,	//vip����
		CM_GREAT_KUNGFU_REMAIN		= 23,	//ǩ��������ʾ���
		CM_FABAO					= 24,	//����
		CM_MUHUN					= 25,	//���
		CM_FLOWER					= 26,	//�ʻ�����
		CM_TREASURE_CHESTS			= 27,	//��������Ч
		CM_WILD_PRINCES				= 28,	//Ұ������
		CM_RANKINGLIST				= 29,	//���а�
		CM_SELCHAR_HEAD				= 30,	//ѡ��ͷ��
		CM_RUNESCAPE_TIP			= 31,	//����Tooltip
		CM_RUNESCAPE_EFFECT			= 32,	//����ս��Ļ��Ч
		CM_GUILD_RAID_BUST			= 33,	//���ḱ��boss������
		CM_CHARACTER				= 34,	//��ɫ����
		CM_WILD_BOSS				= 35,	//���̽���BOSSȫ��
		CM_REWARD_RANK				= 36,	
		CM_FUNCTION_PREVIEW_BUTTON	= 37,	//����Ԥ����ť
		CM_DAILYRECHARGE_LEFT		= 38,	//��ֵ�׳���
		CM_DAILYRECHARGE_RIGHT		= 39,	//��ֵ�׳���
		CM_SPIRIT_PET				= 40,	//���
		CM_SPIRIT_WEAPON			= 41,	//����
		CM_PRIVILEGE_VIP1			= 42,	//����VIP
		CM_PRIVILEGE_VIP2			= 43,	//�ƽ�VIP
		CM_CHAIN_SOUL				= 44,	//����
		CM_VEHICLE_SKIN_0			= 45,	//����Ƥ��1
		CM_VEHICLE_SKIN_1			= 46,	//����Ƥ��2
		CM_VEHICLE_SKIN_2			= 47,	//����Ƥ��3
		CM_VEHICLE_SKIN_3			= 48,	//����Ƥ��4	
		CM_VEHICLE_SKIN_4			= 49,	//����Ƥ��5
		CM_VEHICLE_SKIN_5			= 50,	//����Ƥ��6
		CM_ALLIANCEMAP				= 51,	//�߾���ͼ
		CM_FASHION					= 52,	//ʱװ
		CM_HIDDEN_WEAPON			= 53,	//����
		CM_HIDDEN_WEAPON_INFABAO	= 54,	//�����еķ���Ԥ��
		CM_GOD_WEAPON				= 55,	//����
		CM_PLOUGH					= 56,	//���
		CM_MIJI						= 57,	//�ؼ�
		CM_GOD_WEAPON_PREVIEW		= 58,	//���Ԥ��
		CM_GODWEAPON_TIP			= 59,	//���tips
		CM_EVIL						= 60,	//��ɷ
		CM_VEHICLE_CRYSTAL			= 61,	//����ˮ��
		CM_FAN						= 62,	//����
		CM_SUPERBOW					= 63,	//��
		CM_HOOK						= 64,	//�乳
		CM_JIANGJUNFU				= 65,	//������
		CM_PEEK_BATTLEFIELD			= 66,	//�۷�ս��
		CM_BONESOUL					= 67,	//����
		CM_SPIRIT_FAN				= 68,	//����
		CM_SKY_ARENA				= 69,	//ͨ��ս��
		CM_GEMBUFF					= 70,	//��ʯ�ӳ�
		CM_ARENA_BATTLE_ACTOR_OTHER = 71, //�������������
		CM_WILD_PRINCES_BOSS_FAMILY	= 72,  //BOSS֮��
		CM_GREAT_KUNGFU_REMAIN2		= 73, //SecretRemain������Ҫ2�ֲ�ͬģʽ����ģ�͵��ȳ�&������ģ��
		CM_FUNCTION_PREVIEW_BUTTON2	= 74, //����Ԥ����ť��Ҫ2�ֲ�ͬģʽ����ģ�͵��ȳ�&������ģ��
		CM_GREAT_KUNGFU_TIPS2		= 75,
		CM_ACHIVEMENT				= 76, //����
		CM_TOYPET					= 77, //���
		CM_TOYPET_GAIN				= 78, //��߻�ȡ
		CM_STIGMAS					= 79, //ʥ��
		CM_HOOK_BOT					= 80, //�һ�������
		CM_HOOK_BOT_MONSTER			= 81, //�һ�����
		CM_CORE_BOSS				= 82, //��о����boss
		CM_MULTI_BUDDY				= 83, //Ӣ�۱���
		CM_TOYPET_CHOOSE			= 84, //���ѡ��
		CM_RAID_SETTLE				= 85, //��������
		CM_NONPAREIL_ITEM			= 86, //��Ʒ����
		CM_ROYAL_CITY				= 87, //��ս����
		CM_SPECIALTRAIN				= 88, //��ѵ
		CM_FIRST_RECHARGE_ADV		= 89, //�׳���
		CM_AUTOPICK_ADV				= 90, //�Զ�ʰȡ���
		CM_MAX_COUNT
	};

	//-----------------------------
	// �����ģʽö��
	// ����ö��ֵ�޸ģ�lua��ҲҪ��Ӧ�޸ġ�
	//-----------------------------
	enum SceneMapIndex
	{ 
		SMI_DEFAULT				= 0,

		SMI_ACTOR,
		SMI_VEHICLE,
		SMI_ARROW,
		SMI_DEVAEYE,
		SMI_GODWING,
		SMI_FABAO,
		SMI_WORLDMAP,
		SMI_PLATFORM,
		SMI_MENTAL,
		SMI_VEHICLEFIGHTING,
		SMI_TREASUREITEM,
		SMI_GAMESTORYWILDBOSS,
		SMI_SPIRITPET,
		SMI_SPIRITWEAPON,
		SMI_HIDDENWEAPON,
		SMI_GODWEAPON,
		SMI_PLOUGH,
		SMI_FAN,
		SMI_SUPERBOW,
		SMI_HOOK,
		SMI_SPIRITFAN,
		SMI_HOOKBOT,
		SMI_HOOKBOT_MONSTER,

		SMI_MAX,
	};

	typedef mem::map<SceneMapIndex, AvatarElementSceneMap> AvatarElementSceneMaps;

private:

	//-----------------------------
	// ģ�ʹ�������
	//-----------------------------
	struct AvatarModelParam
	{
		AvatarModelParam(const StringType& szModFileName)
						:m_iModelType(-1)
						,m_iModelIndex(0)
						,m_bModelAsyncLoad(true)
						,m_szModFileName(szModFileName)
		{}

		int m_iModelType;
		int m_iModelIndex;
		bool m_bModelAsyncLoad;
		const StringType& m_szModFileName;
	};

	friend class AvatarVisibleGroup;
	friend class AvatarVisibleGroupSet;
    
public:

	UI_AvatarManager(void);
	virtual ~UI_AvatarManager(void);

//********************************
// Main
//********************************
public:

	void Init();
	void PreUnInit();
	void UnInit();

	void Reset();
	void Update( MG_MessageBase& rkMessage );

	bool RecvMessage(MG_MessageBase& rkMessage);

//********************************
// Callback
//********************************
public:

	void publicCallBack_ActiveGroup(const StringType& groupName, bool isActive);
	bool publicCallBack_IsActiveGroup(const StringType& groupName);

	void publicCallBack_ReleaseAllDeactiveGroup();
	void publicCallBack_ReleaseDeactiveGroup( int iMaxCount );
	void publicCallBack_SetAutoReleaseDeactiveGroupLifeScale( float fScale );

	bool publicCallBack_PrePerformRendering();
	void publicCallBack_PerformRendering(RG_AvatarRenderClick* renderClick);
	void publicCallBack_PreRenderClickProc(NiRenderClick* renderClick);
	void publicCallBack_PostRenderClickProc(NiRenderClick* renderClick);

public:

	void renderCallBack_Active(AvatarElementKeyId keyId);
	void renderCallBack_Deactive(AvatarElementKeyId keyId);
	void renderCallBack_Update(AvatarElementKeyId keyId, float fTime, float fDeltaTime);

	void renderCallBack_ConfigCamera(NiCameraPtr& camera, AvatarElementKeyId keyId);
	void renderCallBack_ConfigShader(int& shaderParamIndex, AvatarElementKeyId keyId);
	void renderCallBack_ConfigObjectShader(AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx, bool bPre);

	void renderCallBack_RenderGroupExpired(const AvatarElementKeyIdList& kElementList);
	void renderCallBack_RenderSolidPass(NiCameraPtr& camera, NiCullingProcess& culler, AvatarElementKeyId keyId);
	void renderCallBack_RenderTransparentPass(NiCameraPtr& camera, NiCullingProcess& culler, AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx=RENDER_CLICK_ORIGIN);
	bool IsShouldRender(AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx);

//********************************
// AvatarScene
//********************************
public:

    bool UpdateActorScene(const StringType& strGroupName, long lCid, int iCharIndex, int iCameraMode);
	bool UpdatePlatformScene(const StringType& strGroupName, long lCharType, int iIndex, int iCameraMode);
	bool UpdateToyPetScene(const StringType& strGroupName, long lCharType, int iIndex, int iCameraMode);
	bool UpdateTalkNpcScene(const StringType& strGroupName, int iCharIndex, long lCharType, int iCameraMode);
	bool UpdateWildPrincesBossScene(const StringType& strGroupName, int iIndex, long lTypeID, float fCameraDist, float fTranslateX, float fTranslateZ, int iCameraMode);
    bool UpdateVehicleScene(const StringType& strGroupName, long lCharType, int iIndex, int iRenderModel = 2);
	bool UpdateDevaEyeScene(const StringType& strGroupName, long lCharType, int iIndex, int iCameraMode = 8);
	bool UpdateFabaoScene(const StringType& strGroupName, long lCharType, int iIndex, int iCameraMode);
	bool UpdateGodWingTextureScene(const StringType& strGroupName, long lCharType, int iIndex, int iCameraMode);
	bool UpdateArenaBattleActorScene(const StringType& strGroupName, int iCid, int iIndex, long lWeaponID, int iCameraMode);
	bool UpdateNonpareilItemAvatarScene(const StringType& strGroupName, long iCharType, int iCharIndex,  float fTranslateX,float fTranslateY, float fTranslateZ, int iCameraMode);
	bool UpdateHookBotScene( const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode );
	bool UpdateHookBotMonsterScene( const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode );
	bool UpdateHookBotBoxScene( const StringType& strGroupName, long lCharType, int iCharIndex, int iCameraMode );

private:

	// avatarSceneMapIndex
	void _loadSceneMaps();
	void _unLoadSceneMaps();
	AvatarElementSceneMap& _getSceneMap(int iSceneMapIndex);
	AvatarElementSceneMap& _getSceneMap(SceneMapIndex eSceneMapIndex);
	
	// avatarSceneIndex
	AvatarElementSceneIndex	_generateAvatarElementSceneIndex(int charIndex, CameraMode cameraMode);
	void _extractAvatarElementInfoFromSceneIndex(AvatarElementSceneIndex sceneIndex, int& charIndex, CameraMode& cameraMode);
	
	// charType in scene
	long _getCharTypeFromScene(AvatarElementSceneIndex sceneIndex, const AvatarElementSceneMap& scene);
	void _addCharTypeToScene(AvatarElementSceneIndex sceneIndex, long CharType, AvatarElementSceneMap& scene);
	void _removeCharTypeFromScene(AvatarElementSceneIndex sceneIndex, AvatarElementSceneMap& scene);
	void _removeAllCharTypeScenes();

	AvatarElementKeyId _getHookBotShootTargetKeyID();
	void _updateHookBotShoot(float fCurrentTime);
	void _updateHookBotState(float fCurrentTime);
	void _updateBullets(float fCurrentTime, float fDeltaTime);
	void _playDamageFxOnModel(int iCharIndex, int iFxExtParam, bool bDead);
//********************************
// Avatar Control
//********************************
public:

	// ��ȡ�����avatar����;
	AvatarElementPtr GetAvatar(AvatarElementKeyId keyId);
	AvatarElementPtr GetAvatar(long lCharType, int CharIndex, int iCameraMode);

	// ���ض���avatar����ʩ����ת����;
	void RotateAvatar(long lCharType, int CharIndex, int iCameraMode, float fRotAngle);
	void SetAvatarRotation(long lCharType, int CharIndex, int iCameraMode, float fRotAngleX, float fRotAngleY, float fRotAngleZ);
    void RotatePlayerAvatar( long lCharType,int CharIndex, float fRotAngle);

	// �����ض���avatar���󶯻�;
	void PlayAvatarAnim(long lCharType, int CharIndex, int iCameraMode, const StringType& sAnimName, bool bForce, int iLoopNum = 1);

	// attach��Ч���ض���avatar����;
	void AttachAvatarFx(long lCharType, int CharIndex, int iCameraMode, const StringType& sFxName, const NiPoint3 *pkPoint);
	void DetachAvatarFx(long lCharType, int CharIndex, int iCameraMode, const StringType& sFxName);
	
	void SetChangeStage(bool b) { m_bChangeStage = b; }

private:

	// avatarKeyId
	AvatarElementKeyId _generateAvatarElementKeyId(long charType, int charIndex, CameraMode cameraMode);
	void _extractAvatarElementInfoFromKeyId(AvatarElementKeyId keyId, long& charType, int& charIndex, CameraMode& cameraMode);

	// create\destroy method
	AvatarElementPtr _createAvatarElement(AvatarElementKeyId keyId, SceneMapIndex sceneMapIndex, const StringType& renderGroup, const AvatarModelParam& modelParam);
	int _getRenderClickIndex(CameraMode eMode);
	void _destroyAvatarElement(AvatarElementKeyId keyId, const StringType& renderGroup);
	void _destroyExpiredAvatarElements(const AvatarElementKeyIdList& keyIdList);
	void _destroyAllAvatarElements();
	void _destroyAllAvatarBullets();

	// specialist create method
	AvatarElementPtr _createActorAvatarElement(AvatarElementKeyId keyId, SceneMapIndex sceneMapIndex, const StringType& renderGroup );

	// avatar control
	void _rotateActorAvatarElementToFront(GD_ClientActorModel& rkActorModel, bool bIsFullBodyWithVehicle);
	void _changeActorAvatarAttachedItem(GD_ClientActorModel& rkActorModel,int iChangeType, long lItemID, long lCharType);

	void _updateAvatarScene();


//********************************
// Camera
//********************************
public:

	void ReloadCameraSetting(void);

	CameraMode GetCameraModeIntMapping(int iCameraMode);
	int	GetShaderParamIndexCameraModeMapping(CameraMode eCameraMode);

private:

	void _loadCameraSetting(void);
	void _calcCameraFrustum(float fFov, float fAspectRatio, NiFrustum& rkFrustum);
	void _calcCameraRotationMatrix(float fPitch, float fYaw, NiMatrix3& rkRotation);
	void _applyCameraSetting(NiCameraPtr& spCamera, CameraMode eCameraMode, const NiRect<float>& kViewPort);
	void _getSubordinatePosition(const CSVFile::CF_CharType::DataEntry *pkEntry, CameraMode eCameraMode, NiPoint3 &kPosition);
	const NiPoint3& _getNodePosition(const NiNodePtr& pkRootNode, const char *szName);

//********************************
// Viewport
//********************************
private:

	void _loadViewPorts(UI_AvatarRender* pkRender);

//********************************
// UIImage 
//********************************
private:

	void _loadGUIImages(UI_AvatarRender* pkRender);

//********************************
// Render
//********************************
public:

	inline UI_AvatarRender* getRender() { return m_pkRender; }

//********************************
// Members
//********************************
private:

	//** Element ** //
	AvatarElementMap		m_kAllAvatarElements;

	//** Scene ** //
	AvatarElementSceneMaps	m_kAllSceneMaps;

	//�ӵ�list
	AvatarBulletList		m_kBulletList;

	//** Render **//
	UI_AvatarRender*		m_pkRender;

	//float					m_fCameraDistance;
	float                   m_fFov[CM_MAX_COUNT];
	float                   m_fPitch[CM_MAX_COUNT];
	float                   m_fYaw[CM_MAX_COUNT];
	float                   m_fDistance[CM_MAX_COUNT];
	float                   m_fNearCameraHeightBias[CM_MAX_COUNT];
	NiMatrix3               m_akCameraRotation[CM_MAX_COUNT];
	NiFrustum               m_akFrustums[CM_MAX_COUNT];

	NiRect<float>           m_akFullBodyViewPorts[5];
	NiRect<float>           m_kSubViewPorts[1];					//������
	NiRect<float>			m_kViewInfoViewPort[1];				//�鿴����
	NiRect<float>			m_kRankingListViewPort[1];			//���а�
	NiRect<float>			m_kFriendTipViewPort[1];			//����tips
	NiRect<float>			m_kRunescapeViewPort[1];			//����ս��ڽ���
	NiRect<float>			m_kGameStoryWildBossViewPort1[1];	//vip����
	NiRect<float>			m_kRunescapeTipViewPort[1];			//��������tip
	NiRect<float>			m_kGreatKungfuRemain[1];			//��һ�λ������/����/����
	NiRect<float>			m_kFlowerViewPort[1];				//�ʻ�
	NiRect<float>			m_kFunctionPreviewBtnViewPort[1];	//����Ԥ����ť
	NiRect<float>			m_kToyPet[1];						//���
	NiRect<float>			m_kHookBot[1];						//������
	NiRect<float>			m_kCoreBoss[1];						//��оBoss
	NiRect<float>			m_kSpecialTrain[1];					//��ѵ
	NiRect<float>			m_kFirstRechargeAdv[1];				//�׳���
	NiRect<float>			m_kAutoPickAdv[1];					//�Զ�ʰȡ���

	//** Logic ** //
    float                   m_fLastUpdateTime;
	float                   m_fLastShootUpdateTime;
	bool					m_bIsElapsedRenderInterval;

	//�����˶���
	//�������������б�־
	bool					m_bChangeStage;

	//** StringTable ** //
	static const char*		ACTOR_WITH_VEHICLE_ANIM_NAME;
	static const char*		VEHICLE_ANIM_NAME;

	static const float		UPDATE_RENDER_TIME_INTERVAL;
	static const float		UPDATE_BOT_SHOOT_TIME_INTERVAL;
};
