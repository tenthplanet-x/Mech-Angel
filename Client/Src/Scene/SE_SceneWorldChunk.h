#ifndef _SE_SCENE_WORLD_CHUNK_H
#define _SE_SCENE_WORLD_CHUNK_H

#include "GD_ClientSceneModel.h"
#include "MG_MessageHandlerInterface.h"
#include "TS_BoundingVolume.h"
#include "SE_Terraincallback.h"

class SE_SceneWorldMap;
class SE_SceneWorldTile;
class SE_SceneWorldChunkCluster;
class SE_SceneTerrainLocalInfo;
class SE_ActorShadowField;
namespace FileSystem { class MTF_ClientTile; }

class SE_SceneWorldChunk : public MG_MessageHandlerInterface,
						   public Memory::MM_BaseObject
{
public:
	SE_SceneWorldChunk(int iIndexX,int iIndexY,
					   SE_SceneWorldMap* pkWorldMapParent,
					   SE_SceneWorldTile* pkWorldTileParent ,
					   SE_SceneWorldChunkCluster* pkWorldChunkClusterParent);
	virtual ~SE_SceneWorldChunk();
	
	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	bool					LoadTerrain();
	bool                    PostLoadTerrain();
	bool                    PreUnLoadTerrain();
	bool					UnLoadTerrain();
    bool					IsTerrainLoaded() { return m_bLoadTerrain; }

	bool					LoadModels();
	bool                    PreUnLoadModels();
    bool					IsModelsLoaded() { return m_bLoadModel; }
	void                    RequestModels();

	bool					LoadGrass();
	bool                    PostLoadGrass();
	bool                    PreUnLoadGrass();
	bool					UnLoadGrass();
    bool					IsGrassLoaded() { return m_bLoadGrass; }
	unsigned int			PerformGrassInstanceRendering(NiInstanceTriShapePtr spInstanceTriShape,NiDX9Renderer* pkInstanceRenderer );
	
	bool					LoadWater();
	bool                    PostLoadWater();
	bool                    PreUnLoadWater();
	bool					UnLoadWater();
    bool					IsWaterLoaded() { return m_bLoadWater; }

    bool                    IsLoaded() { return IsGrassLoaded() && IsWaterLoaded() && IsTerrainLoaded(); }

	int                     GetDebugChunkID(){ return m_iChunkID; }

	
    NiNodePtr				GetNiChunkTerrainRootNode() { return m_spNiTerrainRootNode; }
    NiNodePtr				GetNiChunkWaterRootNode() { return m_spNiLoadedWater; }
    NiGeometryPtr           GetNiChunkGeometry() { return NiSmartPointerCast( NiGeometry , m_spChunkTerrainGeometry ); }

    float                   GetPlayerDistance() { return m_fPlayerDistance; }

	void GetModelVisibleArrary(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller , NiVisibleArray& rkVisibleArray, bool bSceneFx , bool bFirstInFrame );
    void GetWalkableModelVisibleArray(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void GetTransparentVisibleArrary(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller , NiVisibleArray& rkVisibleArray);
	void GetReflectVisibleArrary(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller , NiVisibleArray& rkVisibleArray);
	//���ص�ǰChunk������ˮ��߶���Сֵ
    float                   GetChunkWaterHeight() const { return m_fWaterHeight; }
	
    Utility::UT_AABB&		    GetAreaRect() { return m_kAreaRect; }
	Terrain::TS_BoundingVolume& GetBoundingVolume(){ return m_kWorldBound; }
	bool					    PickMap(NiPick* pkScenePick,const NiPoint3 &origin, const NiPoint3 &dir , const NiPoint3& kPlayerPos );

	void                        RegisterLocalNPC( long lSpawnID );
	bool                        UpdateCameraBlockModels( NiPick* pkPick , const Terrain::TS_BoundingVolume& rkPlayerAABB,const NiPoint3& vOrign,const NiPoint3& vDir);

	int                      GetChunkTileIndexX(){ return m_iChunkTileIndexX; }
	int                      GetChunkTileIndexY(){ return m_iChunkTileIndexY; }

	void                     ResetReflectTexture();

    bool                     IsLoading() { return m_bLoading; }
	void                     SetLoadingFlag( bool bLoading ){ m_bLoading = bLoading; }

protected:
	SE_SceneWorldChunk();

	void	 _event_TimerTick(MG_MessageBase& rkMessage);
	void	 _event_LogicPlayerMove(MG_MessageBase& rkMessage);
	
	void	 _loadTerrainGeometryData(FileSystem::MTF_ClientTile* pkWorldTileFile);
	void     _loadTerrainMaterial(FileSystem::MTF_ClientTile* pkWorldTileFile);

	void     _unloadTerrainGeometryData();
	void     _unloadTerrainMaterial();

	void     _sendModelLoadingMsg(FileSystem::MTF_ClientTile* pkWorldTileFile);
	
	void	 _loadGrassData(FileSystem::MTF_ClientTile* pkWorldTileFile);
	void	 _unloadGrassData();

	void	 _loadWaterData(FileSystem::MTF_ClientTile* pkWorldTileFile);
	void	 _unloadWaterData();

	void	 _computeAreaRect();

	Utility::UT_AABB		m_kAreaRect;
	int						m_iChunkIndexX;
	int						m_iChunkIndexY;
	int						m_iChunkTileIndexX;
	int						m_iChunkTileIndexY;
	int						m_iChunkVertexCount;
	int                     m_iHeightFieldX;
	int                     m_iHeightFieldY;
	int                     m_iWalkableFieldX;
	int                     m_iWalkableFieldY;
	int                     m_iVisibleFieldIndexX;
	int                     m_iVisibleFieldIndexY;
	unsigned char*          m_pucVisibilityData;
	int                     m_iVisibilityDataWidth;
	//�����õ�
	long                    m_nLocalNPC[20];

	float*                  m_pfHeights;
	NiPoint3*               m_pkNormal;
	short*                  m_pfWalkableHeights;
	NiPoint3*               m_pkWalkableNormal;

	float                   m_fPlayerDistance;
	//����NPCϵͳʹ��
	mem::vector<long>       m_kLocalNPCList;
	mem::vector<long>       m_kLocalNPCCreateID;

	SE_SceneWorldMap*			m_pkWorldMapParent;
	SE_SceneWorldTile*			m_pkWorldTileParent;
	SE_SceneWorldChunkCluster*	m_pkWorldChunkClusterParent;


	NiNodePtr				m_spNiTerrainRootNode;
	NiTriStripsPtr			m_spChunkTerrainGeometry;
	NiTriStripsDataPtr		m_spChunkTerrainGeometryData;
	Data::GD_Material*		m_pkTerrainMaterial[4];
	NiSourceTexturePtr		m_spTerrainAlphaMap;

#ifdef _ENABLE_PHYSX
	NxActor*				m_pkPhyxActor;
#endif

	NiNodePtr				m_spNiModelRootNode;
	NiNodePtr               m_spNiPathModelRootNode;

	NiPoint3                           m_kChunkCenter;
	StringType                         m_strPreBlendTexName;
	StringType                         m_strPreBlendTexNameDecoration;

	SE_SceneModelVector                 m_kChunkModels;
	SE_SceneModelVector                 m_kWalkableModels;
    SE_SceneModelVector                 m_kTransparent;
	//��̬������Ҫ��ģ����Ϣ
	int                                 m_iChunkID;
	bool                                m_bLoading;

	typedef mem::vector<ModelInfo>      ChunkModelInfo;
	ChunkModelInfo                      m_kChunkModelInfo;
	//���ڸı���Cull�Ĳ��ԣ����ٴ�Cluster�л�ȡ�ڵ㣬����Ҫ��Chunk�м���һ�������ڵ�
	//��PostLoad��ʱ���Attach��ȥ�����ڵ��νڵ�Cull�ߵ�Cluster�������̣���������
	//�Ͳ�ͳһ�ˣ���ð�Cluster��һ��ȥ��
	NiNodePtr                           m_spNiLoadedWater;

	Terrain::TS_GrassCluster*			m_pkGrassCluster;
	
	NiNodePtr							m_spNiWaterRootNode;
	Terrain::WaterMeshMap				m_kWaterMeshMap;

	bool					m_bLoadTerrain;
	bool					m_bLoadModel;
	bool					m_bLoadGrass;
	bool					m_bLoadWater;
	bool                    m_bLoadingGrass;
	//LOD����ʹ��
	NiPoint3                        m_kCurrentPlayerPos;

#ifdef _ENABLE_PHYSX 
 	Utility::UT_PhysXMemStream*     m_pkPhysXStream;
#endif
	SE_CompactHeightField<float>*   m_pkHeightField;
	SE_ActorShadowField*            m_pkVisibility;
	SE_CompactHeightField<short>*   m_pkWalkableField;
	bool                            m_bShowGrass;
	float                           m_fWaterHeight;

	Terrain::TS_BoundingVolume      m_kWorldBound;

    int m_vPLID[MAX_DYNAMIC_POINT_LIGHT];
};

#endif