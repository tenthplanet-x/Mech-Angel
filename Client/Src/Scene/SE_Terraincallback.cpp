#include "ClientPCH.h"
#include "SE_Terraincallback.h"
#include "SE_SceneManager.h"
#include "SE_SceneWorldChunk.h"
#include "TS_WorldConfig.h"
#include "MTF_ClientTile.h"

using namespace Terrain;
using namespace Utility;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

const float SE_ActorShadowField::INV_255 = 1.0f / 255.0f;

SE_ActorShadowField::SE_ActorShadowField()
: m_nWorldChunckCountX(0)
, m_nChunkLightMapWidth(0)
, m_nWorldLightMapWidth(0)
, m_fWDW(0.0f)
, m_fCDW(0.0f)
, m_nChunkLightMapSize(0)
{
}

SE_ActorShadowField::~SE_ActorShadowField()
{
}

void SE_ActorShadowField::Init()
{
    TS_WorldConfig& rkWorldConfig = GfxGetWorldConfig();

    m_nWorldChunckCountX    = rkWorldConfig.CFG_WORLD_CHUNK_COUNTX_PER_MAP;
	m_nChunkLightMapWidth   = (unsigned int)(rkWorldConfig.CFG_SHADOWWRAP * 16.0f);
	m_nWorldLightMapWidth   = m_nWorldChunckCountX * m_nChunkLightMapWidth;

    m_fWDW = ((float)m_nWorldLightMapWidth) * rkWorldConfig.CFG_WORLD_MAP_INVWIDTH;
    m_fCDW = ((float)m_nWorldChunckCountX) * rkWorldConfig.CFG_WORLD_MAP_INVWIDTH;

    m_kChunkLightMapsInWorld.resize(m_nWorldChunckCountX * m_nWorldChunckCountX);

    m_nChunkLightMapSize    = m_nChunkLightMapWidth * m_nChunkLightMapWidth * 4;
}

void SE_ActorShadowField::UnInit()
{
}

void SE_ActorShadowField::AddChunkLightMap(unsigned int nChunckIndexX, unsigned int nChunckIndexY, 
                                           unsigned char* pkChunkLightMap, 
                                           unsigned int nLightMapSize)
{
    if (nChunckIndexX >= m_nWorldChunckCountX || nChunckIndexY >= m_nWorldChunckCountX)
    {
        return;
    }
    if (nLightMapSize != m_nChunkLightMapWidth)
    {
        RemoveChunkLightMap(nChunckIndexX, nChunckIndexY);
        return;
    }

    ChunkLightMapData& rkLightMapData = m_kChunkLightMapsInWorld[nChunckIndexY * m_nWorldChunckCountX + nChunckIndexX];
    rkLightMapData.nChunkIndexX     = nChunckIndexX;
    rkLightMapData.nChunkIndexY     = nChunckIndexY;
    rkLightMapData.pkLightMap       = pkChunkLightMap;
    rkLightMapData.nChunkLightMapX  = nChunckIndexX * m_nChunkLightMapWidth;
    rkLightMapData.nChunkLightMapY  = nChunckIndexY * m_nChunkLightMapWidth;

	/*GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,
		"Add light map ptr 0x%x at [%d,%d]",
		pkChunkLightMap,
		nChunckIndexX , nChunckIndexY );*/
}

void SE_ActorShadowField::RemoveChunkLightMap(unsigned int nChunckIndexX, unsigned int nChunckIndexY)
{
    if (nChunckIndexX >= m_nWorldChunckCountX || nChunckIndexY >= m_nWorldChunckCountX)
    {
        return;
    }
    ChunkLightMapData& rkLightMapData = m_kChunkLightMapsInWorld[nChunckIndexY * m_nWorldChunckCountX + nChunckIndexX];
	/*GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,
		"Remove light map ptr 0x%x at [%d,%d]",
		rkLightMapData.pkLightMap,
		nChunckIndexX , nChunckIndexY );*/
    rkLightMapData.pkLightMap = NULL;
}
