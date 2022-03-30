#ifndef _SE_SCENE_WORLD_H
#define _SE_SCENE_WORLD_H

#include "MG_MessageHandlerInterface.h"
#include "SE_SceneWorldChunk.h"
#include "TS_MapEffectFile.h"
#include "UT_Vec.h"
#include "SE_Terraincallback.h"
#include "RG_LightProbe.h"

class SE_SceneWorldChunkCluster;
class SE_SceneWorldTile;
class SE_SceneTerrainLocalInfo;
class SE_ActorShadowField;
class BS_ClientWaterMeshManager;

namespace FileSystem { class ClientMapGlobalInfoManager; }


//for multithread task
struct SE_ChunkIndex
{
	int _tileIndexX;
	int _tileIndexY;
	int _clusterIndexX;
	int _clusterIndexY;
	int _chunkIndexX;
	int _chunkIndexY;
};

struct ChunkDistanceCmp
{
	bool operator()( SE_SceneWorldChunk* l , SE_SceneWorldChunk* r ) const
    {
        return (l->GetPlayerDistance() < r->GetPlayerDistance());
    }
};

typedef mem::map<unsigned int, SE_ChunkIndex>   SE_ChunkIndexPool;
typedef SE_ChunkIndexPool::iterator				SE_ChunkIndexPoolIter;
typedef mem::vector<SE_SceneWorldChunk*>        SE_WorldChunkList;
typedef SE_WorldChunkList::iterator             SE_WorldChunkListIter;

class SE_SceneWorldMap : public MG_MessageHandlerInterface,
						 public Memory::MM_BaseObject
{
public:
	
	SE_SceneWorldMap();

	virtual ~SE_SceneWorldMap();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	void	RefreshMessageHandlerTree();
	
	void	GetTerrainVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void	GetModelVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray , bool bFirstInFrame );
	void	GetWalkableModelVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
    void	GetTransparentVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void	GetSceneFxVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);

	void	GetWaterVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray,int nCullChunk);
	void	GetReflectVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray , bool bTerrain );

	void    ResetReflectTexture();

	bool    InitWorldMap(const StringType& rkWorldName,const StringType& rkDecoration, int MapResID);
	void    UnInitWorldMap();

	void    AddToLoadedArray( SE_SceneWorldChunk* pkChunk );
	void    RemoveFromLoadedArray( SE_SceneWorldChunk* pkChunk );

	SE_SceneWorldTile* GetWorldTile(int iTileIndexX,int iTileIndexY)
    {
        int iTileIndex = iTileIndexY * WORLD_TILE_COUNTX_PER_MAP + iTileIndexX;
        if ( m_kSceneWorldTileVector.empty() || iTileIndex >= m_kSceneWorldTileVector.size() )
        {
            return NULL;
        }

        return m_kSceneWorldTileVector[iTileIndex];
    }

    void				SetVisibleChunks(SE_ChunkIndexPool& rkVisibleChunkPool) { m_kVisibleChunkPool = rkVisibleChunkPool; }
    SE_ChunkIndexPool 	GetVisibleChunks() const { return m_kVisibleChunkPool; }
    bool				IsChunkVisible(int iChunkID) { return (m_kVisibleChunkPool.find(iChunkID) != m_kVisibleChunkPool.end()); }

	NiPointLightPtr		    GetPointLight()         { return m_spPointLight; }
	NiDirectionalLightPtr   GetOutDoorLight()       { return m_spOutDoorLight; }
	NiDirectionalLightPtr   GetSceneFxLight()       { return m_spSceneFxLight; }
	NiPointLightPtr         GetSceneFxPointLight()  { return m_spSceneFxPointLight; }
	const NiColor&          GetBackLightColor()     { return m_kCurrentBackLightColor; }
	const NiColor&          GetAmbientColor()       { return m_kCurrentAmbientColor; }
	const NiColor&          GetDiffuseColor()       { return m_kCurrentDiffuseColor; }
	float                   GetCurrentWaterHeight( const NiPoint3& rkPlayer );

	MTF_EffectTile::AreaEffectType GetAreaEffectType( const Utility::UT_Vec3Int& rkPos )
    {
        if ( m_pkMapEffect )
        {
            return m_pkMapEffect->GetType(rkPos.x / (int)WORLD_GRID_WIDTH, m_pkMapEffect->m_pkFieldChunk->nWidth - rkPos.y / (int)WORLD_GRID_WIDTH);
        }
        return MTF_EffectTile::AET_UNKNOWN;
    }

	//深度雾
	const NiColorA&     GetDepthFogParam() { return m_kDepthFogParam; }
	//高度雾
	const NiColorA&     GetHeightFogParam() { return m_kHeightFogParam; }
	const NiColor&		GetFogColor() { return m_kCurrentFogColor; }
	const NiColor&      GetShadowColor() { return m_kCurrentShadowColor; }
	float               GetLightMultiplier() { return m_fCurrentLightMultiplier; }
	
	unsigned int        PerformGrassInstanceRendering( const NiPoint3& kPlayerPos , NiInstanceTriShapePtr spGrassInstanceTriShape, NiDX9Renderer* pkInstanceRenderer);

	bool				PickMap(NiPick* pkScenePick,const NiPoint3 &origin, const NiPoint3 &dir , const NiPoint3& kPlayerPos );
	bool                PickVertical( NiPick* pkScenePick, long lX , long lY , long& rlZ );

	bool				GetTerrainLocalInfo(NiPick* pkPick,
											SE_SceneTerrainLocalInfo& rkLocalInfo,
											float fLocationX,float fLocationY);

	bool				GetWalkableInfo(SE_SceneTerrainLocalInfo& rkLocalInfo, float fLocationX, float fLocationY);
	bool				GetWalkableInfo(SE_SceneTerrainLocalInfo& rkLocalInfo, float fLocationX, float fLocationY , float fPrevHeight );
    float               GetHeightFieldHeight(int iHeightFieldCoordIndexX, int iHeightFieldCoordIndexY);

	void                 UpdateCameraBlockModels( NiPick* pkPick , const Terrain::TS_BoundingVolume& rkPlayerAABB , const NiPoint3& kOrign,const NiPoint3& kDir );
	SE_CompactHeightField<float>*      GetHeightField(){ return m_pkHeightField; }
	SE_ActorShadowField*               GetVisibilityField(){ return m_pkShadowField; }
	SE_CompactHeightField<short>*      GetWalkableField(){ return m_pkWalkableField; }

	const NiPoint3&     GetSunDirection( ){ if(m_spOutDoorLight) return m_spOutDoorLight->GetWorldDirection(); return NiPoint3::ZERO;}

	float				GetCharacterAmbientPower(){return m_fCurrentCharAmbientPower;};
	float				GetCharacterSunPower(){return m_fCurrentCharSunPower;};
	float				GetCharacterMatEmmisive(){return m_fCurrentCharMatEmmisive;};
	float				GetCharacterSelfIllumination(){return m_fCurrentCharSelfIllumination;};
	float				GetCharacterSpecularPower(){return m_fCurrentCharSpecularPower;};
	float				GetCharacterSpecularMultiplier(){return m_fCurrentCharSpecularMultiplier;};
	const NiColor&      GetCharacterDiffuseLightColor(){return m_kCurrentCharDiffuseLightColor;};
	const NiColor&		GetCharacterAmbientColor(){return m_kCurrentCharAmbientColor;};
	const NiPoint3&     GetCharacterLightWorldDir(){return m_kCurrentCharLightWorldDir;};
	const NiColor&      GetCharacterRimLightColor(){return m_kCurrentCharRimLightColor;};
	float				GetCharacterRimLightPower(){return m_fCurrentCharRimLightPower;};
	float				GetCharacterIllumBias(){return m_fCurrentCharIllumBias;};
	//Bloom
	float               GetBloomSharp(){ return m_fCurrentSharpenIntensity; }
	float               GetBloomBlur(){ return m_fCurrentSoftLightScope; }
	float               GetBloomGammaExp(){ return m_fCurrentGammaExponent; }
	float               GetBloomGammaIntensity(){ return m_fCurrentGammaIntensity; }
	bool                GetBloomEnable(){ return m_bBloomEnable; }
	//Weather
	float               GetParticleDensity( ){ return m_fCurrentParticleDensity; }
	float               GetParticleSpeed( ){ return m_fCurrentParticleSpeed; }
	float               GetParticleWidth( ){ return m_fCurrentParticleWidth; }
	float               GetParticleLength(  ){ return m_fCurrentParticleLength; }
	float               GetParticleAlpha( ){ return m_fCurrentParticleAlpha; }
	float               GetParticleWind( ){ return m_fCurrentParticleWind; }
	const StringType&   GetWeatherTexturePath(  ){  return m_kCurrentWeatherTex; }
	bool                GetWeatherEnable(){ return m_bWeatherEnable; }
    // 场景闪电
    void GetCurrentRegionSceneLightningParams(float& fSLInterval, float& fSLPower);

    const NiColor&      GetPointLightColor() { return m_kPointLightColor; }
    const NiPoint3&     GetPointLightPos() { return m_kPointLightPos; }
    const NiPoint3&     GetPointLightParam() { return m_kPointLightParam; }
    float				GetPointLightEnable() { return m_fPointLightEnable; }

	BS_ClientWaterMeshManager* GetWaterMeshManager(){ return m_pkWaterMeshManager; }

	FileSystem::WorldMapFile*  GetWorldMapFile(){ return m_pkWorldMapFile; }
	FileSystem::ClientMapGlobalInfoManager* GetMapGlobalInfo(){ return m_pkMapGlobalInfo; }
	void				GetWhichTile(int&  iTileIndexX,
		                             int&  iTileIndexY,
		                             float fPointX,
		                             float fPointY);
	void                RegisterLocalNPC( long lSpawnPointID , const NiPoint3& rkPos );

    void SetCurrentSceneLightningPower(bool bSceneLightning, float fSceneLightningPower)
    {
        m_bIsCurrentInSceneLightning = bSceneLightning;
        m_fCurrentSceneLightningPower = fSceneLightningPower;
    }
	Render::RG_LightProbeManager* GetLightProbeMgr(){ return m_pkProbeMgr; };

	StringType					 m_kWorldFileName;

protected:

	void				_event_LogicPlayerMove(MG_MessageBase& rkMessageBase);
	void				_event_TimerTick(MG_MessageBase& rkMessage);
	
	void				_getVisibleSet( NiNodePtr spWaterRoot, NiVisibleArray &rkVisibleArray );
	void                _getReflectObjects( const SE_WorldChunkList& rkChunks, NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray, bool bTerrain );
	void _initCharacterParams();
	void _initPaticleParams();
	void _initFinalEffectParams();
	bool _IsEqual();
    bool _IsColorEqual( NiColor& kTarget, NiColor& kCurrent )
    {
        return (abs(kTarget.r - kCurrent.r) < 0.004f && abs(kTarget.g - kCurrent.g) < 0.004f && abs(kTarget.b - kCurrent.b) < 0.004f);
    }

	void	_updateTargetFogParam( float fSampleS, float fSampleT );
	void	_updateTargetCharacterParam( float fSampleS, float fSampleT );
	void	_updateTargetPointLight( float fSampleS, float fSampleT );
	void	_updateTargetPaticleParams( float fSampleS, float fSampleT );
	void	_updateTargetFinalEffectParams( float fSampleS, float fSampleT );

	void	_computeCurrentCharacterParam( float fLeftTime, float fDeltaTime );
	void	_computeCurrentPaticleParams( float fLeftTime, float fDeltaTime );
	void	_computeCurrentFinalEffectParams( float fLeftTime, float fDeltaTime );

	typedef mem::vector<SE_SceneWorldTile*>		SE_SceneWorldTileVector;
	typedef SE_SceneWorldTileVector::iterator	SE_SceneWorldTileVectorIter;

    int WORLD_TILE_COUNTX_PER_MAP;
    int WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;
    int WORLD_CHUNK_COUNTX_PER_CLUSTER;
    float WORLD_GRID_WIDTH;
    float WORLD_MAP_WIDTH;
    float WORLD_TILE_WIDTH;

	SE_SceneWorldTileVector		 m_kSceneWorldTileVector;

	SE_ChunkIndexPool			 m_kVisibleChunkPool;


	NiDirectionalLightPtr		 m_spOutDoorLight;
	NiDirectionalLightPtr        m_spSceneFxLight;
	NiPointLightPtr				 m_spPointLight;
	NiPointLightPtr              m_spSceneFxPointLight;

	NiColor						 m_kTargetBackLightColor;
	NiColor						 m_kTargetAmbientColor;
	NiColor						 m_kTargetDiffuseColor;
	NiColor                      m_kTargetShadowColor;
	NiColor						 m_kCurrentBackLightColor;
	NiColor						 m_kCurrentAmbientColor;
	NiColor						 m_kCurrentDiffuseColor;
	
	NiColor                      m_kCurrentShadowColor;
	float						 m_fTargetLightMultiplier;
	float                        m_fCurrentLightMultiplier;
	//Fog需要特殊处理
	NiColorA					m_kDepthFogParam;
	NiColorA					m_kHeightFogParam;
	NiColorA					m_kTargetDepthFogParam;
	NiColorA					m_kTargetHeightFogParam;
	float						m_fTargetDepthFogDensity;
	float						m_fTargetHeightFogDensity;
	float						m_fCurrentDepthFogDensity;
	float						m_fCurrentHeightFogDensity;
	NiColor						m_kTargetFogColor;
	NiColor						m_kCurrentFogColor;
	bool                        m_bDepthFogFadeIn;
	bool                        m_bDepthFogFadeOut;
	bool                        m_bHeightFogFadeIn;
	bool                        m_bHeightFogFadeOut;

	bool                        m_bWeatherEnable;
	bool                        m_bBloomEnable;
	StringType                  m_kCurrentWeatherTex;

	//Paticle
	float	m_fTargetParticleDensity;
	float	m_fTargetParticleSpeed;
	float	m_fTargetParticleWidth;
	float	m_fTargetParticleLength;
	float	m_fTargetParticleAlpha;
	float	m_fTargetParticleWind;

	float	m_fCurrentParticleDensity;
	float	m_fCurrentParticleSpeed;
	float	m_fCurrentParticleWidth;
	float	m_fCurrentParticleLength;
	float	m_fCurrentParticleAlpha;
	float	m_fCurrentParticleWind;

	//Final Effect
	float	m_fTargetSharpenIntensity;
	float	m_fTargetSoftLightScope;
	float	m_fTargetGammaIntensity;
	float	m_fTargetGammaExponent;

	float	m_fCurrentSharpenIntensity;
	float	m_fCurrentSoftLightScope;
	float	m_fCurrentGammaIntensity;
	float	m_fCurrentGammaExponent;

	//character
	float		m_fCharacterAmbientPower;
	float		m_fCharacterSunPower;
	float		m_fCharacterMatEmmisive;
	float		m_fCharacterSelfIllumination;
	float		m_fCharacterSpecularPower;
	float		m_fCharacterSpecularMultiplier;
	NiColor		m_kCharacterDiffuseLightColor;
	NiColor		m_kCharacterAmbientColor;
	NiPoint3	m_kCharacterLightWorldDir;
	NiColor		m_kCharacterRimLightColor;
	float		m_fCharacterRimLightPower;
	float		m_fCharacterIllumBias;
	//Current
	float		m_fCurrentCharAmbientPower;
	float		m_fCurrentCharSunPower;
	float		m_fCurrentCharMatEmmisive;
	float		m_fCurrentCharSelfIllumination;
	float		m_fCurrentCharSpecularPower;
	float		m_fCurrentCharSpecularMultiplier;
	NiColor		m_kCurrentCharDiffuseLightColor;
	NiColor		m_kCurrentCharAmbientColor;
	NiPoint3	m_kCurrentCharLightWorldDir;
	NiColor		m_kCurrentCharRimLightColor;
	float		m_fCurrentCharRimLightPower;
	float		m_fCurrentCharIllumBias;

	NiColor		m_kPointLightColor;
	NiPoint3	m_kPointLightPos;
	NiPoint3	m_kPointLightParam;
	float		m_fPointLightEnable;
	SE_CompactHeightField<float>*             m_pkHeightField;
	SE_CompactHeightField<short>*      m_pkWalkableField;
	SE_ActorShadowField*        m_pkShadowField;

	SE_WorldChunkList           m_kLoadedChunks;
	//当前活跃的Chunk（以玩家为中心的3*3 Chunk区域）
	SE_WorldChunkList           m_kActiveChunks;

    float                       m_fCurrentWaterHeight;
	BS_ClientWaterMeshManager*  m_pkWaterMeshManager;
	MTF_EffectTile*  m_pkMapEffect;

	bool                        m_bInitParams;

	FileSystem::WorldMapFile*   m_pkWorldMapFile;
	float						m_fTransTimeLeft;
	float						m_fFogDepthTransTimeLeft;
	float						m_fFogHeightTransTimeLeft;
	bool						m_bCompute;

    // 场景闪电
    float m_fCurrentRegionSceneLightningInterval;
    float m_fCurrentRegionSceneLightningPower;
    bool m_bIsCurrentInSceneLightning;
    float m_fCurrentSceneLightningPower;

	Render::RG_LightProbeManager* m_pkProbeMgr;

	// 光照域
	Terrain::TS_LightField*		 m_pkTerrainLightField;

	// 全局配置
	FileSystem::ClientMapGlobalInfoManager*  m_pkMapGlobalInfo;
};


#endif