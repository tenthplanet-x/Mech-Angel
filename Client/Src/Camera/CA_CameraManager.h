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
		CT_NA = -1,		//��Чֵ
		CT_FREE = 0,
		CT_LOCK,		//�̶��ӽ�
		CT_ANIM,		//cutsceneʹ��
		CT_SELCHAR,		//ѡ���ɫ����
		CT_DRACONIS,	//�������ģʽ
		CT_TRACK_ANIM,	//����������켣����
		CT_DUNGEON_BOSS_SHOW,	//������boss����
		CT_BOSS_SHOW_SEPARATE,	//������boss���� �ྵͷ

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

	//��ͷ�켣����
	bool		PlayTrackAnim(int trackIdx, bool AutoStop = false);
	bool		PlayMaxAnim(int chartpyeid, int npcid);
	void		StopTrackAnim();
	void		SetCameraKey(int trackIdx, int keyIdx);
	int			GetCameraKeyCount(int trackIdx);
	void		ModifyCameraKey(int trackIdx, int keyIdx);
	void		AddCameraKey(int trackIdx, int keyIdx);
	void		RemoveCameraKey(int trackIdx, int keyIdx);

	//ֱ���ƶ���ĳĿ������
	void		MoveToTarget(int lTargetID, float go, float back, float stop, bool follow);
	void		MoveToMC(void);

	NiPoint3	GetFreeCameraLoc();

	//����ɫϵͳ
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