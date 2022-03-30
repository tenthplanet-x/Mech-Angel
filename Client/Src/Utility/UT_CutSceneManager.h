#pragma once

#include "UT_Singleton.h"

class UT_CutSceneManager : public Utility::UT_Singleton<UT_CutSceneManager>, public Memory::MM_BaseObject
{
public:
    UT_CutSceneManager() : m_kCuller(NULL) {}
	virtual ~UT_CutSceneManager(){}

	void Init();
	void Shutdown();

	void ReloadConfig();

	void Update( float fTime );

	void SaveFbx();
	void Play();
	void Pause( bool bEnable );
	void Stop();

	bool IsPlayerMoveDisabled(){ return m_bPlay || m_bIsPlayerMoveDisabled; }
    void SetPlayerMoveDisabled(bool bIsPlayerMoveDisabled) { m_bIsPlayerMoveDisabled = bIsPlayerMoveDisabled; }
    const NiPoint3& GetPlayerLocation(void) { return m_kPlayerLocation; }
    void SetPlayerLocation(const NiPoint3& kLocation) { m_kPlayerLocation = kLocation; }

	const StringType & GetSkyDomeName(){ return m_kSkyDomeName; }
	float      GetSkyDomeZOffset(){ return m_fSkyDomeZ; }

	NiCameraPtr GetAnimCamera(){ return m_spCurrentAnimCam; }

	float GetFreeCameraMoveSpeed(){ return m_fFreeMoveSpeed; }
	float GetFreeCameraRotateSpeed(){ return m_fFreeRotSpeed; }
    long GetFreeCameraFollowPlayerID(void) { return m_lFreeCameraFollowPlayerID; }

	bool ShowPlayerName();
	bool ShowPlayerBlood();
	bool ShowNPCBlood() { return m_bNPCBlood;}
	bool ShowUI()		{return m_bShowUI;}
	bool ShowUE()		{return m_bShowUE;}
	bool ShowUECost()		{return m_bUECost;}
    bool ShowSelectionFx(void) const { return m_bShowSelectionFx; }

	bool ShowGamePlayer(){ return m_bShowGamePlayerModel; }
	bool ShowNPC(){ return m_bShowNPCModel; }

	void SetShowUE(bool kFlag)		{ m_bShowUE = kFlag;}

private:
	void _recursiveFindCameras(NiAVObject* pkSceneGraph);
#ifndef _SHIPPING
#ifndef _CLIENT_MT
#ifndef _DEBUG
	Utility::UT_FbxExportManager m_kFbxMgr;
#endif
#endif
#endif
	StringType  m_kFbxFileName;
	StringType  m_kCameraAnimName;
	int         m_nLoadingWidth;
	float       m_fFarClip;
	float       m_fNearClip;
	float       m_fFreeMoveSpeed;
	float       m_fFreeRotSpeed;
    long        m_lFreeCameraFollowPlayerID;
    bool        m_bIsPlayerMoveDisabled;
    NiPoint3    m_kPlayerLocation;
	NiCameraPtr m_spCurrentAnimCam;
	NiNodePtr   m_spAnimCamNode;
	float       m_fSkyDomeZ;
	bool        m_bPlay;
	bool        m_bPause;
	bool        m_bShowGamePlayerModel;
	bool        m_bShowNPCModel;
	bool        m_bFirstUpdate;
	float       m_fPlayTime;
	float       m_fLastTime;
	float       m_fEndTime;
	StringType  m_kSkyDomeName;

	bool		m_bPlayerName;
	bool		m_bPlayerBlood;
	bool		m_bNPCBlood;
	bool		m_bUECost;//显示关闭冒血,攻击，被攻击受伤害
	bool		m_bShowUE;//关闭所有UE,不显示玩家名称，玩家血条等等
	bool		m_bShowUI;//关闭所有UI，游戏中当前的UI界面不显示
    bool        m_bShowSelectionFx;

    Utility::UT_NiCullingProcess m_kCuller;
};