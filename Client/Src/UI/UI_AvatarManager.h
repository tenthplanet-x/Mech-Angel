//-----------------------------------------------------------------------------
/** Avatar元素管理器 **/
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
	// 摄像机模式枚举
	// 这里枚举值修改，lua中也要对应修改。
	//-----------------------------
	enum CameraMode
	{
		CM_HEAD						= 0,	// 头像
		CM_FULLBODY					= 1,	// 全身
		CM_VEHICLE					= 2,	// 骑乘
		CM_TEAM_FULLBODY			= 3,	// 组队全身
		//CM_FULLBODY_WITHOUT_VE	HICLE,
		CM_VIEWINFO					= 4,	//查看资料,
		CM_ARROW					= 5,	//弩炮
		CM_ARENA_BATTLE_ACTOR		= 6,	//竞技场挑战玩家全身+坐骑
		CM_DEVAEYE					= 7,	//天眼
		CM_WILD_PRINCES_BOSS		= 8,	//野外首领Boss全身
		CM_ESC_UI_CHAR				= 9,	//ESC界面角色全身+坐骑+天眼
		CM_HEAD2					= 10,	//头像2
		CM_RUNESCAPE_FULLBODY		= 11,	//江湖玩家全身
		CM_CATAPULTS				= 12,	//攻城车
		CM_HOLYFLAM					= 13,	//圣火令
		CM_TEAMRAID_BOSS			= 14,	//组队副本boss
		CM_GREAT_KUNGFU				= 15,	//绝学
		CM_RUNESCAPE_VORPAL_BLADE	= 16,	//屠龙刀争霸赛
		CM_FRIEND_TIPS				= 17,	//好友Tips全身
		CM_VEHICLEFIGHTING			= 18,
		CM_GREAT_KUNGFU_TIPS		= 19,	//签到奖励Tips
		CM_VIP_RAID					= 20,	//vip副本
		CM_GREAT_KUNGFU_REMAIN		= 23,	//签到奖励提示获得
		CM_FABAO					= 24,	//法宝
		CM_MUHUN					= 25,	//武魂
		CM_FLOWER					= 26,	//鲜花界面
		CM_TREASURE_CHESTS			= 27,	//开宝箱特效
		CM_WILD_PRINCES				= 28,	//野外首领
		CM_RANKINGLIST				= 29,	//排行榜
		CM_SELCHAR_HEAD				= 30,	//选人头像
		CM_RUNESCAPE_TIP			= 31,	//江湖Tooltip
		CM_RUNESCAPE_EFFECT			= 32,	//公会战屏幕特效
		CM_GUILD_RAID_BUST			= 33,	//公会副本boss半身像
		CM_CHARACTER				= 34,	//角色界面
		CM_WILD_BOSS				= 35,	//明教禁地BOSS全身
		CM_REWARD_RANK				= 36,	
		CM_FUNCTION_PREVIEW_BUTTON	= 37,	//功能预览按钮
		CM_DAILYRECHARGE_LEFT		= 38,	//超值首充左
		CM_DAILYRECHARGE_RIGHT		= 39,	//超值首充右
		CM_SPIRIT_PET				= 40,	//灵宠
		CM_SPIRIT_WEAPON			= 41,	//灵武
		CM_PRIVILEGE_VIP1			= 42,	//白银VIP
		CM_PRIVILEGE_VIP2			= 43,	//黄金VIP
		CM_CHAIN_SOUL				= 44,	//炼魂
		CM_VEHICLE_SKIN_0			= 45,	//坐骑皮肤1
		CM_VEHICLE_SKIN_1			= 46,	//坐骑皮肤2
		CM_VEHICLE_SKIN_2			= 47,	//坐骑皮肤3
		CM_VEHICLE_SKIN_3			= 48,	//坐骑皮肤4	
		CM_VEHICLE_SKIN_4			= 49,	//坐骑皮肤5
		CM_VEHICLE_SKIN_5			= 50,	//坐骑皮肤6
		CM_ALLIANCEMAP				= 51,	//边境地图
		CM_FASHION					= 52,	//时装
		CM_HIDDEN_WEAPON			= 53,	//法器
		CM_HIDDEN_WEAPON_INFABAO	= 54,	//法宝中的法器预览
		CM_GOD_WEAPON				= 55,	//神器
		CM_PLOUGH					= 56,	//天罡
		CM_MIJI						= 57,	//秘籍
		CM_GOD_WEAPON_PREVIEW		= 58,	//神兵预览
		CM_GODWEAPON_TIP			= 59,	//神兵tips
		CM_EVIL						= 60,	//地煞
		CM_VEHICLE_CRYSTAL			= 61,	//坐骑水晶
		CM_FAN						= 62,	//羽扇
		CM_SUPERBOW					= 63,	//神弓
		CM_HOOK						= 64,	//弯钩
		CM_JIANGJUNFU				= 65,	//将军府
		CM_PEEK_BATTLEFIELD			= 66,	//巅峰战场
		CM_BONESOUL					= 67,	//命骨
		CM_SPIRIT_FAN				= 68,	//灵扇
		CM_SKY_ARENA				= 69,	//通天战场
		CM_GEMBUFF					= 70,	//宝石加持
		CM_ARENA_BATTLE_ACTOR_OTHER = 71, //竞技场其他玩家
		CM_WILD_PRINCES_BOSS_FAMILY	= 72,  //BOSS之家
		CM_GREAT_KUNGFU_REMAIN2		= 73, //SecretRemain界面需要2种不同模式，新模型的萌宠&其他旧模型
		CM_FUNCTION_PREVIEW_BUTTON2	= 74, //功能预览按钮需要2种不同模式，新模型的萌宠&其他旧模型
		CM_GREAT_KUNGFU_TIPS2		= 75,
		CM_ACHIVEMENT				= 76, //历练
		CM_TOYPET					= 77, //玩具
		CM_TOYPET_GAIN				= 78, //玩具获取
		CM_STIGMAS					= 79, //圣痕
		CM_HOOK_BOT					= 80, //挂机机器人
		CM_HOOK_BOT_MONSTER			= 81, //挂机怪物
		CM_CORE_BOSS				= 82, //核芯界面boss
		CM_MULTI_BUDDY				= 83, //英雄宝箱
		CM_TOYPET_CHOOSE			= 84, //玩具选择
		CM_RAID_SETTLE				= 85, //副本结算
		CM_NONPAREIL_ITEM			= 86, //极品白拿
		CM_ROYAL_CITY				= 87, //决战王城
		CM_SPECIALTRAIN				= 88, //特训
		CM_FIRST_RECHARGE_ADV		= 89, //首充广告
		CM_AUTOPICK_ADV				= 90, //自动拾取广告
		CM_MAX_COUNT
	};

	//-----------------------------
	// 摄像机模式枚举
	// 这里枚举值修改，lua中也要对应修改。
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
	// 模型创建参数
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

	// 获取具体的avatar对象;
	AvatarElementPtr GetAvatar(AvatarElementKeyId keyId);
	AvatarElementPtr GetAvatar(long lCharType, int CharIndex, int iCameraMode);

	// 对特定的avatar对象施加旋转操作;
	void RotateAvatar(long lCharType, int CharIndex, int iCameraMode, float fRotAngle);
	void SetAvatarRotation(long lCharType, int CharIndex, int iCameraMode, float fRotAngleX, float fRotAngleY, float fRotAngleZ);
    void RotatePlayerAvatar( long lCharType,int CharIndex, float fRotAngle);

	// 播放特定的avatar对象动画;
	void PlayAvatarAnim(long lCharType, int CharIndex, int iCameraMode, const StringType& sAnimName, bool bForce, int iLoopNum = 1);

	// attach特效到特定的avatar对象;
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

	//子弹list
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
	NiRect<float>           m_kSubViewPorts[1];					//进阶线
	NiRect<float>			m_kViewInfoViewPort[1];				//查看资料
	NiRect<float>			m_kRankingListViewPort[1];			//排行榜
	NiRect<float>			m_kFriendTipViewPort[1];			//好友tips
	NiRect<float>			m_kRunescapeViewPort[1];			//公会战入口界面
	NiRect<float>			m_kGameStoryWildBossViewPort1[1];	//vip副本
	NiRect<float>			m_kRunescapeTipViewPort[1];			//武林盟主tip
	NiRect<float>			m_kGreatKungfuRemain[1];			//第一次获得坐骑/宠物/弩炮
	NiRect<float>			m_kFlowerViewPort[1];				//鲜花
	NiRect<float>			m_kFunctionPreviewBtnViewPort[1];	//功能预览按钮
	NiRect<float>			m_kToyPet[1];						//玩具
	NiRect<float>			m_kHookBot[1];						//机器人
	NiRect<float>			m_kCoreBoss[1];						//核芯Boss
	NiRect<float>			m_kSpecialTrain[1];					//特训
	NiRect<float>			m_kFirstRechargeAdv[1];				//首充广告
	NiRect<float>			m_kAutoPickAdv[1];					//自动拾取广告

	//** Logic ** //
    float                   m_fLastUpdateTime;
	float                   m_fLastShootUpdateTime;
	bool					m_bIsElapsedRenderInterval;

	//机器人动画
	//过场动画进行中标志
	bool					m_bChangeStage;

	//** StringTable ** //
	static const char*		ACTOR_WITH_VEHICLE_ANIM_NAME;
	static const char*		VEHICLE_ANIM_NAME;

	static const float		UPDATE_RENDER_TIME_INTERVAL;
	static const float		UPDATE_BOT_SHOOT_TIME_INTERVAL;
};
