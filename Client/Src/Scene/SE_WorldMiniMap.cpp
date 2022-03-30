#include "ClientPCH.h"
#include "MG_MessageBase.h"
#include "SE_WorldMiniMap.h"
//-----------------------------------------------------------
SE_WorldMiniMap::SE_WorldMiniMap(SE_SceneWorldMap* pkWorldMap)
: m_iPosX(32)
, m_iPosY(32)
, m_iChunkPixelSize(2)
, m_spScreenTexture(NULL)
, m_spSourceTexture(NULL)
, m_spPixelData(NULL)
, m_spString2D(NULL)
, m_spFont(NULL)
, m_pkSceneWorldMap(pkWorldMap)
{
	_init();
}

//-----------------------------------------------------------
SE_WorldMiniMap::~SE_WorldMiniMap()
{
	m_spScreenTexture = NULL;
	m_spSourceTexture = NULL;
	m_spPixelData = NULL;
	m_spString2D = NULL;
	m_spFont = NULL;
}
//-----------------------------------------------------------
void SE_WorldMiniMap::_init()
{	
	NiRendererPtr pkNiRenderer = NiRenderer::GetRenderer();
	if(!pkNiRenderer)
		return;

	m_iChunkCountX = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_MAP;
	m_iChunkCountY = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_MAP;

	m_iMapWidth =  m_iChunkCountX * m_iChunkPixelSize;
	m_iMapHeight = m_iChunkCountY * m_iChunkPixelSize;

	NiTexture::FormatPrefs kFormatePrefs;
	kFormatePrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;
	kFormatePrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	kFormatePrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
	m_spPixelData = NiNew NiPixelData(m_iMapWidth,
									  m_iMapHeight,
									  NiPixelFormat::RGBA32);


	m_kLoadedChunkColor = NiColorA(0.8f,0.8f,0.8f,0.8f);
	m_kUnLoaedChunkColor = NiColorA(0.5f,0.5f,0.5f,0.8f);
	m_kChunkBoarderColor = NiColorA(0.3f,0.3f,0.3f,1.0f);

	bool bDap = false;
	m_spSourceTexture = NiSourceTexture::Create(m_spPixelData,kFormatePrefs, &bDap);
	m_spSourceTexture->SetStatic(false);
	m_spScreenTexture = NiNew NiScreenTexture(m_spSourceTexture);
	m_spScreenTexture->AddNewScreenRect(m_iPosX,m_iPosY,m_iMapWidth,m_iMapHeight,0,0);


	//m_spFont =  NiFont::Create(pkNiRenderer,"Data\\resource\\Fonts\\help.nff");

	//NiColorA  kFontColor = NiColorA(1.0f,1.0f,1.0f,1.0f);
	//char pcString[256] = {0};
	//sprintf_s(pcString,"X: %f | Y: %f | Z: %f",0.0f,0.0f,0.0f);

	//m_spString2D = NiNew NiOldString2D(m_spFont, 
	//	NiFontString::COLORED, strlen(pcString), 
	//	pcString, kFontColor, m_iPosX + 20, m_iPosY + 20);
	
	_refresh();
}
//-----------------------------------------------------------
void SE_WorldMiniMap::Update()
{
	//_refresh();

	//for( int iChunkWorldIndexY = 0; iChunkWorldIndexY < m_pkSceneWorldMap->GetWorldChunkCountYPerMap(); iChunkWorldIndexY++)
	//{
	//	for( int iChunkWorldIndexX = 0; iChunkWorldIndexX < m_pkSceneWorldMap->GetWorldChunkCountYPerMap(); iChunkWorldIndexX++)
	//	{
	//		int iChunkID = m_pkSceneWorldMap->ComputeChunkID(iChunkWorldIndexX,iChunkWorldIndexY);
	//		if(m_pkSceneWorldMap->IsChunkVisible(iChunkID))
	//			_drawChunk(iChunkWorldIndexX,iChunkWorldIndexY,kVisibleChunkColor,true);
	//		else
	//			_drawChunk(iChunkWorldIndexX,iChunkWorldIndexY,kUnVisibleChunkColor,true);
	//	}
	//}
	//
	m_spScreenTexture->MarkAsChanged(NiScreenTexture::EVERYTHING_MASK);
}
//-----------------------------------------------------------
void SE_WorldMiniMap::_drawChunk(int iChunkIndexX,int iChunkIndexY, NiColorA& rkColor)
{
	NiColorA	 rkPixelColor;

	unsigned int uiChunkBasePixelX = iChunkIndexX * m_iChunkPixelSize;
	unsigned int uiChunkBasePixelY = iChunkIndexY * m_iChunkPixelSize;
	unsigned int uiChunkTopPixelX = uiChunkBasePixelX + m_iChunkPixelSize;
	unsigned int uiChunkTopPixelY = uiChunkBasePixelY + m_iChunkPixelSize;

	unsigned int* puiPixelContent = (unsigned int*)(m_spPixelData->GetPixels());
	for( unsigned int uiPixelIndexY = uiChunkBasePixelY; uiPixelIndexY < uiChunkTopPixelY; uiPixelIndexY++)
	{
		for( unsigned int uiPixelIndexX = uiChunkBasePixelX; uiPixelIndexX < uiChunkTopPixelX; uiPixelIndexX++)
		{
			unsigned int uiPixelIndex = uiPixelIndexX + uiPixelIndexY * m_iMapWidth;

			if((uiPixelIndexX == uiChunkBasePixelX || 
				uiPixelIndexY == uiChunkBasePixelY))
				rkPixelColor = m_kChunkBoarderColor;
			else
				rkPixelColor = rkColor;

			unsigned int uiA = (unsigned int)( rkPixelColor.a * 255);
			unsigned int uiR = (unsigned int)( rkPixelColor.r * 255);
			unsigned int uiG = (unsigned int)( rkPixelColor.g * 255);
			unsigned int uiB = (unsigned int)( rkPixelColor.b * 255);


			unsigned int uiDrawColor = uiA << 24 | 
				uiR << 16 |
				uiG << 8  |
				uiB;

			puiPixelContent[uiPixelIndex] = uiDrawColor;
		}
	}

	m_spPixelData->MarkAsChanged();
}
//-----------------------------------------------------------
void SE_WorldMiniMap::_refresh()
{
	for(int iChunkIndexX = 0; iChunkIndexX < m_iChunkCountX; iChunkIndexX++)
	{
		for(int iChunkIndexY = 0; iChunkIndexY < m_iChunkCountY; iChunkIndexY++)
		{
			_drawChunk(iChunkIndexX,iChunkIndexY,m_kUnLoaedChunkColor);
		}
	}

	m_spScreenTexture->MarkAsChanged(NiScreenTexture::EVERYTHING_MASK);
}
//-----------------------------------------------------------
NiScreenTexturePtr SE_WorldMiniMap::GetNiScreenTexture()
{
	return m_spScreenTexture;
}
//-----------------------------------------------------------
void SE_WorldMiniMap::Render( NiRendererPtr spNiRenderer )
{
	m_spScreenTexture->Draw(spNiRenderer);
#ifndef _SHIPPING
	//m_spString2D->Draw(spNiRenderer);
#endif
}
//-----------------------------------------------------------
bool SE_WorldMiniMap::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_DEBUG_TERRAIN_CHUNK_LOAD,	_event_debugTerrainChunkLoad)
	MESSAGE_HANDLE(MGT_DEBUG_TERRAIN_CHUNK_UNLOAD,	_event_debugTerrainChunkUnLoad)
	MESSAGE_HANDLE(MGT_LOGIC_PLAYER_MOVE,			_event_LogicPlayerMove)
	END_MESSAGE_HANDLE

	return true;
}
//-----------------------------------------------------------
void SE_WorldMiniMap::_event_debugTerrainChunkLoad(MG_MessageBase& rkMessage)
{
	int iChunkIndexX = rkMessage.m_lParam1;
	int iChunkIndexY = rkMessage.m_lParam2;

	_drawChunk(iChunkIndexX,iChunkIndexY,m_kLoadedChunkColor);
}
//-----------------------------------------------------------
void SE_WorldMiniMap::_event_debugTerrainChunkUnLoad(MG_MessageBase& rkMessage)
{
	int iChunkIndexX = rkMessage.m_lParam1;
	int iChunkIndexY = rkMessage.m_lParam2;

	_drawChunk(iChunkIndexX,iChunkIndexY,m_kUnLoaedChunkColor);
}
//-----------------------------------------------------------
void SE_WorldMiniMap::_event_LogicPlayerMove( MG_MessageBase &rkMessage )
{
		NiPoint3 kPlayerLocation;
		kPlayerLocation.x = rkMessage.m_fParam1;
		kPlayerLocation.y = rkMessage.m_fParam2;
		kPlayerLocation.z = rkMessage.m_fParam3;

	/*	m_spString2D->sprintf("X: %f | Y: %f | Z: %f",
			kPlayerLocation.x,
			kPlayerLocation.y,
			kPlayerLocation.z);*/
}