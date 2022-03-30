#ifndef _RG_MINIMAP_H
#define _RG_MINIMAP_H


#include "MG_MessageHandlerInterface.h"
#include "SE_SceneWorldMap.h"

class SE_WorldMiniMap : public MG_MessageHandlerInterface,
						public Memory::MM_BaseObject
{
public:
	SE_WorldMiniMap(SE_SceneWorldMap* pkWorldMap);
	~SE_WorldMiniMap();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	void Update();
	void Render(NiRendererPtr spNiRenderer);
	NiScreenTexturePtr GetNiScreenTexture();

protected:
	void _init();
	void _refresh();
	void _drawChunk(int iChunkIndexX,int iChunkIndexY,NiColorA& rkColor);

	void _event_debugTerrainChunkLoad(MG_MessageBase& rkMessage);
	void _event_debugTerrainChunkUnLoad(MG_MessageBase& rkMessage);
	void _event_LogicPlayerMove( MG_MessageBase &rkMessage );

	int    m_iPosX;
	int    m_iPosY;
	int    m_iChunkPixelSize;
	int    m_iMapWidth;
	int    m_iMapHeight;
	int    m_iChunkCountX;
	int	   m_iChunkCountY;

	NiPixelDataPtr     m_spPixelData;
	NiScreenTexturePtr m_spScreenTexture;
	NiSourceTexturePtr m_spSourceTexture;
	NiOldString2DPtr   m_spString2D;
	NiFontPtr          m_spFont;
	
	NiColorA		   m_kLoadedChunkColor;
	NiColorA		   m_kUnLoaedChunkColor;
	NiColorA		   m_kChunkBoarderColor;

	SE_SceneWorldMap*  m_pkSceneWorldMap;
};



#endif