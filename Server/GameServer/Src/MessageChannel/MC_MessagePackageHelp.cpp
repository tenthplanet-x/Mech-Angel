#include "MC_MessagePackageHelp.h"
#include "LC_ServerGuildManager.h"
#include "LC_ServerLogicManager.h"
#include "../Buddy/BuddyInstance.h"

using namespace Protocol;
using namespace GameLogic;

MC_MessagePackageHelp::MC_MessagePackageHelp(void)
{
}

MC_MessagePackageHelp::~MC_MessagePackageHelp(void)
{
}

void MC_MessagePackageHelp::Packet(MG_SyncPlayerLeave& leaveMsg, object_id_type ID)
{
	if (leaveMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	leaveMsg.m_rEntries[leaveMsg.m_usEntryCount] = ID;
	++leaveMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayerControllersPropertyInFOV& playerPropertyMsg, LC_ServerPlayer* player)
{
	if (playerPropertyMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	LC_ServerPlayer* pkCurrentActivatePlayer = player->GetCurrentActivateBuddy();
	LC_ServerPlayer* pkOwnerPlayer = player->GetOwnerPlayer();
	if(NULL == pkCurrentActivatePlayer || NULL ==pkOwnerPlayer)
	{
		return;
	}

	PlayerControllerPropertyEntry& kEntry = playerPropertyMsg.m_rEntries[playerPropertyMsg.m_usEntryCount];
	kEntry.clear();
	kEntry.m_lControllerCharID  = pkOwnerPlayer->GetInstance();
	kEntry.m_lControllerID = pkOwnerPlayer->GetID();
	kEntry.m_lCurrentPlayerID = pkCurrentActivatePlayer->GetID();
	kEntry.m_lSelfID = player->GetID();
	kEntry.m_lRiderCharType = player->GetRiderCharType();
	//kEntry.m_lRiderCharType = player->GetOwnerPlayer()->GetRiderCharType();
	kEntry.m_bPlayerGroupCheifFlag = player->GetPlayerGroupCheifFlag();
	kEntry.m_bFightState = player->GetCurrentFightState();
	kEntry.m_iPKState = player->GetPKState();
	kEntry.m_iPKValue = player->GetPKRecorder()->GetPKValue();
	kEntry.m_bInEvilState = player->GetPKRecorder()->IsInEvilState();
	kEntry.m_iLevel = player->GetLevel();
	kEntry.m_lShapeShiftInfoID = player->GetShapeShiftCharTypeID();
	kEntry.m_iFactionId = player->GetFactionID();
	kEntry.m_VipLevel = player->GetVIPAsset().GetVIPLevel();
	kEntry.m_iUiFlag = player->GetVIPAsset().GetUiFlag();
	kEntry.m_PlaceCanPK = player->GetPlaceCanPK();
	kEntry.m_iSitRecoverID = player->GetPairSitRecoverID();
	kEntry.m_uiGMPriority = player->GetGMPriority();
	kEntry.m_nNewVipLevel = pkOwnerPlayer->GetNewVipLevel();
	kEntry.m_nBornType = pkOwnerPlayer->GetProfilePictureID();
	kEntry.m_nCommanderTitle = pkOwnerPlayer->GetCommanderTitle();
	const platform_vip_info& pf_info = player->GetPlatformVipInfo();
	for(platform_vip_info::const_iterator iter = pf_info.begin(); iter != pf_info.end(); iter++)
	{
		const platform_node& rPfNode = iter->second;
		kEntry.m_Platform[iter->first.c_str()] = rPfNode.format_vip_info();
	}
	kEntry.m_cPlayerControllerName = player->GetOwnerCharName().c_str();
	kEntry.m_GuildName = player->GetOwnerPlayer()->GetGuildName().c_str();

	LC_TitleAsset* pkTitleAsset = player->GetOwnerPlayer()->GetTitleAsset();
	int titleType = pkTitleAsset->GetCurrentTitleType();
	if(titleType == TNT_SYSTEM_TITLE)
	{
		kEntry.m_TitleID = pkTitleAsset->GetCurrentTitle();
	}
	else
	{
		kEntry.m_TitleID = 0;
	}
	kEntry.m_iCombatScore = player->GetControllerCombatScore();
	++playerPropertyMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayersPropertyInFOV& playerPropertyMsg, LC_ServerPlayer* player)
{
	if (playerPropertyMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerPropertyEntry& kEntry = playerPropertyMsg.m_rEntries[playerPropertyMsg.m_usEntryCount];
	kEntry.clear();

	LC_ServerPlayer* pkOwnerPlayer = player->GetOwnerPlayer();
	if(NULL==pkOwnerPlayer)
	{
		return;
	}

	kEntry.m_lID = player->GetID();
	kEntry.m_lRiderCharType	= player->GetRiderCharType();
	kEntry.m_bPlayerGroupCheifFlag = player->GetPlayerGroupCheifFlag();

	LC_PKRecorderBase* pPKRecorder = player->GetPKRecorder();
	kEntry.m_bFightState = pPKRecorder->GetFightState();
	kEntry.m_iPKState = pPKRecorder->GetPKStateType();
	kEntry.m_iPKValue = pPKRecorder->GetPKValue();
	kEntry.m_bInEvilState = pPKRecorder->IsInEvilState();
	TPSTR2STDSTR(pkOwnerPlayer->GetOwnerCharName(), kEntry.m_cPlayerControllerName);

	kEntry.m_iLevel	= (uint16_t)player->GetBuddyLevel();
	kEntry.m_lShapeShiftInfoID = player->GetShapeShiftCharTypeID();
	kEntry.m_iFactionId	= player->GetFactionID();
	LC_VIPAsset& rVipAsset = pkOwnerPlayer->GetVIPAsset();
	kEntry.m_VipLevel = rVipAsset.GetVIPLevel();
	kEntry.m_iUiFlag = rVipAsset.GetUiFlag();
	kEntry.m_PlaceCanPK	= player->GetPlaceCanPK();
	kEntry.m_iSitRecoverID = player->GetPairSitRecoverID();
	kEntry.m_uiGMPriority = player->GetGMPriority();

	const platform_vip_info& pf_info = player->GetPlatformVipInfo();
	for(platform_vip_info::const_iterator iter = pf_info.begin(); iter != pf_info.end(); iter++)
	{
		const platform_node& rPfNode = iter->second;
		kEntry.m_Platform[iter->first.c_str()] = rPfNode.format_vip_info();
	}
	++playerPropertyMsg.m_usEntryCount;
}
void MC_MessagePackageHelp::Packet(msg_define::player_infos_in_fov& playerInfoMsg, LC_ServerPlayer* player, bool bSyncAll)
{
	if (playerInfoMsg.entries_size() >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	msg_define::player_info_entry* pkentry = playerInfoMsg.add_entries();
	if (pkentry)
	{
		uint64_t nCharId = 0;
		nCharId = pkOwnerPlayer->GetInstance();
		pkentry->set_id(player->GetID());
		//pkentry->set_charid(player->GetInstance());
		pkentry->set_charid(nCharId);
		pkentry->set_flag(player->GetPlayerFlag());
		pkentry->set_char_type(player->GetCharType());
		pkentry->set_weapon(player->GetWeaponAttribute());
		pkentry->set_state(player->GetDead() ? StateMachine::ST_PLAYER_DEAD : player->GetCurrentStateType());
		pkentry->mutable_location()->set_x(player->GetCurrentLocation().x);
		pkentry->mutable_location()->set_y(player->GetCurrentLocation().y);
		pkentry->set_char_name(player->GetOwnerCharName().c_str());
		pkentry->set_enabled_pet_tid(player->GetEnabledPetInstanceResId());
		pkentry->set_is_current_op(player->GetControlType() == LC_ServerPlayer::eControlType_Player);
		object_id_type nOwnerId = player->GetOwnerPlayerId();
		pkentry->set_owner_id(nOwnerId);
		pkentry->set_cur_state(player->GetCurrentStateType());
		pkentry->set_star_level(player->GetBuddyStarLevel());
		player->FillRuneLevelInfoPB(*(pkentry->mutable_rune_item()), *(pkentry->mutable_rune_level()));
		pkentry->set_bactivateruneslot(pkOwnerPlayer->GetBuddyRuneEquipState());
		pkentry->set_newviplevel(pkOwnerPlayer->GetNewVipLevel());
		pkentry->set_commandertitle(pkOwnerPlayer->GetCommanderTitle());
		pkentry->set_angle( LC_Helper::Angle_Float2Char( player->GetForwardDirAngle() ) );

		int pictureID = pkOwnerPlayer->GetProfilePictureID();
		if(0 == pictureID)
		{
			pictureID = pkOwnerPlayer->GetBornCharType();
		}
		pkentry->set_pictureid(pictureID);

		pkentry->set_commandertitleid(pkOwnerPlayer->GetCurrentCommanderTitleFrameID());

		BuddyInstance* pkBuddy = player->GetAssignedBuddyInstance();
		if(NULL != pkBuddy)
		{
			pkentry->set_equipslotlevelcount(pkBuddy->GetEquipSlotTotalLevel());
			pkentry->set_buddycurrenttransformid(pkBuddy->GetBuddyCurrentTransformID());

			int skillCount = pkBuddy->GetTotalActivatedSkillCount();
			bool tag = skillCount==4 ? true : false;
			pkentry->set_boolactivateallskill(tag);

			std::map<int32_t,PS_EquipItemInfo_BuddyCS> equipData;
			pkBuddy->GetEquipInfo(equipData);

			::msg_define::char_equipSlotInfo* equipInfo = pkentry->mutable_equipslotinfo();
			equipInfo->set_equipslotstarstate(pkOwnerPlayer->GetBuddyEquipSlotStarState());
			for(std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator it=equipData.begin(); it!=equipData.end(); ++it)
			{
				equipInfo->add_equipslotindex(it->first);
				equipInfo->add_equipitem(it->second.m_nEquipItemID);
				equipInfo->add_equipslotstarlevel(it->second.m_nEquipSlotStarLevel);
			}
		}
		
		if (nOwnerId)
		{
			pkentry->set_hp(player->GetHP());
			pkentry->set_max_hp(player->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
		}
		else
		{
			pkentry->set_hp(1);
			pkentry->set_max_hp(1);
		}
	}
}
void MC_MessagePackageHelp::Packet(msg_define::player_propertys_in_fov& playerInfoMsg, LC_ServerPlayer* player, bool bSyncAll)
{
	if (playerInfoMsg.entries_size() >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	msg_define::player_property* pkentry = playerInfoMsg.add_entries();
	if (pkentry)
	{
		int id = player->GetID();
		int64_t hp = player->GetHP();
		pkentry->set_id(player->GetID());
		LC_ActorAttributeMap* pkAttrMap = player->GetAttributeMap();
		pkentry->set_hp(player->GetHP());
		pkentry->set_max_hp(pkAttrMap->GetAttribute(ATT_MAX_HP));
		pkentry->set_hp_recover(pkAttrMap->GetAttribute(ATT_HP_RECOVER));
		pkentry->set_atk(pkAttrMap->GetAttribute(ATT_MAX_ATK));
		pkentry->set_cir_rate(pkAttrMap->GetAttribute(ATT_CIR_RATE));
		pkentry->set_cir_dam(pkAttrMap->GetAttribute(ATT_CIR_DAM));
		pkentry->set_def(pkAttrMap->GetAttribute(ATT_DEF));
		pkentry->set_dog_rate(pkAttrMap->GetAttribute(ATT_DOG_RATE));
		pkentry->set_speed(pkAttrMap->GetAttribute(ATT_MOVE_SPEED));
		pkentry->set_skill_flag(player->GetAllSkillFlag());
		//pkentry->set_score(player->GetTotalScore());
		//pkentry->set_score(player->GetControllerCombatScore());
		pkentry->set_score(player->GetCombatScore());
	}
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayersExtPropertyInFOV& playerInfoMsg, LC_ServerPlayer* player)
{
	if (playerInfoMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerExtPropertyEntry& kEntry = playerInfoMsg.m_rEntries[playerInfoMsg.m_usEntryCount];
	kEntry.clear();
	kEntry.m_lID = player->GetID();

	LC_ServerPlayer* pkOwnerPlayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
		return;

	LC_PlayerGroupBase* group = pkOwnerPlayer->GetPlayerGroup();
	if (group != NULL)
	{
		kEntry.m_iGroupId = group->GetID();
		kEntry.m_iGroupNum = (uint8_t)group->GetMemberCount(false);
	}
	else
	{
		kEntry.m_iGroupId = 0;
		kEntry.m_iGroupNum = 0;
	}

	++playerInfoMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayersSocietyInFOV& playerPropertyMsg, LC_ServerPlayer* player)
{
	if (playerPropertyMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}
	PlayerSocietyEntry& kEntry = playerPropertyMsg.m_rEntries[playerPropertyMsg.m_usEntryCount];
	kEntry.clear();

	LC_ServerPlayer* pkOwnerPlayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	kEntry.m_lID	= player->GetID();
	kEntry.m_lPlayerGuildID	= pkOwnerPlayer->GetGuildID();
	kEntry.m_iTitleID	= pkOwnerPlayer->GetCurrentSystemTitleID();
	kEntry.m_iServerID	= pkOwnerPlayer->GetServerID();
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuild = pkGuildManager->GetGuildInfoByID(pkOwnerPlayer->GetGuildID());
	if (pkGuild != NULL)
	{
		kEntry.m_iGuildRaidPage = (uint8_t)pkGuild->GetGuildBasicInfo().m_GuildRaidPage;
		kEntry.m_iGuildRaidStage = (uint8_t)pkGuild->GetGuildBasicInfo().m_GuildRaidStage;
	}
	TPSTR2STDSTR(pkOwnerPlayer->GetCharTitle(), kEntry.m_strTitle);
	TPSTR2STDSTR(pkOwnerPlayer->GetGuildName(), kEntry.m_strGuildName);
	++playerPropertyMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayersEquipInFOV& playerEquipMsg, LC_ServerPlayer* player)
{
	if (playerEquipMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerEquipEntry& kEntry = playerEquipMsg.m_rEntries[playerEquipMsg.m_usEntryCount];
	kEntry.m_lID	= player->GetID();
	kEntry.m_lHeadAvatarItemID		= player->GetCurrentEquipItem(ET_HEAD, true);
	kEntry.m_lBodyAvatarItemID		= player->GetCurrentEquipItem(ET_BODY, true);
	kEntry.m_lFootAvatarItemID		= player->GetCurrentEquipItem(ET_FOOT, true);
	kEntry.m_lWeaponAvatarItemID	= player->GetCurrentEquipItem(ET_WEAPON, true);
	++playerEquipMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayerHPMPChange& HPMPChangeMsg, LC_ServerPlayer* player)
{
	if (HPMPChangeMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerHPMPStateEntry& kEntry = HPMPChangeMsg.m_rEntries[HPMPChangeMsg.m_usEntryCount];
	kEntry.m_lID = player->GetID();
	kEntry.m_iHP = player->GetHP();

	++HPMPChangeMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayerPVEStateChange& PVEStateChangeMsg, LC_ServerPlayer* player)
{
	if (PVEStateChangeMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerPVEStateEntry& kEntry = PVEStateChangeMsg.m_rEntries[PVEStateChangeMsg.m_usEntryCount];

	kEntry.m_lID = player->GetID();
	kEntry.m_bFightState = player->GetPKRecorder()->GetFightState();

	++PVEStateChangeMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayerStateChange& stateChangeMsg, LC_ServerPlayer* player)
{
	if (stateChangeMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerStateEntry& kEntry = stateChangeMsg.m_rEntries[stateChangeMsg.m_usEntryCount];

	kEntry.m_lID = player->GetID();
	kEntry.m_iMoveSpeed = (uint16_t)player->GetMoveSpeed();
	kEntry.m_iPlayerState = 0;//(uint32_t)player->GetCurrentStateType();
	kEntry.m_iSitPairID = (uint32_t)player->GetPairSitRecoverID();

	++stateChangeMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayerLocation& locationChangeMsg, LC_ServerPlayer* player)
{
	if (locationChangeMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerLocationEntry& kEntry = locationChangeMsg.m_rEntries[locationChangeMsg.m_usEntryCount];
	kEntry.m_lPlayerID = player->GetID();
	kEntry.m_lCurrentLocationX = player->GetCurrentLocation().x;
	kEntry.m_lCurrentLocationY = player->GetCurrentLocation().y;
	kEntry.m_lTargetLocationX = player->GetTargetLocation().x;
	kEntry.m_lTargetLocationY = player->GetTargetLocation().y;
	kEntry.m_nActionMode = player->GetMoveActionMode();

	++locationChangeMsg.m_usEntryCount;

}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncPlayerAngle& angleChangeMsg, LC_ServerPlayer* player)
{
	if (angleChangeMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	PlayerAngleEntry& kEntry = angleChangeMsg.m_rEntries[angleChangeMsg.m_usEntryCount];
	kEntry.m_lPlayerID = player->GetID();
	kEntry.m_chCurrentDirAngle = LC_Helper::Angle_Float2Char( player->GetForwardDirAngle() );

	++angleChangeMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(msg_define::skill_states_in_fov& skillStateChangeMsg, LC_ServerPlayer* player, bool bSyncAll)
{
	if (skillStateChangeMsg.entries_size() >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	LC_ActorStateMap* pkStateMap = player->GetStateMap();
	LC_ActorStateMap* pkControllerStateMap = player->GetControllerStateMap();
	if (NULL == pkStateMap || NULL == pkControllerStateMap)
	{
		return;
	}

	msg_define::skill_state_info* pkentry = skillStateChangeMsg.add_entries();
	if (pkentry)
	{
		pkentry->set_id(player->GetID());

		//全量同步
		if (bSyncAll)
		{
			pkStateMap->SetToProtocol(*pkentry, SSC_ALL);
			pkControllerStateMap->SetToProtocol(*pkentry, SSC_ALL);
		}
		//增量同步
		else
		{
			//每隔90秒全量同步一次状态
			if (GET_PROCESS_TIME - pkStateMap->GetLastFovTime() > 90)
			{
				pkStateMap->SetToProtocol(*pkentry, SSC_ALL);
				pkControllerStateMap->SetToProtocol(*pkentry, SSC_ALL);
				pkStateMap->SetLastFovTime(GET_PROCESS_TIME);
			}
			else
			{
				pkStateMap->SetToProtocol(*pkentry, SSC_MAX_COUNT);
				pkControllerStateMap->SetToProtocol(*pkentry, SSC_MAX_COUNT);
			}
		}
	}
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncSubordinateInfoInFOV& subChangeMsg, LC_ServerPlayer* player)
{
	if (subChangeMsg.m_usEntryCount >= MAX_PLAYER_COUNT_IN_FOV)
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = player->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	MG_SyncSubordinateInfo psSub;
	psSub.m_lPlayerID = player->GetID();
	pkOwnerPlayer->GetSubordinateAsset().SetDataToProtocolStruct(psSub, LC_SUB_SYNC_MASK_FOV);

	subChangeMsg.m_rEntries[subChangeMsg.m_usEntryCount] = psSub;

	++subChangeMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncNpcEnter& npcEnterMsg, LC_ServerNPC* npc)
{
	npcEnterMsg.m_fServerTime = GET_PROCESS_TIME;
	if (npcEnterMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}

	NPCInfoEntry& kEntry = npcEnterMsg.m_rEntries[npcEnterMsg.m_usEntryCount];
	kEntry.m_lID	= npc->GetID();
	kEntry.m_lFollowedID	= npc->GetFollowedTargetID();
	kEntry.m_lSummonOwnerID	= npc->GetSummonOwnerID();
	kEntry.m_lAdditionTargetID	= npc->GetAdditionTargetID();
	kEntry.m_lNPCCharType	= npc->GetCharType();
	kEntry.m_iFactionType	= npc->GetFactionID();
	kEntry.m_iAutoFitLevel	= npc->GetAutoFitLevel();
	kEntry.m_iRealLevel	= npc->GetLevel();
	kEntry.m_iSpawnRotAngle	= npc->GetSpawnZRotAngle();
	kEntry.m_Elite_Level	= (int8_t)(npc->GetEliteLevel() | (npc->GetProperty_IsClone() ? 0x80 : 0));
	kEntry.m_iCurrentLocationX	= npc->GetCurrentLocation().x;
	kEntry.m_iCurrentLocationY	= npc->GetCurrentLocation().y;
	kEntry.m_iCurrentLocationZ	= npc->GetCurrentLocation().z;
	kEntry.m_SpawnTime	= (int32_t)npc->GetSpawnTime();
	kEntry.m_iNPCState	= npc->GetCurrentStateType();
	kEntry.m_nMapLogicId = npc->GetMapLogicID();

	++npcEnterMsg.m_usEntryCount;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncNpcLeave& npcLeaveMsg, object_id_type ID)
{
	if (npcLeaveMsg.m_lNpcNum >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}
	npcLeaveMsg.m_rEntries[npcLeaveMsg.m_lNpcNum] = ID;

	++npcLeaveMsg.m_lNpcNum;
}

void MC_MessagePackageHelp::Packet(msg_define::npc_propertys_in_fov& npcPropertyMsg, LC_ServerNPC* npc, bool bSyncAll)
{
	if (npcPropertyMsg.entries_size() >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}

	object_id_type hitPlayerID = npc->GetHitPlayerID();
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(hitPlayerID));

	msg_define::npc_property* pkentry = npcPropertyMsg.add_entries();
	if (pkentry)
	{
		pkentry->set_id(npc->GetID());
		pkentry->mutable_property()->set_max_hp(npc->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
		pkentry->mutable_property()->set_hp(npc->GetHP());
		pkentry->set_faction(npc->GetFactionID());
		pkentry->set_shape_shift(npc->GetShapeShiftCharTypeID());
		pkentry->set_skill_flag(npc->GetAllSkillFlag());
		const StringType& hitPlayerName = (NULL!=pkPlayer ? pkPlayer->GetOwnerCharName() : "");
		//std::string name = TPSTR2STDSTR(hitPlayerName);
		pkentry->set_hitplayer_name( TPSTR2STDSTR(hitPlayerName));

		uint64_t guildID = (NULL!=pkPlayer ? pkPlayer->GetGuildID() : 0);
		pkentry->set_hitplayer_guildid(guildID);
		pkentry->set_hitplayer_id(hitPlayerID);

		const int32_t* pskill = npc->GetSkill();
		{
			for (size_t i=0; i<MAX_NPC_SKILL; ++i)
			{
				pkentry->add_skill_ids(pskill[i]);
			}
		}

		pkentry->set_summor(npc->GetSummonOwnerID());
		pkentry->set_additional_target(npc->GetAdditionTargetID());
		pkentry->set_guild_id(npc->GetGuildID());
		pkentry->set_server_id(npc->GetServerID());
	}
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncNPCsMoveInFOV& npcMoveMsg, LC_ServerNPC* npc)
{
	if (npcMoveMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}

	NPCMoveEntry& kEntry = npcMoveMsg.m_rEntries[npcMoveMsg.m_usEntryCount];

	kEntry.m_lID	= npc->GetID();
	kEntry.m_lLockedTargetID	= npc->GetLockedTargetID();
	kEntry.m_iMoveSpeedAtt	= (uint16_t)npc->GetAttributeMap()->GetAttribute(ATT_MOVE_SPEED);
	kEntry.m_iWalkSpeedAtt	= (uint16_t)npc->GetAttributeMap()->GetAttribute(ATT_WALK_SPEED);
	//kEntry.m_iNPCState		= npc->GetCurrentStateType();
	kEntry.m_iCurrentLocationX	= npc->GetCurrentLocation().x;
	kEntry.m_iCurrentLocationY	= npc->GetCurrentLocation().y;
	kEntry.m_iTargetLocationX	= npc->GetMoveLineTargetLocation().x;
	kEntry.m_iTargetLocationY	= npc->GetMoveLineTargetLocation().y;

	++npcMoveMsg.m_usEntryCount;
	npcMoveMsg.m_fServerTime = GET_PROCESS_TIME;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncNPCsStateInFOV& npcStateMsg, LC_ServerNPC* npc)
{
	if (npcStateMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}

	NPCStateEntry& kEntry = npcStateMsg.m_rEntries[npcStateMsg.m_usEntryCount];

	kEntry.m_lID	= npc->GetID();
	kEntry.m_iState		= npc->GetCurrentStateType();

	++npcStateMsg.m_usEntryCount;
	npcStateMsg.m_fServerTime = GET_PROCESS_TIME;
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncNPCsAngleInFOV& npcAngleMsg, LC_ServerNPC* npc)
{
	if (npcAngleMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}

	NPCAngleEntry& kEntry = npcAngleMsg.m_rEntries[npcAngleMsg.m_usEntryCount];

	kEntry.m_lID	= npc->GetID();
	kEntry.m_iAngle		= LC_Helper::Angle_Float2Char( npc->GetForwardDirAngle() );

	++npcAngleMsg.m_usEntryCount;
	npcAngleMsg.m_fServerTime = GET_PROCESS_TIME;
}

void MC_MessagePackageHelp::Packet(msg_define::skill_states_in_fov& npcSkillStateMsg, LC_ServerNPC* npc, bool bSyncAll)
{
	if (npcSkillStateMsg.entries_size() >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}

	LC_ActorStateMap* pkStateMap = npc->GetStateMap();
	msg_define::skill_state_info* pkentry = npcSkillStateMsg.add_entries();
	if (pkentry)
	{
		pkentry->set_id(npc->GetID());
		//全量同步
		if (bSyncAll)
		{
			pkStateMap->SetToProtocol(*pkentry, SSC_ALL);
		}
		//增量同步
		else
		{
			//每隔90秒全量同步一次状态
			if (GET_PROCESS_TIME - pkStateMap->GetLastFovTime() > 90)
			{
				pkStateMap->SetToProtocol(*pkentry, SSC_ALL);
				pkStateMap->SetLastFovTime(GET_PROCESS_TIME);
			}
			else
			{
				pkStateMap->SetToProtocol(*pkentry, SSC_MAX_COUNT);
			}
		}
	}
}

void MC_MessagePackageHelp::Packet(Protocol::MG_SyncNPCsInteractionValueInFOV& npcInteractionValueMsg, LC_ServerNPC* npc)
{
	if (npcInteractionValueMsg.m_usEntryCount >= MAX_NPC_COUNT_IN_FOV)
	{
		return;
	}

	NPCInteractionEntry& kEntry = npcInteractionValueMsg.m_rEntries[npcInteractionValueMsg.m_usEntryCount];
	kEntry.m_lID = npc->GetID();
	kEntry.m_lValue	= npc->GetInteractionValue();

	++npcInteractionValueMsg.m_usEntryCount;
}
