#ifndef _LC_SERVER_SECTOR_H
#define _LC_SERVER_SECTOR_H

#include "LC_SectorBase.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlaceHolder.h"

class LC_ServerSector : public GameLogic::LC_SectorBase 
{
public:
	LC_ServerSector();
	virtual ~LC_ServerSector();

	virtual	void	Update(float fCurrentTime,float fDeltaTime);
	
	//检查周围9个格子，如果都不存在玩家，则取消激活
	void			CheckUpdateFlag();
	
	void			BroadCast(int32_t iMsgType, Protocol::MG_BODY* pkMsgBody = NULL);

	void			GetPlayerPtrList(LC_PlayerPtrList& playerPtrList);
	void			GetNPCPtrList(LC_NPCPtrList& npcPtrList);
	void			GetPlaceHolderPtrList(PlaceHolderListType& npcPtrList);

 	void			ClearPlayerFOV();

	void			AddNPCToFOV(object_id_type lNPCID);
	void			RemoveNPCFromFOV(object_id_type lNPCID);

	void			AddPlayerToFOV(object_id_type lPlayerID);
	void			RemovePlayerFromFOV(object_id_type lPlayerID);

	void			AddPlaceHolderToFOV(object_id_type lPlaceHolderID);
	void			RemovePlaceHolderFromFOV(object_id_type lPlaceHolderID);

	void			SyncPlayerEnterFOV(LC_ServerPlayer* pkServerPlayer);
	void			SyncPlayerLeaveFOV(LC_ServerPlayer* pkServerPlayer);

	void			SetFOVDirtyFlag(int iFlag,bool bDirty);
};



typedef mem::vector<LC_ServerSector*>					LC_ServerSectorVector;
typedef mem::set<LC_ServerSector*>						LC_ServerSectorSet;
typedef LC_ServerSectorVector::iterator					LC_ServerSectorVectorIter;
typedef mem::vector<LC_ServerSectorVector>				LC_ServerSectorVectorVector;
typedef LC_ServerSectorVectorVector::iterator			LC_ServerSectorVectorVectorIter;


struct ActiveSectorCluster
{
	ActiveSectorCluster()
		:activeCnt(0)
	{}

	inline void Clear()
	{
		activeCnt = 0;
		kSectors.clear();
		kReverseIndices.clear();
	}

	size_t activeCnt;
	LC_ServerSectorVector				kSectors; 
	mem::map<GameLogic::SectorIndex, size_t>		kReverseIndices;
};
typedef mem::vector<ActiveSectorCluster> ActiveSectorClusterVector;


#endif