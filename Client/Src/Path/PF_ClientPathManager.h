#pragma once

#include "MG_MessageHandlerInterface.h"

class PF_ClientTerrainCallback : public iTerrainCallBack , public Memory::MM_BaseObject
{
public:
	PF_ClientTerrainCallback(tSigned32 meshCenterX , tSigned32 meshCenterY ){ m_lX = meshCenterX , m_lY = meshCenterY; }
	virtual float getHeightAt(tSigned32 x, tSigned32 y);
	virtual float getHeightAtWithPrecision(tSigned32 x, tSigned32 y, float precisionX, float precisionY);
private:
	tSigned32 m_lX;
	tSigned32 m_lY;
};

class PF_ClientPathManager : public Path::PF_PathManager,
							 public MG_MessageHandlerInterface
{
public:
	PF_ClientPathManager();
	virtual ~PF_ClientPathManager();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	static PF_ClientPathManager& GetSingleton();

	virtual bool          UnLoadGroundMesh();

	void FinishTileLoad( long lIndex );
	void FinishTileUnLoad( long lIndex );
protected:
	typedef mem::set<long> TileIDSet;
	virtual iTerrainCallBack* _create_height_callback( tSigned32 tileIndex );
	virtual void              _load_mesh_federation( const StringType& kPathFileBase, const StringType& rkDecoration);
	void                      _compute_active_path_tile( const NiPoint3& rkPlayerPos );
	void                      _log_set( TileIDSet& rkSet , const char* name );

	TileIDSet                 m_kActiveTile;
	TileIDSet                 m_kLoadingTile;
	TileIDSet                 m_kUnLoadingTile;
	bool                      m_bLoadAvenue;
};