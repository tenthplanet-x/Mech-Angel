#include "MC_MessageChannel.h"
#include "MC_IMessageSender.h"
#include "NW_Helper.h"

#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerSector.h"
#include "LC_ServerNPC.h"

#include "UT_GameServerProfile.h"

using namespace GameLogic;

MC_MessageChannel::MC_MessageChannel(float interval)
	: m_interval(interval)
{
	m_lastTime = 0.0;
}

MC_MessageChannel::~MC_MessageChannel(void)
{
}

void MC_MessageChannel::AddMessageSender(MC_IUniCastMessageSender* sender)
{
	m_unicastMessages.push_back(sender);
}

void MC_MessageChannel::AddMessageSender(MC_IGridPacketPlayerDataMessageSender* sender)
{
	//Protocol::MG_BODY* message = sender->GetMessageBody();
	//if (message->GetMessageID() == MGPT_SYNC_PLAYERS_LOCATION_IN_FOV)
	//{
	//	printf( "got it.\n" );
	//}
	m_gridPlayerMessages.push_back(sender);
}

void MC_MessageChannel::AddMessageSender(MC_IAllPacketPlayerDataMessageSender* sender)
{
	m_allPacketPlayerMessages.push_back(sender);
}

void MC_MessageChannel::AddMessageSender(MC_IAllPacketNPCDataMessageSender* sender)
{
	m_allPacketNPCMessages.push_back(sender);
}

void MC_MessageChannel::Update(float fCurrentTime)
{
	//if (fCurrentTime - m_lastTime > m_interval)
	{
		m_lastTime = fCurrentTime;

		if (!NeedPacketPlayerMessage() && !NeedPacketNPCMessage())
		{
			return;
		}

		LC_ServerWorldManager* worldManager = SERVER_GET_WORLD_MANAGER();
		ActiveSectorClusterVector& kActiveSectorClusVec = worldManager->SetUpActiveSector();

		size_t iActivePlayerSectorCount = worldManager->ActiveSectorClusSize();
		for (size_t mapID = 0; mapID < iActivePlayerSectorCount; ++mapID)
		{
			{
				//UT_SERVER_PROFILE(kProfile, "MC_MessageChannel CreateActor", 0.001f);
				m_playerVector.CreateActor(kActiveSectorClusVec[mapID]);
			}
			{
				//UT_SERVER_PROFILE(kProfile, "MC_MessageChannel UnicastMessage", 0.001f);
				UnicastMessage(m_playerVector);
			}
			{
				//UT_SERVER_PROFILE(kProfile, "MC_MessageChannel BroadCastGridPlayerMessage", 0.001f);
				BroadCastGridPlayerMessage(m_playerVector);
			}
			{
				//UT_SERVER_PROFILE(kProfile, "MC_MessageChannel BroadCastAllPlayerMessage", 0.001f);
				BroadCastAllPlayerMessage(m_playerVector);
			}
			{
				//UT_SERVER_PROFILE(kProfile, "MC_MessageChannel BroadCastAllNPCMessage", 0.001f);
				BroadCastAllNPCMessage(m_playerVector);
			}
		}
	}
}

bool MC_MessageChannel::NeedPacketPlayerMessage()
{
	return !m_unicastMessages.empty() || !m_gridPlayerMessages.empty() || !m_allPacketPlayerMessages.empty() || !m_allPacketNPCMessages.empty();
}

bool MC_MessageChannel::NeedPacketNPCMessage()
{
	return !m_unicastMessages.empty() || !m_allPacketNPCMessages.empty();
}
/////////////////////////////////////////////////////////////////////////////
struct UnicastOperator
{
	UnicastOperator(UniCastMessageSenderContainer& unicastMessages): m_unicastMessages(unicastMessages)
	{
	}

	void operator()(MC_PlayerFovVector& kPlayerVector, size_t iSectorIndex, LC_ServerPlayer* player)
	{
		for (UniCastMessageSenderContainer::iterator itMessage = m_unicastMessages.begin(); itMessage != m_unicastMessages.end(); ++itMessage)
		{
			MC_IUniCastMessageSender* unicastMessageSender = *itMessage;
			if (unicastMessageSender->IsDataDirty(player))
			{
				LC_ServerPlayer* pkCurrentPlayer = player->GetCurrentActivateBuddy();
				if(NULL == pkCurrentPlayer) 
				{
					return;
				}

				//只同步当前操作buddy的视野
				if(player->GetID() == pkCurrentPlayer->GetID())
				{
					unicastMessageSender->SendMessage(pkCurrentPlayer, kPlayerVector, iSectorIndex);
				}

				unicastMessageSender->ClearData(player);
			}
		}
	}

	UniCastMessageSenderContainer& m_unicastMessages;
};

void MC_MessageChannel::UnicastMessage(MC_PlayerFovVector& playerVector)
{
	if (m_unicastMessages.empty())
	{
		return;
	}
	UnicastOperator kUnicast(m_unicastMessages);
	size_t iSectorCount = m_playerVector.GetPlayerSectorCount();
	for (size_t sectorIndex = 0 ; sectorIndex < iSectorCount; ++sectorIndex)
	{
		playerVector.ForEachPlayerInSector(sectorIndex, kUnicast);
	}
}
/////////////////////////////////////////////////////////////////////////////
struct BroadcastGridPlayerPackOperator
{
	BroadcastGridPlayerPackOperator(GridPacketPlayerDataMessageSenderContainer& gridPlayerMessages): m_gridPlayerMessages(gridPlayerMessages)
	{
	}

	void operator()(MC_PlayerFovVector& kPlayerVector, size_t iSectorIndex, LC_ServerPlayer* player)
	{
		for (GridPacketPlayerDataMessageSenderContainer::iterator it = m_gridPlayerMessages.begin(); it != m_gridPlayerMessages.end(); ++it)
		{
			MC_IGridPacketPlayerDataMessageSender* gridPacketPlayerDataMessageSender = *it;
			if (gridPacketPlayerDataMessageSender->IsDataChange(player))
			{
				if (gridPacketPlayerDataMessageSender->GetSectorCount(iSectorIndex) < MAX_PLAYER_COUNT_IN_FOV)
				{
					gridPacketPlayerDataMessageSender->PacketSectorPlayerMessage(iSectorIndex, player);
				}
				gridPacketPlayerDataMessageSender->ClearDataChange(player);
			}
		}
	}

	GridPacketPlayerDataMessageSenderContainer& m_gridPlayerMessages;
};

void MC_MessageChannel::BroadCastGridPlayerMessage(MC_PlayerFovVector& playerVector)
{
	if (m_gridPlayerMessages.empty())
	{
		return;
	}

	BroadcastGridPlayerPackOperator kPackOP(m_gridPlayerMessages);
	size_t iSectorCount = m_playerVector.GetPlayerSectorCount();
	for (size_t sectorIndex = 0 ; sectorIndex < iSectorCount; ++sectorIndex)
	{
		for (GridPacketPlayerDataMessageSenderContainer::iterator it = m_gridPlayerMessages.begin(); it != m_gridPlayerMessages.end(); ++it)
		{
			(*it)->ClearPacketSectorMessage(sectorIndex);
		}
		playerVector.ForEachPlayerInSector(sectorIndex, kPackOP);
	}

	for (size_t sectorIndex = 0 ; sectorIndex < iSectorCount; ++sectorIndex)
	{
		for (GridPacketPlayerDataMessageSenderContainer::iterator it = m_gridPlayerMessages.begin(); it != m_gridPlayerMessages.end(); ++it)
		{
			MC_IGridPacketPlayerDataMessageSender* gridPacketPlayerDataMessageSender = *it;
			mem::vector<size_t>& neighborSector = playerVector.GetNeighborSector(sectorIndex);
			gridPacketPlayerDataMessageSender->BeginBroadCastMessage();
			for (size_t i = 0; i < neighborSector.size(); ++i)
			{
				if (neighborSector[i] >= iSectorCount)
				{
					continue;
				}
				if (!gridPacketPlayerDataMessageSender->MergeSectorBroadData(playerVector, neighborSector[i]))
				{
					break;
				}
			}
			if (gridPacketPlayerDataMessageSender->CanBroadCastMessage())
			{
				Protocol::MG_BODY* message = gridPacketPlayerDataMessageSender->GetMessageBody();
				MulticastMsgToGateway(playerVector.GetPlayerInSectorVector(sectorIndex), message->GetMessageID(), message);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
struct BroadcastAllPlayerPackOperator
{
	BroadcastAllPlayerPackOperator(MC_IAllPacketPlayerDataMessageSender* allPacketPlayerDataMessageSender): m_allPacketPlayerDataMessageSender(allPacketPlayerDataMessageSender)
	{
	}

	void operator()(MC_PlayerFovVector& kPlayerVector, size_t iSectorIndex, LC_ServerPlayer* player)
	{
		if (m_allPacketPlayerDataMessageSender->IsDataChange(player))
		{
			m_allPacketPlayerDataMessageSender->PacketPlayerMessage(player);
		}
	}

	MC_IAllPacketPlayerDataMessageSender* m_allPacketPlayerDataMessageSender;
};

struct BroadcastAllPlayerClearOperator
{
	BroadcastAllPlayerClearOperator(MC_IAllPacketPlayerDataMessageSender* allPacketPlayerDataMessageSender): m_allPacketPlayerDataMessageSender(allPacketPlayerDataMessageSender)
	{
	}

	void operator()(MC_PlayerFovVector& kPlayerVector, size_t iSectorIndex, LC_ServerPlayer* player)
	{
		m_allPacketPlayerDataMessageSender->ClearDataChange(player);
	}

	MC_IAllPacketPlayerDataMessageSender* m_allPacketPlayerDataMessageSender;
};

void MC_MessageChannel::BroadCastAllPlayerMessage(MC_PlayerFovVector& playerVector)
{
	if (m_allPacketPlayerMessages.empty())
	{
		return;
	}

	size_t iSectorCount = m_playerVector.GetPlayerSectorCount();
	for (size_t sectorIndex = 0 ; sectorIndex < iSectorCount; ++sectorIndex)
	{
		for (AllPacketPlayerDataMessageSenderContainer::iterator it = m_allPacketPlayerMessages.begin(); it != m_allPacketPlayerMessages.end(); ++it)
		{
			MC_IAllPacketPlayerDataMessageSender* allPacketPlayerDataMessageSender = *it;
			allPacketPlayerDataMessageSender->BeginBroadCastMessage();

			BroadcastAllPlayerPackOperator rtmpop(allPacketPlayerDataMessageSender);
			playerVector.ForEachPlayerInSectorFov(sectorIndex, rtmpop);

			if (allPacketPlayerDataMessageSender->CanBroadCastMessage())
			{
				Protocol::MG_BODY* message = allPacketPlayerDataMessageSender->GetMessageBody();
				MulticastMsgToGateway(playerVector.GetPlayerInSectorVector(sectorIndex), message->GetMessageID(), message);
			}
		}
	}

	for (size_t sectorIndex = 0 ; sectorIndex < iSectorCount; ++sectorIndex)
	{
		for (AllPacketPlayerDataMessageSenderContainer::iterator it = m_allPacketPlayerMessages.begin(); it != m_allPacketPlayerMessages.end(); ++it)
		{
			MC_IAllPacketPlayerDataMessageSender* allPacketPlayerDataMessageSender = *it;
			BroadcastAllPlayerClearOperator rtmpop(allPacketPlayerDataMessageSender);
			playerVector.ForEachPlayerInSector(sectorIndex, rtmpop);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
struct BroadcastAllNPCPackOperator
{
	BroadcastAllNPCPackOperator(MC_IAllPacketNPCDataMessageSender* allPacketNPCDataMessageSender): m_allPacketNPCDataMessageSender(allPacketNPCDataMessageSender)
	{

	}

	void operator()(MC_PlayerFovVector& kPlayerVector, size_t iSectorIndex, LC_ServerNPC* npc)
	{
		if (m_allPacketNPCDataMessageSender->IsDataChange(npc))
		{
			m_allPacketNPCDataMessageSender->PacketNPCMessage(npc);
			// NPC移动优化
			m_allPacketNPCDataMessageSender->ClearDataChange(npc);
		}
	}

	MC_IAllPacketNPCDataMessageSender* m_allPacketNPCDataMessageSender;
};

struct BroadcastAllNPCClearOperator
{
	BroadcastAllNPCClearOperator(MC_IAllPacketNPCDataMessageSender* allPacketNPCDataMessageSender): m_allPacketNPCDataMessageSender(allPacketNPCDataMessageSender)
	{
	}

	void operator()(MC_PlayerFovVector& kPlayerVector, size_t iSectorIndex, LC_ServerNPC* npc)
	{
		m_allPacketNPCDataMessageSender->ClearDataChange(npc);
	}

	MC_IAllPacketNPCDataMessageSender* m_allPacketNPCDataMessageSender;
};

void MC_MessageChannel::BroadCastAllNPCMessage(MC_PlayerFovVector& playerVector)
{
	static int nFlag[10] = {0};
	if (m_allPacketNPCMessages.empty())
	{
		return;
	}

	size_t iSectorCount = m_playerVector.GetPlayerSectorCount();
	for (size_t sectorIndex = 0 ; sectorIndex < iSectorCount; ++sectorIndex)
	{
		for (AllPacketNPCDataMessageSenderContainer::iterator it = m_allPacketNPCMessages.begin(); it != m_allPacketNPCMessages.end(); ++it)
		{
			MC_IAllPacketNPCDataMessageSender* allPacketNPCDataMessageSender = *it;
			allPacketNPCDataMessageSender->BeginBroadCastMessage();

			BroadcastAllNPCPackOperator rtmpop(allPacketNPCDataMessageSender);
			playerVector.ForEachNPCInSectorFov(sectorIndex, rtmpop);
			if (allPacketNPCDataMessageSender->CanBroadCastMessage())
			{
				Protocol::MG_BODY* message = allPacketNPCDataMessageSender->GetMessageBody();
				mem::vector<LC_ServerPlayer*>& vecData = playerVector.GetPlayerInSectorVector(sectorIndex);
				uint16_t nMsgId = message->GetMessageID();
				int nSize = vecData.size();
				if ( nSize <= 0 )
					continue;
				if ( !nFlag[0])
					MulticastMsgToGateway(vecData, message->GetMessageID(), message);
				//MulticastMsgToGateway(playerVector.GetPlayerInSectorVector(sectorIndex), message->GetMessageID(), message);
			}
		}
	}

	//边缘fov 应该不会发生datachange 有待确定
	for (size_t sectorIndex = 0 ; sectorIndex < iSectorCount; ++sectorIndex)
	{
		for (AllPacketNPCDataMessageSenderContainer::iterator it = m_allPacketNPCMessages.begin(); it != m_allPacketNPCMessages.end(); ++it)
		{
			MC_IAllPacketNPCDataMessageSender* allPacketNpcDataMessageSender = *it;
			BroadcastAllNPCClearOperator rtmpop(allPacketNpcDataMessageSender);
			playerVector.ForEachNPCInSector(sectorIndex, rtmpop);
		}
	}
}

