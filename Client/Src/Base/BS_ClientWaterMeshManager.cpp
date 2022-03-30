#include "ClientPCH.h"

#include "BS_ClientWaterMeshManager.h"
#include "RG_RenderManager.h"

using namespace Terrain;
//----------------------------------------------------
BS_ClientWaterMeshManager::BS_ClientWaterMeshManager()
: TS_WaterMeshManager()
{
	m_kFreeWaterMeshList.clear();
}
//----------------------------------------------------
BS_ClientWaterMeshManager::~BS_ClientWaterMeshManager()
{
	T_ASSERT(m_kFreeWaterMeshList.size() == 0 && "mem leak");
}
//----------------------------------------------------
bool BS_ClientWaterMeshManager::Init()
{
	//TS_WaterMeshManager::Init();
	TS_WaterMeshBase::InitTable();
	m_bReflect = false;
	m_bRefract = false;
	
	int iFreeWaterMeshCount = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER * 
							  GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER  ; 

	_allocWaterMesh(iFreeWaterMeshCount);
	return true;
}
//----------------------------------------------------
bool BS_ClientWaterMeshManager::UnInit()
{
	//TS_WaterMeshManager::UnInit();
	TS_WaterMeshBase::ClearTable();

	WaterMeshListIter IterBe = m_kFreeWaterMeshList.begin();
	WaterMeshListIter IterEd = m_kFreeWaterMeshList.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		TS_WaterMeshBase* pkWaterMesh = (*IterBe);
		if(pkWaterMesh)
		{
			T_DELETE_D pkWaterMesh;
			pkWaterMesh = NULL;
		}
	}
	m_kFreeWaterMeshList.clear();
	
	return true;
}
//----------------------------------------------------
TS_WaterMeshBase* BS_ClientWaterMeshManager::RequestFreeWaterMesh()
{
	if(m_kFreeWaterMeshList.size() == 0)
	{
		int iAllocSize = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;
		_allocWaterMesh(iAllocSize);
	}

	T_ASSERT(m_kFreeWaterMeshList.size() > 0);

	TS_WaterMeshBase* pkWaterMesh = m_kFreeWaterMeshList.front();
	m_kFreeWaterMeshList.pop_front();

	return pkWaterMesh;
}
//----------------------------------------------------
void BS_ClientWaterMeshManager::RegisterFreeWaterMesh(Terrain::TS_WaterMeshBase* pkWaterMesh)
{
	m_kFreeWaterMeshList.push_back(pkWaterMesh);
}
//----------------------------------------------------
void BS_ClientWaterMeshManager::_allocWaterMesh(int iAllocCount)
{
	for(int iIndex = 0; iIndex < iAllocCount; iIndex++)
	{
		TS_WaterMeshBase* pkWaterMesh = T_NEW_D TS_WaterMeshBase(0,0);
		pkWaterMesh->InitWaterMeshGeometry( false );
		m_kFreeWaterMeshList.push_back(pkWaterMesh);
	}
}

NiTexture* BS_ClientWaterMeshManager::GetReflectMap()
{
	RG_RenderManager* pkRGMgr = RG_RenderManager::GetSingletonPtr();
	if ( pkRGMgr )
	{
		return pkRGMgr->GetWaterReflectRenderTex();
	}
	else
	{
		return NULL;
	}
}