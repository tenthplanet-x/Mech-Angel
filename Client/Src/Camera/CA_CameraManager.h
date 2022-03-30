#pragma once

#include "MG_MessageHandlerInterface.h"

class MG_MessageBase;
class CA_BaseCamera;
class CA_BossSeparateCamera;

class CA_CameraManager : public Utility::UT_Singleton<CA_CameraManager> , 
						 public MG_MessageHandlerInterface,
						 public Memory::MM_BaseObject
{
public:
	enum CAMERA_TYPE
	{
		CT_NA = -1,		//无效值
		CT_FREE = 0,
		CT_LOCK,		//固定视角
		CT_ANIM,		//cutscene使用
		CT_SELCHAR,		//选择角色界面
		CT_DRACONIS,	//天龙相机模式
		CT_TRACK_ANIM,	//场景内相机轨迹动画
		CT_DUNGEON_BOSS_SHOW,	//副本内boss出现
		CT_BOSS_SHOW_SEPARATE,	//副本内boss出现 多镜头

        CT_MAX_COUNT
	};
	
	CA_CameraManager();
	virtual ~CA_CameraManager();

	bool Init();
	bool UnInit();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

    bool         SetCameraType( CAMERA_TYPE eCameraType );
	CAMERA_TYPE	 GetCameraType() { return m_eCurrentCamera; }
	void		 UpdateCameraType();
    void		 ResetAllCamera();
	void		 OnRendererResize();

    NiCameraPtr GetNiCamera() const { return m_spCamera; }
	NiCameraPtr GetNiReflectCamera( float fReflectPlaneHeight , bool bUpdate = false );
    NiCameraPtr GetSelCharNiCamera(void) const { return m_spSelCharCamera; }
    void        SetSelCharNiCamera(NiCameraPtr spSelCharNiCamera);

	float		 GetLockCameraOffset();
	float		 GetLockCameraScale();
	float        GetLockCameraFov();

	void         SetFarClipDistance( float fDistance );
	void         SetLockCameraPitch(float fPitch );
	void         SetLockCameraDistance( float fDistance );
	void         SetCameraPitchAndDistance( float fPStart , float fPEnd , float fDistance );

    bool         GetCurrActorPlayerLocation(NiPoint3& rkActorPlayerLocation);

	NiPoint3	 GetListenerLocation();

	void		 MoveCameraToNoUnLock();

	//镜头轨迹动画
	bool		PlayTrackAnim(int trackIdx, bool AutoStop = false);
	bool		PlayMaxAnim(int chartpyeid, int npcid);
	void		StopTrackAnim();
	void		SetCameraKey(int trackIdx, int keyIdx);
	int			GetCameraKeyCount(int trackIdx);
	void		ModifyCameraKey(int trackIdx, int keyIdx);
	void		AddCameraKey(int trackIdx, int keyIdx);
	void		RemoveCameraKey(int trackIdx, int keyIdx);

	//直线移动到某目标身上
	void		MoveToTarget(int lTargetID, float go, float back, float stop, bool follow);
	void		MoveToMC(void);

	NiPoint3	GetFreeCameraLoc();

	//三角色系统
	void		ResetPlayerCamera();

protected:
	#ifdef _MAP_VIEWER_BUILD
	void	_event_InputKey9(MG_MessageBase& rkMessage);
	#endif
	void	_eventTimerTick(MG_MessageBase& rkMessage);
	void	_event_NPCBornBegin(MG_MessageBase& rkMessage);

	void    _update_world_to_camera_matrix( NiCameraPtr spCamera, const NiFixedString& rkKey );

protected:
	typedef mem::map<CAMERA_TYPE , CA_BaseCamera*> CameraMap;
	typedef mem::vector<CA_BossSeparateCamera*> SeparateCameraVec;

	CameraMap	m_kCameraMap;
	SeparateCameraVec m_kSeparateCameraVec;

	NiCameraPtr	m_spCamera;
	NiCameraPtr m_spReflectCamera;
    NiCameraPtr m_spSelCharCamera;

	CAMERA_TYPE	m_eCurrentCamera;

	float       m_pfWorldToCamera[16];
    int         m_iLockCameraLastLevel;
};