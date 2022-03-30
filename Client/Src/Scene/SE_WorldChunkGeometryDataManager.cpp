#include "ClientPCH.h"

#include "SE_WorldChunkGeometryDataManager.h"

#ifdef _WOW_MESH
//
//////////////////////////////////////////////////////////////////////////

const int stripsize3 = 16*16 + 6*8;
template <class V>
void stripify3(V *in, V *out)
{
	for (int row=0; row<8; row++) 
	{ 
		V *thisrow = &in[indexMapBuf(0,row*2)];
		V *nextrow = &in[indexMapBuf(0,row*2+1)];
		V *overrow = &in[indexMapBuf(0,(row+1)*2)];

		for (int col=0; col<8; col++) 
		{
			*out++ = thisrow[col];
			*out++ = nextrow[col];
		}

		*out++ = thisrow[8];
		*out++ = overrow[8];
		*out++ = overrow[8];


		for (int col=7; col>=0; col--)
		{
			*out++ = nextrow[col];
			*out++ = overrow[col];
		}

		*out++ = thisrow[0];
		*out++ = overrow[0];
		*out++ = overrow[0];
	}
}

#define BEGIN_LOOP_VECTOR_DELTA \
	for(int y = 0; y < (Terrain::CHUNK_VERTEX_COUNT + 1); ++y)\
{\
	int nCol =  (y % 2) ? WMI_CHUNK_IN_VECTOR_NUM : WMI_CHUNK_OUT_VECTOR_NUM;\
	int nDelta =  (y % 2) ? 1 : 0;\
	for(int x = 0; x < nCol; ++x)\
{\


#define BEGIN_LOOP_VECTOR \
	for(int y = 0; y < (Terrain::CHUNK_VERTEX_COUNT + 1); ++y)\
{\
	int nCol =  (y % 2) ? WMI_CHUNK_IN_VECTOR_NUM : WMI_CHUNK_OUT_VECTOR_NUM;\
	for(int x = 0; x < nCol; ++x)\
{\


#define END_LOOP_VECTOR }}


static void FillVerticesData(NiPoint3* pVector,NodeSizeType nX,NodeSizeType nY)
{
	NiPoint3* pTemp;
	BEGIN_LOOP_VECTOR_DELTA
	pTemp = pVector + indexMapBuf(x,y);
	pTemp->x = (nX + x * 2 + nDelta) * GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
	pTemp->y = (nY + y) * GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
	pTemp->z = 0.0f;
	END_LOOP_VECTOR
}

static void FillNormalData(NiPoint3* pNormal)
{
	NiPoint3* pTemp;
	BEGIN_LOOP_VECTOR
	pTemp = pNormal + indexMapBuf(x,y);
	pTemp->x = 0.0f;
	pTemp->y = 0.0f;
	pTemp->z = 1.0f;
	END_LOOP_VECTOR
}

static void FillUVSets(NiPoint2* pUV)
{
	NiPoint2* pTemp;
	float fAlphaScale = 1.0f;
	pTemp = pUV; 
	//UV2 For Terrain Blend
	//UV1 For Texture UV
	const float alpha_half = 0.5f * 1.0f / 8.0f;
	float tx,ty;
	BEGIN_LOOP_VECTOR_DELTA
	pTemp = pUV + WMI_CHUNK_TOTAL_VECTOR + indexMapBuf(x,y);
	tx = 1.0f / 8.0f * x;
	ty = 1.0f / 8.0f * y * 0.5f;
	if (y%2) 
	{
		// offset by half
		tx += alpha_half;
	}
	//Alpha Map UV
	*pTemp = NiPoint2(tx * fAlphaScale, ty * fAlphaScale);
	//Base Texture UV
	pTemp = pUV + indexMapBuf(x,y);
	//change uv orientation for hillyao's request
	*pTemp = NiPoint2(ty * GfxGetWorldConfig().CFG_TEXTUREWRAP,tx * GfxGetWorldConfig().CFG_TEXTUREWRAP); 

	END_LOOP_VECTOR
}
static void FillIndexData(unsigned short* pIndex)
{
	unsigned short* defstrip = (unsigned short*)NiMalloc(sizeof(unsigned short) * (stripsize3));
	for (int i = 0; i < stripsize3; i++)
	{
		defstrip[i] = i; // note: this is ugly and should be handled in stripify
	}

	stripify3<unsigned short>(defstrip, pIndex);
	NiFree(defstrip);
}
#endif
//-----------------------------------------------------------------
SE_WorldChunkGeometryDataManager::SE_WorldChunkGeometryDataManager(SE_SceneWorldMap* pkWorldMap)
:	m_pusDefaultHighDetailIndex(NULL)
,	m_usDefaultHightDetailIndexCount(0)
{
	m_fChunkWidth = GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;
	m_fChunkHeight = m_fChunkWidth;
	m_fInvChunkWidth = GfxGetWorldConfig().CFG_WORLD_CHUNK_INVWIDTH;
	m_fInvChunkHeight = m_fInvChunkWidth;

	m_iChunkAddSize = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;
	m_iChunkDataSize = m_iChunkAddSize * m_iChunkAddSize; 

	m_iChunkVertexCountX = GfxGetWorldConfig().CFG_WORLD_CHUNK_VERTEXCOUNTX;
	m_iChunkVertexCountY = m_iChunkVertexCountX;
	m_iChunkVertexCount = m_iChunkVertexCountX * m_iChunkVertexCountY;

	m_iChunkQuadCountX	= GfxGetWorldConfig().CFG_WORLD_CHUNK_QUADCOUNTX;
	m_iChunkQuadCountY	= m_iChunkQuadCountX;
	m_iChunkQuadCount = m_iChunkQuadCountX * m_iChunkQuadCountY;

	m_fChunkStrideWidth = m_fChunkWidth /((float)( m_iChunkQuadCountX ));
	m_fChunkStrideHeight = m_fChunkHeight / ((float)( m_iChunkQuadCountY));

	_addFreeChunkGeometryData(m_iChunkDataSize);

	//m_pfTempLightMap = NiAlloc( float , g_settings.LOD_CPU_BLEND_TEX_SIZE * g_settings.LOD_CPU_BLEND_TEX_SIZE );
}
//-----------------------------------------------------------------
SE_WorldChunkGeometryDataManager::~SE_WorldChunkGeometryDataManager()
{
	m_kFreeChunkGeometryDataPool.RemoveAll();
	//NiFree( m_pfTempLightMap );
}
//-----------------------------------------------------------------
void SE_WorldChunkGeometryDataManager::RegisterFreeChunkGeometryData(NiTriStripsDataPtr pkFreeChunkGeoemtryData)
{
	m_kFreeChunkGeometryDataPool.AddTail(pkFreeChunkGeoemtryData);
}
//-----------------------------------------------------------------
NiTriStripsDataPtr SE_WorldChunkGeometryDataManager::RequestFreeChunkGeometryData()
{
	if(m_kFreeChunkGeometryDataPool.IsEmpty())
		_addFreeChunkGeometryData(m_iChunkAddSize);

	NiTriStripsDataPtr kResult = m_kFreeChunkGeometryDataPool.RemoveHead();
	return kResult;
}
//-----------------------------------------------------------------
void SE_WorldChunkGeometryDataManager::_addFreeChunkGeometryData(int iAddSize)
{
	for(int i = 0; i < iAddSize; i++)
	{
		NiTriStripsDataPtr pkFreeData = _createFreeChunkGeometryData();
		m_kFreeChunkGeometryDataPool.AddTail(pkFreeData);
	}
}
//-----------------------------------------------------------------
NiTriStripsDataPtr SE_WorldChunkGeometryDataManager::_createFreeChunkGeometryData()
{
#ifndef _WOW_MESH
	NiPoint3* pkVertices = NiNew NiPoint3[m_iChunkVertexCount];
	NiPoint3* pkNormals = NiNew NiPoint3[m_iChunkVertexCount];
	NiColorA* pkColors = NiNew NiColorA[m_iChunkVertexCount];

	//texcoord 0 for base texture, texcoord1 for alpha and shadow texture
	NiPoint2* pkTexcoord = NiNew NiPoint2[m_iChunkVertexCount * 2];
	NiPoint2* pkBaseTexCoord = pkTexcoord;
	NiPoint2* pkAlphaTexCoord = pkTexcoord + m_iChunkVertexCount;

	//set texcoord
	float fBaseTextureRepeat = GfxGetWorldConfig().CFG_TEXTUREWRAP;;
	float fAlphaTextureRepeat = 1.0f;
	float fBaseTextureStrideX = fBaseTextureRepeat / (float)m_iChunkQuadCountX;
	float fBaseTextureStrideZ = fBaseTextureRepeat / (float)m_iChunkQuadCountY;

	float fAlphaTextureStrideX = fAlphaTextureRepeat / (float)m_iChunkQuadCountX;
	float fAlphaTextureStrideZ = fAlphaTextureRepeat / (float)m_iChunkQuadCountY;


	for( int iVertexIndexZ = 0; iVertexIndexZ < m_iChunkVertexCountY; iVertexIndexZ++)
	{
		for( int iVertexIndexX = 0; iVertexIndexX < m_iChunkVertexCountX; iVertexIndexX++)
		{
			int iVertexIndex = ChunkVertexIndex(iVertexIndexX,iVertexIndexZ);
			
			float fX = iVertexIndexX * ( m_fChunkStrideWidth  );
			float fY = iVertexIndexZ * ( m_fChunkStrideHeight );
			pkVertices[iVertexIndex] = NiPoint3(fX,fY,0.0f);
			pkNormals[iVertexIndex] = NiPoint3(0.0f,0.0f,1.0f);
			pkColors[iVertexIndex] = NiColorA(iVertexIndexX * (1.0f / (float)m_iChunkVertexCountX),
				1.0f,
				iVertexIndexZ * (1.0f / (float)m_iChunkVertexCountY),
				1.0f);

			pkBaseTexCoord[iVertexIndex] = NiPoint2( iVertexIndexZ * fBaseTextureStrideX, iVertexIndexX * fBaseTextureStrideZ);
			pkAlphaTexCoord[iVertexIndex] = NiPoint2(iVertexIndexX * fAlphaTextureStrideX,iVertexIndexZ * fAlphaTextureStrideZ);
		}
	}


	unsigned short  usIndexCount = 0;
	unsigned short* pusIndices = _getHighDetailIndex(usIndexCount);
	unsigned short* pusIndexCount = NiAlloc(unsigned short,1);
	pusIndexCount[0] = usIndexCount;

	NiTriStripsData* pkResult = NiNew NiTriStripsData(m_iChunkVertexCount,
		pkVertices,
		pkNormals,
		pkColors,
		pkTexcoord,
		2,
		NiGeometryData::NBT_METHOD_NONE,
		usIndexCount - 2,
		1,
		pusIndexCount,
		pusIndices);

	pkResult->SetConsistency(NiGeometryData::MUTABLE);
	pkResult->SetKeepFlags(NiGeometryData::KEEP_XYZ | NiGeometryData::KEEP_NORM | NiTriStripsData::KEEP_INDICES | NiTriStripsData::KEEP_COLOR);

	return pkResult;
#else
	int nStripSize = 16*16 + 6*8;
	NiPoint3* pkVertices = NiNew NiPoint3[WMI_CHUNK_TOTAL_VECTOR];
	NiColorA* pkColor = NiNew NiColorA[WMI_CHUNK_TOTAL_VECTOR];
	NiPoint3* pkNormals = NiNew NiPoint3[WMI_CHUNK_TOTAL_VECTOR];
	NiPoint2* pkUVs = NiNew NiPoint2[WMI_CHUNK_TOTAL_VECTOR * 2]; //2 UVSet
	unsigned short* pusIndex = (unsigned short*)NiMalloc(sizeof(unsigned short) * nStripSize);//Index

	FillVerticesData( pkVertices , 0 , 0 );
	FillNormalData( pkNormals );
	FillUVSets( pkUVs );
	FillIndexData( pusIndex );

	unsigned short* pusIndexArray = (unsigned short*)NiMalloc(sizeof(unsigned short));
	pusIndexArray[0] = nStripSize;

	NiTriStripsData* pkResult = NiNew NiTriStripsData(WMI_CHUNK_TOTAL_VECTOR,
		pkVertices,
		pkNormals,
		pkColor,
		pkUVs,
		2,
		NiGeometryData::NBT_METHOD_NONE,
		nStripSize - 2,
		1,
		pusIndexArray,
		pusIndex);

	pkResult->SetConsistency(NiGeometryData::MUTABLE);
	pkResult->SetKeepFlags(NiGeometryData::KEEP_XYZ | NiGeometryData::KEEP_NORM | NiTriStripsData::KEEP_INDICES | NiTriStripsData::KEEP_COLOR);

	return pkResult;
#endif
}
//-----------------------------------------------------------------
void SE_WorldChunkGeometryDataManager::_buildDefaultHighDetailIndex()
{
	m_pusDefaultHighDetailIndex = NiAlloc(unsigned short,m_iChunkQuadCount * 6);
	m_usDefaultHightDetailIndexCount = 0;
	unsigned short* pusRowStrip = NiAlloc(unsigned short,m_iChunkQuadCountX * 2 + 2);

	for( int iQuadIndexY = 0; iQuadIndexY < m_iChunkQuadCountY; iQuadIndexY++)
	{
		int iIndices = 0;
		unsigned short usIndicesCount = 0;

		for(int iCol = 0; iCol < m_iChunkVertexCountX; iCol++)
		{
			pusRowStrip[iIndices++] = ChunkVertexIndex(iCol,iQuadIndexY );
			pusRowStrip[iIndices++] = ChunkVertexIndex(iCol,(iQuadIndexY + 1));
		}

		usIndicesCount = _appendTriStripLists(m_pusDefaultHighDetailIndex,
			m_usDefaultHightDetailIndexCount,
			pusRowStrip,
			iIndices);

		m_usDefaultHightDetailIndexCount = usIndicesCount;
	}

	NiFree(pusRowStrip);
}
//-----------------------------------------------------------------
unsigned short* SE_WorldChunkGeometryDataManager::_getHighDetailIndex(unsigned short& rusIndexCount)
{
	//return defaul strip if no holes
	//if(!m_pusDefaultHighDetailIndex)
		_buildDefaultHighDetailIndex();
	rusIndexCount = m_usDefaultHightDetailIndexCount;
	return m_pusDefaultHighDetailIndex;
}
//-----------------------------------------------------------------
unsigned short SE_WorldChunkGeometryDataManager::_appendTriStripLists(
	unsigned short* pusStrip0,
	unsigned short usStrip0Count, 
	unsigned short* pusStrip1,
	unsigned short usStrip1Count)
{

	if(usStrip0Count == 0 || !pusStrip0)
	{
		NiMemcpy(pusStrip0,pusStrip1,usStrip1Count * sizeof(unsigned short));
		return usStrip1Count;
	}
	if(usStrip1Count == 0 || !pusStrip1)
	{
		return usStrip0Count;
	}

	unsigned short usFinalStripCount = 0;

	if(pusStrip0[usStrip0Count - 2] == pusStrip1[0] && pusStrip0[usStrip0Count - 1] == pusStrip1[1])
	{
		usFinalStripCount = usStrip0Count + usStrip1Count - 2;

		NiMemcpy(pusStrip0 + usStrip0Count,pusStrip1 + 2,(usStrip1Count - 2) * sizeof(unsigned short));
	}
	else
	{
		usFinalStripCount = usStrip0Count + 2 + usStrip1Count;

		pusStrip0[usStrip0Count] = pusStrip0[usStrip0Count - 1];
		pusStrip0[usStrip0Count + 1] = pusStrip1[0];
		NiMemcpy(pusStrip0 + usStrip0Count + 2,pusStrip1,usStrip1Count * sizeof(unsigned short));
	}

	return usFinalStripCount;
}

int SE_WorldChunkGeometryDataManager::ChunkVertexIndex( int iVertexIndexX,int iVertexIndexY )
{
	return iVertexIndexX + iVertexIndexY * m_iChunkVertexCountX;
}
