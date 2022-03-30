#include "MC_PlayerFovVector.h"
#include "LC_ServerSector.h"

using namespace GameLogic;

MC_PlayerFovVector::MC_PlayerFovVector(void): m_playerSectorCount(0), m_npcSectorCount(0)
{
	m_sectorNeighborVector.resize(MAX_USER_IN_GAME_SERVER);
	for (size_t index = 0 ; index < MAX_USER_IN_GAME_SERVER; ++index)
	{
		m_sectorPlayerVector[index].reserve(MAX_USER_IN_GAME_SERVER);
	}
	//for (size_t index = 0 ; index < MAX_USER_IN_GAME_SERVER*25; ++index)
	for (size_t index = 0 ; index < MAX_USER_IN_GAME_SERVER; ++index)
	{
		m_sectorNPCVector[index].reserve(MAX_NPC_COUNT_IN_FOV);
	}
}

MC_PlayerFovVector::~MC_PlayerFovVector(void)
{
}

size_t MC_PlayerFovVector::GetPlayerInSectorCount(size_t sectorIndex)
{
	return m_sectorPlayerVector[sectorIndex].size();
}

LC_ServerPlayer* MC_PlayerFovVector::GetPlayerInSector(size_t sectorIndex,size_t playerIndex)
{
	return m_sectorPlayerVector[sectorIndex][playerIndex];
}

mem::vector<LC_ServerPlayer*>& MC_PlayerFovVector::GetPlayerInSectorVector(size_t sectorIndex)
{
	if(sectorIndex < 0 || sectorIndex >= MAX_USER_IN_GAME_SERVER)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"GetPlayerInSectorVector error");
	}
	return m_sectorPlayerVector[sectorIndex];
}

size_t MC_PlayerFovVector::GetPlayerFovInSectorCount(size_t sectorIndex)
{
	size_t playerCount = 0;
	mem::vector<size_t>	& neighborSector = m_sectorNeighborVector[sectorIndex];
	for (size_t neighborIndex = 0 ; neighborIndex < neighborSector.size(); ++neighborIndex)
	{
		playerCount += m_sectorPlayerVector[neighborSector[neighborIndex]].size();
	}
	return playerCount;
}

LC_ServerPlayer* MC_PlayerFovVector::GetPlayerFovInSector( size_t sectorIndex, size_t playerIndex)
{
	LC_ServerPlayer* player = NULL;
	size_t playerCount = 0;
	mem::vector<size_t>& neighborSector = m_sectorNeighborVector[sectorIndex];
	for (size_t neighborIndex = 0 ; neighborIndex < neighborSector.size(); ++neighborIndex)
	{
		size_t neighborPlayerCount = m_sectorPlayerVector[neighborSector[neighborIndex]].size();
		if (playerCount +  neighborPlayerCount <= playerIndex)
		{
			playerCount += neighborPlayerCount;
		}
		else
		{
			size_t see = neighborSector[neighborIndex];
			player = m_sectorPlayerVector[see][playerIndex - playerCount];
			break;
		}
	}
	return player;
}

mem::vector<size_t>	&	MC_PlayerFovVector::GetNeighborSector(size_t sectorIndex)
{
	return m_sectorNeighborVector[sectorIndex];
}

size_t MC_PlayerFovVector::GetNPCInSectorCount( size_t sectorIndex )
{
	return m_sectorNPCVector[sectorIndex].size();
}

LC_ServerNPC* MC_PlayerFovVector::GetNPCInSector( size_t sectorIndex,size_t npcIndex )
{
	return m_sectorNPCVector[sectorIndex][npcIndex];
}

size_t MC_PlayerFovVector::GetNPCFovInSectorCount(size_t sectorIndex)
{
	if(sectorIndex < 0 || sectorIndex >= m_sectorNeighborVector.size())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"GetNPCFovInSectorCount error");
		return 0;
	}
	
	size_t npcCount = 0;
	mem::vector<size_t>& neighborSector = m_sectorNeighborVector[sectorIndex];
	for (size_t neighborIndex = 0 ; neighborIndex <neighborSector.size(); ++neighborIndex)
	{
		npcCount += m_sectorNPCVector[neighborSector[neighborIndex]].size();
	}
	return npcCount;

}

LC_ServerNPC* MC_PlayerFovVector::GetNPCFovInSector(size_t sectorIndex,size_t npcIndex)
{
	LC_ServerNPC* npc = NULL;
	size_t npcCount = 0;
	mem::vector<size_t>& neighborSector = m_sectorNeighborVector[sectorIndex];
	for (size_t neighborIndex = 0 ; neighborIndex < neighborSector.size(); ++neighborIndex)
	{
		size_t neighborNpcCount = m_sectorNPCVector[neighborSector[neighborIndex]].size();
		if (npcCount +  neighborNpcCount <= npcIndex)
		{
			npcCount += neighborNpcCount;
		}
		else
		{
			size_t see = neighborSector[neighborIndex];
			npc = m_sectorNPCVector[see][npcIndex - npcCount];
			break;
		}
	}
	return npc;
}

MC_PlayerFovVector::NeighborSector::NeighborSector()
{
	m_playerNeighbors.reserve(25);
	m_NPCNeighbors.reserve(25);
}

void MC_PlayerFovVector::CreateActor(ActiveSectorCluster& kAvtiveSectorClus)
{
	LC_ServerSectorVector& kSectors = kAvtiveSectorClus.kSectors;
	mem::map<SectorIndex, size_t>& kReverseIndices = kAvtiveSectorClus.kReverseIndices;

	m_playerSectorCount = kAvtiveSectorClus.activeCnt;
	//m_playerSectorCount = 1;
	m_npcSectorCount = kSectors.size();


	LC_PlayerIDPtrMapIter playerIDPtrIter;
	LC_PlayerIDPtrMapIter endPlayerIDPtrIter;

	LC_NPCIDPtrMapIter npcIDPtrIter;
	LC_NPCIDPtrMapIter endNpcIDPtrIter;
	int nSectorCount = kSectors.size();
	if ( nSectorCount > MAX_USER_IN_GAME_SERVER )
		nSectorCount = MAX_USER_IN_GAME_SERVER;
	for (size_t sectorIndex = 0 ; sectorIndex < nSectorCount; ++sectorIndex)
	{
		LC_ServerSector* pkSector = kSectors[sectorIndex];
		if (NULL == pkSector)
		{
			continue;
		}

		m_sectorPlayerVector[sectorIndex].clear();
		m_sectorNPCVector[sectorIndex].clear();
		m_sectorNeighborVector[sectorIndex].clear();

		if (sectorIndex < m_playerSectorCount)
		{
			//m_sectorPlayerVector[sectorIndex].clear();
			//m_sectorNPCVector[sectorIndex].clear();
			//m_sectorNeighborVector[sectorIndex].clear();

			LC_PlayerIDPtrMap&	playerID2Ptr = pkSector->GetPlayerIDPtrMap();
			endPlayerIDPtrIter = playerID2Ptr.end();
			for (playerIDPtrIter = playerID2Ptr.begin(); playerIDPtrIter!=endPlayerIDPtrIter; ++playerIDPtrIter )
			{
				LC_ServerPlayer* player = (LC_ServerPlayer*)(*playerIDPtrIter).second; 
				m_sectorPlayerVector[sectorIndex].push_back(player);
			}
			SectorIndexVector& sectorSet = pkSector->GetSectorNeighborIndexVector();
			for (SectorIndexVectorIter it = sectorSet.begin(); it != sectorSet.end(); ++it)
			{
				mem::map<SectorIndex, size_t>::iterator sectorIt = kReverseIndices.find(*it);
				if (sectorIt != kReverseIndices.end())
				{
					m_sectorNeighborVector[sectorIndex].push_back(sectorIt->second);
				}
			}
		}
		//if (sectorIndex < m_npcSectorCount)
		{
			//m_sectorPlayerVector[sectorIndex].clear();
			//m_sectorNPCVector[sectorIndex].clear();
			//m_sectorNeighborVector[sectorIndex].clear();

			LC_NPCIDPtrMap&	npcID2Ptr = pkSector->GetNPCIDPtrMap();
			endNpcIDPtrIter = npcID2Ptr.end();
			for (npcIDPtrIter = npcID2Ptr.begin(); npcIDPtrIter != endNpcIDPtrIter; ++npcIDPtrIter)
			{
				LC_ServerNPC* npc = (LC_ServerNPC*)(*npcIDPtrIter).second;
				m_sectorNPCVector[sectorIndex].push_back(npc);
			}
		}
	}
}
