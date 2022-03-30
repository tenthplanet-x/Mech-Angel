#pragma once
//客户端角色模型，支持后台线程异步加载资源
#include "GD_ActorModel.h"
#include "GD_ClothingModel.h"
#include "SE_SceneManager.h"
#include "CF_SD_Call.h"

class GD_ClientActorModel : public Data::GD_ActorModel
{
protected:
private:
    enum AnimType
    {
        AT_SINGLE_ANIM,
        AT_IDLE_ANIM_GROUP,
        AT_WEAPONED_IDLE_ANIM_GROUP,
		AT_WEAPONED_IDLE_ANIM_WITH_HOBBY_GROUP, //only players have hobby animation
        AT_ATTACK_ANIM_GROUP,
		AT_GREATKUNGFU_GROUP,
		AT_ATTACK_NORMAL_GROUP,
		AT_ATTACK_NORMAL_1,
		AT_DISPLAY_GROUP,
		AT_DISPLAY_NPC_GROUP,
		AT_DISPLAY_NPC_GROUP3,
    };

    enum ClientActorPendingSettingsType
    {
        CAPST_NONE = 0,

        CAPST_PLAY_ANIM = 0x0001,

        CAPST_MAX_COUNT
    };

    struct ClientActorPendingSettings
    {
        int eTypes;

        AnimType eAnimPendingPlayingType;
        GameLogic::WeaponType eWeaponType;
		StringType strHobbyAnim;

        ClientActorPendingSettings(void)
            : eTypes(CAPST_NONE)
            , eAnimPendingPlayingType(AT_SINGLE_ANIM)
            , eWeaponType(GameLogic::WT_NONE)
        {
        }

		void clear()
		{
			eTypes = CAPST_NONE;
			eAnimPendingPlayingType = AT_SINGLE_ANIM;
			eWeaponType = GameLogic::WT_NONE;
		}
    };

    typedef mem::vector<int> IdleAnimIndexList;

public:
    GD_ClientActorModel(void);
	virtual ~GD_ClientActorModel(void);

	virtual void ReUse(void);

    long GetCharType(void) const { return m_lCharType; }
    void SetCharType(long lCharType) { m_lCharType = lCharType; }

    void SetIsPlayer(bool bIsPlayer) { m_bIsPlayer = bIsPlayer; SetClothingChangeable(false); }
    bool IsPlayer(void) const { return m_bIsPlayer; }

    void HideBeforeSuitApplied(void);
    void ShowAfterSuitApplied(void);
    bool IsHideBeforeSuitApplied(void) { return m_bHideBeforeSuitApplied; }

    NiPoint3 GetHeadPosition(void) const;
	NiNodePtr GetFxDummyNode(FxDummyNodeType kType) const;

    void PlayRandomIdleAnimation(void);
    void PlayIdleAnimationWithAttackReady(void);
	bool IsPlayingHobbyAnimation(void); //判断是否有爱好动作播放, 注意, 必须在调用下面的函数后再调用!
	void PlayWeaponedRandomIdleAnimationWithHobby(GameLogic::WeaponType eWeaponType, const StringType& strAnim);
    void PlayWeaponedRandomIdleAnimation(GameLogic::WeaponType eWeaponType);
    void PlayWeaponedIdleAnimationWithAttackReady(GameLogic::WeaponType eWeaponType);
    void PlayRandomAttackAnimation(void);
	void PlayDefaultAnimation(bool bForce,  float fFrequency,  bool bAutoAdjustFrequency);
	void PlayAttackRepeatAnimation();
	void PlayAttack1Animation();
	void PlayDisplayIdleAnimation(GameLogic::WeaponType eWeaponType);
	void PlayDisplayNPCIdleAnimation();
	void PlayDisplayNPCIdle3Animation();


    // 仅供后台任务处理调用！
    NiNodePtr GenerateNewSuitData(void);
    void ApplyNewSuit(NiNodePtr spNewSuitNiRootNode);

	//GM指令用，非正常流程调用
	void SetWingRot(float x, float y, float z);
	void SetWingXYZ(float x, float y, float z);

    // Normal Dead
    void ActiveNormalDead(float fCurrentTime, float fDeadTime, int iPlayFlag);
	void ActiveDyingDead(float fCurrentTime, float fDeadTime);
    void DeactiveNormalDead(void);
    bool IsActiveNormalDead(void) { return m_bIsActiveNormalDead; }
    bool IsNormalDeadEnd(float fCurrentTime);
    // RagDoll
#ifdef _ENABLE_PHYSX
	bool InitPhysXAnimation(const StringType& strAnimationName);
    void ActiveRagDoll(float fCurrentTime, float fRagDollTime);
    void DeactiveRagDoll(void);
    void SetRagDollMomentum(const NiPoint3& kLineMomentum);
    bool CanActiveRagDoll(void);
    bool IsActiveRagDoll(void) { return m_bIsActiveRagDoll; }
    bool IsRagDollEnd(float fCurrentTime);
     Utility::UT_Vec3Int GetRagDollRootGlobalPos(void);
    void AddRagDollForce(const NiPoint3& kForceDir, float fForceApplayTime, float fCurrentTime);
#else
	 bool IsActiveRagDoll(void) { return false; }	
	 bool IsRagDollEnd(float fCurrentTime) {	return false;}
	 void DeactiveRagDoll(void) {;}
#endif 

    virtual bool IsPickable(Base::BS_IMInterface::PickMask ePickMask);

    void EnableFakeShadow(bool bEnable);

    void SetAreaEffectInfo(MTF_EffectTile::AreaEffectType eAreaEffectType, 
        const StringType& strFxFileName, const StringType& strSoundFileName);
    void EnableAreaEffect(void);
    void DisableAreaEffect(void);

    void AffectPointLight(float fStart, float fEnd);
    void StopAffectPointLight(void);

    bool GetDynamicCollisionShape(NiPoint3& rkCenterPoint, 
        NiPoint2& rkVertex1, NiPoint2& rkVertex2, NiPoint2& rkVertex3, NiPoint2& rkVertex4);

	void ReattachToLightEffect(void);

    void SetVehicleModel(GD_ClientActorModel* pkVehicleModel);
    void SetIsVehicleModel(bool bIsVehicle) { m_bIsVehicle = bIsVehicle; }

	void SetVehicleLastPos(const NiPoint3& pos);

	const NiPoint3& GetVehicleLastPos()	{	return m_vehicleLast;	}

    void SetPitchByTerrain(bool bPitchByTerrain) { m_bPitchByTerrain = bPitchByTerrain; }

	void SetVehicleVerOffset(float fHeight) {	m_fVehicleHeight = fHeight;	}

	void DoDetachAkkAttacgFxs(void);

	void DoDetachFxsOnAnim(void);
	const AttachedFxMap& GetAttachedFxMap(FxDummyNodeType eDummyType) { return m_kAttachedFxMap[eDummyType];	}

	// 绑定特效到挂点上Ext
	void AttachFxExt(FxDummyNodeType eDummy, int eBindMode, bool bStayAfterBound, const StringType& strFxFileName, 
		const NiPoint3* pkTranslate = NULL, const NiPoint3* pkRotate = NULL, float fScale = 1.0f, bool bScaledWithParent = false,
		NiColor kColor = NiColor::WHITE, bool bExclusively = true, bool bPlayImmediately = true, const NiPoint3* pKScale = NULL, bool bOnAnim = false);

	// 从挂点解除特效Ext
	void DetachFxExt(FxDummyNodeType eDummy, const StringType& strFxFileName, bool bSmoothly = true);

	void FixPos(const NiPoint3& pos, GD_ModelBase::FxDummyNodeType eType);
	void FixRot(const NiMatrix3& rot, GD_ModelBase::FxDummyNodeType eType);
	void FixTransform(const NiTransform& tfm, GD_ModelBase::FxDummyNodeType eType);

	void SetFxAutoScale(float fScale);

	void SetClientCull(bool isCull);

	virtual void AnimActivated(NiActorManager* pkManager, NiActorManager::SequenceID eSequenceID, 
		float fCurrentTime, float fEventTime);

protected:
    virtual bool _doLoad(const StringType& strModFileName, int iActiveModelIndex);
    virtual void _doInitAfterLoad(void);
    virtual void _doPreRelease(void);

    virtual void _doTranslated(const NiPoint3& kTranslate);
    virtual void _doRotated(void);
    //virtual void _doAlphaChanged(float fAlpha);
    virtual void _onRequestSettingVisibility(bool bVisible);
    virtual void _doVisibilityChanged(bool bVisible);

    virtual void _doUpdatePendingSettings(void);

    virtual void _playAnimation(const StringType& strAnimationName, bool bForce, 
        float fFrequency, bool bAutoAdjustFrequency);
    virtual void _doEndOfAnimation(const StringType& strAnimationName, float fCurrentTime);

    virtual bool _doIsUpdatable(float fTime);
    virtual void _doPreUpdate(float fTime);
    virtual void _doPostUpdate(float fTime);

    virtual void _onCullStateChanged(bool bCulled);

    //virtual void _doSetSubnodeVisibility(NiNodePtr spSubNiNode, bool bVisible);

    virtual bool _doIsAttachedFxUnrestricted(void);

    virtual int _doGetSoundPriority(void) { return IsPlayer() ? 1 : 0; }

//     virtual bool _doIsReusable(void) { return !m_bPhysXInited; }

    virtual void _createPickRoot(void);	

private:
    

    /*virtual*/ void _doApplyNewSuit(void);
    /*virtual*/ void _onNewSuitApplied(void);

	/*virtual*/  void _doInitClothingModel(Data::GD_ClothingModel* pkClothingModel);
	/*virtual*/  void _doReleaseClothingModel(Data::GD_ClothingModel* pkClothingModel, bool isExtraClothing);
	/*virtual*/  void _doReleaseLoadingClothingModel(Data::GD_ClothingModel* pkClothingModel);

	/*virtual*/  void _doInitWeaponModel(Data::GD_WeaponModel* pkWeaponModel);
	/*virtual*/  void _doReleaseWeaponModel(Data::GD_WeaponModel* pkWeaponModel);
	/*virtual*/  void _doReleaseLoadingWeaponModel(Data::GD_WeaponModel* pkWeaponModel);

	/*virtual*/  void _doInitBowModel(Data::GD_WeaponModel* pkBowModel);
	/*virtual*/  void _doReleaseBowModel(Data::GD_WeaponModel* pkBowModel);
	/*virtual*/  void _doReleaseLoadingBowModel(Data::GD_WeaponModel* pkBowModel);

	/*virtual*/  void _doInitWingModel(Data::GD_WeaponModel* pkWingModel);
	/*virtual*/  void _doReleaseWingModel(Data::GD_WeaponModel* pkWingModel);
	/*virtual*/  void _doReleaseLoadingWingModel(Data::GD_WeaponModel* pkWingModel);

	bool _doIsColorAffectedByFx(void);

    void _initLightEffectAndShadow(void);
    void _uninitLightEffectAndShadow(void);

    void _initIdleAnimationIndexList(void);
    void _updateIdleAnimationIndexList(void);
    void _changeToNextIdleAnimation(void);
    void _changeToNextWeaponedIdleAnimation(void);
	void _changeToNextWeaponedIdleAnimationWithHobby(void);
    void _changeToNextAttackAnimation(void);
    void _changeAnimationType(AnimType eTargetAnimType) { m_eCurrentAnimType = eTargetAnimType; m_bAnimTypeChanged = true; }
	void _changeToNextNormalAttackGroupAnimation(void);
	void _changeToNextNormalAttack1Animation();
	void _changeToNextDisplayIdleAnimation();
	void _changeToNextDisplayNPCIdleAnimation();
	void _changeToNextDisplayNPCIdle3Animation();

//     bool _addPhysXPropToSceneManager(void);
//     bool _removePhysXPropFromSceneManager(void);

    //void _applyForce(float fCurrentTime);

    void _setCurrentAreaEffect(const NiPoint3& kTranslate);

    bool _isExternalEnabledFakeShadow(void);
    void _attachFakeShadowFx(void);
    void _detachFakeShadowFx(void);

	virtual int _extraAnimSoundEventDeal();

public:
    // 默认的角色在 Chartype 表中的资源号 ID，如果找不到角色资源就用这个资源替代
    static const int DEFAULT_ACTOR_ID = 1;

private:
    static const int            CLOTHING_TYPE_COUNT = Data::GD_ClothingModel::CT_MAX_COUNT;
    static const unsigned int   IDLE_ANIM_COUNT     = 4;
    static const char*          IDLE_ANIM_NAMES[IDLE_ANIM_COUNT];

    ClientActorPendingSettings  m_kClientActorPendingSettings;

    long                        m_lCharType;

    bool                        m_bNeedToRecalcPitching;
    bool                        m_bFirstPitching;
    NiMatrix3                   m_kPitchingRotation;
    float                       m_fLastPitchingAngle;

    bool                        m_bIsPlayer;

    float                       m_fLastAlpha;

    bool                        m_bHideBeforeSuitApplied;
    bool                        m_bAllClothingChanged;
    bool                        m_bVisibilityWhenHideBeforeSuitApplied;
    bool                        m_bRecordVisibilityWhenHideBeforeSuitApplied;

    // NPC 假阴影
    bool                        m_bFakeShadow;
    FxDummyNodeType             m_eCurrentFakeShadowDummyNode;
    NiColor                     m_kFakeShadowColor;

    AnimType                    m_eCurrentAnimType;
    bool                        m_bAnimTypeChanged;
    GameLogic::WeaponType       m_eWeaponType;
    // 待机动作循环次数最小/最大值
    int                         m_anIdleAnimRepeatMinNums[IDLE_ANIM_COUNT];
    int                         m_anIdleAnimRepeatMaxNums[IDLE_ANIM_COUNT];
    // 当前存在的休闲待机动作索引列表（只包含 1、2、3！0 是普通待机）
    IdleAnimIndexList           m_kAvailableFreeIdleAnimIndices;
    // 当前待机动作索引
    int                         m_iCurrIdleAnimIndex;
    // 当前待机动作剩余的循环次数
    int                         m_nCurrIdleAnimRemain;
	//爱好的待机动作独立出来, 目前爱好仅仅播放一次
	StringType					m_strCurrHobbyIdleAnim;
	// 当前待机动作索引
	int                         m_iCurrDisplayAnimIndex;

#ifdef _ENABLE_PHYSX
    bool                        m_bPhysXInited;
	bool                        m_bHasAddedPhyXPropToSceneMgr;
#endif
    // Normal dead
    bool				        m_bIsActiveNormalDead;
    float				        m_fNormalDeadStartTime;
    float				        m_fNormalDeadLastTime;
    // RagDoll
#ifdef _ENABLE_PHYSX
	float				        m_fRagDollStartTime;
    float				        m_fRagDollLastTime;
    bool				        m_bIsActiveRagDoll;
    float				        m_fForceApplyTime;
    float				        m_fForceStartTime;
    NiPoint3			        m_kForce;
    bool				        m_bForceApply;
#endif 
    NiNodePtr                   m_spBip01;

    StringType m_astrAreaEffectFxNames[MTF_EffectTile::AET_MAX_COUNT];
	StringType m_astrAreaEffectSoundNames[MTF_EffectTile::AET_MAX_COUNT];

    bool m_bAreaEffectEnabled;
    MTF_EffectTile::AreaEffectType m_eLastAreaEffectType;
    float m_fWaterAreaEffectHeight;
    int m_iCurrentAreaEffectFxId;
	int m_iCurrentAreaEffectSoundId;

    bool m_bAffectPointLight;
    NiPointLightPtr m_spNiPointLight;
    NiPoint3 m_kPointLightParam;

    NiInteger64ExtraDataPtr m_spPickRootExtraData;

    GD_ClientActorModel* m_pkVehicleModel;
    bool m_bIsVehicle;
	float m_fVehicleHeight;

	NiPoint3 m_vehicleLast;

	bool m_clietNoCull;

    bool m_bPitchByTerrain;

	//脚步身计时时刻
	float					m_fStepSoundTick;
};
