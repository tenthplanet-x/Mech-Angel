#include "ClientPCH.h"

#include "CF_SD_Call.h"
#include "CF_NPCSoundList.h"
#include "CF_HittedSoundList.h"
#include <NiMilesAudioSystem.h>
#include <NiMilesListener.h>
#include <NiMilesAudioSDM.h>
#include <mss.h>

#include "AS_ClientSoundManager.h"
#include "AS_SoundManagerBase.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "UT_GlobalSettings.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "CA_CameraManager.h"
#include "UI_UIManager.h"

#define NPC_BORN_SOUND_DIS 1500

using namespace CSVFile;
using namespace Utility;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct FileHandle
{
	int iSeekPos;
	NiFile* pkFile; 
};
//Miles的回调函数会在其他线程中调用，所以需要加锁，由于我们主线程保证没有文件操作
//所以只会和后台加载线程有等待关系
//但是文件的读取方式还需要优化
U32 AILCALLBACK FileOpenMilesCB( char const * filename,UINTa * file_handle )
{
	//T_PROFILE("AudioFileOpen");
	
	NiFile* pkNiFile = AS_ClientSoundManager::GetSingleton().CreateSoundFile( filename );
	if( pkNiFile )
	{
		FileHandle* pkHandle = NiAlloc( FileHandle , 1 );
		if (pkHandle)
		{
			pkHandle->iSeekPos = 0;
			pkHandle->pkFile = pkNiFile;
		}

		(*file_handle) = ( ((INT)(INT_PTR) (pkHandle) ));
		return 1;
	}

	return 0;
}
void AILCALLBACK FileCloseMilesCB( UINTa file_handle )
{
	UT_AutoLock kLock( &AS_ClientSoundManager::m_kSoundLock );
	FileHandle * fD = (FileHandle*) ((VOID *)(INT_PTR)((int)file_handle));
	if (fD)
	{
		NiDelete fD->pkFile;
		NiFree(fD);
	}
}
S32 AILCALLBACK FileSeekMilesCB( UINTa file_handle,S32 offset,U32 type )
{
	//T_PROFILE("AudioFileSeek");
	UT_AutoLock kLock( &AS_ClientSoundManager::m_kSoundLock );
	FileHandle * fD = (FileHandle*) ((VOID *)(INT_PTR)((int)file_handle));

	switch(type)
	{
	case AIL_FILE_SEEK_BEGIN:
		fD->iSeekPos = offset;
		fD->pkFile->Seek( offset , NiFile::ms_iSeekSet );
		break;

	case AIL_FILE_SEEK_END:
		fD->iSeekPos = fD->pkFile->GetFileSize() - offset ;
		fD->pkFile->Seek( fD->iSeekPos , NiFile::ms_iSeekSet );
		break;

	case AIL_FILE_SEEK_CURRENT:
		fD->iSeekPos += offset;
		fD->pkFile->Seek( offset , NiFile::ms_iSeekCur );
		break;
	};

	return fD->iSeekPos;
}
U32 AILCALLBACK FileReadMilesCB( UINTa file_handle,void * buffer,U32 bytes )
{
	//T_PROFILE("AudioFileRead");
	UT_AutoLock kLock( &AS_ClientSoundManager::m_kSoundLock );
	FileHandle * fD = (FileHandle*) ((VOID *)(INT_PTR)((int)file_handle));

	int nActually = fD->pkFile->Read( buffer , bytes );

	fD->iSeekPos += nActually;
	
	return nActually;
}

UT_Lock AS_ClientSoundManager::m_kSoundLock;
UT_Lock AS_ClientSoundManager::m_kSoundCacheLock;

AS_ClientSoundManager::AS_ClientSoundManager()
:	m_iLocalPlayerID(0)
,	m_fEnvSoundUpdateTime(0.0f)
,   m_kFileCache(16)
{
	m_bLoading = false;
	m_pkCSVSoundList   = CF_SoundObjectList::GetSingletonPtr();
	m_pkLogicAreaInfo = CF_LogicAreaInfo::GetSingletonPtr();
	m_pkCSVHittedList  = CF_HittedSoundList::GetSingletonPtr();
	m_pkCSVUISoundList = CF_UISound::GetSingletonPtr();
	m_pkCSVAllSoundList = CF_SD_Call::GetSingletonPtr();
	AIL_set_file_callbacks( FileOpenMilesCB ,
							FileCloseMilesCB,
							FileSeekMilesCB,
							FileReadMilesCB);
	m_fVolumeControl = 1.0f;
	m_uiLastBGMPos = 0;
	m_fLastVolume = 1.0f;
	m_bPrevActive = true;

	m_kFlagNPCSoundPlay = false;

	m_bNPCSoundFade = false;
}
AS_ClientSoundManager::~AS_ClientSoundManager()
{
	//析构时会自动清除m_kFileCache中的内容，见SoundFile::~SoundFile()
}
void AS_ClientSoundManager::Init( HWND hWnd )
{
	AS_SoundManagerBase::Init( hWnd ); 
	// 目录名
	StringType strWorkDir = Utility::UT_BaseStringUtility::GetRootPath(0);
	StringType strAudioDir = GfxGetDirConfig().GetDir(CF_NODE_AUDIO);

	//add by lxm
	m_kAllSoundMap.clear();
	CF_SD_Call::DataEntryMapExternIterator iter2 = m_pkCSVAllSoundList->GetIterator();
	while ( iter2.HasMoreElements() )
	{
		CF_SD_Call::DataEntry* pkData = iter2.PeekNextValuePtr();

#ifndef _SHIPPING
		bool flag = true;
		
		if ( !pkData->_strSoundName1.empty()  )
		{
			StringType fdd = strAudioDir + pkData->_strSoundName1; 
			StringType full_path = strWorkDir + fdd;
			flag = flag && Utility::IsFileExist(full_path);
		}
		if ( !pkData->_strSoundName2.empty() )
		{
			StringType fdd = strAudioDir + pkData->_strSoundName2; 
			StringType full_path = strWorkDir + fdd;
			flag = flag && Utility::IsFileExist(full_path);
		}
		if ( !pkData->_strSoundName3.empty() )
		{
			StringType fdd = strAudioDir + pkData->_strSoundName3; 
			StringType full_path = strWorkDir + fdd;
			flag = flag && Utility::IsFileExist(full_path);
		}

		if(!flag)
		{
			GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"音效表格配置错误，%s对应的文件不存在",pkData->_strKeyName.c_str());
		}
#endif
		
		const StringType & kFixKey = pkData->_strKeyName;
		m_kAllSoundMap.insert(std::make_pair(kFixKey,pkData));
		iter2.MoveNext();
	}

	_initChannel(  );
}

void AS_ClientSoundManager::UnInit()
{
	/// 异步加载队列
	for ( AudioSourceMapIter iter = m_kAsyncLoadingSource.begin(); iter != m_kAsyncLoadingSource.end(); ++iter )
	{
		NiDelete iter->second;
	}
	m_kAsyncLoadingSource.clear();
	//////////////////////////////////////////////////////////////////////////

	AudioSourceMapIter iter;
	m_fCurrentTime = NiGetCurrentTimeInSec();
	TRAV( m_kSourcesMap , iter )
	{
		NiAudioSource* pkSrc = iter->second;
		if ( pkSrc )
		{
			pkSrc->FadeToGain( MIN_VOLUME,
				m_fCurrentTime, 2.0f );
		}
	}
	while ( _getNumPlaying() > 0 )
	{
		Sleep(0);
		float fDelta = NiGetCurrentTimeInSec() - m_fCurrentTime;
		if ( fDelta > 2.1f )
		{
			break;
		}
		m_pkAudioSys->Update( NiGetCurrentTimeInSec() , true );
	}
	AS_SoundManagerBase::UnInit();
}
bool AS_ClientSoundManager::RecvMessage( MG_MessageBase& rkMessage )
{
	bool bRecv = MG_MessageHandlerInterface::RecvMessage(rkMessage);
	if ( !m_bSoundEnable )
		return bRecv;

	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_LOGIC_PLAYER_MOVE    ,_event_LogicPlayerMove);
	    MESSAGE_HANDLE(MGT_TIMER_TICK           ,_event_TimerTick)
		MESSAGE_HANDLE(MGT_LOGIC_AREA_SOUND_NAME,_event_LogicAreaSoundName)
		MESSAGE_HANDLE(MGT_WINDOWS_LOST_FOCUS   ,_on_windows_lost_focus)
		MESSAGE_HANDLE(MGT_NPC_BORN_BEGIN,		_event_NPCBornBegin)
		MESSAGE_HANDLE(MGT_NPC_DEAD_BEGIN,		_event_NPCDeadBegin)
	END_MESSAGE_HANDLE
		return true;
}

void AS_ClientSoundManager::_event_LogicAreaSoundName(MG_MessageBase& rkMessage)
{
	int nAreaID = rkMessage.m_uiParam1;
	int nMapResID = rkMessage.m_lParam2;

	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	CF_WorldMapList::DataEntry* pkData = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr(nMapResID);
	if (pkData != NULL)
	{
		const StringType & soundName = pkData->_sReplaceBGM;
		if (!soundName.empty())
		{
			FadingSoundListIter kFind =	m_kFadingSoundQueue.end();
			for ( FadingSoundListIter iterFade = m_kFadingSoundQueue.begin();
				iterFade != m_kFadingSoundQueue.end(); ++iterFade )
			{
				if (iterFade->iSoundChannel == SCT_BGM)
				{
					kFind = iterFade;
					break;
				}
			}

			if (pkGamePlayer && kFind != m_kFadingSoundQueue.end() )		
			{
				StringType tmp;
				if ( _KeyToRandomOgg(soundName, tmp) )
					kFind->strSoundName = tmp;
			}
			else if (pkGamePlayer && kFind == m_kFadingSoundQueue.end() && !m_kSoundChannel[SCT_BGM].kSoundList.empty())
			{
				if (pkGamePlayer->GetMapResID() != nMapResID 
					|| pkGamePlayer->GetMapResID() == nMapResID && m_kSoundChannel[SCT_BGM].kSoundList[0].strKeyCodeName != soundName)
				{
					//不在BOSS BGM才播放
					if ( !m_kBossFightSound.playState )
						Play2DEx( soundName, SCT_BGM, true, 0, 0.0f);
				}
			}
			else
			{
				//不在BOSS BGM才播放
				if ( !m_kBossFightSound.playState )
					Play2DEx( soundName, SCT_BGM, true, 0, 0.0f);
			}

			return;
		}
	}

	LogicAreaNameMap::const_iterator iter = m_kLogicAreaName.find( nAreaID );
	if ( iter != m_kLogicAreaName.end() )
	{
		//当前是否存在正在淡出的背景音乐
		FadingSoundListIter kFind =	m_kFadingSoundQueue.end();
		for ( FadingSoundListIter iterFade = m_kFadingSoundQueue.begin();
			iterFade != m_kFadingSoundQueue.end(); ++iterFade )
		{
			if (iterFade->iSoundChannel == SCT_BGM)
			{
				kFind = iterFade;
				break;
			}
		}

		if (pkGamePlayer && kFind != m_kFadingSoundQueue.end() )		
		{
			StringType tmp;
			if ( _KeyToRandomOgg(iter->second, tmp) )
				kFind->strSoundName = tmp;
		}
		else if (pkGamePlayer && kFind == m_kFadingSoundQueue.end() && !m_kSoundChannel[SCT_BGM].kSoundList.empty())
		{
			if (pkGamePlayer->GetMapResID() != nMapResID 
				|| pkGamePlayer->GetMapResID() == nMapResID && m_kSoundChannel[SCT_BGM].kSoundList[0].strKeyCodeName != iter->second)
			{
				//不在BOSS BGM才播放
				if ( !m_kBossFightSound.playState )
					Play2DEx( iter->second, SCT_BGM, true, 0, 0.0f);
			}
		}
		else
		{
			//不在BOSS BGM才播放
			if ( !m_kBossFightSound.playState )
				Play2DEx( iter->second, SCT_BGM, true, 0, 0.0f);
		}
	}
}

void AS_ClientSoundManager::_event_LogicPlayerMove(MG_MessageBase& rkMessage)
{
	T_PROFILE("Update::SoundManager::LogicPlayerMove");
	NiPoint3 kPlayerLocation( rkMessage.m_fParam1, rkMessage.m_fParam2, rkMessage.m_fParam3 + 500 );

	//用摄像机的位置，作为Listener位置
	NiCameraPtr spcamera = CA_CameraManager::GetSingleton().GetNiCamera();
	NiPoint3 kCameraLocation = CA_CameraManager::GetSingleton().GetListenerLocation();
	//kCameraLocation.z = 0;
	/*if ( kCameraLocation != NiPoint3::ZERO )
		m_pkAudioListener->SetWorldTranslate(kCameraLocation);
	else
		m_pkAudioListener->SetWorldTranslate(spcamera->GetWorldTranslate());*/
	m_pkAudioListener->SetWorldTranslate(kPlayerLocation);
	if ( m_iLocalPlayerID == 0 )
	{
		m_iLocalPlayerID = 
			( (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr() )->GetOperationPlayerID();
		
	}
	
	m_pkAudioListener->SetUpVector(spcamera->GetWorldUpVector());
	m_pkAudioListener->SetDirectionVector(spcamera->GetWorldDirection());

	/*LC_ClientGamePlayerManager *pkTemp = 
		(LC_ClientGamePlayerManager*)LC_ClientLogicManager::GetSingletonPtr()->GetGamePlayerManager();
	LC_PlayerBase* pkPlayer = pkTemp->FindPlayer(m_iLocalPlayerID);
	if ( pkPlayer )
	{
		NiPoint3 kDir;
		kDir.z = 0.0f;
		kDir.x = pkPlayer->GetForwardDir().x;
		kDir.y = pkPlayer->GetForwardDir().y;
		m_kDirection = kDir;
		m_pkAudioListener->SetDirectionVector( kDir);
	}*/
}
void AS_ClientSoundManager::_event_TimerTick( MG_MessageBase& rkMessage )
{
	T_PROFILE( "Update::SoundManager"); 
	m_fCurrentTime = rkMessage.m_fParam1;
	float fDeltaTime = rkMessage.m_fParam2;

	this->_updateMetrics();

	m_pkAudioSys->Update(m_fCurrentTime, true);

	
	_updateFadingSoundQueue();
	
	m_fEnvSoundUpdateTime += fDeltaTime;

	if ( m_fEnvSoundUpdateTime > 0.1f)
	{
		m_fEnvSoundUpdateTime = 0.0f;		
		_updateEnvSound();
		_filterStaticSound();
	}
	//Process Autoplay sounds
	SoundInfoList& rkAutoPlay = m_kSoundChannel[SCT_2D].kSoundList;
	for( SoundInfoListIter iter = rkAutoPlay.begin() ; iter!= rkAutoPlay.end(); )
	{
		int nID = iter->iSourceID;
		float fStopTime = iter->fPlayTime;
		if ( fStopTime > 0.0f && fStopTime < m_fCurrentTime )
		{
			AS_SoundManagerBase::StopAudioSource( nID );
			iter = rkAutoPlay.erase( iter );
		}
		else
		{
			++iter;
		}
	}
	//BGM Loop check
	SoundChannel& rkChannel = m_kSoundChannel[SCT_BGM];
	if( !rkChannel.kSoundList.empty() )
	{
		int nID = rkChannel.kSoundList[0].iSourceID;

		NiAudioSource* pkSrc = _getSound( nID );
		if ( pkSrc )
		{
			if ( pkSrc->GetStatus() == NiAudioSource::NOT_SET)
			{
				_stop( nID );
				rkChannel.fLastAddTime = m_fCurrentTime;
				rkChannel.kSoundList.pop_back();
			}
		}
	}
	else if ( rkChannel.fLastAddTime + rkChannel.fPlayInterval < m_fCurrentTime )
	{
		Play2DEx( rkChannel.kLastPlayKeyCode, SCT_BGM, true, 0, 0.0f);
	}
	
//player_skill sound
	//主要是清理 离开视野的循环音效
	SoundInfoList& soundList = m_kSoundChannel[SCT_PLAYER_SKILL].kSoundList;
	LC_ClientGamePlayer* gameplayerPtr = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	int soundID = 0;
	for( SoundInfoListIter iter = soundList.begin() ; iter!= soundList.end(); )
	{
		soundID = iter->iSourceID;
		NiAudioSource* sourcePtr = _getSound(soundID);
		if (NULL != sourcePtr)
		{
			if ( NULL == gameplayerPtr
				|| UT_MathBase::LineLengthXYInt(gameplayerPtr->GetCurrentLocation(), UT_EngineMath::ConvertNiPoint3ToVec3Int(sourcePtr->GetTranslate())) > g_settings.SOUND_COMMON_MAX_DISTANCE + 100
				)
			{
				_stop(soundID);
			}
			++iter;
		}
		else
		{
			iter = soundList.erase(iter);
		}
	}
//--player_skill sound
	//NPC sound

	if(_updateNPCSoundSeq())
		return ;
	
	SoundChannel& rkNPCChannel = m_kSoundChannel[SCT_NPC];
	if (!rkNPCChannel.kSoundList.empty())
	{
		int nID = rkNPCChannel.kSoundList[0].iSourceID;
		NiAudioSource* pkSrc = _getSound( nID );
		if ( pkSrc )
		{
			if ( /*pkSrc->GetGain() < MIN_VOLUME + 0.01f*/ pkSrc->GetStatus() == NiAudioSource::DONE )
			{
				AudioSourceMapIter iter =m_kSourcesMap.find( nID );
				NiDelete iter->second;
				m_kSourcesMap.erase( iter );
				rkNPCChannel.kSoundList.pop_back();
				OnEndPlayNPCSound();
			}
		}
		else
		{
			rkNPCChannel.kSoundList.pop_back();
		}
	}
	
	//如果有语音播放失败，只要
	if( m_kFlagNPCSoundPlay && rkNPCChannel.kSoundList.empty() )
	{
		OnEndPlayNPCSound();
		m_kFlagNPCSoundPlay = false;
	}


	LC_IDList idvec;
	float fCurrentTime = GET_PROCESS_TIME;
	//出生语音，BGM切换，死亡语音
	for(NPCBornSoundMap::iterator ite = m_kNPCBornSoundMap.begin(); ite != m_kNPCBornSoundMap.end(); ++ite)
	{
		NPCBornSoundData& cbdata = ite->second;
		//从表格中读取
		CSVFile::CF_NPCSoundList::DataEntry *pkSoundData = SafeGetCSVFileDataEntryPtr<CSVFile::CF_NPCSoundList>(cbdata.iCharTypeID);
		if (!pkSoundData)
		{
			idvec.push_back(cbdata.iID);
			continue ;
		}

		//出生语音
		if ( !pkSoundData->_kBornSound.empty() )
		{	
			bool kFlag = true;
			NPCBornSoundMap::iterator fd = m_kNPCBornVoiceMap.find(cbdata.iID);
			if ( fd != m_kNPCBornVoiceMap.end() )
			{
				NPCBornSoundData& sd = fd->second;
				kFlag = (fCurrentTime - sd.iTimeTick) > 900.0f;
			}
			if ( kFlag  )
			{		
				Play2DEx(pkSoundData->_kBornSound, SCT_NPC);

				OnPlayNPCSound(g_settings.SOUND_LOWER_BGM_FOR_NPCSOUND, cbdata.iCharTypeID);

				NPCBornSoundData sd = cbdata;
				sd.iTimeTick = fCurrentTime;
				m_kNPCBornVoiceMap[cbdata.iID] =	sd;
			}
		}

		//背景音乐逻辑
		if (!m_kBossFightSound.playState && !pkSoundData->_kReplaceBGM.empty())
		{ 
			//BGM_Boss_Common 
			if (!m_kSoundChannel[SCT_BGM].kSoundList.empty())
			{
				m_kBossFightSound.lastPlayName = m_kSoundChannel[SCT_BGM].kSoundList[0].strKeyCodeName;
			}
			m_kBossFightSound.bossID = cbdata.iID;
			m_kBossFightSound.lastFadeTime = m_kSoundChannel[SCT_BGM].fFadeTime;
			m_kBossFightSound.lastInterval = m_kSoundChannel[SCT_BGM].fPlayInterval;
			m_kSoundChannel[SCT_BGM].fFadeTime = g_settings.SOUND_BGM_BOSS_FADE_TIME;
			m_kSoundChannel[SCT_BGM].fPlayInterval = 0.0f;
			Play2DEx( pkSoundData->_kReplaceBGM, SCT_BGM, true, 0, 0.0f);
			m_kBossFightSound.playState = true;
		}

		idvec.push_back(cbdata.iID);
	}

	for( LC_IDList::iterator ite = idvec.begin(); ite != idvec.end(); ++ite)
	{
		m_kNPCBornSoundMap.erase(*ite);
	}
}

void AS_ClientSoundManager::_event_NPCBornBegin(MG_MessageBase& rkMessage)
{
	object_id_type	lNpcID = rkMessage.m_lParam1;
	int				lNpcCharType = rkMessage.m_lParam2;

	//从表格中读取
	CSVFile::CF_NPCSoundList::DataEntry *pkSoundData = SafeGetCSVFileDataEntryPtr<CSVFile::CF_NPCSoundList>(lNpcCharType);
	if (!pkSoundData)
	{
		return ;
	}

	//记录，在update中处理
	if ( m_kNPCBornSoundMap.size() < 10 )
	{
		NPCBornSoundData data;
		data.iCharTypeID = lNpcCharType;
		data.iID		= lNpcID;
		data.iTimeTick	= 0;
		m_kNPCBornSoundMap[lNpcID] = data;
	}
}

void AS_ClientSoundManager::_event_NPCDeadBegin(MG_MessageBase& rkMessage)
{
	object_id_type	lNpcID = rkMessage.m_lParam1;
	int				lNpcCharType = rkMessage.m_lParam2;
	//从表格中读取
	CSVFile::CF_NPCSoundList::DataEntry *pkSoundData = SafeGetCSVFileDataEntryPtr<CSVFile::CF_NPCSoundList>(lNpcCharType);
	if (!pkSoundData)
	{
		return ;
	}

	//播放死亡语音
	{
		if (!pkSoundData->_kDeadSound.empty())
		{
			Play2DEx(pkSoundData->_kDeadSound, SCT_NPC);
		}	
	}

	//背景音乐逻辑
	{
		if (!m_kBossFightSound.playState)
			return ;
	
		if (m_kBossFightSound.bossID == lNpcID)
		{
			Play2DEx(m_kBossFightSound.lastPlayName, SCT_BGM, true, 0, 0.0f);
			m_kSoundChannel[SCT_BGM].fFadeTime = m_kBossFightSound.lastFadeTime;
			m_kSoundChannel[SCT_BGM].fPlayInterval = m_kBossFightSound.lastInterval;
			m_kBossFightSound.playState = false;
		}	
	}
}

void AS_ClientSoundManager::ClearBossFightInfo()
{
	m_kBossFightSound.clear();

	m_kNPCBornBGMMap.clear();
	m_kNPCBornSoundMap.clear();
	m_kNPCBornVoiceMap.clear();
}

bool AS_ClientSoundManager::_fade2DSound(int iAudioID, const StringType& rkName,
										 float fVolume, const StringType& rkLastName , 
										 float fFadeTime,
										 int iChannel)
{
// 	if ( rkName == "" || rkName == "xx" || rkName == "NULL" ) 
// 	{
// 		NiAudioSource *pkTemp = _getSound(iAudioID);
// 		if (pkTemp == NULL)
// 		{
// 			return false;
// 		}
// 		pkTemp->FadeToGain(MIN_VOLUME, m_fCurrentTime, fFadeTime);
// 		return true;
// 	}
	if ( iAudioID != INVALID_ID )
	{
		bool bSuccess = _addFadingTask(iAudioID,
			rkName, iChannel, fVolume, 
			NiAudioSource::LOOP_INFINITE,fFadeTime);
		if ( !bSuccess )
		{
			return false;
		}
		return true;
	}
	return false;
}

void AS_ClientSoundManager::_filterStaticSound()
{
	float fDistance = m_kSoundChannel[SCT_ENV].fDistance;
	
	CF_SoundObjectList::DataEntryMapExternIterator iter = m_pkCSVSoundList->GetIterator();
	while ( iter.HasMoreElements() )
	{
		const CF_SoundObjectList::DataEntry* pkData = iter.PeekNextValuePtr();
		NiPoint3 kListenerPos = m_pkAudioListener->GetWorldTranslate();
		if ( _trimFunc( 
			kListenerPos , 
			UT_EngineMath::ConvertVec3IntToNiPoint3( pkData->_kCenter ) , 
			fDistance , fDistance  ) )
		{
			if( pkData->_iCount < 1 )
			{
				GfxWriteLog( LOG_WARNING_FILELINE , LOG_SWITCH_WARNING ,
					"地图%d的音效球%d没有有效的音效文件名！" , 
					pkData->_iMapID , pkData->_iID );
			}
			float fMinDistance = pkData->_fRadius/g_settings.SOUND_OBJECT_FALLOFF_SCALE;
			bool bLoop = false;
			if ( pkData->_fFreq == 1.0f )
			{
				bLoop = true;
			}
			else
			{
				m_kSoundChannel[SCT_ENV].fPlayInterval = pkData->_fFreq;
			}
			int nID = Play3DEx( UT_EngineMath::ConvertVec3IntToNiPoint3( pkData->_kCenter ), 
			pkData->_kName_1 , SCT_ENV ,true , bLoop , fMinDistance , pkData->_fRadius );
			break;
		}
		iter.MoveNext();
	}
}

void AS_ClientSoundManager::_updateEnvSound()
{
	// 建立Env音效的淡出队列，当音效降低为0.0f，清除m_kSourcesMap
	FadedEnvSoundListIter FadedIter;
	for ( FadedIter = m_kFadedEnvSoundList.begin(); FadedIter != m_kFadedEnvSoundList.end(); )
	{
		NiAudioSource* pkSrc = _getSound( *FadedIter );
		if ( pkSrc == NULL )
		{
			FadedIter = m_kFadedEnvSoundList.erase( FadedIter );
		}
		else if ( pkSrc->GetGain() < MIN_VOLUME + 0.01f  )
		{
			AudioSourceMapIter iter =m_kSourcesMap.find( *FadedIter );
			NiDelete iter->second;
			m_kSourcesMap.erase( iter );
			FadedIter = m_kFadedEnvSoundList.erase( FadedIter );
		}
		else
		{
			++FadedIter;
		}
	}

	NiPoint3 kListenerPos = m_pkAudioListener->GetWorldTranslate();
	SoundInfoList& rkSounds = m_kSoundChannel[SCT_ENV].kSoundList;
	float fFadeTime = m_kSoundChannel[SCT_ENV].fFadeTime;
	float fDistance = m_kSoundChannel[SCT_ENV].fDistance;
	SoundInfoListIter iter;
	for ( iter = rkSounds.begin() ; iter != rkSounds.end(); )
	{
		NiAudioSource* pkSrc = _getSound( iter->iSourceID );
		if ( pkSrc == NULL )
		{
			iter = rkSounds.erase( iter );
		}
		else
		{
			bool bRunAway = !_trimFunc(
				kListenerPos, 
				pkSrc->GetWorldTranslate(), 
				fDistance, fDistance);
			bool bFinished = ((pkSrc->GetStatus() == NiAudioSource::DONE) || (pkSrc->GetStatus() == NiAudioSource::NOT_SET));
			if ( bFinished || bRunAway )
			{
				AS_SoundManagerBase::StopAudioSource( iter->iSourceID , true ,fFadeTime  );
				m_kFadedEnvSoundList.push_back(iter->iSourceID);
				iter = rkSounds.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}

bool AS_ClientSoundManager::_updateNPCSoundSeq()
{
	if (m_kNPCSoundSeqData.IsValid())
	{
		SoundChannel& rkNPCChannel = m_kSoundChannel[SCT_NPC];
		if (!rkNPCChannel.kSoundList.empty())
		{
			int nID = rkNPCChannel.kSoundList[0].iSourceID;
			NiAudioSource* pkSrc = _getSound( nID );
			if ( pkSrc )
			{
				SoundSeq seq = m_kNPCSoundSeqData.GetCurrentSeq();
				if ( m_kNPCSoundSeqData.m_playTick != 0.0f && seq.fTimeLength != 0.0f && (GET_PROCESS_TIME - m_kNPCSoundSeqData.m_playTick) > seq.fTimeLength)
				{
					//Play2DEx(m_kNPCSoundSeqData.GetCurrentKeyCode(), SCT_NPC);
					//m_kNPCSoundSeqData.m_kCurrentIndex++;
					rkNPCChannel.kSoundList.erase(rkNPCChannel.kSoundList.begin());
					_stop(nID);
				}
			}
		}
		else
		{
			SoundSeq seq = m_kNPCSoundSeqData.GetCurrentSeq();
			if (seq.fTimeLength != 0.0f)
			{
				Play2DEx(seq.keyCode, SCT_NPC);
				m_kNPCSoundSeqData.m_kCurrentIndex++;
				m_kNPCSoundSeqData.m_playTick = GET_PROCESS_TIME;
			}
		}
		return true;
	}

	return false;
}

bool AS_ClientSoundManager::_trimFunc(const NiPoint3& kListenerPos,const NiPoint3& kSoundPlayPos, float fX, float fY )
{
	return (((int)kListenerPos.x - fX) < kSoundPlayPos.x && 
		kSoundPlayPos.x < ((int)kListenerPos.x + fX)) && 
		(((int)kListenerPos.y - fY) < kSoundPlayPos.y && 
		kSoundPlayPos.y < ((int)kListenerPos.y + fY));
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool AS_ClientSoundManager::_addFadingTask(int iID, 
										   const StringType& strSoundName,
										   int iChannel,
										   float fVolume , int iPlayCount , 
										   float fFadeTime)
{
	_stop(iID, true , fFadeTime );

	FadingSoundDescriptor rkTemp;
	rkTemp.iAudioID     = iID;
	rkTemp.strSoundName = strSoundName;
	rkTemp.iPlayCount	= iPlayCount;
	rkTemp.fVolume = fVolume;
	rkTemp.fFadeTime = fFadeTime;
	rkTemp.iSoundChannel = iChannel;
	if ( _getSound( iID ) != NULL )
	{
		m_kFadingSoundQueue.push_back( rkTemp );
		return true;
	}
	else
	{
		GfxWriteLog( LOG_TAG_WARNING , LOG_SWITCH_SOUND ,
			"Can't fading sound %d with file name %s" ,
			iID , strSoundName.c_str() );
		return false;
	}
}
void AS_ClientSoundManager::_updateFadingSoundQueue()
{
//  	if ( !g_settings.SOUND_ENABLE )
//  		return;

	for ( FadingSoundListIter iter = m_kFadingSoundQueue.begin();
		iter != m_kFadingSoundQueue.end();  )
	{
		FadingSoundDescriptor& rkFadingSound = *iter;
		NiAudioSource* pkAudioSource = _getSound( rkFadingSound.iAudioID );
		if ( pkAudioSource == NULL )
		{
			iter = m_kFadingSoundQueue.erase(iter);
			continue;
		}
		float fTemp = pkAudioSource->GetGain();
		if ( fTemp <= MIN_VOLUME+0.01f )
		{
			pkAudioSource->Stop();
			bool isBGM = rkFadingSound.iSoundChannel == SCT_BGM;
			if (isBGM && rkFadingSound.strSoundName != "")
			{
				bool bFade = rkFadingSound.fFadeTime == 0.0f ? false : true;
				bool bSuccess = _playSound( pkAudioSource,
					rkFadingSound.strSoundName, 
					rkFadingSound.fVolume, 
					bFade , rkFadingSound.iPlayCount,
					g_settings.SOUND_COMMON_MIN_DISTANCE , g_settings.SOUND_COMMON_MAX_DISTANCE , rkFadingSound.fFadeTime );
				if ( !bSuccess )
				{
					_stop( rkFadingSound.iAudioID );
				}
			}
			else
			{
				_stop(rkFadingSound.iAudioID);
			}
#ifndef _USE_STL_PORT
			iter = m_kFadingSoundQueue.erase(iter);
#else
			m_kFadingSoundQueue.erase(iter++);
#endif
			if (m_bNPCSoundFade && isBGM)
			{
				ApplyBGMVolumeChange();
			}
		}
		else
		{
			++iter;
		}
	}
}

bool AS_ClientSoundManager::_KeyToRandomOgg(const StringType& ke, StringType& ogg)
{
	AllSoundMap::iterator findIter = m_kAllSoundMap.find(ke);
	if (findIter == m_kAllSoundMap.end())
	{
		return false;
	}
	CF_SD_Call::DataEntry* entry = findIter->second;
	if (entry)
	{
		StringType buff[3] = { entry->_strSoundName1, entry->_strSoundName2, entry->_strSoundName3};
		ogg = GetRandomPlayName(buff);
		return true;
	}
	return false;
}

//-------------------------------   System     ----------------------------------
void AS_ClientSoundManager::_apply_volume_all(float fVolume)
{
	fVolume = MAX_VOLUME < fVolume ? MAX_VOLUME : fVolume;
	fVolume = MIN_VOLUME > fVolume ? MIN_VOLUME : fVolume;

	for (AudioSourceMapIter iter = m_kSourcesMap.begin(); iter != m_kSourcesMap.end(); ++iter)
	{
		NiAudioSource* pkAudioSource = iter->second;
		pkAudioSource->SetGain( fVolume );
	}
}
//-------------------------------   Source     --------------------------------
bool AS_ClientSoundManager::SetSoundVolume(int iID, float fVolume)
{
	fVolume = MAX_VOLUME < fVolume ? MAX_VOLUME : fVolume;
	fVolume = MIN_VOLUME > fVolume ? MIN_VOLUME : fVolume;

	bool bSet = false;
	NiAudioSource *pkAudioSource = _getSound( iID );
	if ( pkAudioSource != NULL )
	{
		bSet = pkAudioSource->SetGain( fVolume );
	}

	return bSet;
}

bool AS_ClientSoundManager::FadeSoundVolume(int iID, float fVolume, float fadeTime /*= 0.5f*/)
{
	fVolume = MAX_VOLUME < fVolume ? MAX_VOLUME : fVolume;
	fVolume = MIN_VOLUME > fVolume ? MIN_VOLUME : fVolume;

	bool bSet = false;
	NiAudioSource *pkAudioSource = _getSound( iID );
	if ( pkAudioSource != NULL )
	{
		bSet = pkAudioSource->FadeToGain( fVolume , m_fCurrentTime, fadeTime);
	}

	return bSet;
}

int AS_ClientSoundManager::Play3D(const NiPoint3& kPos, 
								  const StringType& strSoundName, 
								  const StringType& strKeyCodeName,
								  Base::Importance ePriority,
								  bool bLoop, float fVolume /*=1.0f*/)
{
	/*
	GamePlayer is IM_VERY_IMPORTANT
	NetPlayer is IM_NORMAL
	Monster NPC Chest is IM_UNIMPORTANT
	*/
	float minDistance = g_settings.SOUND_COMMON_MIN_DISTANCE;
	float maxDistance = g_settings.SOUND_COMMON_MAX_DISTANCE;

	SOUND_CHANNEL_TYPE eChannel;
	switch( ePriority )
	{
	case Base::IM_VERY_IMPORTANT:
		eChannel = SCT_PLAYER_SKILL;
		break;
	case Base::IM_NORMAL:
		eChannel = SCT_NET_PLAYER_SKILL;
		break;
	case Base::IM_UNIMPORTANT:
		eChannel = SCT_MONSTER_SKILL;
		break;
	default:
		eChannel = SCT_MONSTER_SKILL;
		break;
	}
	//if( strSoundName.find("Voice") != StringType::npos)
	//{
	//	eChannel = SCT_NPC;
	//}
	//float fDistance = m_kSoundChannel[eChannel].fDistance;

	if (eChannel == SCT_MONSTER_SKILL)
	{
		minDistance = g_settings.SOUND_MONSTER_SKILL_MIN_DISTANCE;
		maxDistance = g_settings.SOUND_MONSTER_SKILL_MAX_DISTANCE;
	}

	return Play3D( kPos , strSoundName ,strKeyCodeName, eChannel , false , bLoop , 
		minDistance, maxDistance, fVolume);

}

int AS_ClientSoundManager::Play3D(const NiPoint3& kPos, 
								  const StringType& strSoundName,
								  const StringType& strKeyCodeName,
								  SOUND_CHANNEL_TYPE eChannelType,
								  bool bFadeToGain, 
								  bool bLoop , float fMinDist , float fMaxDist,
									float ffVolume/*=1.0f*/)
{	
	T_PROFILE("AS_SoundManager::Play3D");
	if ( strSoundName.empty() )
	{
		GfxWriteLog( LOG_TAG_WARNING , LOG_SWITCH_SOUND , 
			"音效文件名为空，请检查资源, %d, %s", eChannelType, strKeyCodeName.c_str());
		return INVALID_ID;
	}
	if ( !m_bSoundEnable || m_bLoading )
		return INVALID_ID_RULE_FAIL; 
	if( eChannelType == SCT_MAX )
		return INVALID_ID;
	
	int nLoop = bLoop ? 
		NiAudioSource::LOOP_INFINITE : NiAudioSource::LOOP_SINGLE;

// 	NiPoint3 kListenerPos = m_pkAudioListener->GetWorldTranslate();
// 	if ( !_trimFunc(kListenerPos, kPos, 
// 		m_kSoundChannel[eChannelType].fDistance, 
// 		m_kSoundChannel[eChannelType].fDistance) )
// 	{
// 		return INVALID_ID;
// 	}
	float fVolume = m_kSoundChannel[eChannelType].fVolume * m_fVolumeControl * ffVolume; 
	if ( !_checkPlayingRules( eChannelType , strSoundName, strKeyCodeName, fVolume ) )
	{
		return INVALID_ID_RULE_FAIL;
	}

	int iID = _createSound(NiAudioSource::TYPE_3D, true , false );

	NiAudioSource* pkAudioSource = _getSound(iID);
	if (pkAudioSource == NULL)
		return INVALID_ID;
	NiPoint3 kNewPos = kPos /* + m_pkAudioListener->GetDirectionVector() * 200.0f*/;
	//kNewPos.z = 0;
	//kNewPos += kPos;
	float fFadeTime = m_kSoundChannel[eChannelType].fFadeTime;
	pkAudioSource->SetTranslate(kNewPos.x, kNewPos.y, kNewPos.z );
	//如果是ogg的音效不能
	if ( strSoundName.find(".ogg")!= StringType::npos )
	{
		pkAudioSource->SetStreamed( false ); 
	}

	//*
	// 同步加载
	bool bSuccess = _playSound(pkAudioSource, strSoundName,
		fVolume, bFadeToGain, nLoop , fMinDist, fMaxDist, fFadeTime );
	if ( !bSuccess )
	{
		_stop( iID );
		return INVALID_ID;
	}
	SoundInfo kNewSound;
	kNewSound.iSourceID = iID;
	kNewSound.strName = strSoundName;
	kNewSound.strKeyCodeName = strKeyCodeName;
	kNewSound.fPrevVolume = ffVolume;
	m_kSoundChannel[eChannelType].kSoundList.push_back( kNewSound );
	/*/
	// 异步加载
	AudioLoadArg	arg(pkAudioSource, strSoundName,
		fVolume, bFadeToGain, nLoop , fMinDist, fMaxDist, fFadeTime, eChannelType, 300);
	SoundInfo kNewSound;
	kNewSound.iSourceID = iID;
	kNewSound.strName = strSoundName;
	kNewSound.strKeyCodeName = strKeyCodeName;
	kNewSound.fPrevVolume = ffVolume;

	this->PlaySound_AsyncLoad(iID, &arg, &kNewSound);
	//*/

	return iID;
}
void AS_ClientSoundManager::PlayHittedSound( const NiPoint3& kPos, 
											const StringType& rstrFrom, 
											const StringType& rstrTo )
{
	T_PROFILE("AS_SoundManager::PlayHittedSound");
	if ( m_pkCSVHittedList == NULL )
		return;
	CF_HittedSoundList::DataEntryMapExternIterator iter = m_pkCSVHittedList->GetIterator();
	while ( iter.HasMoreElements() )
	{
		const CF_HittedSoundList::DataEntry* pkData = iter.PeekNextValuePtr();
		if ( rstrFrom == pkData->_strFrom && rstrTo == pkData->_strTo )
		{
			const StringType & strTemp = pkData->_strSoundName;
			AllSoundMap::iterator findIter = m_kAllSoundMap.find(strTemp);
			if (findIter == m_kAllSoundMap.end())
			{
				return ;
			}
			NS_CF_SD_Call::DataEntry* pkEntry = findIter->second;
			if (pkEntry)
			{
				//判断概率
				bool bFlag = true;
				if (pkEntry->_fPercent < 1.0f && pkEntry->_fPercent < UT_MathBase::RandFloat01())
				{
					bFlag = false;
				}
				if (bFlag)	
				{	
					int iRand = 0;
					StringType buff[3] = { pkEntry->_strSoundName1, pkEntry->_strSoundName2, pkEntry->_strSoundName3};
					StringType finalPlayName = GetRandomPlayName(buff);
					Play3D( kPos, finalPlayName ,strTemp,SCT_PLAYER_HITTED, false, false, g_settings.SOUND_COMMON_MIN_DISTANCE, g_settings.SOUND_COMMON_MAX_DISTANCE, _randomSoundVolume(pkEntry));
				}
				break;
			}
		}
		iter.MoveNext();
	}
}

void AS_ClientSoundManager::ApplyConfigSettings()
{
	if ( !m_bSoundEnable )	//灰化处理音效界面？
		return;

	_initChannel(  );
	if ( !g_settings.SOUND_ENABLE )
	{
		_apply_volume_all( MIN_VOLUME );	
		return;
	}

	//BGM
	if ( !m_kSoundChannel[SCT_BGM].kSoundList.empty() )
	{
		SetSoundVolume( m_kSoundChannel[SCT_BGM].kSoundList[0].iSourceID,
			m_fVolumeControl * m_kSoundChannel[SCT_BGM].kSoundList[0].fPrevVolume * m_kSoundChannel[SCT_BGM].fVolume * m_kSoundChannel[SCT_BGM].extenalAffect);
	}
	
	SoundInfoList& rkEnvSound = m_kSoundChannel[SCT_ENV].kSoundList;
	SoundInfoListIter iter;
	TRAV( rkEnvSound , iter )
	{
		NiAudioSource* pkSrc = _getSound( iter->iSourceID );
		if ( pkSrc )
		{
			SetSoundVolume( iter->iSourceID , iter->fPrevVolume * m_fVolumeControl * m_kSoundChannel[SCT_ENV].fVolume * m_kSoundChannel[SCT_ENV].extenalAffect);
		}
	}
}

void AS_ClientSoundManager::ApplyBGMVolumeChange()
{
	if ( !m_kSoundChannel[SCT_BGM].kSoundList.empty() )
	{
		bool fret  = false;
		for( FadingSoundList::iterator it = m_kFadingSoundQueue.begin(); it < m_kFadingSoundQueue.end(); ++it)
		{
			if ( (*it).iSoundChannel == SCT_BGM) 
			{
				fret = true;
				break;
			}
		}

		if ( !fret)
		{
			m_bNPCSoundFade = false;
			FadeSoundVolume( m_kSoundChannel[SCT_BGM].kSoundList[0].iSourceID,
				m_fVolumeControl * m_kSoundChannel[SCT_BGM].kSoundList[0].fPrevVolume * m_kSoundChannel[SCT_BGM].fVolume * m_kSoundChannel[SCT_BGM].extenalAffect);
		}
		else
		{
			m_bNPCSoundFade = true;
		}
	}
}

int AS_ClientSoundManager::Play2D(const StringType& strSoundName, const StringType& strKeyCodeName,float fVolume, bool bLoop /*= false*/)
{
    return Play2D(strSoundName, strKeyCodeName, SCT_2D, false, bLoop ? 0 : 1);
}

int AS_ClientSoundManager::Play2D(const StringType& strSoundName, 
								  const StringType& strKeyCodeName,
								  SOUND_CHANNEL_TYPE eChannelType, 
								  bool bFadeToGain /*= false*/, 
								  int  nPlayCount ,
								  float fAutoStop /*= 0.0f*/ )
{
	if ( m_bLoading && eChannelType != SCT_BGM )
	{
		return INVALID_ID;
	}
	if ( !m_bSoundEnable )
		return INVALID_ID; 
	if( eChannelType == SCT_MAX )
		return INVALID_ID;
	float fVolume = m_kSoundChannel[eChannelType].fVolume * m_fVolumeControl;
	if ( !_checkPlayingRules( eChannelType , strSoundName ,strKeyCodeName, fVolume ) )
	{
		return INVALID_ID;
	}
	int nLoop = NiAudioSource::LOOP_SINGLE;
	if ( nPlayCount == 0 || nPlayCount > 1 )
	{
		nLoop = NiAudioSource::LOOP_INFINITE;
	}
	bool bStreamed = false;
	if ( eChannelType == SCT_BGM || eChannelType == SCT_2D || eChannelType == SCT_AREA_EFFECT || eChannelType == SCT_NPC )
	{
		bStreamed = true;
	}
	
	int nAudioID = _createSound(NiAudioSource::TYPE_AMBIENT, true , bStreamed );

	NiAudioSource* pkAudioSource = _getSound(nAudioID);
	float fFadeTime = m_kSoundChannel[eChannelType].fFadeTime;
	bool bSuccess = _playSound( pkAudioSource, strSoundName, fVolume, 
		bFadeToGain , nLoop , g_settings.SOUND_COMMON_MIN_DISTANCE, g_settings.SOUND_COMMON_MAX_DISTANCE,
		fFadeTime );
	if ( !bSuccess )
	{
		_stop(nAudioID);
		return INVALID_ID;
	}
	float fTotalPlayTime = 0.0f;
	if ( nPlayCount > 1 && fAutoStop == 0.0f &&
		 pkAudioSource->GetPlayLength( fTotalPlayTime ) )
	{
		//如果设置了播放次数，那么就根据当前的长度来计算一个Autostop的时间
		fAutoStop = nPlayCount * fTotalPlayTime;
	}
	SoundInfo kNewSound;
	kNewSound.iSourceID = nAudioID;
	kNewSound.strName = strSoundName;
	kNewSound.strKeyCodeName = strKeyCodeName;
	if ( fAutoStop != 0.0f )
	{
		kNewSound.fPlayTime = m_fCurrentTime + fAutoStop;
	}
	else
	{
		kNewSound.fPlayTime = 0.0f;
	}
	m_kSoundChannel[eChannelType].kSoundList.push_back( kNewSound );
	m_kSoundChannel[eChannelType].fLastAddTime = m_fCurrentTime;
	m_kSoundChannel[eChannelType].kLastPlayKeyCode = strKeyCodeName;

	return nAudioID;
}

int AS_ClientSoundManager::Play2DWithVolume(const StringType& strSoundName,
										const StringType& strKeyCodeName,
								  SOUND_CHANNEL_TYPE eChannelType, 
								  float ffVolume /*= 1.0f*/,
								  bool bFadeToGain /*= false*/, 
								  int  nPlayCount ,
								  float fAutoStop /*= 0.0f*/ )
{
	if ( m_bLoading && eChannelType != SCT_BGM )
	{
		return INVALID_ID;
	}
	if ( !m_bSoundEnable )
		return INVALID_ID; 
	if( eChannelType == SCT_MAX )
		return INVALID_ID;
	float fVolume = m_kSoundChannel[eChannelType].fVolume * m_fVolumeControl * ffVolume;
	if ( !_checkPlayingRules( eChannelType , strSoundName ,strKeyCodeName, fVolume ) )
	{
		return INVALID_ID;
	}
	int nLoop = NiAudioSource::LOOP_SINGLE;
	if ( nPlayCount == 0 || nPlayCount > 1 )
	{
		nLoop = NiAudioSource::LOOP_INFINITE;
	}
	bool bStreamed = false;
	if ( eChannelType == SCT_BGM || eChannelType == SCT_2D || eChannelType == SCT_AREA_EFFECT || eChannelType == SCT_NPC )
	{
		bStreamed = true;
	}

	//对背景音乐施加外部影响
	if (eChannelType == SCT_BGM)
	{
		fVolume = fVolume * m_kSoundChannel[eChannelType].extenalAffect;
	}

	int nAudioID = _createSound(NiAudioSource::TYPE_AMBIENT, true , bStreamed );

	NiAudioSource* pkAudioSource = _getSound(nAudioID);
	float fFadeTime = m_kSoundChannel[eChannelType].fFadeTime;
	bool bSuccess = _playSound( pkAudioSource, strSoundName, fVolume, 
		bFadeToGain , nLoop , 0.0f , 0.0f ,
		fFadeTime );
	if ( !bSuccess )
	{
		_stop(nAudioID);
		return INVALID_ID;
	}
	float fTotalPlayTime = 0.0f;
	if ( nPlayCount > 1 && fAutoStop == 0.0f &&
		pkAudioSource->GetPlayLength( fTotalPlayTime ) )
	{
		//如果设置了播放次数，那么就根据当前的长度来计算一个Autostop的时间
		fAutoStop = nPlayCount * fTotalPlayTime;
	}
	SoundInfo kNewSound;
	kNewSound.iSourceID = nAudioID;
	kNewSound.strName = strSoundName;
	kNewSound.strKeyCodeName = strKeyCodeName;
	kNewSound.fPrevVolume = ffVolume;
	if ( fAutoStop != 0.0f )
	{
		kNewSound.fPlayTime = m_fCurrentTime + fAutoStop;
	}
	else
	{
		kNewSound.fPlayTime = 0.0f;
	}
	m_kSoundChannel[eChannelType].kSoundList.push_back( kNewSound );
	m_kSoundChannel[eChannelType].fLastAddTime = m_fCurrentTime;
	m_kSoundChannel[eChannelType].kLastPlayKeyCode = strKeyCodeName;

	return nAudioID;
}

bool AS_ClientSoundManager::_checkPlayingRules( SOUND_CHANNEL_TYPE eChannel 
											   , const StringType& strName , const StringType& strKeyName ,float fVolume )
{
	if ( strName == "" )
	{
		return false;
	}
	bool bPlay = false;
	SoundChannel& rkChannel = m_kSoundChannel[eChannel];
	SoundInfoListIter iter;
	switch ( eChannel )
	{
	case SCT_2D:
		bPlay = true;
		break;
	case SCT_ENV:
		if ( rkChannel.kSoundList.size() < rkChannel.nMaxActive )
		{
			bPlay = true;
			if ( rkChannel.fPlayInterval == 0.0f )
			{
				TRAV( rkChannel.kSoundList , iter )
				{
					if ( iter->strName == strName )
					{
						bPlay = false;
					}
				}
			}
			else
			{
				if( m_fCurrentTime < rkChannel.fLastAddTime + rkChannel.fPlayInterval )
				{
					bPlay = false;
				}
				else
				{
					rkChannel.fLastAddTime = m_fCurrentTime;
				}
			}
		}
		break;
	case SCT_BGM:
	case SCT_AREA_EFFECT:
		//BGM要处理淡入淡出
		if( m_kSoundChannel[eChannel].kSoundList.empty() )
		{
			bPlay = true;
		}
// 		else if ( rkChannel.kSoundList[0].strName != strName )
		else
		{
			T_ASSERT( rkChannel.kSoundList.size() == 1 );
			bool bSuccess = _fade2DSound( 
				rkChannel.kSoundList[0].iSourceID ,
				strName , fVolume , 
				rkChannel.kSoundList[0].strName , rkChannel.fFadeTime,
				eChannel);
			if ( bSuccess )
			{
				rkChannel.kSoundList[0].strName = strName;
				rkChannel.kSoundList[0].strKeyCodeName = strKeyName;
				rkChannel.kLastPlayKeyCode = strKeyName;
				bPlay = false;
			}
			else
			{
				rkChannel.kSoundList.pop_back();
				bPlay = true;
			}
			
		}
// 		else
// 		{
// 			bPlay = false;
// 		}
		break;
	case SCT_PLAYER_SKILL_EVENT:
		if( m_fCurrentTime < rkChannel.fLastAddTime + rkChannel.fPlayInterval )
		{
			bPlay = false;
		}
		else
		{
			rkChannel.fLastAddTime = m_fCurrentTime;
			bPlay = true;
		}
		break;
	case SCT_PLAYER_SKILL:
	case SCT_PLAYER_HITTED:
		if ( rkChannel.kSoundList.size() < rkChannel.nMaxActive )
		{
			bPlay = true;
		}
		else
		{
			//清理一下过期的，再试一次
			_cleanChannelSourceMap(rkChannel.kSoundList);
			if ( rkChannel.kSoundList.size() < rkChannel.nMaxActive )
			{
				bPlay = true;
			}
		}
		break;
	case SCT_NET_PLAYER_SKILL:
	case SCT_MONSTER_SKILL:
	case SCT_MONSTER_SKILL_EVENT:
		if ( rkChannel.kSoundList.size() < rkChannel.nMaxActive )
		{
			bPlay = true;
		}
		else
		{
			//清理一下过期的，再试一次
			_cleanChannelSourceMap(rkChannel.kSoundList);
			if ( rkChannel.kSoundList.size() < rkChannel.nMaxActive )
			{
				bPlay = true;
			}
		}
		break;
	case SCT_NPC:
		if ( rkChannel.kSoundList.size() == 0 )
		{
			bPlay = true;
		}
		else if ( rkChannel.kSoundList.size() == rkChannel.nMaxActive )
		{
			if ( rkChannel.kSoundList[0].strName == strName )
			{
				bPlay = false;
			}
			else
			{								
				StopAudioSource(rkChannel.kSoundList[0].iSourceID, true );
				bPlay = true;
			}
		}
		break;
	default:
		break;
	}
	return bPlay;
}

float AS_ClientSoundManager::_randomSoundVolume(const CF_SD_Call::DataEntry* entry)
{
	float min = entry->_fMinVolume;
	float max = entry->_fMaxVolume;
	return UT_MathBase::RandInRangeFloat(min, max);
}

void AS_ClientSoundManager::LoadMap( const StringType& rkWorldName , const StringType& rkDecoration, int MapResID)
{
	char buffer[MAX_PATH];
	bool bLoad = false;
	if ( !rkDecoration.empty() )
	{
		NiSprintf( buffer , MAX_PATH ,"%s_Logic/SoundObjectList.csv",
			rkDecoration.c_str() );
		m_pkCSVSoundList->SetCSVFileName( buffer );
		bLoad = m_pkCSVSoundList->Load("", false);
	}
	if ( !bLoad )
	{
		NiSprintf( buffer , MAX_PATH ,"%s_Logic/SoundObjectList.csv",
			rkWorldName.c_str() );
		m_pkCSVSoundList->SetCSVFileName( buffer );
		bLoad = m_pkCSVSoundList->Load("", false);
	}
	if ( !bLoad )
	{
		GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
			"地图%s无法加载音效球csv，请检查地图的_Logic文件夹内容",
			rkWorldName.c_str());
	}
	m_kSoundChannel[SCT_BGM].fFadeTime = g_settings.SOUND_BGM_FADE_TIME;
	LC_LogicMapManager* pkMapMgr = LC_LogicMapManager::GetSingletonPtr();
	T_ASSERT( pkMapMgr );
	LC_LogicAreaManager* pkAreaMgr = pkMapMgr->GetLogicAreaManager(MapResID /*GfxGetWorldConfig().CFG_CURRENT_MAP_ID*/ );
	if ( pkAreaMgr == NULL )
	{
		GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
			"无法找到地图%d csv-%d的逻辑区域" , GfxGetWorldConfig().CFG_CURRENT_MAP_ID, MapResID);
		return;
	}
	LogicAreaInfoList& rkList = pkAreaMgr->GetEntireLogicAreaInfoList();

	m_kLogicAreaName.clear();
	LogicAreaInfoListIter i;
	TRAV( rkList , i )
	{
		LC_LogicAreaInfo* pkArea = *i;
		m_kLogicAreaName.insert( std::make_pair( pkArea->lLogicAreaID , 
			pkArea->kBGM ) );
	}
	
}

void AS_ClientSoundManager::UnLoadMap()
{
	//切换地图的时候把当前播放的所有音效都淡出
	//最多给3秒来停止所有的音效
	
}

int AS_ClientSoundManager::_getNumPlaying()
{
	int nCount = 0;
	AudioSourceMapIter iter;
	TRAV( m_kSourcesMap , iter )
	{
		NiAudioSource* pkSrc = iter->second;
		if ( pkSrc && 
			 pkSrc->GetStatus() == NiAudioSource::PLAYING && 
			 pkSrc->GetStatus() != NiAudioSource::NOT_SET )
		{
			nCount++;
		}
	}
	return nCount;
}

void AS_ClientSoundManager::_initChannel(  )
{
	m_kSoundChannel[SCT_2D].nMaxActive = g_settings.SOUND_MAX_2D;
	m_kSoundChannel[SCT_2D].fVolume = g_settings.SOUND_UI_VOLUME;

	m_kSoundChannel[SCT_BGM].fVolume = g_settings.SOUND_BGM_VOLUME;
	m_kSoundChannel[SCT_BGM].nMaxActive = 1;
	m_kSoundChannel[SCT_BGM].fFadeTime = g_settings.SOUND_BGM_FADE_TIME;
	m_kSoundChannel[SCT_BGM].fPlayInterval = g_settings.SOUND_BGM_PLAY_INTERVAL;

	m_kSoundChannel[SCT_AREA_EFFECT].fVolume = g_settings.SOUND_UI_VOLUME;
	m_kSoundChannel[SCT_AREA_EFFECT].nMaxActive = 1;
	m_kSoundChannel[SCT_AREA_EFFECT].fDistance = g_settings.SOUND_AREA_DISTANCE;

	m_kSoundChannel[SCT_PLAYER_SKILL_EVENT].fPlayInterval = 5.0f;
	m_kSoundChannel[SCT_PLAYER_SKILL_EVENT].nMaxActive = g_settings.SOUND_MAX_PLAYER_EVENT;
	m_kSoundChannel[SCT_PLAYER_SKILL_EVENT].fVolume = g_settings.SOUND_SKILL_VOLUME;

	m_kSoundChannel[SCT_PLAYER_HITTED].nMaxActive = g_settings.SOUND_MAX_PLAYER_HITTED;
	m_kSoundChannel[SCT_PLAYER_HITTED].fVolume = g_settings.SOUND_SKILL_VOLUME;
	m_kSoundChannel[SCT_PLAYER_HITTED].fDistance = g_settings.SOUND_MAX_NPC_SKILL_DISTANCE;

	m_kSoundChannel[SCT_PLAYER_SKILL].fDistance = g_settings.SOUND_MAX_NPC_SKILL_DISTANCE;
	m_kSoundChannel[SCT_PLAYER_SKILL].fVolume = g_settings.SOUND_SKILL_VOLUME;
	m_kSoundChannel[SCT_PLAYER_SKILL].nMaxActive = g_settings.SOUND_MAX_PLAYER_SKILL;

	m_kSoundChannel[SCT_ENV].fDistance = g_settings.SOUND_MAX_ENV_DISTANCE;
	m_kSoundChannel[SCT_ENV].nMaxActive = g_settings.SOUND_MAX_ENV;
	m_kSoundChannel[SCT_ENV].fVolume = g_settings.SOUND_ENVIRONMENT_EFFECT_VOLUME;
	m_kSoundChannel[SCT_ENV].fFadeTime = 3.0f;

	m_kSoundChannel[SCT_NET_PLAYER_SKILL].fDistance = g_settings.SOUND_MAX_NPC_SKILL_DISTANCE;
	m_kSoundChannel[SCT_NET_PLAYER_SKILL].nMaxActive = g_settings.SOUND_MAX_NET_PLAYER_SKILL;
	m_kSoundChannel[SCT_NET_PLAYER_SKILL].fVolume = g_settings.SOUND_SKILL_VOLUME;

	m_kSoundChannel[SCT_MONSTER_SKILL].fDistance = g_settings.SOUND_MAX_NPC_SKILL_DISTANCE;
	m_kSoundChannel[SCT_MONSTER_SKILL].nMaxActive = g_settings.SOUND_MAX_NPC_3D_SOUND;
	m_kSoundChannel[SCT_MONSTER_SKILL].fVolume = g_settings.SOUND_SKILL_VOLUME;
	m_kSoundChannel[SCT_MONSTER_SKILL].fPlayInterval = 2.0f;

	m_kSoundChannel[SCT_MONSTER_SKILL_EVENT].fDistance = g_settings.SOUND_MAX_NPC_SKILL_DISTANCE;
	m_kSoundChannel[SCT_MONSTER_SKILL_EVENT].nMaxActive = g_settings.SOUND_MAX_MONSTER_EVENT;
	m_kSoundChannel[SCT_MONSTER_SKILL_EVENT].fVolume = g_settings.SOUND_SKILL_VOLUME;
	m_kSoundChannel[SCT_MONSTER_SKILL_EVENT].fPlayInterval = 2.0f;

    m_kSoundChannel[SCT_NPC].fVolume = g_settings.SOUND_ENVIRONMENT_EFFECT_VOLUME;
    m_kSoundChannel[SCT_NPC].nMaxActive = 1;
	m_kSoundChannel[SCT_NPC].fPlayInterval = 0.0f;

	if ( g_settings.SOUND_ENABLE )
	{
		m_fVolumeControl = 1.0f;
	}
	else
	{
		m_fVolumeControl = 0.0f;
	}
}

void AS_ClientSoundManager::StopAudioSource(int iID, bool bFade /* = false  */, float fFadeTime /* = 0.0f */ )
{
	if ( iID != INVALID_ID )
	{
		_cleanChannel(iID);
		AS_SoundManagerBase::StopAudioSource( iID , bFade, fFadeTime );
		/*GfxWriteLog( LOG_SOUND_FILELINE , LOG_SWITCH_SOUND ,
			"Stop Sound %d", iID );*/
	}
}

void AS_ClientSoundManager::StopAudioSourceFade(int iID, float fadeTime)
{
	if ( iID != INVALID_ID )
	{
		_cleanChannel(iID);
	AS_SoundManagerBase::StopAudioSource( iID , true, fadeTime );
	}
}

void AS_ClientSoundManager::CleanSourceMap()
{
	AS_SoundManagerBase::CleanSourceMap();
	for ( int i = SCT_2D ; i < SCT_MAX ; i++ )
	{
		SoundInfoList& rkSounds = m_kSoundChannel[i].kSoundList;
		SoundInfoListIter iter;

		if (i == SCT_BGM )
		{
			//不停止背景音乐
			continue;
		}
		
		for( iter = rkSounds.begin() ; iter != rkSounds.end() ; )
		{
			NiAudioSource* pkSrc = _getSound( iter->iSourceID );
			if ( pkSrc == NULL )
			{
				iter = rkSounds.erase( iter );
			}
			else if ( pkSrc->GetStatus() == NiAudioSource::DONE || pkSrc->GetStatus() == NiAudioSource::NOT_SET )
			{
				_stop( iter->iSourceID , false );
				iter = rkSounds.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}

void AS_ClientSoundManager::_cleanChannelSourceMap(SoundInfoList& sil)
{
	for(SoundInfoListIter iter = sil.begin() ; iter != sil.end() ; )
	{
		NiAudioSource* pkSrc = _getSound( iter->iSourceID );
		if ( pkSrc == NULL )
		{
			iter = sil.erase( iter );
		}
		else if ( pkSrc->GetStatus() == NiAudioSource::DONE || pkSrc->GetStatus() == NiAudioSource::NOT_SET )
		{
			_stop( iter->iSourceID , false );
			iter = sil.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

bool AS_ClientSoundManager::IsAudioFile2d(const StringType& kSoundName)
{
	AllSoundMap::iterator findIter = m_kAllSoundMap.find(kSoundName);
	if (findIter == m_kAllSoundMap.end())
	{
		return false;
	}
	return findIter->second->_bIs2D;
}

bool AS_ClientSoundManager::_cleanChannel( int iID )
{
	bool bFind = false;
	for ( int i = SCT_2D ; i < SCT_MAX ; i++ )
	{
		SoundInfoList& rkSounds = m_kSoundChannel[i].kSoundList;
		SoundInfoListIter iter;
		
		TRAV( rkSounds , iter )
		{
			if ( iter->iSourceID == iID )
			{
				rkSounds.erase( iter );
				bFind = true;
				break;
			}
		}
		if ( bFind )
		{
			break;
		}
	}
	return bFind;
}

void AS_ClientSoundManager::FastFadingMode( bool bEnable )
{
	if ( bEnable )
	{
		m_kSoundChannel[SCT_BGM].fFadeTime = g_settings.SOUND_LOGIN_FADE_TIME;
	}
	else
	{
		m_kSoundChannel[SCT_BGM].fFadeTime = g_settings.SOUND_BGM_FADE_TIME;
	}
}

StringType AS_ClientSoundManager::GetBGMName()
{
	if ( !m_kSoundChannel[SCT_BGM].kSoundList.empty() )
	{
		return m_kSoundChannel[SCT_BGM].kSoundList[0].strName;
	}
	else
	{
		return "NULL";
	}
}

bool AS_ClientSoundManager::FilleAnimEventItem(const StringType& key, FileSystem::MTF_ModelAnimSoundEventItem* item)
{
	AllSoundMap::iterator itr = m_kAllSoundMap.find(key);
	if (itr	!= m_kAllSoundMap.end() && item)
	{
		item->bIs2D = itr->second->_bIs2D;
		item->bIsCycled = itr->second->_bIsCycle;
		item->fPercent = itr->second->_fPercent;
		item->fVolume = itr->second->_fMaxVolume;
		item->strSoundNameVec[0] = itr->second->_strSoundName1;
		item->strSoundNameVec[1] = itr->second->_strSoundName2;
		item->strSoundNameVec[2] = itr->second->_strSoundName3;
		return true;
	}
	else
	{
		return false;
	}
}

void AS_ClientSoundManager::_on_windows_lost_focus( MG_MessageBase& rkMessage )
{
	bool bActive = rkMessage.m_lParam1 > 0 ? true : false;

	if ( m_bPrevActive != bActive )
	{
		//失去焦点禁掉所有音乐
		if ( !bActive )
		{
			m_fVolumeControl = 0.0f;
		}
		else if ( g_settings.SOUND_ENABLE )
		{
			m_fVolumeControl = 1.0f;
		}
		for ( int i = SCT_2D; i < SCT_MAX; ++i )
		{
#if 1
			SoundChannel& rkChannel = m_kSoundChannel[i];
			if( !rkChannel.kSoundList.empty() && rkChannel.bSubAvtiveflag )
			{
				SoundInfoList& rkSounds = m_kSoundChannel[i].kSoundList;
				SoundInfoListIter iter;

				/*if (!m_kFadingSoundQueue.empty() && i == SCT_BGM)
				{
					continue;
				}*/

				for( iter = rkSounds.begin() ; iter != rkSounds.end() ; ++iter )
				{
					NiAudioSource* pkSrc = _getSound( iter->iSourceID );
					if ( pkSrc != NULL && pkSrc->GetStatus() == NiAudioSource::PLAYING )
					{
						if ( !bActive )
						{
							pkSrc->SetGain(0.0f);
						}
						else
						{
							pkSrc->SetGain( rkChannel.fVolume * m_fVolumeControl * iter->fPrevVolume * rkChannel.extenalAffect);
						}
					}
				}
			}
#else
			if ( bActive )
			{
				m_fVolumeControl = m_fLastVolume;
			}
			else
			{
				m_fLastVolume = m_fVolumeControl;
				m_fVolumeControl = 0.0f;
			}
			for ( int i = SCT_2D ; i < SCT_MAX ; i++ )
			{
				SoundInfoList& rkSounds = m_kSoundChannel[i].kSoundList;
				SoundInfoListIter iter;

				for( iter = rkSounds.begin() ; iter != rkSounds.end() ; iter++ )
				{
					NiAudioSource* pkSrc = _getSound( iter->iSourceID );
					if ( pkSrc != NULL && pkSrc->GetStatus() == NiAudioSource::PLAYING )
					{
						if ( !bActive )
						{
							pkSrc->SetGain( 0.0f );
						}
						else
						{
							pkSrc->SetGain( m_fVolumeControl * m_kSoundChannel[i].fVolume );
						}
					}
				}
			}
#endif
		}

		m_bPrevActive = bActive;
	}

}

int AS_ClientSoundManager::Play2DEx( const StringType& strKeyName, SOUND_CHANNEL_TYPE eChannelType /*=AS_ClientSoundManager::SCT_2D*/, bool bFadeToGain /*= false*/, int nPlayCount /*= 1 */, float fAutoStop /*= 0.0f*/ )
{
	AllSoundMap::iterator findIter = m_kAllSoundMap.find(strKeyName);
	if (findIter == m_kAllSoundMap.end())
	{
		return 0;
	}
	CF_SD_Call::DataEntry* entry = findIter->second;
	if (entry)
	{
		//判断概率
		bool bFlag = true;
		if (entry->_fPercent < 1.0f && entry->_fPercent < UT_MathBase::RandFloat01())
		{
			bFlag = false;
		}

		if (bFlag)	
		{	
			StringType buff[3] = { entry->_strSoundName1, entry->_strSoundName2, entry->_strSoundName3};
			StringType finalPlayName = GetRandomPlayName(buff);
			int soundId = Play2DWithVolume(finalPlayName, strKeyName,eChannelType, _randomSoundVolume(entry), bFadeToGain, nPlayCount, fAutoStop);
			return soundId;
		}
		return 0;
	}
	return 0;

}

int AS_ClientSoundManager::PlayAreaEffect(const StringType& strAllSoundKeyName)
{
	AllSoundMap::iterator findIter = m_kAllSoundMap.find(strAllSoundKeyName);
	if (findIter == m_kAllSoundMap.end())
	{
		return 0;
	}
	CF_SD_Call::DataEntry* entry = findIter->second;
	if (entry)
	{
		//判断概率
		bool bFlag = true;
		if (entry->_fPercent < 1.0f && entry->_fPercent < UT_MathBase::RandFloat01())
		{
			bFlag = false;
		}

		if (bFlag)	
		{	
			StringType buff[3] = { entry->_strSoundName1, entry->_strSoundName2, entry->_strSoundName3};
			StringType finalPlayName = GetRandomPlayName(buff);

			int soundId = Play2DWithVolume(finalPlayName, strAllSoundKeyName,AS_ClientSoundManager::SCT_AREA_EFFECT, _randomSoundVolume(entry), false, 1);
			return soundId;
		}
		return 0;
	}
	return 0;
}

int AS_ClientSoundManager::PlayAreaEffectPos(const StringType& strAllSoundKeyName, const NiPoint3& pos, int channel)
{
	AllSoundMap::iterator findIter = m_kAllSoundMap.find(strAllSoundKeyName);
	if (findIter == m_kAllSoundMap.end())
	{
		return 0;
	}
	CF_SD_Call::DataEntry* entry = findIter->second;
	if (entry)
	{
		//判断概率
		bool bFlag = true;
		if (entry->_fPercent < 1.0f && entry->_fPercent < UT_MathBase::RandFloat01())
		{
			bFlag = false;
		}

		if (bFlag)	
		{	
			StringType buff[3] = { entry->_strSoundName1, entry->_strSoundName2, entry->_strSoundName3};
			StringType finalPlayName = GetRandomPlayName(buff);
			//现在脚步声的音效不是在Area_Effect里面而是在PlayerSkill里面
			int soundId = Play3D(pos, finalPlayName, strAllSoundKeyName,AS_ClientSoundManager::SCT_PLAYER_SKILL, false, false,g_settings.SOUND_COMMON_MIN_DISTANCE, g_settings.SOUND_COMMON_MAX_DISTANCE, _randomSoundVolume(entry));
			return soundId;
		}
		return 0;
	}
	return 0;
}

int AS_ClientSoundManager::Play3DEx( const NiPoint3& kPos, const StringType& strKeyName,SOUND_CHANNEL_TYPE eChannelType, bool bFadeToGain /*= false */, bool bLoop /*= false */,
									float fControlVolume /*=1.0f */, float fMinDist /*= MIN_DISTANCE_3D_FALLOFF */, float fMaxDist /*= MAX_DISTANCE_3D_FALLOFF */)
{
	AllSoundMap::iterator findIter = m_kAllSoundMap.find(strKeyName);
	if (findIter == m_kAllSoundMap.end())
	{
		return 0;
	}
	CF_SD_Call::DataEntry* entry = findIter->second;
	if (entry)
	{
		//判断概率
		bool bFlag = true;
		if (entry->_fPercent < 1.0f && entry->_fPercent < UT_MathBase::RandFloat01())
		{
			bFlag = false;
		}
		if (bFlag)	
		{
			StringType buff[3] = { entry->_strSoundName1, entry->_strSoundName2, entry->_strSoundName3};
			StringType finalPlayName = GetRandomPlayName(buff);
			return Play3D(kPos,finalPlayName,strKeyName, eChannelType,bFadeToGain,bLoop,fMinDist,fMaxDist,_randomSoundVolume(entry) * fControlVolume);
		}
		return 0;
	}
	return 0;
}

int AS_ClientSoundManager::PlayAutoEx(const StringType& strKeyName, const NiPoint3& kPos, SOUND_CHANNEL_TYPE eChannelType, bool bFadeToGain /* = false */, int nPlayCount /* = 1  */, float fAutoStop /* = 0.0f */)
{
	AllSoundMap::iterator findIter = m_kAllSoundMap.find(strKeyName);
	if (findIter == m_kAllSoundMap.end())
	{
		return 0;
	}
	CF_SD_Call::DataEntry* entry = findIter->second;
	if (entry)
	{
		//判断概率
		bool bFlag = true;
		if (entry->_fPercent < 1.0f && entry->_fPercent < UT_MathBase::RandFloat01())
		{
			bFlag = false;
		}
		if (bFlag)	
		{
			StringType buff[3] = { entry->_strSoundName1, entry->_strSoundName2, entry->_strSoundName3};
			StringType finalPlayName = GetRandomPlayName(buff);
			if (!entry->_bIs2D)
				return Play3D(kPos,finalPlayName,strKeyName, eChannelType,bFadeToGain, nPlayCount != 1, g_settings.SOUND_COMMON_MIN_DISTANCE, g_settings.SOUND_COMMON_MAX_DISTANCE, _randomSoundVolume(entry));
			else
				return Play2DWithVolume(finalPlayName, strKeyName,eChannelType, _randomSoundVolume(entry), false, nPlayCount,fAutoStop);
		}
		return 0;
	}
	return 0;
}

void AS_ClientSoundManager::StopCurrentNPCSound()
{
	SoundInfoList& rkSounds = m_kSoundChannel[SCT_NPC].kSoundList;
	if ( !rkSounds.empty() )
	{
		NiAudioSource* pkSrc = _getSound( rkSounds[0].iSourceID );
		if ( pkSrc != NULL && pkSrc->GetStatus() == NiAudioSource::PLAYING )
		{
			bool bSuccess = _fade2DSound(rkSounds[0].iSourceID, "", 
										0.0f, "", 1.0f,SCT_NPC);
			if ( bSuccess )
			{
				m_kSoundChannel[SCT_NPC].kSoundList.pop_back();
			}
			//pkSrc->FadeToGain( MIN_VOLUME ,m_fCurrentTime , 1.0f );
			OnEndPlayNPCSound();
		}
	}
}

void AS_ClientSoundManager::StopAllEnvSound()
{
	//清空数据,防止在选人界面还有Env音效残留
	m_pkCSVSoundList->Clear();

	SoundInfoList& rkSounds = m_kSoundChannel[SCT_ENV].kSoundList;
	for(SoundInfoList::iterator iter = rkSounds.begin(); rkSounds.end() != iter; )
	{
		NiAudioSource* pkSrc = _getSound( iter->iSourceID );
		if ( pkSrc != NULL && pkSrc->GetStatus() == NiAudioSource::PLAYING )
		{
			AS_SoundManagerBase::StopAudioSource( iter->iSourceID , true, 0.5f );
			m_kFadedEnvSoundList.push_back(iter->iSourceID);
			iter = rkSounds.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void AS_ClientSoundManager::StopCurrentBGM()
{
	if ( !m_kSoundChannel[SCT_BGM].kSoundList.empty() )
	{
		SoundInfoList& rkSounds = m_kSoundChannel[SCT_BGM].kSoundList;
		bool bSuccess = _fade2DSound(rkSounds[0].iSourceID, "", 
			rkSounds[0].fPrevVolume * m_fVolumeControl * m_kSoundChannel[SCT_BGM].extenalAffect * m_kSoundChannel[SCT_BGM].fVolume, 
			"", 1.0f, SCT_BGM);

		if ( bSuccess )
		{
			m_kSoundChannel[SCT_BGM].kSoundList.pop_back();
			m_kSoundChannel[SCT_BGM].kLastPlayKeyCode = "";
		}	
	}
}

void AS_ClientSoundManager::StopPlayerSkillSounds()
{
	if ( !m_kSoundChannel[SCT_PLAYER_SKILL].kSoundList.empty() )
	{
		SoundInfoList& soundList = m_kSoundChannel[SCT_PLAYER_SKILL].kSoundList;
		for(SoundInfoList::size_type i = 0; i< soundList.size(); ++i)
		{
			StopAudioSource(soundList[i].iSourceID);
		}
	}
}

void AS_ClientSoundManager::StopNPCSkillSounds()
{
	if ( !m_kSoundChannel[SCT_MONSTER_SKILL].kSoundList.empty() )
	{
		SoundInfoList& soundList = m_kSoundChannel[SCT_MONSTER_SKILL].kSoundList;
		for(SoundInfoList::size_type i = 0; i< soundList.size(); ++i)
		{
			StopAudioSource(soundList[i].iSourceID);
		}
	}
}

void AS_ClientSoundManager::PauseCurrentBGM()
{
	if (m_kSoundChannel[SCT_BGM].bSubAvtiveflag)
	{
		SoundChannel& rkChannel = m_kSoundChannel[SCT_BGM];
		if( !rkChannel.kSoundList.empty() )
		{
			SoundInfoList& rkSounds = rkChannel.kSoundList;
			SoundInfoListIter iter;

			for( iter = rkSounds.begin() ; iter != rkSounds.end() ; ++iter )
			{
				NiAudioSource* pkSrc = _getSound( iter->iSourceID );
				if ( pkSrc != NULL && pkSrc->GetStatus() == NiAudioSource::PLAYING )
				{
					pkSrc->FadeToGain( 0.0f ,m_fCurrentTime , 1.0f );
					m_kSoundChannel[SCT_BGM].bSubAvtiveflag = false;
				}
			}
		}
	}	
}

void AS_ClientSoundManager::ResumeCurrentBGM()
{
	if (!m_kSoundChannel[SCT_BGM].bSubAvtiveflag)
	{
		SoundChannel& rkChannel = m_kSoundChannel[SCT_BGM];
		if( !rkChannel.kSoundList.empty() )
		{
			SoundInfoList& rkSounds = rkChannel.kSoundList;
			SoundInfoListIter iter;

			for( iter = rkSounds.begin() ; iter != rkSounds.end() ; ++iter )
			{
				NiAudioSource* pkSrc = _getSound( iter->iSourceID );
				if ( pkSrc != NULL && pkSrc->GetStatus() == NiAudioSource::PLAYING )
				{
					pkSrc->FadeToGain( rkChannel.fVolume * m_fVolumeControl ,m_fCurrentTime , 1.0f );
					m_kSoundChannel[SCT_BGM].bSubAvtiveflag = true;
				}
			}
		}
	}				
}

NiFile* AS_ClientSoundManager::CreateSoundFile( const StringType& rkName )
{
	AS_ClientSoundManager::m_kSoundCacheLock.Lock();
	SoundFile* pkFileStruct = m_kFileCache.fetch_ptr( rkName );
	if ( pkFileStruct == NULL )
	{
		m_kFileCache.insert( rkName , SoundFile() );
		pkFileStruct = m_kFileCache.fetch_ptr( rkName );
	}
	AS_ClientSoundManager::m_kSoundCacheLock.UnLock();

	if (pkFileStruct->pkFile == NULL)
	{
		UT_FileOpenBlock fb;
		bool bLoadOk = UT_FileManagerBase::GetSingletonPtr()->LoadFile( rkName.c_str(), fb );

		if( bLoadOk )
		{
			pkFileStruct->pkFile = NiNew T6NiFile(fb.data(), fb.size());
		}
	}
	if ( pkFileStruct->pkFile != NULL )
	{
		T6NiFile* pkNewFile = NiNew T6NiFile(*pkFileStruct->pkFile);	//复制一份，在FileCloseMilesCB中删除
		T_ASSERT(pkNewFile);
		T_ASSERT(pkNewFile->operator bool());
		if (pkNewFile && !pkNewFile->operator bool())	//copy constructor succeed? T6NiFile::m_bGood
		{
			NiDelete pkNewFile;
			pkNewFile = NULL;
		}
		return pkNewFile;
	}
	return NULL;
}

void AS_ClientSoundManager::ParseSequenceString( const StringType& strBuff )
{
	m_kNPCSoundSeqData.m_NPCSoundSeq.clear();
	TStringVector tmp = UT_EngineStringUtility::SplitString(strBuff, '|');
	for(TStringVector::iterator iter = tmp.begin(); iter != tmp.end(); ++iter)
	{
		if( (*iter) != "" )
		{
			TStringVector tmp1 = UT_EngineStringUtility::SplitString((*iter), '#');
			if(tmp1.size() == 2)
			{
				SoundSeq kSeq;
				const StringType & tt = tmp1[1];
				kSeq.fTimeLength = float(atoi(tt.c_str()));
				kSeq.keyCode = tmp1[0];
				m_kNPCSoundSeqData.m_NPCSoundSeq.push_back(kSeq);
			}
			
		}
	}
}

void AS_ClientSoundManager::PlaySequenceNPCSound()
{
	m_kNPCSoundSeqData.Active();
}

void AS_ClientSoundManager::OnPlayNPCSound(float affect, object_id_type charTypeID)
{
	m_kSoundChannel[SCT_BGM].extenalAffect = affect;
	
	ApplyBGMVolumeChange();

	m_kFlagNPCSoundPlay = true;

	CSVFile::CF_CharType::DataEntry* pkData = CSVFile::CF_CharType::GetSingletonPtr()->GetEntryPtr(charTypeID);
	if (pkData && !pkData->_kTalkHeadIcon.empty() )
	{
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GLET_UI_BEGIN_PLAY_NPCSOUND);

		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(pkData->_kTalkHeadIcon);
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void AS_ClientSoundManager::OnEndPlayNPCSound()
{
	m_kSoundChannel[SCT_BGM].extenalAffect = 1.0f;
	
	ApplyBGMVolumeChange();

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GameLogic::GLET_UI_END_PLAY_NPCSOUND);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void AS_ClientSoundManager::PlayBGMForLogin(const StringType& bgmName)
{
	FadingSoundListIter kFind =	m_kFadingSoundQueue.end();
	for ( FadingSoundListIter iterFade = m_kFadingSoundQueue.begin();
		iterFade != m_kFadingSoundQueue.end(); ++iterFade )
	{
		if (iterFade->iSoundChannel == SCT_BGM)
		{
			kFind = iterFade;
			break;
		}
	}

	if (kFind != m_kFadingSoundQueue.end() )		
	{
		StringType tmp;
		if ( _KeyToRandomOgg(bgmName, tmp) )
			kFind->strSoundName =  tmp;
	}
	else 
	{
		Play2DEx( bgmName, SCT_BGM, true, 0, 0.0f);
	}
}

bool AS_ClientSoundManager::IsSoundPlaying(int iSoundID)
{
	bool flag = false;
	bool bFind = false;

	for ( int i = SCT_2D ; i < SCT_MAX ; i++ )
	{
		SoundInfoList& rkSounds = m_kSoundChannel[i].kSoundList;
		SoundInfoListIter iter;

		for( iter = rkSounds.begin(); iter != rkSounds.end(); ++iter)
		{
			if (iter->iSourceID == iSoundID)
			{
				bFind = true;
				NiAudioSource* pkSrc = _getSound(iSoundID);
				if ( pkSrc == NULL )
				{
					iter = rkSounds.erase( iter );
				}
				else if ( pkSrc->GetStatus() == NiAudioSource::DONE || pkSrc->GetStatus() == NiAudioSource::NOT_SET )
				{
					_stop(iSoundID, false);
					iter = rkSounds.erase(iter);
				}
				else
				{
					flag = true;
				}
				break;
			}
		}

		if (bFind)
		{
			break;
		}
	}

	return flag;
}

//////////////////////////////////////////////////////////////////////////
/// async load

bool AS_ClientSoundManager::_stop(int iID, bool bFadeToGain /*= false */, float fFade /*= 0.0f */)
{
	AudioSourceMap::iterator it = m_kAsyncLoadingSource.find(iID);
	if (it != m_kAsyncLoadingSource.end())
	{
		NiAudioSource * pkAudioSource = this->_popSource(iID);
		if (pkAudioSource != NULL)	/// 防止 _stop 和 _popSource 重入，第二次为NULL
		{
			T_ASSERT(it->second == NULL);
			it->second = pkAudioSource;
		}
		return true;
	}
	else
	{
		return AS_SoundManagerBase::_stop(iID, bFadeToGain, fFade);
	}
}

bool AS_ClientSoundManager::PlaySound_AsyncLoad(int iID, AudioLoadArg * pkArg, SoundInfo * pkSoundInfo)
{
	AudioLoadArg * arg = T_NEW_E(AudioLoadArg, Memory::MEMCATEGORY_GENERAL)(*pkArg);
	SoundInfo     * si  = NULL;
	if (pkSoundInfo != NULL)
		si = T_NEW_E(SoundInfo, Memory::MEMCATEGORY_GENERAL)(*pkSoundInfo);

	MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_ADD_PLAY_AUDIO_LOAD_TASK(iID, arg, si), true);	//必须true

	/// 记录一下
	NiAudioSource * pkAudioSource = NULL;
	m_kAsyncLoadingSource.insert(std::make_pair(iID, pkAudioSource));

	return true;
}

bool AS_ClientSoundManager::PlaySound_AsyncLoading(AudioLoadArg * pkArg)
{
	bool bLoaded = this->_loadSound(pkArg->pkAudioSource, pkArg->strSoundName, 
		pkArg->fVolume, pkArg->bFadeToGain, 
		pkArg->fMinDistance, pkArg->fMaxDistance, pkArg->fFadeTime);

	return bLoaded;
}

void AS_ClientSoundManager::PlaySound_AsyncPostLoad(bool bLoaded, bool bTimeOut, int iID, AudioLoadArg * pkArg, void * pkSoundInfo)
{
	SoundInfo * pkSI = (SoundInfo *)pkSoundInfo;
	bool bPlay = false;

	/// 是否调用过stop了，加载过程中取消了
	AudioSourceMap::iterator it = m_kAsyncLoadingSource.find(iID);
	if (it != m_kAsyncLoadingSource.end())
	{
		NiAudioSource * pkAudioSource = it->second;
		m_kAsyncLoadingSource.erase(it);
		/// 被取消的
		if (pkAudioSource != NULL)
		{
			pkAudioSource->Stop();
			NiDelete pkAudioSource;
			return;
		}
	}

	/// 加载成功，未超时，未取消
	if (bLoaded && !bTimeOut)
	{
		NiAudioSource * pkAudioSource = pkArg->pkAudioSource;
		pkAudioSource->SetLoopCount( pkArg->iPlayCount );
		pkAudioSource->Update(0.0f);
		bPlay = pkAudioSource->Play();		// 播放成功

		if (bPlay && pkSoundInfo != NULL)
		{
			m_kSoundChannel[pkArg->eChannelType].kSoundList.push_back( *pkSI );
		}
	}
	T_DELETE_E(pkArg, AudioLoadArg, Memory::MEMCATEGORY_GENERAL);
	T_DELETE_E(pkSI, SoundInfo, Memory::MEMCATEGORY_GENERAL);

	if (!bPlay)
	{
		_stop(iID);
	}
}

//////////////////////////////////////////////////////////////////////////

#ifndef _SHIPPING

static NiApplicationMetrics::NiMetric	ChannelClassToMetricsType(int enmClass)
{
	T_ASSERT(enmClass >= AS_ClientSoundManager::SCT_2D && enmClass < AS_ClientSoundManager::SCT_MAX);
	int v = enmClass + NiApplicationMetrics::AUDIO_AOC_SCT_BEGIN;
	T_ASSERT(enmClass < NiApplicationMetrics::AUDIO_AOC_SCT_END);
	return (NiApplicationMetrics::NiMetric)v;
}

void AS_ClientSoundManager::_updateMetrics(void)
{
	unsigned int nSum = 0;
	for (int i = SCT_2D; i < SCT_MAX; ++i)
	{
		unsigned int n = m_kSoundChannel[i].kSoundList.size();
		nSum += n;
		NIMETRICS_APPLICATION_AGGREGATEVALUE_VAR(ChannelClassToMetricsType(i), n);
	}
	NIMETRICS_APPLICATION_AGGREGATEVALUE(AUDIO_ACTIVE_OBJ_COUNT, nSum);
}

#else	// _SHIPPING

void AS_ClientSoundManager::_updateMetrics(void)
{
}

#endif	// _SHIPPING
