#ifndef _IM_MANAGER_H
#define _IM_MANAGER_H
//游戏内可交互模型管理
#include "BS_IMInterface.h"
#include "GD_ModelBase.h"
#include "MG_MessageHandlerInterface.h"

namespace Data
{
	class GD_SequenceManager;
}
class GD_ClientActorModel;
class GD_ClientChestModel;
class GD_ClientBreakModel;

class IM_Manager : public Memory::MM_BaseObject,
                   public MG_MessageHandlerInterface,
                   public Utility::UT_Singleton<IM_Manager>

{
    typedef mem::map<object_id_type, GD_ClientChestModel*>        ChestIdPtrMap;
    typedef mem::map<object_id_type, GD_ClientActorModel*>        ActorIdPtrMap;
	typedef mem::map<object_id_type, GD_ClientBreakModel*>        SceneIdPtrMap;

public:
    IM_Manager(void);
    virtual ~IM_Manager(void);


	enum ShieldType 
	{
		SHIELD_ALL = 0,
		SHIELD_PLAYER = 1,
		SHIELD_GUILDE_PLAYER = 2,
		SHIELD_NPC = 3,
		SHIELD_PLAYER_VEHICLE = 4,
		SHIELD_PLAYER_EYE = 5,
		SHIELD_PLAYER_BOW = 6,
		SHIELD_PLAYER_WING = 7,
		SHIELD_SCENE_FX = 8,
		SHIELD_PLAYER_FABAO = 9,
		SHILED_PLAYER_LC = 10,
		SHIELD_PLAYER_AQ = 11,
		SHIELD_PLAYER_TITLE = 12,
		SHIELD_WEATHER = 13,

		SHIELD_MAX = SHIELD_WEATHER+1,
	} ;

    void Init(void);
    void UnInit(void);
	void Reset();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

    void PreCullModels(void);
    void GetVisibleArray(NiGeometryList& rkCasterList);
	void GetShadowCasters(NiGeometryList& rkCasterList);
	void GetVisibleArray(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);

    unsigned int GetCurrentIMPlayerCreationCapacity(void);
    Base::BS_IMInterface* CreateInteractModel(long lCharType, object_id_type lLogicID,bool  bIsPlayer, 
        const NiPoint3& kPos, const NiMatrix3& kRot = NiMatrix3::IDENTITY, float fAlpha = 1.0f);
	//为了微端优化
	Base::BS_IMInterface* CreateInteractPlayerModel(long lCharType, object_id_type lLogicID,bool  bIsGamePlayer,int weaponType,
		const NiPoint3& kPos, bool bTransform=true, const NiMatrix3& kRot = NiMatrix3::IDENTITY, float fAlpha = 1.0f);
	//创建坐骑ID
	Base::BS_IMInterface* CreateInteractRiderModel(long lCharType, object_id_type lLogicID,
		const NiPoint3& kPos, const NiMatrix3& kRot = NiMatrix3::IDENTITY, float fAlpha = 1.0f );
	//创建法宝
	Base::BS_IMInterface* CreateInteractFabaoModel(long lCharType, object_id_type lLogicID,
		const NiPoint3& kPos, const NiMatrix3& kRot = NiMatrix3::IDENTITY, float fAlpha = 1.0f );
	//创建暗器
	Base::BS_IMInterface* CreateInteractAnQiModel(long lCharType, object_id_type lLogicID,
		const NiPoint3& kPos, const NiMatrix3& kRot = NiMatrix3::IDENTITY, float fAlpha = 1.0f);
	//创建角色死亡模型
	GD_ClientBreakModel* CreateInteractDeadModel(const StringType& _ResName, object_id_type lLogicID, 
		const NiPoint3& kPos, const NiMatrix3& kRot = NiMatrix3::IDENTITY, float fAlpha = 1.0f);

	Base::BS_IMInterface* CreateChestModel(const StringType& strModel, object_id_type lLogicID,
		const NiPoint3& kSrcPos, const NiPoint3& kDestPos, bool bNewModel, float kSpawnZRotAngle, int bShowGfxLevel = 0, float fAlpha = 1.0f);
    void                  DestroyInteractModel(object_id_type lLogicID);
	void                  DestroyRiderInteractModel(object_id_type lLogicID);
	void                  DestroyFabaoInteractModel(object_id_type lLogicID);
	void                  DestroyAnqiInteractModel(object_id_type lLogicID);
	void				  DestoryDeadInteractModel(object_id_type lLogicID);

    Data::GD_ModelBase*   GetModel(object_id_type lLogicID);	//GetActor || GetChest 
    GD_ClientChestModel*  GetChest(object_id_type lChestLogicID);
    GD_ClientActorModel*  GetActor(object_id_type lActorLogicID);
	GD_ClientActorModel*  GetRiderActor(object_id_type lActorLogicID);
	GD_ClientActorModel*  GetFabaoActor(object_id_type lActorLogicID);
	GD_ClientActorModel*  GetAnQiActor(object_id_type lActorLogicID);

	void ModifyPlayerModeID(object_id_type lOldLogicID, object_id_type lNewLogicID);
	void ModifyRiderModeID(object_id_type lOldLogicID, object_id_type lNewLogicID);
	void ModifyFabaoModeID(object_id_type lOldLogicID, object_id_type lNewLogicID);
	void ModifyAnQiModeID(object_id_type lOldLogicID, object_id_type lNewLogicID);

	void EnableActorShow( bool bEnable ){ m_bShowActor = bEnable; }
	bool GetActorShow( ){ return m_bShowActor; }

    void SetPickSwitch(bool bPickSwitch) { m_bPickSwitch = bPickSwitch; };

	void SwitchShieldVisibility(int iIndex)	{	m_bPendingShowOrHideShield[iIndex] = true; }
	void CloseShieldPending(int iIndex)	{	m_bPendingShowOrHideShield[iIndex] = false; }
	bool IsPendingShowOrHideShield(int iIndex)	{	return m_bPendingShowOrHideShield[iIndex]; }
	bool GetShieldVisibility(int iMapResID, int iIndex)	
	{	
		if ( iMapResID == MAPID_ARENA)
		{
			return true;
		}
		else
		{
			return m_bShowOrHideShield[iIndex];
		}
	}
	void SetShieldVisibility(int iIndex, bool val)	{	m_bShowOrHideShield[iIndex] = val;	}



private:
    void _event_InputMouseMove(MG_MessageBase& rkMessage);
	void _event_InputLButtonDown(MG_MessageBase& rkMessage);
	void _event_InputLButtonDBClk(MG_MessageBase& rkMessage);
	void _event_InputRButtonDown(MG_MessageBase& rkMessage);
	void _event_IMPlayAnimation(MG_MessageBase& rkMessage);

    long _pickPlaceHolderOrActorByMouse(long lMousePosX, long lMousePosY, Base::BS_IMInterface::PickMask ePickMask);	//_pickChest || _pickActor
    long _pickPlaceHolder(const NiPoint3& rkOrigin, const NiPoint3& rkRay, Base::BS_IMInterface::PickMask ePickMask);
    long _pickActor(const NiPoint3& rkOrigin, const NiPoint3& rkRay, Base::BS_IMInterface::PickMask ePickMask);
	long _pickPlaceHolderByNameArea(long lMousePosX, long lMousePosY, Base::BS_IMInterface::PickMask ePickMask) const;
	long _pickNetPlayerByInteractiveBtnArea(long lMousePosX, long lMousePosY, Base::BS_IMInterface::PickMask ePickMask) const;
	long _pickNpcByInteractiveBtnArea(long lMousePosX, long lMousePosY, Base::BS_IMInterface::PickMask ePickMask) const;

    void _debug_SwitchPickRootVisible();

private:
    bool			m_bInit;

	NiPick*			m_pkPick;
	bool            m_bShowActor;

    ChestIdPtrMap	m_kPlaceHolders;
	ActorIdPtrMap	m_kActors;
	ActorIdPtrMap	m_kRiderActors;
	ActorIdPtrMap	m_kFabaoActors;
	ActorIdPtrMap	m_kAnqiActors;
	SceneIdPtrMap	m_kNpcDeadActors;
	Data::GD_SequenceManager* m_pkSequenceMgr;

	bool			m_bPickSwitch;

	bool            m_bPendingShowOrHideShield[SHIELD_MAX];
	bool            m_bShowOrHideShield[SHIELD_MAX];

    NiCullingProcess m_kCuller;
    NiVisibleArray  m_kVisibleArrayCache;
};

#endif // _IM_MANAGER_H
