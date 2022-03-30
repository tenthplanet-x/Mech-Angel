#pragma once

#include "MG_MessageHandlerInterface.h"
#include "MG_MessageBase.h"
#include "CF_HittedSoundList.h"
#include "UT_Lock.h"
#include "AS_SoundManagerBase.h"
#include "CF_UISound.h"
#include "CF_LogicAreaInfo.h"
#include "UT_LRUCacheTemplate.h"
#include "CF_SoundObjectList.h"
#include "CF_SD_Call.h"
#include "UT_Filemanager.h"

#define MAX_NPC_COUNT	2
#define MAX_CACHE_COUNT	24

//////////////////////////////////////////////////////////////////////////

class	AudioLoadArg
{
public:
	AudioLoadArg(NiAudioSource *pkAudioSource
		, const StringType& strSoundName
		, float fVolume
		, bool bFadeToGain
		, int iPlayCount
		, float fMinDistance
		, float fMaxDistance
		, float fFadeTime
		, int eChannelType
		, unsigned int	uTimeOutMs
		)
	{
		this->pkAudioSource = pkAudioSource;
		this->strSoundName	= strSoundName;
		this->fVolume		= fVolume;
		this->bFadeToGain	= bFadeToGain;
		this->iPlayCount	= iPlayCount;
		this->fMinDistance	= fMinDistance;
		this->fMaxDistance	= fMaxDistance;
		this->fFadeTime		= fFadeTime;
		this->eChannelType	= eChannelType;
		this->uTimeOutMs	= uTimeOutMs;
	}
	AudioLoadArg() { pkAudioSource = NULL; }

	NiAudioSource *	pkAudioSource;
	StringType		strSoundName;
	float			fVolume;
	bool			bFadeToGain;
	int				iPlayCount;
	float			fMinDistance;
	float			fMaxDistance;
	float			fFadeTime;
	int				eChannelType;	// SOUND_CHANNEL_TYPE
	unsigned int	uTimeOutMs;
};

//////////////////////////////////////////////////////////////////////////

class AS_ClientSoundManager :	public Audio::AS_SoundManagerBase,
						public MG_MessageHandlerInterface
{
public:
	enum SOUND_CHANNEL_TYPE
	{
		SCT_2D,
		SCT_BGM,
		SCT_ENV,
		SCT_AREA_EFFECT,
		SCT_PLAYER_SKILL_EVENT,
		SCT_PLAYER_SKILL,
		SCT_PLAYER_HITTED,
		SCT_NET_PLAYER_SKILL,
		SCT_MONSTER_SKILL_EVENT,
		SCT_MONSTER_SKILL,
		SCT_NPC,
		SCT_MAX,
	};


	struct SoundFile
	{
		SoundFile()
		{
			pkFile = NULL;
		}
		~SoundFile()
		{
			if ( pkFile )
			{
				NiDelete pkFile;
				pkFile = NULL;
			}
		}
		Utility::T6NiFile* pkFile;
	};
	typedef Utility::LRUCache<StringType,SoundFile> SoundFileCache;

	SoundFileCache m_kFileCache;

	struct FadingSoundDescriptor
	{
		int             iAudioID;
		StringType		strSoundName;
		int				iPlayCount;
		float			fVolume;
		float           fFadeTime;
		int				iSoundChannel;
	};
	typedef mem::vector<FadingSoundDescriptor> FadingSoundList;
	typedef FadingSoundList::iterator	       FadingSoundListIter;
	struct SoundInfo
	{
		SoundInfo()
		{
			iSourceID = INVALID_ID;
			fPlayTime = 0.0f;
			fPrevVolume = 1.0f;
		}
		int			iSourceID;
		StringType	strName;
		StringType	strKeyCodeName;
		float       fPlayTime;
		float       fPrevVolume;    //失去焦点的时候保存一下音量，以备恢复
	};
	typedef mem::list<SoundInfo>	            SourceCacheList;
	typedef SourceCacheList::iterator	        SourceCacheListIter;

	typedef mem::vector<SoundInfo>              SoundInfoList;
	typedef SoundInfoList::iterator             SoundInfoListIter;

	typedef mem::vector<int>                    FadedEnvSoundList;
	typedef FadedEnvSoundList::iterator          FadedEnvSoundListIter;

	struct SoundChannel
	{
		SoundChannel()
		{
			fPlayInterval = 0.0f;
			fLastAddTime = 0.0f;
			fVolume = 1.0f;
			fFadeTime = 0.0f;
			nMaxActive = 1;
			fDistance = 3000.0f;
			nPlayCount = 0;
			bSubAvtiveflag = true;
			extenalAffect = 1.0f;
		}
		float         fPlayInterval;  //播放的时间间隔
		float         fLastAddTime;
		float         fVolume;
		float         fFadeTime;
		SoundInfoList kSoundList;     //当前的音效对象ID列表
		int           nMaxActive;     //这种类型的音效允许多少同时播放
		float         fDistance;      //距离多近的才播放
		StringType	  kLastPlayKeyCode;	//上次播放的字符键值
		int           nPlayCount;     //播放次数，和自动
		bool		  bSubAvtiveflag;	//该声道是否处于激活状态
		float		  extenalAffect;	//外部临时影响声道播放	(目前只有背景音乐用到）

	};	

	enum NPCBornSoundType 
	{
		NBST_BORN_SOUND = 1,
		NBST_BORM_BGM = 2,
	};
	struct NPCBornSoundData
	{
		object_id_type iID;
		object_id_type iCharTypeID;
		//NPCBornSoundType kType;
		float			iTimeTick;
	};

	typedef mem::map<object_id_type, NPCBornSoundData> NPCBornSoundMap;

    static AS_ClientSoundManager& GetSingleton(void)
    {
        return *(static_cast<AS_ClientSoundManager*>(AS_SoundManagerBase::GetSingletonPtr()));
    }

    static AS_ClientSoundManager* GetSingletonPtr(void)
    {
        return static_cast<AS_ClientSoundManager*>(AS_SoundManagerBase::GetSingletonPtr());
    }

	static Utility::UT_Lock          m_kSoundLock;
	static Utility::UT_Lock          m_kSoundCacheLock;

	AS_ClientSoundManager();
	~AS_ClientSoundManager();

	virtual void Init(HWND hWnd);

	virtual void UnInit();

	void         LoadMap( const StringType& rkWorldName, 
		const StringType& rkDecoration , int MapResID);
	void         UnLoadMap(  );
	void         FastFadingMode( bool bEnable );

	virtual bool RecvMessage( MG_MessageBase& rkMessage );
	

	//---------------   System   ---------------------
	void        ApplyConfigSettings();
	//--------------    Source   ----------------------
	bool        SetSoundVolume(int iID, float fVolume);
	bool		FadeSoundVolume(int iID, float fVolume, float fadeTime = 0.5f);
	void		ApplyBGMVolumeChange();

	void        SetLoading( bool bEnable ){ m_bLoading = bEnable; }

	StringType  GetBGMName();

	virtual bool FilleAnimEventItem(const StringType& key, FileSystem::MTF_ModelAnimSoundEventItem* item);

    virtual int Play2D(
        const StringType& strSoundName,
		const StringType& strKeyCodeName,
        float fVolume,
        bool bLoop = false);
	int         Play2D(
		const StringType& strSoundName,
		const StringType& strKeyCodeName,
		SOUND_CHANNEL_TYPE eChannelType,
		bool bFadeToGain = false,
		int  nPlayCount = 1 ,
		float fAutoStop = 0.0f);
	int         Play2DWithVolume(
		const StringType& strSoundName,
		const StringType& strKeyCodeName,
		SOUND_CHANNEL_TYPE eChannelType,
		float ffVolume = 1.0f,
		bool bFadeToGain = false,
		int  nPlayCount = 1 ,
		float fAutoStop = 0.0f);
	int			Play2DEx(	const StringType& strKeyName,
		SOUND_CHANNEL_TYPE eChannelType = AS_ClientSoundManager::SCT_2D,
		bool bFadeToGain = false,
		int  nPlayCount = 1 ,
		float fAutoStop = 0.0f);

	int PlayAreaEffect( const StringType& strAllSoundKeyName);
	int PlayAreaEffectPos(const StringType& strAllSoundKeyName, const NiPoint3& pos, int channel);
	//需要重新实现，把优先级转换为客户端Channel规则
	virtual int Play3D(
		const NiPoint3& kPos, 
		const StringType& strSoundName,
		const StringType& strKeyCodeName,
		Base::Importance ePriority,bool bLoop, float fVolume =1.0f );

	int         Play3D(
		const NiPoint3& kPos, 
		const StringType& strSoundName,
		const StringType& strKeyCodeName,
		SOUND_CHANNEL_TYPE eChannelType,
		bool bFadeToGain = false , 
		bool bLoop = false , float fMinDist = MIN_DISTANCE_3D_FALLOFF , 
		float fMaxDist = MAX_DISTANCE_3D_FALLOFF,
		float ffVolume=1.0f);

	int         Play3DEx(
		const NiPoint3& kPos, 
		const StringType& strKeyName,
		SOUND_CHANNEL_TYPE eChannelType,
		bool bFadeToGain = false , 
		bool bLoop = false , 
		float fControlVolume = 1.0f,
		float fMinDist = g_settings.SOUND_COMMON_MIN_DISTANCE, 
		float fMaxDist = g_settings.SOUND_COMMON_MAX_DISTANCE);

	//根据2d/3d类型播放音效
	int			PlayAutoEx(const StringType& strKeyName,
		const NiPoint3& kPos, 
		SOUND_CHANNEL_TYPE eChannelType,
		bool bFadeToGain = false,
		int  nPlayCount = 1 ,
		float fAutoStop = 0.0f);

	void        PlayHittedSound( 
		const NiPoint3& kPos, 
		const StringType& rstrFrom,
		const StringType& rstrTo );

	virtual void StopAudioSource(int iID, bool bFade = false , float fFadeTime = 0.0f );

	virtual void StopAudioSourceFade(int iID, float fadeTime);

	virtual void CleanSourceMap();

	virtual bool IsAudioFile2d(const StringType& kSoundName);

	void StopCurrentNPCSound();
	void StopCurrentBGM();
	void StopPlayerSkillSounds();
	void StopNPCSkillSounds();

	void StopAllEnvSound();

	void PauseCurrentBGM();
	void ResumeCurrentBGM();

	NiFile* CreateSoundFile( const StringType& rkName );

	void		ClearBossFightInfo();

	void ParseSequenceString(const StringType& strBuff);
	void PlaySequenceNPCSound();

	void OnPlayNPCSound(float affect, object_id_type charTypeID);
	void OnEndPlayNPCSound();

	void PlayBGMForLogin(const StringType& bgmName);
	bool IsSoundPlaying(int iSoundID);

public:
	// async load
	bool	PlaySound_AsyncLoad( int iID, AudioLoadArg * pkArg, SoundInfo * pkSoundInfo );
	bool	PlaySound_AsyncLoading( AudioLoadArg * pkArg );
	void	PlaySound_AsyncPostLoad( bool bLoaded, bool bTimeOut, int iID, AudioLoadArg * pkArg, void * pkSoundInfo );	// SoundInfo

private :
	void         _on_windows_lost_focus(MG_MessageBase& rkMessage);
	void         _initChannel(  );
	void         _event_LogicPlayerMove(MG_MessageBase& rkMessage);
	void         _event_TimerTick(MG_MessageBase& rkMessage);
	void         _event_LogicAreaSoundName(MG_MessageBase& rkMessage);
	void		 _event_NPCBornBegin(MG_MessageBase& rkMessage);
	void		 _event_NPCDeadBegin(MG_MessageBase& rkMessage);



	int          _getNumPlaying();
	bool         _cleanChannel( int iID );
	bool         _addFadingTask(int iID, const StringType& strSoundName,  int iChannel,
		float fVolume = 0.8f, 
		int iPlayCount = NiAudioSource::LOOP_SINGLE , float fFade = 0.0f );
	bool         _fade2DSound( int iAudioID, const StringType& rkName, 
		float fVolume, const StringType& rkLastName,float fFadeTime, int iChannel);

	void         _updateFadingSoundQueue();
	void         _loadCSVSoundList();
	void         _filterStaticSound();
	void         _updateEnvSound();

	bool		_updateNPCSoundSeq();

	float        _getRandomLastTime();
	void         _apply_volume_all(float fVolume);
	bool         _trimFunc( const NiPoint3& kListenerPos, 
		const NiPoint3& kSoundPlayPos, 
		float fX, float fY );

	void	     _addToCache( int iID, const StringType& rstrSoundName );
	int		     _lookUpCache(const StringType& strSoundName, const NiPoint3& k3DPosition);

	void	     _initHittedSound();

	bool	_KeyToRandomOgg(const StringType& ke, StringType& ogg);

	void	_cleanChannelSourceMap(SoundInfoList& sil);

	bool        _checkPlayingRules( 
		SOUND_CHANNEL_TYPE eChannel,
		const StringType& strName, const StringType& strKeyName, float fVolume );

	float	_randomSoundVolume(const CF_SD_Call::DataEntry* entry);

private:

	void	_updateMetrics(void);

private:

	SoundChannel                    m_kSoundChannel[SCT_MAX];
	FadingSoundList                 m_kFadingSoundQueue;
	bool							m_bNPCSoundFade;
	CSVFile::CF_SoundObjectList*	m_pkCSVSoundList;
	CSVFile::CF_HittedSoundList*	m_pkCSVHittedList;
	CSVFile::CF_UISound*			m_pkCSVUISoundList;
	CSVFile::CF_SD_Call*			m_pkCSVAllSoundList;
	//逻辑区域音乐
	CSVFile::CF_LogicAreaInfo*     m_pkLogicAreaInfo;

	typedef mem::map<int,StringType> LogicAreaNameMap;
	LogicAreaNameMap                m_kLogicAreaName;
	bool                            m_bLoading;	

	float							m_fEnvSoundUpdateTime;
	int								m_iLocalPlayerID;
	NiPoint3                        m_kDirection;
	float                           m_fVolumeControl;
	unsigned int                    m_uiLastBGMPos;
	bool                            m_bPrevActive;
	float                           m_fLastVolume;
	typedef mem::map<StringType, CSVFile::NS_CF_SD_Call::DataEntry*> AllSoundMap;
	AllSoundMap						m_kAllSoundMap;
	FadedEnvSoundList               m_kFadedEnvSoundList;

	NPCBornSoundMap					m_kNPCBornSoundMap;
	NPCBornSoundMap					m_kNPCBornVoiceMap;
	NPCBornSoundMap					m_kNPCBornBGMMap;
						
	struct BossFightSound
	{
		bool		playState;
		StringType	lastPlayName;
		object_id_type bossID;
		float		lastFadeTime;
		float		lastInterval;
		BossFightSound()
		{
			clear();
		}
		void clear()
		{
			playState = false;
			lastPlayName = "";
			bossID = 0;
			lastFadeTime = 0.0f;
			lastInterval = 0.0f;
		}
	};

	BossFightSound					m_kBossFightSound;

		
	struct SoundSeq 
	{
		SoundSeq()
		{
			keyCode ="";
			fTimeLength = 0.0f;
		}
		StringType keyCode;
		float fTimeLength;
	};

	struct NPCSoundSeqData
	{	
		NPCSoundSeqData()
		{
			Reset();
		}
		void Reset()
		{
			m_NPCSoundSeq.clear();
			m_kFlag =false;
			m_kCurrentIndex =0;
			m_playTick = 0.0f;
		}
		void Active()
		{
			if (!m_NPCSoundSeq.empty())
			{
				m_kFlag = true;
				m_kCurrentIndex = 0;
				m_playTick = 0.0f;
			}
			else
			{
				m_kFlag = false;
				m_kCurrentIndex = 0;
				m_playTick = 0.0f;
			}
		}
		int IsValid()
		{
			return (m_kFlag && !m_NPCSoundSeq.empty() && m_kCurrentIndex < m_NPCSoundSeq.size());
		}
		SoundSeq GetCurrentSeq()
		{
			if (m_kCurrentIndex>=0 && m_kCurrentIndex<m_NPCSoundSeq.size())
				return m_NPCSoundSeq[m_kCurrentIndex];
			else
				return SoundSeq();
		}

		mem::vector<SoundSeq>		m_NPCSoundSeq;
		bool						m_kFlag;
		int							m_kCurrentIndex;
		float						m_playTick;
	};

	NPCSoundSeqData			m_kNPCSoundSeqData;

	int						m_kFlagNPCSoundPlay;

private:

	virtual bool   _stop(int iID, bool bFadeToGain = false , float fFade = 0.0f );

	AudioSourceMap		m_kAsyncLoadingSource;
	/// 刚播放的时候，往这里插入的是NULL，如果调用_stop，把指针从m_kSourcesMap移到这里
	/// 相当于把 AS_SoundManagerBase::_stop 拆成两部分
};
#define g_soundMgr AS_ClientSoundManager::GetSingleton()