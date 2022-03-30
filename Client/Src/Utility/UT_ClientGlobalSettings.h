#pragma once
#include "UT_GlobalSettings.h"

class UT_ClientGlobalSettings : public Utility::UT_GlobalSettings
{
public:

	enum MemoryState
	{
		CS_MEM_UNKOWN = 0,
		CS_MEM_VERYLOW,
		CS_MEM_LOW,
		CS_MEM_MID,
		CS_MEM_HIGH,
	};

	enum LodLevel
	{
		CS_LOD_BASE = 0,
		CS_LOD_LEVEL_2,
		CS_LOD_LEVEL_3,
		CS_LOD_MAX,

		CS_LOD_COUNT,
	};

	static UT_ClientGlobalSettings& GetSingleton(void)
	{
		return *(static_cast<UT_ClientGlobalSettings*>(UT_GlobalSettings::GetSingletonPtr()));
	}
	static UT_ClientGlobalSettings* GetSingletonPtr(void)
	{
		return static_cast<UT_ClientGlobalSettings*>(UT_GlobalSettings::GetSingletonPtr());
	}

	UT_ClientGlobalSettings();
	~UT_ClientGlobalSettings();

public:

	void	Analyse( float fTimeDelta, size_t stSystemWorkingSet, bool bMemorySampled );

	/*事件先写在这里*/
	void	onEventSimpleMode(bool bSimpleMode);	// 简单模式
	void	onEventMinimumWindow(bool bMinimumWindow );// 最小化窗口

//-------------------------------------------------
// SampleMemoryState
//-------------------------------------------------
private:

	// 采样内存状况
	void	_sampleMemoryState( size_t stSystemWorkingSet, float fTimeDelta );

	MemoryState	_get_client_mmstate( size_t mmsizeInMB );
	void _update_client_mmstate( MemoryState mmstate );

//-------------------------------------------------
// ClientSettingLod
//-------------------------------------------------
public:

	// 获取当前lod状况
	LodLevel GetLodLevel();
	LodLevel GetLodLevelLimit();
	int GetLodLevelShift();
	
	// 根据lod情况获取参数
	// SE_SceneVisibleChunkSelector
	int	GetLod_ScenePrefetchWidth();
	bool GetLod_SceneAddCameraCull();

private:

	/* 暂时不开放这些接口 */
	void	SetLodLevel( LodLevel eLodLevel );
	void	SetLodLevelLimit( LodLevel eLodLevelLimit );
	void	SetLodLevelShift( int iLodLevelShift ); 

	void	_updateLodLevel( float fTimeDelta );
	void	_applyLodLevel( LodLevel eLodLevel );

//-------------------------------------------------
// UT_GlobalSetting
//-------------------------------------------------
private:

	void	_recreateWindow();
	void	_recreateRender();
	void	_resetPhysics();

	//应用当前设定
	/*virtual*/void	_applySetting(bool showMessage);

private:

	//@ Memory
	bool	m_bFirstMemorySampleTime;
	size_t	m_stCurrentMemorySampleFrame;
	float	m_fLastMemorySampleFrameElapsed;

	enum { MAX_SAMPLE_FRAME = 10 };
	size_t	m_stMemoryWorkingSetSamples[MAX_SAMPLE_FRAME];
	
	MemoryState	m_eStatisticsMemoryState;

	//@ ClientSettingLod
	float m_fLodLevelTimeElapsed;
	float m_fLodLevelTimeDuration[CS_LOD_COUNT];

	LodLevel m_eMaxLodLevel;
	LodLevel m_eCurrentLodLevel;
	LodLevel m_eTargetLodLevel;

	//@ other
	bool	m_bIsSimpleMode;
	bool	m_bIsMiniumWindowed;
};

#define g_clientsettings UT_ClientGlobalSettings::GetSingleton()