#pragma once
//�첽IO��������
#include "FX_ClientFxManager.h"
#include "SE_SceneWorldMap.h"
#include "GD_ClientAsyncModelManager.h"
#include "CR_UIType.h"
#include "BL_AsyncOperationBase.h"

//==============================================================================
// ������Ҫ�첽��ʽ������Դ�Ĳ����Ļ���
//==============================================================================
enum BL_PRIORITY_VALUE
{
	BPV_1 = 79,
	BPV_2 = 80,
	BPV_3 = 81,
	BPV_4 = 82,
	BPV_5 = 90,
	BPV_6 = 99,
	BPV_7 = 100,
};
#define COMMON_MODEL_LOAD_OP_PRIORITY      BPV_2
#define SCENE_MODEL_LOAD_OP_PRIORITY       BPV_4
#define TERRAIN_LOAD_OP_PRIORITY           BPV_6
#define PLAYER_MODEL_LOAD_OP_PRIORITY      BPV_5
#define NPC_MODEL_LOAD_OP_PRIORITY         BPV_7
#define ITEM_MODEL_LOAD_OP_PRIORITY        BPV_4
#define CHANGE_AVATAR_OP_PRIORITY          BPV_3
#define FX_LOAD_OP_PRIORITY                BPV_6
#define UI_LOAD_OP_PRIORITY                BPV_5
#define PATHMESH_LOAD_OP_PRIORITY          BPV_7
#define UNLOAD_OP_PRIORITY                 BPV_2
#define WALKABLE_LOAD_OP_PRIORITY          BPV_5
#define VIDEO_LOAD_OP_PRIORITY			   BPV_2
#define TMAP_LOAD_OP_PRIORITY			   BPV_3
#define CONFIG_RELOAD_OP_PRIORITY		   BPV_2
#define DUOWAN_REPORT_OP_PRIORITY		   BPV_1
#define AUDIO_LOAD_OP_PRIORITY			   BPV_3
#define HTTP_UPLOAD_OP_PRIORITY			   BPV_1
#define HTTP_DOWNLOAD_OP_PRIORITY		   BPV_1


typedef mem::list<BL_AsyncOperation*> AsyncOperationQueue;
typedef AsyncOperationQueue::iterator AsyncOperationQueueIter;


class SortedOpeartionQueue
{
public:
	enum	POP_STRATEGY
	{
		POPS_TOP,
		POPS_TOP_READY_LOAD_OP,

		POPS_MaxCount,
	};

	SortedOpeartionQueue();
	~SortedOpeartionQueue();

	bool empty() const;
	size_t size() const;

	void push( BL_AsyncOperation* p );
	BL_AsyncOperation* pop_top(POP_STRATEGY s);

	BL_AsyncOperation* pop_top();
	BL_AsyncOperation* pop_top_ready_load();

    unsigned int count_if_priority_is_ge(int nPriority);

	bool remove( BL_AsyncOperation* op );
	void clear();

private:
	typedef mem::list<BL_AsyncOperation*> OperationQueue;
	typedef mem::map<int,OperationQueue*,std::greater<int> > OperationQueueMap;	//�����ȼ��Ӹߣ��󣩵��ͣ�С������
	OperationQueueMap c;
};

typedef SortedOpeartionQueue AsyncOperationHeap;

//==============================================================================
// �����첽������Դ�Ĳ���
//==============================================================================

#ifdef USE_DOWNLOADER
#define	USE_MULTI_LOAD_THREADS	//�������������߳�
#endif


class BL_AsyncTerrainOperation : public BL_AsyncOperation
{
public:
    BL_AsyncTerrainOperation(void);
	virtual ~BL_AsyncTerrainOperation(void);

	void Init(SE_SceneWorldMap* pkOperateMap, const SE_ChunkIndexPool& rkLoadingChunks);

	virtual void Reset(void);
    virtual bool Load(IThreadControl *pCtrl);
    virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

	virtual void SetStatus( bool bLoad );

	const SE_ChunkIndexPool& GetWorkSetIndexMap(void) { return m_kWorkSetIndexMap; }

protected:
	SE_SceneWorldMap*   m_pkOperateMap;
	SE_ChunkIndexPool   m_kWorkSetIndexMap;
};

//==============================================================================
// ��ɫ�첽������Դ�Ĳ���
//==============================================================================
namespace Data
{
	class GD_ModelBase;
}

class BL_AsyncActorOperation : public BL_AsyncOperation
{
public:
	BL_AsyncActorOperation();
	virtual ~BL_AsyncActorOperation();

    void Init( GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE eModelType, Data::GD_ModelBase* pkAsyncModel, 
												const StringType& strModFileName, 
												int iModelIndex, unsigned int m_uiShaderIdentifier );
	virtual void SetStatus( bool bLoad );

	Data::GD_ModelBase* GetModel() const { return m_pkAsyncModel; }

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

    bool IsSceneModel() { return m_eModelType == GD_ClientAsyncModelManager::AMT_SCENE; }
	const StringType & GetModFileName() const { return m_strModFileName; }

protected:
    GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE m_eModelType;
    Data::GD_ModelBase* m_pkAsyncModel;

    StringType          m_strModFileName;

	int					m_iModelIndex;
	unsigned int		m_uiShaderIdentifier;
};

//==============================================================================
// ��װ�첽������Դ�Ĳ���
//==============================================================================
class GD_ClientActorModel;

class BL_AsyncChangeAvatarOperation : public BL_AsyncOperation
{
public:
    BL_AsyncChangeAvatarOperation(void);
	virtual ~BL_AsyncChangeAvatarOperation(void);

	void Init(GD_ClientActorModel* pkActorModel);

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void) { return true; }
	virtual bool UnLoad(void) { return true; }

protected:
	GD_ClientActorModel*    m_pkActorModel;

    NiNodePtr               m_spActorNewSuitNiRootNode;
};

//==============================================================================
// UI Effect �첽������Դ�Ĳ���
//==============================================================================

//class UE_UiEffectBase;
//
//class BL_AsyncUIEffectLoadOperation : public BL_AsyncOperation
//{
//public:
//	BL_AsyncUIEffectLoadOperation(void);
//	virtual ~BL_AsyncUIEffectLoadOperation(void);
//
//	void Init(long lID, long lType);
//	void InitUnLoad(BYTE* pValue);
//
//	virtual void Reset(void);
//	virtual bool Load(void);
//	virtual bool PostLoad(void);
//	virtual bool PreUnLoad(void);
//	virtual bool UnLoad(void);
//
//protected:
//	long m_nLogicID;
//	long m_lType;
//	UE_UiEffectBase* m_pkToDelete;
//};

//==============================================================================
// ��Ч�첽������Դ�Ĳ���
//==============================================================================

namespace FX
{
	class FX_Composite;
}

class BL_AsyncFxLoadOperation : public BL_AsyncOperation
{
public:
	BL_AsyncFxLoadOperation(void);
	virtual ~BL_AsyncFxLoadOperation(void);

	FX_ClientFxManager::FxObject* GetFxObject()  const { return m_pkFxObject; }

	void Init(FX_ClientFxManager::FxObject* pkFxObject);
	void InitUnLoad(FX_ClientFxManager::FxObject* pkFxObject);

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

private:
	static void QueryDestroySmoothly(FX_ClientFxManager::FxObject& rkFxObject);

protected:
	FX_ClientFxManager::FxObject* m_pkFxObject;
};

//==============================================================================
// ??? �첽������Դ�Ĳ���
//==============================================================================

namespace GUISys
{
	class IG_Material;
}

class BL_AsyncUIImgLoadOperation : public BL_AsyncOperation	//< һ��UI��ͼ
{
public:
	BL_AsyncUIImgLoadOperation(void);
	virtual ~BL_AsyncUIImgLoadOperation(void);

	void Init(BYTE* pValue);
	void InitUnLoad(BYTE* pValue, long iErase);

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);
	const StringType &   GetImgSetName() {return m_sImgName;}

protected:
	StringType		     m_sImgName;
	StringType			 m_sDeleteName;
	GUISys::IG_Material* m_pkToDelete;
	bool				 m_bLoad;
	int					 m_iErase;//��erase����0ʱ��Ѷ���ɾ��,ֻ����unload��Ϣʱerase��������

};

class BL_AsyncUIImgClusterLoadOperation : public BL_AsyncOperation	//< һ��UI��ͼ
{
public:
	BL_AsyncUIImgClusterLoadOperation(void);
	virtual ~BL_AsyncUIImgClusterLoadOperation(void);

	void Init(BYTE* pValue);
	void InitUnLoad(BYTE* pValue, long iErase);

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

protected:
	StringType		     m_sImgClusterName;
	bool				 m_bLoad;
};
//==============================================================================
// Ѱ·Ƭ�첽������Դ�Ĳ���
//==============================================================================

class BL_AsyncPathMeshOperation : public BL_AsyncOperation
{
public:
	BL_AsyncPathMeshOperation();
	virtual ~BL_AsyncPathMeshOperation();

	void Init( long lTileIndex );
	long GetTileIndex(){ return m_lTileIndex; }

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

protected:
	long m_lTileIndex;
};

//==============================================================================
// ��Ƶ�첽������Դ�Ĳ���
//==============================================================================

class BL_AsyncPlayVideoOperation : public BL_AsyncOperation
{
public:
	BL_AsyncPlayVideoOperation();
	virtual ~BL_AsyncPlayVideoOperation();

	void Init( int iVideoType, const char *cpImagename, const char *cpFilename, unsigned uLoopCount );

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

protected:
	int			m_iVideoType;
	StringType	m_kImagename;
	StringType	m_kFilename;
	unsigned	m_uLoopCount;
	bool		m_bLoad;

	//����StringType��Ϊ����Release���˳�ʱ�������ڴ�й¶
	static 	char	ms_kValidVideo[MAX_PATH];		//ͬʱֻ����һ����Ƶ���ţ����ڶ��PlayVideoOpʱ�����ͨ����֤����ִ��PostLoad
};

//==============================================================================
// �����첽������Դ�Ĳ���
//==============================================================================
class AudioLoadArg;
class BL_AsyncPlayAudioOperation : public BL_AsyncOperation
{
public:
	BL_AsyncPlayAudioOperation();
	virtual ~BL_AsyncPlayAudioOperation();

	void Init( int iID, AudioLoadArg * pkArg, void * pkSoundInfo );

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

protected:
	bool			m_bLoad;
	bool			m_bPlayOK;
	int				m_iID;
	AudioLoadArg *	m_pkArg;
	void *			m_pkSoundInfo;
	unsigned int    m_uStartTimeMs;
};

//==============================================================================
// ��ͼ.tmap�첽������Դ�Ĳ���
//==============================================================================

class BL_AsyncLoadMapOperation : public BL_AsyncOperation
{
public:
	BL_AsyncLoadMapOperation();
	virtual ~BL_AsyncLoadMapOperation();

	void Init( const char *cpWorldMapName );

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);
};

//==============================================================================
// csv��lua�� reload�첽������Դ�Ĳ���
//==============================================================================

class BL_AsyncReloadConfigOperation : public BL_AsyncOperation
{
public:
	BL_AsyncReloadConfigOperation();
	virtual ~BL_AsyncReloadConfigOperation();

	void Init( TStringVector * lst, uint32_t timestamp );

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

protected:
	TStringVector	m_kCfgFileList;
	uint32_t		m_uitimestamp;
};

//==============================================================================
// duowan�����ϱ��Ĳ���
//==============================================================================

class BL_AsyncDuowanReportOperation : public BL_AsyncOperation
{
public:
	BL_AsyncDuowanReportOperation();
	virtual ~BL_AsyncDuowanReportOperation();

	void Init_1( int sceneId, const char * sid, const char * rolename );
	void Init_2( const char * chatContent, const char * toUserName, const char * fromUserName, int bMultipy, int sceneId );
	void Init_3( int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power );

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

protected:
	int		m_opType;		//< 1: GetReportable, 2: ReportChatMsg, 3: ReportLevelUp

	struct  GetReportable
	{
		int				m_sceneId;
		StringType		m_sid;
		StringType		m_role_name;
	}o1;

	struct  ReportChatMsg
	{
		StringType		m_chatmsg;
		StringType		m_toUserName;
		StringType		m_fromUserName;
		int				m_bMultipy;
		int				m_sceneId;
	}o2;

	struct  ReportLevelUp
	{
		int				m_role_level;
		unique_id_type	m_uuid;
		StringType		m_role_name;
		int				m_role_gender;
		int				m_role_profession;
		int				m_role_power;
	}o3;
};

//==============================================================================
// http�ļ��ϴ��Ĳ���
//==============================================================================

class BL_AsyncCurlOperation : public BL_AsyncOperation
{
public:
	BL_AsyncCurlOperation();
	virtual ~BL_AsyncCurlOperation();

	void Init( bool bUpload, const char *cpUrl, const char *cpAuth, const char *cpFilename, int64_t nTransId, unsigned nTimeOutMs );

	virtual void Reset(void);
	virtual bool Load(IThreadControl *pCtrl);
	virtual bool PostLoad(void);
	virtual bool PreUnLoad(void);
	virtual bool UnLoad(void);

protected:

	bool		m_bUpload;		// true: upload, false: download
	StringType	m_strUrl;		// url
	StringType	m_strAuth;		// username:password
	StringType	m_strFilename;	// �����ļ���
	int64_t		m_nTransId;		// ����id
	unsigned	m_nTimeoutMs;	// ��ʱʱ��
	unsigned	m_nTimeCostMs;	// �������ʱ��

	int			m_eTaskCode;	// ������
	int			m_eErrCode;		// ������
};
