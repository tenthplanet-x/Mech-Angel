#pragma once

#include "MG_Stream.h"
#include "MG_SyncProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPC.h"

#include "msg.pb.h"

class MC_MessagePackageHelp
{
public:
	MC_MessagePackageHelp(void);
	~MC_MessagePackageHelp(void);

	static void Packet(Protocol::MG_SyncPlayerLeave& leaveMsg, object_id_type ID);
	static void Packet(Protocol::MG_SyncPlayersPropertyInFOV& playerPropertyMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayerControllersPropertyInFOV& playerPropertyMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayersExtPropertyInFOV& playerInfoMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayersSocietyInFOV& playerPropertyMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayersEquipInFOV& playerEquipMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayerHPMPChange& HPMPChangeMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayerPVEStateChange& PVEStateChangeMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayerStateChange& stateChangeMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayerLocation& locationChangeMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncPlayerAngle& locationChangeMsg, LC_ServerPlayer* player);
	static void Packet(Protocol::MG_SyncSubordinateInfoInFOV& subChangeMsg, LC_ServerPlayer* player);

	static void Packet(Protocol::MG_SyncNpcEnter& npcEnterMsg, LC_ServerNPC* npc);
	static void Packet(Protocol::MG_SyncNpcLeave& npcLeaveMsg, object_id_type ID);
	static void Packet(Protocol::MG_SyncNPCsMoveInFOV& npcMoveMsg, LC_ServerNPC* npc);
	static void Packet(Protocol::MG_SyncNPCsStateInFOV& npcStateMsg, LC_ServerNPC* npc);
	static void Packet(Protocol::MG_SyncNPCsAngleInFOV& npcAngleMsg, LC_ServerNPC* npc);
	static void Packet(Protocol::MG_SyncNPCsInteractionValueInFOV& npcInteractionValueMsg, LC_ServerNPC* npc);

	static void Packet(msg_define::player_infos_in_fov& playerInfoMsg, LC_ServerPlayer* player, bool bSyncAll = false);
	static void Packet(msg_define::player_propertys_in_fov& playerInfoMsg, LC_ServerPlayer* player, bool bSyncAll = false);
	static void Packet(msg_define::skill_states_in_fov& playerInfoMsg, LC_ServerPlayer* player, bool bSyncAll = false);

	static void Packet(msg_define::npc_propertys_in_fov& npcPropertyMsg, LC_ServerNPC* npc, bool bSyncAll = false);
	static void Packet(msg_define::skill_states_in_fov& npcSkillStateMsg, LC_ServerNPC* player, bool bSyncAll = false);

	template <typename PBT, typename OT, int msg_type>
	static inline void Packet(Protocol::MG_SCOPED_PB<PBT, msg_type>& msg, OT* target, bool bSyncAll=false)
	{
		Packet(*msg.m_value, target, bSyncAll);
	}

	template <typename PBT, int msg_type>
	static inline void Copy(Protocol::MG_SCOPED_PB<PBT, msg_type>& dst, const Protocol::MG_SCOPED_PB<PBT, msg_type>& src, int32_t limit)
	{
		PBT& dst_val = *dst.m_value;
		const PBT& src_val = *src.m_value;
		for (int i=0; i<src_val.entries_size()&&dst_val.entries_size()<limit; ++i)
		{
			dst_val.add_entries()->CopyFrom(src_val.entries(i));
		}
	}

	template <typename MGT>
	static inline void Copy(MGT& dst, const MGT& src, int32_t limit)
	{
		for (uint8_t i=0; i<src.m_usEntryCount&&dst.m_usEntryCount<limit; ++i)
		{
			dst.m_rEntries[dst.m_usEntryCount++] = src.m_rEntries[i];
		}
	}

	template <typename PBT, int msg_type>
	static inline void Begin(Protocol::MG_SCOPED_PB<PBT, msg_type>& rkPBMSG)
	{
		rkPBMSG.Clear();
	}
	template <typename PBT, int msg_type>
	static inline uint8_t EntryCount(const Protocol::MG_SCOPED_PB<PBT, msg_type>& rkPBMSG)
	{
		return (uint8_t)rkPBMSG.m_value->entries_size();
	}
	template <typename MGT>
	static inline void Begin(MGT& rkMGMSG)
	{
		rkMGMSG.m_usEntryCount = 0;
	}
	template <typename MGT>
	static inline uint8_t EntryCount(const MGT& rkMGMSG)
	{
		return rkMGMSG.m_usEntryCount;
	}
};
