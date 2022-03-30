#ifndef _SE_WORLD_CHUNK_GEOMETRY_DATA_MANAGER
#define _SE_WORLD_CHUNK_GEOMETRY_DATA_MANAGER

#include "SE_SceneWorldMap.h"
#include "UT_LRUCacheTemplate.h"
//虽然高度图数据一样，两种方式还是会有微弱的差别，所以先禁用WOW格式的地形Mesh
//#define _WOW_MESH 

#ifdef _WOW_MESH

enum WORLD_METRICS_INFO
{
	WMI_CHUNK_OUT_VECTOR_NUM = 9,
	WMI_CHUNK_IN_VECTOR_NUM = 8,
	WMI_CHUNK_TOTAL_VECTOR = WMI_CHUNK_IN_VECTOR_NUM * WMI_CHUNK_IN_VECTOR_NUM + WMI_CHUNK_OUT_VECTOR_NUM * WMI_CHUNK_OUT_VECTOR_NUM,
};
static inline int indexMapBuf(int x, int y)
{
	return ( ( y + 1 ) / 2 ) * WMI_CHUNK_OUT_VECTOR_NUM + ( y / 2 ) * WMI_CHUNK_IN_VECTOR_NUM + x;
}
#endif

class SE_WorldChunkGeometryDataManager : public Memory::MM_BaseObject,
	public Utility::UT_Singleton<SE_WorldChunkGeometryDataManager>

{
public:
	SE_WorldChunkGeometryDataManager(SE_SceneWorldMap* pkWorldMap);
	~SE_WorldChunkGeometryDataManager();

	void RegisterFreeChunkGeometryData(NiTriStripsDataPtr pkFreeChunkGeoemtryData);
	NiTriStripsDataPtr RequestFreeChunkGeometryData();


	int				ChunkVertexIndex(int iVertexIndexX,int iVertexIndexY);

protected:

	int   m_iChunkDataSize;
	int   m_iChunkAddSize;
	float m_fChunkWidth;
	float m_fChunkHeight;
	float m_fInvChunkWidth;
	float m_fInvChunkHeight;
	float m_fChunkStrideWidth;
	float m_fChunkStrideHeight;
	int   m_iChunkVertexCountX;
	int   m_iChunkVertexCountY;
	int	  m_iChunkVertexCount;
	int   m_iChunkQuadCountX;
	int   m_iChunkQuadCountY;
	int   m_iChunkQuadCount;

	//float* m_pfTempLightMap;		注释此变量，无用，之前删除的代码SE_WorldChunkGeometryDataManager::GetBlendedImage中有引用，请翻阅svn历史

	void                 _addFreeChunkGeometryData(int iAddSize);
	NiTriStripsDataPtr   _createFreeChunkGeometryData();

	// every chunk has a NiTriStripsDataPtr
	typedef NiTPointerList<NiTriStripsDataPtr>         FreeChunkGeometryDataPool;
	FreeChunkGeometryDataPool   m_kFreeChunkGeometryDataPool;

	unsigned short* _getHighDetailIndex(unsigned short& rusIndexCount);
	void			_buildDefaultHighDetailIndex();

	// triangle strip, index buffer
	unsigned short*   m_pusDefaultHighDetailIndex;
	unsigned short    m_usDefaultHightDetailIndexCount;

	unsigned short _appendTriStripLists(unsigned short* pusStrip0,
		unsigned short usStrip0Count,
		unsigned short* pusStrip1,
		unsigned short usStrip1Count);

};
#endif