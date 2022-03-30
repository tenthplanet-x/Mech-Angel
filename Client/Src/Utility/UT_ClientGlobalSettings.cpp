#include "ClientPCH.h"
#include "UT_ClientGlobalSettings.h"

#include "RG_RenderManager.h"
#include "CA_CameraManager.h"
#include "FX_ClientFxManager.h"
#include "SE_SceneManager.h"
#include "GD_ClientAsyncModelManager.h"
#include "AS_ClientSoundManager.h"
#include "UI_AvatarManager.h"

#include "UT_LogicInfoMessage.h"


UT_ClientGlobalSettings::UT_ClientGlobalSettings()
:m_bFirstMemorySampleTime(true)
,m_stCurrentMemorySampleFrame(0)
,m_fLastMemorySampleFrameElapsed(0.0f)
,m_eStatisticsMemoryState(CS_MEM_UNKOWN)
,m_eCurrentLodLevel(CS_LOD_MAX)
,m_eTargetLodLevel(CS_LOD_MAX)
,m_eMaxLodLevel(CS_LOD_MAX)
,m_fLodLevelTimeElapsed(0.0f)
,m_bIsSimpleMode(false)
,m_bIsMiniumWindowed(false)
{
	memset( m_stMemoryWorkingSetSamples, 0, sizeof(size_t) * MAX_SAMPLE_FRAME );

	m_fLodLevelTimeDuration[CS_LOD_BASE] = 60.0f;
	m_fLodLevelTimeDuration[CS_LOD_LEVEL_2] = 30.0f;
	m_fLodLevelTimeDuration[CS_LOD_LEVEL_3] = 15.0f;
	m_fLodLevelTimeDuration[CS_LOD_MAX] = 0.0f;
}

UT_ClientGlobalSettings::~UT_ClientGlobalSettings()
{
}

void UT_ClientGlobalSettings::Analyse( float fTimeDelta, size_t stSystemWorkingSet, bool bMemorySampled )
{
	if ( bMemorySampled )
	{
		_sampleMemoryState(stSystemWorkingSet, fTimeDelta);
	}

	_updateLodLevel(fTimeDelta);
}

void UT_ClientGlobalSettings::onEventSimpleMode( bool bSimpleMode )
{
	if ( bSimpleMode )
		SetLodLevelLimit(CS_LOD_LEVEL_2);
	else
		SetLodLevelLimit(CS_LOD_MAX);

	m_bIsSimpleMode = bSimpleMode;
}

void UT_ClientGlobalSettings::onEventMinimumWindow( bool bMinimumWindow )
{
	/*if ( m_bIsSimpleMode == bMinimumWindow )
	{
		if ( bMinimumWindow )
			SetLodLevelLimit(CS_LOD_BASE);
		else
			SetLodLevelLimit(CS_LOD_MAX);
	}
	else if ( m_bIsSimpleMode != bMinimumWindow )
	{
		SetLodLevelLimit(CS_LOD_LEVEL_2);
	}*/

	m_bIsMiniumWindowed = bMinimumWindow;
}

// 采样内存
void UT_ClientGlobalSettings::_sampleMemoryState( size_t stSampledMemoryWorkingSet, float fTimeDelta )
{
	//@ 转化数据
	size_t stSampledMemoryWorkingSetInMB = (stSampledMemoryWorkingSet >> 20);

	//@ 初始化采样数据
	if ( m_bFirstMemorySampleTime )
	{
		m_bFirstMemorySampleTime = false;
		m_stCurrentMemorySampleFrame = 0;
		m_fLastMemorySampleFrameElapsed = 0.0f;
	
		int i = 0;
		while ( i < MAX_SAMPLE_FRAME )
			m_stMemoryWorkingSetSamples[i++] = stSampledMemoryWorkingSetInMB;

		m_eStatisticsMemoryState = _get_client_mmstate( stSampledMemoryWorkingSetInMB );
		_update_client_mmstate( m_eStatisticsMemoryState );

		return;
	}

	//@ 采样内存数据
	if ( m_fLastMemorySampleFrameElapsed > 0.5f )
	{
		// 采样并且计算期望值
		m_stMemoryWorkingSetSamples[m_stCurrentMemorySampleFrame] = stSampledMemoryWorkingSetInMB;
		m_stCurrentMemorySampleFrame = ( m_stCurrentMemorySampleFrame + 1 ) % MAX_SAMPLE_FRAME;

		size_t stTotalMemoryWorkingSetInMB = 0;
		size_t stAverageMemoryWorkingSetInMB = 0;

		int i = 0;
		while ( i < MAX_SAMPLE_FRAME )
			stTotalMemoryWorkingSetInMB += m_stMemoryWorkingSetSamples[i++];
		stAverageMemoryWorkingSetInMB = (stTotalMemoryWorkingSetInMB / MAX_SAMPLE_FRAME);

		// 内存状态改变触发事件
		m_eStatisticsMemoryState = _get_client_mmstate(stAverageMemoryWorkingSetInMB );
		_update_client_mmstate( m_eStatisticsMemoryState );
	}

	//@ 更新采样计时器
	m_fLastMemorySampleFrameElapsed += fTimeDelta;
}

UT_ClientGlobalSettings::MemoryState UT_ClientGlobalSettings::_get_client_mmstate( size_t mmSizeInMB )
{
	/*
		@ 根据测试目前在进程内存占用1.4G左右的时候就出现不稳定的情况了.
		其实应该更新
	*/
	MemoryState eMemState = CS_MEM_HIGH;

	if ( mmSizeInMB > 1300 )
	{
		eMemState = CS_MEM_VERYLOW;
	}
	else if ( mmSizeInMB > 1250 )
	{
		eMemState = CS_MEM_LOW;
	}
	else if ( mmSizeInMB > 1150 )
	{
		eMemState = CS_MEM_MID;
	}
	else 
	{
		eMemState = CS_MEM_HIGH;
	}

	return eMemState;
}

void UT_ClientGlobalSettings::_update_client_mmstate( MemoryState mmstate )
{
	switch ( mmstate )
	{
	case CS_MEM_HIGH:
		{
			SetLodLevel(CS_LOD_MAX);
			break;
		}
	case CS_MEM_MID:
		{
			SetLodLevel(CS_LOD_LEVEL_3);
			break;
		}
	case CS_MEM_LOW:
		{
			SetLodLevel(CS_LOD_LEVEL_2);
			break;
		}
	case CS_MEM_VERYLOW:
		{
			SetLodLevel(CS_LOD_BASE);
			break;
		}
	default:
		{
			// 不会跑到这里来的
			T_ASSERT(false);
		}
	}
}


void  UT_ClientGlobalSettings::SetLodLevel( LodLevel eLodLevel )
{
	if ( m_eTargetLodLevel == eLodLevel )
		return;

	if ( eLodLevel >= CS_LOD_BASE && 
		eLodLevel <= CS_LOD_MAX )
	{
		m_eTargetLodLevel = eLodLevel;

		if ( m_eTargetLodLevel > m_eMaxLodLevel )
			m_eTargetLodLevel = m_eMaxLodLevel;
	}
}

void  UT_ClientGlobalSettings::SetLodLevelLimit( LodLevel eLodLevelLimit )
{
	if ( eLodLevelLimit == m_eMaxLodLevel )
		return;

	if ( eLodLevelLimit >= CS_LOD_BASE && 
		eLodLevelLimit <= CS_LOD_MAX )
	{
		m_eMaxLodLevel = eLodLevelLimit;

		if ( m_eTargetLodLevel > m_eMaxLodLevel )
			m_eTargetLodLevel = m_eMaxLodLevel;
	}
}


void  UT_ClientGlobalSettings::_updateLodLevel( float fTimeDelta )
{
	m_fLodLevelTimeElapsed += fTimeDelta;

	if ( (int)m_eTargetLodLevel < (int)m_eCurrentLodLevel )
	{
		m_fLodLevelTimeElapsed = 0.0f;	
		m_eCurrentLodLevel = m_eTargetLodLevel;

		_applyLodLevel(m_eCurrentLodLevel);
	}
	else if ( (int)m_eTargetLodLevel > (int)m_eCurrentLodLevel )
	{
		if ( m_fLodLevelTimeElapsed > m_fLodLevelTimeDuration[(int)m_eCurrentLodLevel] )
		{
			m_fLodLevelTimeElapsed = 0.0f;
			m_eCurrentLodLevel = (LodLevel)((int)m_eCurrentLodLevel + 1);

			_applyLodLevel(m_eCurrentLodLevel);
		}
	}
}

void  UT_ClientGlobalSettings::_applyLodLevel( LodLevel eLodLevel )
{
	switch ( eLodLevel )
	{
	case CS_LOD_MAX:
		{
			UI_AvatarManager* pkAvatarManager = UI_AvatarManager::GetSingletonPtr();
			if ( pkAvatarManager )
				pkAvatarManager->publicCallBack_SetAutoReleaseDeactiveGroupLifeScale(1.0f);

			GD_ClientAsyncModelManager* pkModelManager = GD_ClientAsyncModelManager::GetSingletonPtr();
			if ( pkModelManager )
			{
				pkModelManager->SetDiscardedPoolCapcityScale(1.0f);
				pkModelManager->SetDiscardedPoolClearIntensityScale(1.0f);
			}

			FX_ClientFxManager* pkClientFxManager = FX_ClientFxManager::GetSingletonPtr();
			if ( pkClientFxManager )
			{
				pkClientFxManager->SetActiveElementAll(true);

				int iGrade = GetIntSetting(UT_GlobalSettings::GS_GFX_QUALITY);
				pkClientFxManager->SetCurrentGrade((FX_ClientFxManager::RenderGrade)iGrade);
			}

			break;
		}
	case CS_LOD_LEVEL_3:
		{
			UI_AvatarManager* pkAvatarManager = UI_AvatarManager::GetSingletonPtr();
			if ( pkAvatarManager )
				pkAvatarManager->publicCallBack_SetAutoReleaseDeactiveGroupLifeScale(0.75f);

			GD_ClientAsyncModelManager* pkModelManager = GD_ClientAsyncModelManager::GetSingletonPtr();
			if ( pkModelManager )
			{
				pkModelManager->SetDiscardedPoolCapcityScale(0.75f);
				pkModelManager->SetDiscardedPoolClearIntensityScale(2.0f);
			}

			FX_ClientFxManager* pkClientFxManager = FX_ClientFxManager::GetSingletonPtr();
			if ( pkClientFxManager )
			{
				pkClientFxManager->SetActiveElementAll(true);

				int iGrade = GetIntSetting(UT_GlobalSettings::GS_GFX_QUALITY) - 1;
				iGrade = ( iGrade > (int)FX_ClientFxManager::RG_BASIC ) ? iGrade : (int)FX_ClientFxManager::RG_BASIC;
				pkClientFxManager->SetCurrentGrade((FX_ClientFxManager::RenderGrade)iGrade);
			}

			break;
		}
	case CS_LOD_LEVEL_2:
		{
			UI_AvatarManager* pkAvatarManager = UI_AvatarManager::GetSingletonPtr();
			if ( pkAvatarManager )
				pkAvatarManager->publicCallBack_SetAutoReleaseDeactiveGroupLifeScale(0.5f);

			GD_ClientAsyncModelManager* pkModelManager = GD_ClientAsyncModelManager::GetSingletonPtr();
			if ( pkModelManager )
			{
				pkModelManager->SetDiscardedPoolCapcityScale(0.5f);
				pkModelManager->SetDiscardedPoolClearIntensityScale(3.0f);
			}

			FX_ClientFxManager* pkClientFxManager = FX_ClientFxManager::GetSingletonPtr();
			if ( pkClientFxManager )
			{
				pkClientFxManager->SetActiveElementAll(true);
				pkClientFxManager->SetActiveElement(FX::FER_TRACK, false);

				int iGrade = GetIntSetting(UT_GlobalSettings::GS_GFX_QUALITY) - 2;
				iGrade = ( iGrade > (int)FX_ClientFxManager::RG_BASIC ) ? iGrade : (int)FX_ClientFxManager::RG_BASIC;
				pkClientFxManager->SetCurrentGrade((FX_ClientFxManager::RenderGrade)iGrade);
			}

			break;
		}
	case CS_LOD_BASE:
		{
			UI_AvatarManager* pkAvatarManager = UI_AvatarManager::GetSingletonPtr();
			if ( pkAvatarManager )
				pkAvatarManager->publicCallBack_SetAutoReleaseDeactiveGroupLifeScale(0.25f);

			GD_ClientAsyncModelManager* pkModelManager = GD_ClientAsyncModelManager::GetSingletonPtr();
			if ( pkModelManager )
			{
				pkModelManager->SetDiscardedPoolCapcityScale(0.25f);
				pkModelManager->SetDiscardedPoolClearIntensityScale(5.0f);
			}

			FX_ClientFxManager* pkClientFxManager = FX_ClientFxManager::GetSingletonPtr();
			if ( pkClientFxManager )
			{
				pkClientFxManager->SetActiveElementAll(false);
				pkClientFxManager->SetActiveElement(FX::FER_DYNAMIC_PATCH, true);
				pkClientFxManager->SetCurrentGrade(FX_ClientFxManager::RG_BASIC);
			}

			break;
		}
	default:
		{
			// 不会跑到这里来的
			T_ASSERT(false);
		}
	}
}

int	UT_ClientGlobalSettings::GetLod_ScenePrefetchWidth()
{
	if ( m_eCurrentLodLevel == CS_LOD_MAX && 
		!g_settings.BL_ADD_CAMERA_CULL )
	{
		return g_settings.LOD_SCENE_PREFETCH_WIDTH + 1;
	}

	return g_settings.LOD_SCENE_PREFETCH_WIDTH;
}

bool UT_ClientGlobalSettings::GetLod_SceneAddCameraCull()
{
	if ( m_eCurrentLodLevel > CS_LOD_LEVEL_2 )
	{
		return g_settings.BL_ADD_CAMERA_CULL;
	}
	
	return false;
}

/// 修改桌面分辨率时回调
void UT_ClientGlobalSettings::_applySetting(bool showMessage)
{
	//能够实时生效的一些配置选项
	_updateInstantSetting();

	AS_ClientSoundManager::GetSingleton().ApplyConfigSettings();
	CA_CameraManager::GetSingleton().UpdateCameraType();

	if( IsResolutionChanged() )//|| 
		//IsSettingChanged(GS_FULLSCREEN))
	{
		/* 等待微端版本验证 */ 
		{
			// 多余的创建，跳过
			// _recreateRender();
		}
		if(!g_dllRunInBrowser)
			_recreateWindow();

		_recreateRender();
	}
    else if (
		IsSettingChanged(GS_MSAA) || 
		IsSettingChanged(GS_16BIT_BUFFER) ||
		IsSettingChanged(GS_VSYNC) )
    {
		/* 等待微端版本验证 */ 
		{
			// 现在GS_MSAA && GS_16BIT_BUFFER && GS_VSYNC都没有起效果，所以这里直接跳过
			_recreateRender();
		}
    }

	if( ( IsSettingChanged(GS_PHYSX) || 
		IsSettingChanged(GS_ENV_QUALITY) || 
		IsSettingChanged( GS_CHAR_MAT_QUALITY ) ||
		IsSettingChanged( GS_SCENE_MAT_QUALITY ) || 
		IsSettingChanged( GS_GFX_QUALITY ) || 
		IsSettingChanged( GS_WATER_QUALITY ) ) && IsValid() &&showMessage)
	{
		SendLogicInfoMessage(RE_SYSDLG_SHOWRESTARTCLIENT);
	}

	
	UT_GlobalSettings::_applySetting(showMessage);

	// 其他系统响应分辨率设置回调时候，会动到一些参数，
	// 需要复原一下
	_applyLodLevel(m_eCurrentLodLevel);
}

void UT_ClientGlobalSettings::_recreateWindow()
{
	UT_Vec3Int kRes = g_settings.GetResolution();
	g_settings.SetWindowWidth(kRes.x);
	g_settings.SetWindowHeight(kRes.y);

	if (!g_dllRunInBrowser)
	{
		SetupMainWindow(g_hWnd); 
	}
}

void UT_ClientGlobalSettings::_recreateRender()
{
	RG_RenderManager::GetSingleton().Recreate();
}


