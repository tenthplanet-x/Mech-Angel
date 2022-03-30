#pragma once

#include "MM_MemoryAllocatorConfig.h"
#include "LC_Define.h"

class LC_ServerSector;
class LC_ServerPlayer;
class LC_ServerNPC;

struct ActiveSectorCluster;

class MC_PlayerFovVector
{
public:
	class NeighborSector
	{
	public:
		NeighborSector();
		mem::vector<size_t> m_playerNeighbors;
		mem::vector<size_t> m_NPCNeighbors;
	};


	MC_PlayerFovVector(void);
	~MC_PlayerFovVector(void);

	inline size_t		GetPlayerSectorCount() const{return m_playerSectorCount;}
	size_t				GetPlayerInSectorCount(size_t sectorIndex);
	LC_ServerPlayer*	GetPlayerInSector(size_t sectorIndex,size_t playerIndex);
	mem::vector<LC_ServerPlayer*>& GetPlayerInSectorVector(size_t sectorIndex);
	size_t				GetPlayerFovInSectorCount(size_t sectorIndex);
	LC_ServerPlayer*	GetPlayerFovInSector(size_t sectorIndex,size_t playerIndex);

	mem::vector<size_t>&		GetNeighborSector(size_t sectorIndex);

	inline size_t		GetNPCSectorCount(){return m_npcSectorCount;}
	size_t				GetNPCInSectorCount(size_t sectorIndex);
	LC_ServerNPC*		GetNPCInSector(size_t sectorIndex,size_t npcIndex);
	size_t				GetNPCFovInSectorCount(size_t sectorIndex);
	LC_ServerNPC*		GetNPCFovInSector(size_t sectorIndex,size_t npcIndex);

	template <typename T>
	void				ForEachPlayerInSector(size_t sectorIndex, T& op)
	{
		mem::vector<LC_ServerPlayer*>& kPlayers = m_sectorPlayerVector[sectorIndex];

		mem::vector<LC_ServerPlayer*>::iterator it = kPlayers.begin();
		for (; it!=kPlayers.end(); ++it)
		{
			LC_ServerPlayer* pPlayer = *it;
			//if (*it )
			if ( pPlayer && pPlayer->GetMapResID() )
			{
				op(*this, sectorIndex, *it);
			}
		}
	}

	template <typename T>
	void				ForEachNPCInSector(size_t sectorIndex, T& op)
	{
		mem::vector<LC_ServerNPC*>& kNPCs = m_sectorNPCVector[sectorIndex];

		mem::vector<LC_ServerNPC*>::iterator it = kNPCs.begin();
		for (; it!=kNPCs.end(); ++it)
		{
			if (*it)
			{
				op(*this, sectorIndex, *it);
			}
		}
	}

	template <typename T>
	void				ForEachPlayerInSectorFov(size_t sectorIndex, T& op)
	{
		mem::vector<size_t>& kNeighbours = m_sectorNeighborVector[sectorIndex];

		for (size_t i=0; i<kNeighbours.size(); ++i)
		{
			ForEachPlayerInSector(kNeighbours[i], op);
		}
	}

	template <typename T>
	void				ForEachNPCInSectorFov(size_t sectorIndex, T& op)
	{
		mem::vector<size_t>& kNeighbours = m_sectorNeighborVector[sectorIndex];

		for (size_t i=0; i<kNeighbours.size(); ++i)
		{
			ForEachNPCInSector(kNeighbours[i], op);
		}
	}

	void				CreateActor(ActiveSectorCluster& kAvtiveSectorClus);
private:
	mem::vector<LC_ServerPlayer*>		m_sectorPlayerVector[MAX_USER_IN_GAME_SERVER];
	//mem::vector<LC_ServerNPC*>			m_sectorNPCVector[MAX_USER_IN_GAME_SERVER*25];
	mem::vector<LC_ServerNPC*>			m_sectorNPCVector[MAX_USER_IN_GAME_SERVER];
	mem::vector<mem::vector<size_t> >	m_sectorNeighborVector;

	size_t							m_playerSectorCount;
	size_t							m_npcSectorCount;
};
