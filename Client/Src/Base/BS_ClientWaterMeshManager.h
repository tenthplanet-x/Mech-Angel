#ifndef _BS_CLIENT_WATER_MESH_MANAGER_H
#define _BS_CLIENT_WATER_MESH_MANAGER_H

class BS_ClientWaterMeshManager : public Terrain::TS_WaterMeshManager
{
public:
	BS_ClientWaterMeshManager();
	virtual ~BS_ClientWaterMeshManager();

	virtual bool	Init();
	virtual bool	UnInit();

	Terrain::TS_WaterMeshBase* RequestFreeWaterMesh();
	void					   RegisterFreeWaterMesh(Terrain::TS_WaterMeshBase* pkWaterMesh);
	virtual NiTexture*         GetReflectMap();

protected:
	void					   _allocWaterMesh(int iAllocCount);

	Terrain::WaterMeshList		m_kFreeWaterMeshList;
};

#endif