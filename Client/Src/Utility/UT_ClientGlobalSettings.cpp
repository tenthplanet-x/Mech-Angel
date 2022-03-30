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

// �����ڴ�
void UT_ClientGlobalSettings::_sampleMemoryState( size_t stSampledMemoryWorkingSet, float fTimeDelta )
{
	//@ ת������
	size_t stSampledMemoryWorkingSetInMB = (stSampledMemoryWorkingSet >> 20);

	//@ ��ʼ����������
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

	//@ �����ڴ�����
	if ( m_fLastMemorySampleFrameElapsed > 0.5f )
	{
		// �������Ҽ�������ֵ
		m_stMemoryWorkingSetSamples[m_stCurrentMemorySampleFrame] = stSampledMemoryWorkingSetInMB;
		m_stCurrentMemorySampleFrame = ( m_stCurrentMemorySampleFrame + 1 ) % MAX_SAMPLE_FRAME;

		size_t stTotalMemoryWorkingSetInMB = 0;
		size_t stAverageMemoryWorkingSetInMB = 0;

		int i = 0;
		while ( i < MAX_SAMPLE_FRAME )
			stTotalMemoryWorkingSetInMB += m_stMemoryWorkingSetSamples[i++];
		stAverageMemoryWorkingSetInMB = (stTotalMemoryWorkingSetInMB / MAX_SAMPLE_FRAME);

		// �ڴ�״̬�ı䴥���¼�
		m_eStatisticsMemoryState = _get_client_mmstate(stAverageMemoryWorkingSetInMB );
		_update_client_mmstate( m_eStatisticsMemoryState );
	}

	//@ ���²�����ʱ��
	m_fLastMemorySampleFrameElapsed += fTimeDelta;
}

UT_ClientGlobalSettings::MemoryState UT_ClientGlobalSettings::_get_client_mmstate( size_t mmSizeInMB )
{
	/*
		@ ���ݲ���Ŀǰ�ڽ����ڴ�ռ��1.4G���ҵ�ʱ��ͳ��ֲ��ȶ��������.
		��ʵӦ�ø���
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
			// �����ܵ���������
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
			// �����ܵ���������
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

/// �޸�����ֱ���ʱ�ص�
void UT_ClientGlobalSettings::_applySetting(bool showMessage)
{
	//�ܹ�ʵʱ��Ч��һЩ����ѡ��
	_updateInstantSetting();

	AS_ClientSoundManager::GetSingleton().ApplyConfigSettings();
	CA_CameraManager::GetSingleton().UpdateCameraType();

	if( IsResolutionChanged() )//|| 
		//IsSettingChanged(GS_FULLSCREEN))
	{
		/* �ȴ�΢�˰汾��֤ */ 
		{
			// ����Ĵ���������
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
		/* �ȴ�΢�˰汾��֤ */ 
		{
			// ����GS_MSAA && GS_16BIT_BUFFER && GS_VSYNC��û����Ч������������ֱ������
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

	// ����ϵͳ��Ӧ�ֱ������ûص�ʱ�򣬻ᶯ��һЩ������
	// ��Ҫ��ԭһ��
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


