#ifndef _SE_SECENEMANAGER_H
#define _SE_SECENEMANAGER_H

#include "MG_MessageHandlerInterface.h"
#include "SE_SceneTerrainInfo.h"
#include "TS_MapEffectFile.h"
#include "TS_DynamicWeather.h"
#include "UT_RenderHelper.h"
#include "RG_CustomeShadowGenerator.h"

class BL_MainProcess;
class SE_SceneWorldMap;
class SE_WorldMiniMap;
class SE_WorldChunkGeometryDataManager;
class AC_ActorNPC;
class SE_TerrainCallback;
class BL_AsyncOperationManager;
class GD_ClientSceneModel;

#ifdef	_ENABLE_PHYSX
class UT_PhysXLog : public NxUserOutputStream
{
public:
	virtual void reportError(NxErrorCode code, const char *message,
		const char *file, int line); 
	virtual NxAssertResponse reportAssertViolation (const char *message,
		const char *file, int line);
	virtual void print (const char *message);    
};

class UT_PhysXAllocator : public NxUserAllocator
{
public:
	virtual void* malloc(NxU32 size);
	virtual void* mallocDEBUG(NxU32 size, const char* fileName, int line);    
	virtual void* realloc(void* memory, NxU32 size);   
	virtual void free(void* memory);
};

typedef mem::map<NxActor*,NxTriangleMesh*> NxSceneMeshMap;
typedef NxSceneMeshMap::iterator NxSceneMeshMapIter;
#endif

class SE_SceneManager : public Memory::MM_BaseObject,
						public MG_MessageHandlerInterface,
						public Utility::UT_Singleton<SE_SceneManager>
						 
{
public:
    DECL_RTTI_NAME(SE_SceneManager)

	SE_SceneManager();
	~SE_SceneManager();

	bool Init();
	bool UnInit();
	//由于退出时的依赖关系比较复杂
	//所以在退出的时候先中止后台加载线程，再删除SceneManager

	bool LoadWorldMap(const StringType& rkWorldName, const StringType& rkDecoration , long nMapID, long lR );

	bool UnLoadWorldMap();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

    void PreCullModels(void);

    void GetModelVisibleArray(NiGeometryList& rkCasterList);

	void GetInfiniteModel(NiCameraPtr spNiCamera , NiVisibleArray& rkVisible);
	void GetTerrainVisibleArrary(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void GetModelVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void GetShadowCasters(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiGeometryList& rkVisibleArray);

    void GetWalkableModelVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void GetWaterVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray,int nCullChunk);
    void GetReflectVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray,bool bTerrain);
	void GetSceneFxVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void GetPathMeshVisibleArray(NiCameraPtr spNiCamera, NiVisibleArray& rkVisibleArray);
	void GetSceneTransparentVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);

	float GetCurrentWaterHeight();

	void SetFogShow( bool bShow );
	void SetWaterShow( bool bShow );
	void SetModelShow( bool bShow );
	void SetShadowShow( bool bShow );
	void SetGrassShow( bool bShow );
	void SetTerrainShow( bool bShow );
	void SetSceneFxShow( bool bShow );

	SE_WorldMiniMap*    GetWorldMiniMap() {	return m_pkWorldMiniMap; }
	SE_SceneWorldMap*   GetWorldMap() { return m_pkSceneWorldMap; }

	void AttachToLightEffect(NiNodePtr spAffectNode , bool bSceneFx );
	void DetachToLightEffect(NiNodePtr spUnAffectNode , bool bSceneFx );

	void RemoveUnShadowCastNiNode(NiNodePtr spUnShadwoCastNiNode);
	void RemoveUnShadowReceiveNiNode(NiNodePtr spUnShadowReceiveNiNode);
	void ClearShadowGenerator();
	void ResetReflectTexture();
	
	unsigned int		PerformGrassInstanceRendering();
	//获得地形层高度和法线
	const SE_SceneTerrainLocalInfo& GetTerrainLocalInfo(float fLocationX, float fLocationY, 
        bool bGetHeight = true, bool bGetNoraml = false);
	//获得行走层高度和法线
	const SE_SceneTerrainLocalInfo& GetWalkableInfo(float fLocationX, float fLocationY, 
		bool bGetHeight = true, bool bGetNoraml = false);

	const SE_SceneTerrainLocalInfo& GetWalkableInfo(float fLocationX, float fLocationY, 
		float fPrevHeight,
		bool bGetHeight = true, bool bGetNoraml = false);
    // 获得高度图顶点高度
    float GetHeightFieldHeight(float fHeightFieldCoordX, float fHeightFieldCoordY);

	bool                PickVerticalHeight( long lX , long lY , long& rlZ );

    NiColor             GetCurrentShadowColor(void);
	float               QueryVisibilityMap( const NiPoint3& kPos );

#ifdef _ENABLE_PHYSX
	bool				AddPhyxProp(NiPhysXPropPtr spProp);
	void				RemovePhyxProp(NiPhysXPropPtr spProp);
	void				AddPhyxCollisionSphere(const NiPoint3& rkCenter,float fRadius);
	NxActor*            AddPhyxCollisionSceneChunk(const Utility::UT_PhysXMemStream& rkMemStream );
	void				RemovePhyxCollisionSceneChunk(NxActor* pkActor);
	void				BeginPhyxUpdateLock();
	void				EndPhyxUpdateLock();
	void				UpdatePhyx(float fCurrentTime,bool bForce);
	void				UpdatePhyxSource(float fCurrentTime);
#endif	

	long GetCurrentMapID(){return m_nCurrentMapID;};    //返回当前地图ID号
	bool WorldMapLoaded(){return m_bLoadWorldMap;};     //返回地图是否已经加载
	void UpdateShadowManager();

	void MarkVisibleArrayDirty();

    //enum ForcibleWeatherType
    //{
    //    FWT_LIGHT_RAIN,     // 小雨
    //    FWT_MODERATE_RAIN,  // 中雨
    //    FWT_HEAVY_RAIN,     // 大雨

    //    FWT_LIGHT_SNOW,     // 小雪
    //    FWT_MODERATE_SNOW,  // 中雪
    //    FWT_HEAVY_SNOW      // 大雪
    //};
    void StartForcibleWeather(float fDensity, float fSpeed, float fLength, float fWidth, float fAlpha, float fWind);
    void StopForcibleWeather(void);

	Terrain::TS_DynamicWeather* GetWeatherManager(){ return m_pkWeatherManager; }

	bool IsGrassShow() { return m_bEnableGrass; }
	bool IsWaterShow() { return m_bEnableWater; }
	bool IsModelShow() { return m_bEnableModelShow; }
	bool IsTerrainShow() { return m_bEnableTerrainShow; }

	MTF_EffectTile::AreaEffectType GetLocationAreaEffect(const Utility::UT_Vec3Int& kLocation);

    void UseExternalPointLight(bool bUse) { m_bUseExternalPointLight = bUse; }
    void SetPlayerPointLightParams(const NiColor& kColor, const NiPoint3& kPos, const NiPoint3& kParam);

    void ReloadSkyBox(void);


	enum
	{
		OUTDOOR_DIRLIGHT_SHADOW  = 0,
		INDOOR_POINTLIGHT_SHADOW = 1,
		INVALID_SHADOW,
	};

	int					GetCurrentShadowMode(void) { return m_nCurrentShadowMode; }

    // 更新场景shader参数
    void UpdateSceneShaderGlobalParams(float fTime, const NiColor& kGroundAmbient, const NiColorA& kAmbientLight, 
        const NiColorA& kDepthFogParam, const NiColorA& kHeightFogParam, const NiColor& kFogColor, 
        const NiColorA& kDiffuseSunLightColor, const NiColorA& kShadowColor, const NiPoint3& kLightDir, bool bShadowIndoor, 
        const NiPoint3& kActorPlayerLocation);

    // 更新or获取人物shader参数
    void UpdateCharacterShaderGlobalParams(float fAmbientPower, float fSunPower, float fMatEmmisive, 
        float fSelfIllumination, float fSpecularPower, float fSpecularMultiplier, 
        const NiColor& kDiffuseLightColor, const NiColor& kAmbientColor, const NiPoint3& kLightWorldDir, 
        const NiColor& kRimLightColor, float fRimLightPower, float fIllumBias);
	void GetCharacterShaderGlobalParams(float& fAmbientPower, float& fSunPower, float& fMatEmmisive, 
		float& fSelfIllumination, float& fSpecularPower, float& fSpecularMultiplier, 
		NiColor& kDiffuseLightColor, NiColor& kAmbientColor, NiPoint3& kLightWorldDir, 
		NiColor& kRimLightColor, float& fRimLightPower, float& fIllumBias);

    void UpdatePlayerPointLightShaderGlobalParams(float fPLEnable, 
        const NiColor& kPLColor, const NiPoint3& kPLPos, const NiPoint3& kPLParam);
    // 更新后期shader参数
    void UpdatePostProcessShaderGlobalParams(float fSharp, float fBlur, float fExp, float fPower);




	//空气墙
	void    SetFxWallShow( bool bShow );
	bool    GetFxWallShow() { return m_bValidFXWall; }

	void    SetFxWallID( int iIndex, int iID);
	int     GetFxWallID( int iIndex);

	bool    PickScene( const NiPoint3& rkPos , const NiPoint3& rkDir , NiPoint3 * pkPickPos = NULL );
	bool	PickScene( long lWx, long lWy, NiPoint3 * pkPickPos = NULL );

    // 本地NPC注册管理
    void RegisterLocalNpcToLocation(long lLocalNpcID, const Utility::UT_Vec3Int& kLocation);

protected:
#ifdef _ENABLE_PHYSX
   void _beginPhysxLock();
    void _endPhysxLock();
#endif
	void _initMapEffectConfig( long nMapID );

	void	_event_TimerTick(MG_MessageBase& rkMessage);
	void	_event_LogicPlayerMove(MG_MessageBase& rkMessage);
	void	_event_InputMouseLButtonDown(MG_MessageBase& rkMessage);
	void	_event_InputMouseLButtonDBClk(MG_MessageBase& rkMessage);
	void	_event_InputMouseLButtonUp(MG_MessageBase& rkMessage);
	void	_event_InputMouseRButtonDown(MG_MessageBase& rkMessage);
	void	_event_InputMouseRButtonUp(MG_MessageBase& rkMessage);
	void    _event_InputMouseMove(MG_MessageBase& rkMessage);

	void	_initShadowManager();
	void    _uninitShadowManager();
	void	_initGrassInstanceData();
	void    _uninitGrassInstanceData();
#ifdef _ENABLE_PHYSX
	void	_initPhysXManager();
	void	_unintPhysXManager();
#endif

	void	_switchShadowMode(int nMode);

protected:
    const NiFixedString GSC_NAME_time;
    const NiFixedString GSC_NAME_GroundAmbient;
    const NiFixedString GSC_NAME_AmbientLight;
    const NiFixedString GSC_NAME_DepthFogParam;
    const NiFixedString GSC_NAME_HeightFogParam;
    const NiFixedString GSC_NAME_FogColor;
    const NiFixedString GSC_NAME_DiffuseSunLightColor;
    const NiFixedString GSC_NAME_ShadowColor;
    const NiFixedString GSC_NAME_LightDir;
    const NiFixedString GSC_NAME_ShadowIndoor;
    const NiFixedString GSC_NAME_ActorPlayerLocation;
	const NiFixedString GSC_NAME_SceneObjectSaturationInc;

    const NiFixedString GSC_NAME_Sharp;
    const NiFixedString GSC_NAME_Blur;
    const NiFixedString GSC_NAME_Exp;
    const NiFixedString GSC_NAME_Power;

    const NiFixedString strCharacterAmbientPower;
    const NiFixedString strCharacterSunPower;
    const NiFixedString strCharacterMatEmmisive;
    const NiFixedString strCharacterSelfIllumination;
    const NiFixedString strCharacterSpecularPower;
    const NiFixedString strCharacterSpecularMultiplier;
    const NiFixedString strCharacterDiffuseLightColor;
    const NiFixedString strCharacterAmbientColor;
    const NiFixedString strCharacterLightWorldDir;
    const NiFixedString strCharacterRimLightColor;
    const NiFixedString strCharacterRimLightPower;
    const NiFixedString strCharacterIllumBias;

    const NiFixedString strPlayerPointLightPos;
    const NiFixedString strPlayerPointLightColor;
    const NiFixedString strPlayerPointLightParam;
    const NiFixedString strPlayerPointLightEnable;

    float HEIGHT_FIELD_GRID_SIZE;

	SE_SceneWorldMap*			m_pkSceneWorldMap;
	SE_WorldMiniMap*			m_pkWorldMiniMap;
	SE_WorldChunkGeometryDataManager* m_pkWorldChunkGeometryDataManager;
	
    NiCullingProcess            m_kPreCuller;

	NiVisibleArray				m_kShadowVisibleArray;
	NiShadowGeneratorPtr		m_spCurrentShadowGenerator; //general shadow generator
	NiShadowGeneratorPtr		m_spDirShadowGenerator;
	NiShadowGeneratorPtr        m_spFxLightShadowGenerator;
	RG_CustomeShadowClickGeneratorPtr m_spShadowClickGenerator;
	Utility::UT_FakeCullingProcessPtr m_spShadowCullingProcess;

	NiCullingProcess			m_kPathMeshCuller;
	SE_SceneTerrainLocalInfo	m_kTerrainLocalInfo;

	Terrain::TS_DynamicWeather* m_pkWeatherManager;
    Terrain::WeatherParameters  m_kWeatherParam;
    bool                        m_bUseExternalWeather;

	NiPick*						m_pkNiScenePick;
	NiPick*                     m_pkModelPick;
	NiNodePtr                   m_spGrassRoot;
	AC_ActorNPC*				m_pkLastPickedNPC;
	NiInstanceTriShapePtr		m_spGrassInstanceTriShape;
	bool						m_bLoadWorldMap;

#ifdef _ENABLE_PHYSX

	NiPhysXManager*				m_pkPhysManager;
 	NiPhysXScenePtr				m_spPhysScene;
 	UT_PhysXLog*                m_pkPhysXLog;
 	UT_PhysXAllocator*          m_pkPhysXAlloc;
	NiCriticalSection			m_kPhyxLock;
	NiCriticalSection           m_kPhyxUpdateLock;
 	NxSceneMeshMap              m_kSceneMeshMap;
#endif

	NiPoint3                    m_kCurrentPlayerPos;
	Terrain::TS_BoundingVolume  m_kPlayerAABB;
    NiPoint3                    m_kLastCameraPos;

	bool                        m_bEnableFog;
	bool                        m_bEnableWater;
	bool                        m_bEnableGrass;
	bool                        m_bEnblePathMeshShow;
	bool                        m_bEnableModelShow;
	bool                        m_bEnableTerrainShow;
	bool                        m_bEnableSceneFxShow;
	
    bool                        m_bFirstTerrainInFrame;
    bool                        m_bFirstStaticInFrame;
    bool                        m_bFirstModelInFrame;
	NiVisibleArray              m_kTerrainVisibleCache;
	NiVisibleArray              m_kSceneModelCache;
    NiVisibleArray              m_kSceneWalkableModelCache;
    NiVisibleArray              m_kSceneTransparentModelCache;
	NiVisibleArray				m_kTempVisibles;
	bool                        m_bLoadingMap;

	float                       m_fLastLightUpdateTime;
	float						m_fLastPhyxUpdateTime;
	float						m_fLastShadowUpdateTime;
	float                       m_fLastBlockModelUpdateTime;
	long                        m_nCurrentMapID;
	bool                        m_bUpdateLightPos;

    bool                        m_bUseExternalPointLight;

	int 						m_nCurrentShadowMode;

    GD_ClientSceneModel*        m_pkSkyBoxModel;

	bool						m_bValidFXWall;
	int							m_iFXWallID[MAX_FX_WALL_NUM];
};

#endif
