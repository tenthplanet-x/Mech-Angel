#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include "CF_EXPList.h"
#include "oss_define.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPC.h"
#include "LC_ItemBase.h"
#include "LC_PackStruct.h"
#include "LC_Task.h"
#include "SK_Define.h"
#include "LC_ServerGuildManager.h"
#include "GameServerApp.h"
#include "CF_ItemList.h"
#include "CF_WorldMapList.h"
#include "CF_SkillExecuteList.h"
#include "LC_ServerPlayerGroup.h"
#include "UT_ServerHelper.h"
#include "OSSDefine.h"
#include "MG_PACKET.h"
#include "MG_Struct.h"
using namespace GameLogic;
using namespace CSVFile;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline int get_union_id()
{
	return GetGameServerApp()->GetServerUnionInfo().union_id;
}

inline int get_world_id(LC_ServerPlayer * player)
{
	return player ? player->GetCitizenship() : GetGameServerApp()->GetServerInfo().m_iServerID;
}

inline int cur_svr_id(void)
{
	return GetGameServerApp()->GetServerInfo().m_iServerID;
}

oss::oss_obj& operator , (oss::oss_obj& lv , LC_BackPackEntry* item)
{
	//物品ID<<物品类型<<物品数量<<物品标示
	lv << 0 << (item ? item->GetBackPackValueByType(LPSFET_TYPE_ID) : 0) << (item ? item->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) : 0) << (item ? make_item_uid(item) : "0");
	return lv;
}

oss::oss_obj& operator <<(oss::oss_obj& lv , LC_BackPackEntry* item)
{
	return operator, (lv, item);
}

oss::oss_obj& operator <<(oss::oss_obj& lv , Protocol::PS_ItemAssetEntry* item)
{
	lv << 0 << (item ? item->GetItemBaseData_TypeID() : 0) << (item ? item->GetItemBaseData_OverlapCount() : 0) << (item ? item->GetItemBaseData_Bind() : 0);
	return lv;
}

oss::oss_obj& operator , (oss::oss_obj& lv , LC_ServerPlayer* player)
{
	lv
		<< (player ? player->GetInstance() : null_unique_impl)		//iRoleId
		<< (player ? player->GetOwnerCharName_utf8() : "")				//vRoleName
		<< (player ? (int)player->GetWeaponAttribute() : 0)			//iJobId
		<< (player ? player->GetCitizenship() : 0)					//iGender
		<< (player ? player->GetLevel() : 0);						//iRoleLevel
	return lv;
}

#define oss_gbk_player(player)\
		(player ? player->GetInstance() : null_unique_impl)\
		<< (player ? player->GetOwnerCharName() : "")\
		<< (player ? (int)player->GetWeaponAttribute() : 0)\
		<< (player ? player->GetCitizenship() : 0)\
		<< (player ? player->GetLevel() : 0)\


#define oss_player_charid(charid) (charid) << "" << 0 << charid.catagory() << 0


oss::oss_obj& operator <<(oss::oss_obj& lv , LC_ServerPlayer* player)
{
	return operator, (lv, player);
}

inline int32_t ip_addr_to_string(uint32_t ip, char* ip_buff, size_t buff_size)
{
	// 192.168.111.222，格式长度为15
	if (NULL == ip_buff || buff_size == 0 || buff_size < 16)
	{
		return -1;
	}
	ip = htonl(ip);
	unsigned char* src = (unsigned char*)&ip;
	const char* fmt = "%u.%u.%u.%u";
	sprintf(ip_buff, fmt, src[0], src[1], src[2], src[3]);
	return 0;
}

StringType make_empty_ip_string(LC_ServerPlayer* player)
{
	//make empty ip, just for smaller oss log
	return "";
}

StringType make_ip_string(LC_ServerPlayer* player)
{
	if (player)
	{
		char buffer[64];
		ip_addr_to_string(player->GetLoginIP(), buffer, sizeof(buffer));
		return StringType(buffer);
	}
	return "0";
}

static StringType g_defAccount = "";
inline const StringType& make_Account_string(LC_ServerPlayer* player)
{
	return player ? player->GetUserName_utf8() : g_defAccount;
}

inline StringType make_item_uid(LC_BackPackEntry* pack)
{
	return pack ? pack->MakeItemGuid() : "0";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static unsigned int g_seq = 0;
StringType make_event_id(int nId, tm& result)
{
	char buffer[512] = {0};
	static int32_t iCurUnionID = GetGameUnionID();
	static int32_t iCurServerID = GetGameServerID();
	sprintf_s(buffer, sizeof(buffer), "%06d%06d%04d%04d%02d%02d%02d%02d%02d%03d", 
		iCurUnionID, iCurServerID, nId, 
		result.tm_year + 1900, result.tm_mon + 1, result.tm_mday,
		result.tm_hour, result.tm_min, result.tm_sec, (++g_seq) % 1000);
	return StringType(buffer);
}

#define CHECKNULLPTR(ptr)\
	if(NULL == ptr)\
		return;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void oss_role_lvup(LC_ServerPlayer* player, int nReason)
{
	CHECKNULLPTR(player);

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("RoleLevelUp", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<iRoleExp<<iTotalOnlineTime<<iReason<<iLoginWay
				 make_event_id(OSS_GAMESVR_USER_ROLEUP, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << player->GetEXP() << player->GetTotalOnlineTime() << nReason << get_union_id()
				);
}

void oss_login(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("RoleLogin", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel<<iRoleExp<<iCoin<<iRepute角色声望值
				 //<<iVipLevel<<iMoney<<iAccumDeposit<<iGamePoints<<dtCreateTime<<iOnlineTotalTime<<dtLoginTime<<iLoginWay
				 make_event_id(OSS_GAMESVR_USER_LOGIN, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_ip_string(player) << player << player->GetEXP() << player->GetUnBindCash().GetMoney()
				 << player->GetVigor() << player->GetVIPLevel() << player->GetUnBindYuanBao().GetMoney() << player->GetVIPAsset().GetCredit() << player->GetBindYuanBao().GetMoney()
				 << make_time(player->GetRegTime()) << player->GetTotalOnlineTime() << make_time(player->GetAccountLoginTime()) << get_union_id()
				);
}

void oss_logout(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("RoleLogout", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<dtLogintime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel<<dtCreateTime<<iOnlineTime<<iOnlineTotalTime<<iRoleExp
				 //<<iCoin<<iRepute<<iVipLevel<<iMoney<<iAccumDeposit<<iGamePoints<<iMapID<<iPosX<<iPosY<<iPosZ<<iLoginWay
				 make_event_id(OSS_GAMESVR_USER_LOGOUT, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_time(player->GetAccountLoginTime()) << make_empty_ip_string(player) << player << make_time(player->GetRegTime())
				 <<  player->GetAntiWallowOnlineTime() << player->GetTotalOnlineTime() << player->GetEXP() << player->GetUnBindYuanBao().GetMoney() << player->GetVigor() << player->GetVIPLevel() << player->GetUnBindYuanBao().GetMoney() << player->GetVIPAsset().GetCredit()
				 << player->GetBindYuanBao().GetMoney() << player->GetMapResID() << player->GetCurrentLocation().x << player->GetCurrentLocation().y << player->GetCurrentLocation().z << get_union_id()
				);
}

void oss_online(int count)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("OnlineCount", get_world_id(),
				 //<<iEventId<<dtEventTime<<iWorldId<<iAccountCount<<iAccountMax<<iPlayerCount<<iPlayerMax
				 make_event_id(OSS_GAMESVR_USER_ONLINENUM, result) << make_time(result) << get_world_id() << count << count << count << count
				);
}

void oss_recharge(LC_ServerPlayer* player, int32_t yb, const std::string& sn, uint32_t viplvl, int64_t vipcredit, int srcType)
{
	int64_t pyb = player->GetUnBindYuanBao().GetMoney();
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("Recharge", get_world_id(),
				 //<<iEventId<<dtEventTime<<iworldid<<iUin<<iSrcUin<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<vRemarks<<iPayBefore<<iPayDelta<<iPayAfter<<vDesc<<iLoginWay
				 make_event_id(OSS_GAMESVR_USER_RECHARGE, result) << make_time(result) << get_world_id(player) << make_Account_string(player) << make_Account_string(player) << make_ip_string(player) << player
				 << sn << pyb - yb << yb << pyb << "" << get_union_id() << viplvl << vipcredit << srcType << (player ? player->GetGuildID() : INVALID_GUILD_ID)
				);
}

void oss_recharge(const unique_id_impl& charID, int32_t yb, const std::string& sn, int srcType)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("Recharge", get_world_id(),
		//<<iEventId<<dtEventTime<<iworldid<<iUin<<iSrcUin<<vClientIp
		//<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
		//<<vRemarks<<iPayBefore<<iPayDelta<<iPayAfter<<vDesc<<iLoginWay
		make_event_id(OSS_GAMESVR_USER_RECHARGE, result) << make_time(result) << get_world_id(NULL) << make_Account_string(NULL) << make_Account_string(NULL) << make_ip_string(NULL)
		<< charID << "" << 0 << charID.catagory() << 0
		<< sn << 0 << yb << yb << "" << get_union_id() << 0 << 0 << srcType << INVALID_GUILD_ID
		);
}

void oss_role_comsume(LC_ServerPlayer* player, int cost_type, int64_t cost_yb , int64_t bind_yb, int shop_type, int goods_type, int shop_id, int shop_count, const StringType& item_id)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("Consume", get_world_id(),
				 //<<iEventId<<iWorldId<<dtEventTime<<vClientIp<<iUin<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel<<iCostType<<iCost<<iRemain<<iShopType<<iShopID<<iGoodsId<<iGoodsNum<<iItemGuid<<iLoginWay
				 make_event_id(OSS_GAMESVR_USER_CONSUME, result) << get_world_id(player) << make_time(result) << make_empty_ip_string(player) << make_Account_string(player) << player
				 << cost_type << cost_yb << bind_yb << shop_type << shop_id << goods_type << shop_count << item_id << get_union_id()
				);
}


void oss_role_task_start(LC_ServerPlayer* player , LC_Task* task , LC_BackPackEntry* nitem , LC_BackPackEntry* litem)
{
	CHECKNULLPTR(player);
	CHECKNULLPTR(task);
	StringType npc_name = "0";
	if (task->GetAcceptNPCID())
	{
		CF_CharType::DataEntry* entry = SafeGetCSVFileDataEntryPtr<CF_CharType>(task->GetAcceptNPCID());
		npc_name = entry->_kCharName;
	}

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("TaskStart", get_world_id(),
				 make_event_id(OSS_GAMESVR_USER_TASKSTART, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << player->GetUnBindCash().GetMoney() << player->GetUnBindCash().GetMoney() << task->GetTaskID() << "" << task->GetAcceptNPCID() << npc_name << 0
				 << nitem << litem << get_union_id()
				);
	//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, strLogInfo.c_str());
}

void oss_role_task_finished(LC_ServerPlayer* player , LC_Task* task , LC_BackPackEntryPtrList items , LC_BackPackEntry* litem , int gp , int exp , int sp)
{
	CHECKNULLPTR(player);
	CHECKNULLPTR(task);
	const char* tag = "TaskFinished";
	StringType npc_name = "0";
	//StringType strLogInfo = "";

	DEF_LOCAL_TIME(result);
	StringType eventID = make_event_id(OSS_GAMESVR_USER_TASKEND, result);
	if (task->GetFinishNPCID())
	{
		CF_CharType::DataEntry* entry = SafeGetCSVFileDataEntryPtr<CF_CharType>(task->GetFinishNPCID());
		npc_name = entry->_kCharName;
	}

	bool bAddItem = false;
	for (LC_BackPackEntryPtrList::iterator i = items.begin() ; i != items.end(); ++i)
	{
		bAddItem = true;
		LC_BackPackEntry* item = *i;
		OSS_LOG_INFO(tag, get_world_id(),
					 eventID << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << "" << player
					 << player->GetUnBindCash().GetMoney()-gp << player->GetUnBindCash().GetMoney() << task->GetTaskID() << "" << task->GetAcceptNPCID() << npc_name << exp << gp << sp <<0
					 << item << litem << get_union_id()
					);
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, strLogInfo.c_str());
	}
	if(bAddItem == false)
	{
		OSS_LOG_INFO(tag, get_world_id(),
			eventID << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << "" << player
			<< player->GetUnBindCash().GetMoney()-gp << player->GetUnBindCash().GetMoney() << task->GetTaskID() << "" << task->GetAcceptNPCID() << npc_name << exp << gp << sp <<0
			<< NULL << litem << get_union_id()
			);
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, strLogInfo.c_str());
	}
}

void oss_role_task_cancel(LC_ServerPlayer* player , LC_Task* task , LC_BackPackEntry* nitem , LC_BackPackEntry* litem)
{
	CHECKNULLPTR(player);
	CHECKNULLPTR(task);
	StringType npc_name = "0";
	if (task->GetAcceptNPCID())
	{
		CF_CharType::DataEntry* entry = SafeGetCSVFileDataEntryPtr<CF_CharType>(task->GetAcceptNPCID());
		npc_name = entry->_kCharName;
	}

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("CancelTask", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<imoneybeforetask<<imoneyaftertask<<iTaskId<<vTaskName<<iNpcId<<vNpcName<<imoneytransfer
				 //<<igetitemid<<igetitemnum<<IgetitemGuid<<igetitemtype<<ilostitemid<<ilostitemnum<<iIostitemGuid<<ilostitemtype<<iLoginWay
				 make_event_id(OSS_GAMESVR_USER_TASKSTART, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << make_time() << player
				 << player->GetUnBindCash().GetMoney() << player->GetUnBindCash().GetMoney() << task->GetTaskID() << "" << task->GetAcceptNPCID() << npc_name << 0
				 << nitem << litem << get_union_id()
				);
}

void oss_role_sk_lvup(LC_ServerPlayer* player , int sk_id, int type, int sp , int cash)
{
	CHECKNULLPTR(player);
	int sk_lv = sk_id % SKILL_LEVEL_MASK;
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("RefinedSkillLevelUp", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iRoleExp<<iType<<iSubType<<iPreSpeSkill<<iCurSpeSkill<<iRefinedLevel<<iPreLevel<<iCurLevel
				 //<<iPreExpPoint<<iCurExpPoint<<iUsePointExp<<iPreMoney<<iCurMoney<<iUseMoney
				 make_event_id(OSS_GAMESVR_USER_SKILLLVLUP, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player)
				 << oss_player_charid((player ? player->GetInstance() : null_unique_impl))
				 << player->GetEXP() << type <<sk_id / SKILL_LEVEL_MASK << sk_id - 1 << sk_id << sk_lv << sk_lv - 1 << sk_lv
				 << player->GetSkillLevelupPoints() + sp << player->GetSkillLevelupPoints() << sp << player->GetUnBindCash().GetMoney() + cash << player->GetUnBindCash().GetMoney() << cash
				);
}

void oss_role_killed_by_monster(LC_ServerPlayer* player , LC_ServerNPC* npc)
{
	CHECKNULLPTR(player);
	CHECKNULLPTR(npc);
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("MonsterKill", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iMapId<<iPosX<<iPosY<<iZoneId<<iMonsterId<<vMonsterName<<iMonsterLevel<<iIsCall
				 make_event_id(OSS_GAMESVR_USER_MONSTERKILL, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << player->GetMapResID() << player->GetCurrentLocation().x << player->GetCurrentLocation().y << 0 << npc->GetCharType() << npc->GetOwnerCharName() << npc->GetLevel() << (npc->GetSummonOwnerID() > 0 ? 1 : 0)
				);
}

void oss_role_killed_by_player(LC_ServerPlayer* player , LC_ServerPlayer* kill , int pk_type)
{
	CHECKNULLPTR(player);
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("PlayerKill", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iKillUin<<iKillRoleId<<vKillRoleName<<iKillJobId<<iKillGender<<iKillRoleLevel<<iMapId<<iPosX<<iPosY<<iZoneId<<iKillerPkCount<<iPKType<<ikilleraddexp
				 make_event_id(OSS_GAMESVR_USER_PLAYERKILL, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << make_Account_string(kill) << kill << player->GetMapResID() << player->GetCurrentLocation().x << player->GetCurrentLocation().y << 0 << 0 << pk_type << 0
				);
}

void yb_income(LC_ServerPlayer* player , int yb , const std::string& reason)
{
	CHECKNULLPTR(player);
	//int64_t pyb = player->GetUnBindYuanBao().GetMoney() + yb;
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("FreeRecharge", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<vReMarks<<iPayBefore<<iPayDelta<<iPayAfter<<vDesc<<iLoginWay
				 make_event_id(OSS_GAMESVR_USER_FREECHARGE, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << "" << player->GetUnBindYuanBao().GetMoney() - yb << yb << player->GetUnBindYuanBao().GetMoney() << reason << get_union_id()
				);
}

void oss_party_build(LC_ServerPlayer* player, int nID, const StringType& sName, int nLvl, int nMoney, int nPolity)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("PartyBuild", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iPartyId<<vPartyName<<iPartyLevel<<iPartyMoney<<iPartyPolity
				 make_event_id(OSS_GAMESVR_USER_PARTYBUILD, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << nID << sName << nLvl << nMoney << nPolity
				);
}

void oss_party_dismiss(LC_ServerPlayer* player, int nID, LC_ServerGuildInfo*  guild)
{
	CHECKNULLPTR(guild);
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("PartyDismiss", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iPartyId<<vPartyName<<iPartyLevel<<iPartyMoney<<iPartyPolity<<iPartyNum<<iPartyPower
				 //<<iPartyRedToken<<iPartyIronToken<<iPartyPurleToken<<iPartyCaptiveToken
				 make_event_id(OSS_GAMESVR_USER_PARTYDISMISS, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << nID << guild->GetNameUTF8() << guild->GetLevel() << guild->GetAssertCount(0) << 0 << guild->GetMemberSize() << guild->GetGuildTotalScore()
				 << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_1) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_2) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_3) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_4)
				);
}

void oss_guild_contribution(LC_ServerPlayer* player , int type , int num , int con_num , int p_coin, LC_ServerGuildInfo*  guild)
{
	return;
	CHECKNULLPTR(player);
	CHECKNULLPTR(guild);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_PARTYCONTRAB, result);
	//<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName
	//<<iPartyId<<vPartyName<<iPartyNum<<iPartyLevel<<iPartyPower<<iPartyMoneyBefore<<iPartyRedTokenBefore<<iPartyIronTokenBefore<<iPartyPurleTokenBefore<<iPartyCaptiveTokenBefore
	//<<iContributeType<<iContributeNum<<iPartyMoney<<iPartyRedToken<<iPartyIronToken<<iPartyPurleToken<<iPartyCaptiveToken<<iPartyContributeBefore<<iPartyContribute<<iPartyContributeMoney
	//<<iPartyContributeBefore<<iPartyContributeMoneyBefore
	OSS_LOG_INFO("PartyContribution", get_world_id(),
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player->GetID() << player->GetOwnerCharName()
				 << guild->GetId() << guild->GetNameUTF8() << guild->GetMemberSize() << guild->GetLevel() << guild->GetGuildTotalScore()
				 << guild->GetAssertCount(0) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_1) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_2) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_3) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_4)
				 << type << num << guild->GetAssertCount(0) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_1) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_2) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_3) << guild->GetAssertCount(GUILD_ASSET_TYPE_ITEM_4)
				 << 0 << con_num << p_coin << 0 << 0
				);
}

void oss_party_levelup(LC_ServerPlayer* player, int nID, const StringType& sName, int nLvl, int nMoney, int nPolity)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("PartyLevelUp", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iPartyId<<vPartyName<<iPartyLevel<<iPartyMoney<<iPartyPolity
				 make_event_id(OSS_GAMESVR_USER_PARTYLEVELUP, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << nID << sName << nLvl << nMoney << nPolity
				);
}

void oss_party_addmem(unique_id_type operateId, unique_id_type charID, int nID, const StringType& sName, int nLvl, int nMoney, int nReason)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("PartyAddMember", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<iRoleId<<iPartyId<<vPartyName<<iPartyLevel<<iPartyMoney<<iReason
				 make_event_id(OSS_GAMESVR_USER_PARTYADDMEM, result) << get_world_id() << operateId << make_time(result) << charID
				 << nID << sName << nLvl << nMoney << nReason
				);
}

void oss_party_delmem(unique_id_type operateId, unique_id_type charID, int nID, const StringType& sName, int nLvl, int nMoney, int nReason)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("PartyDelMember", get_world_id(),
				 make_event_id(OSS_GAMESVR_USER_PARTYDELMEM, result) << get_world_id() << operateId << make_time(result) << charID
				 << nID << sName << nLvl << nMoney << nReason
				);
}
void oss_role_del_item(LC_ServerPlayer* player, LC_BackPackEntry* pack)
{
	return;
	CHECKNULLPTR(pack);
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("DestroyItem", get_world_id(),
				 make_event_id(OSS_GAMESVR_USER_DESTROYITEM, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << pack->GetBackPackValueByType(LPSFET_TYPE_ID) << pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) << 0 << pack->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) << make_item_uid(pack) << 0
				);
}


void oss_role_pickup(LC_ServerPlayer* player, LC_BackPackEntry* pack)
{
	return;
	CHECKNULLPTR(player);
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("PickupItem", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iMapId<<iPosX<<iPosY<<iPosZ<<iItemId
				 make_event_id(OSS_GAMESVR_USER_PICKITEM, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << player->GetMapResID() << player->GetCurrentLocation().x << player->GetCurrentLocation().y << player->GetCurrentLocation().z << pack
				);
}

void oss_role_compound_item(LC_ServerPlayer* player, LC_BackPackEntry* pack , GameLogic::PackOPRecordSet::PackOPRecords& list , int gp)
{
	return;
	CHECKNULLPTR(pack);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_COMPOUNDITEM, result);
	StringType iuuid = make_item_uid(pack);
	const char* tag = "CompoundItem";

	bool bAddItem = false;
	for (GameLogic::PackOPRecordSet::PackOPRecords::iterator i = list.begin(); i != list.end(); ++i)
	{
		bAddItem = true;
		LC_BackPackEntry* cost_item = &(*i).m_CloneEntry;
		OSS_LOG_INFO(tag, get_world_id(),
					 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
					 << pack->GetBackPackValueByType(LPSFET_TYPE_ID) << iuuid << 0 << cost_item << gp
					);
	}
	if(bAddItem == false)
	{
		OSS_LOG_INFO(tag, get_world_id(),
			eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
			<< pack->GetBackPackValueByType(LPSFET_TYPE_ID) << iuuid << 0 << NULL << gp
			);
	}
}

void oss_role_store(LC_ServerPlayer* player , LC_BackPackEntry* item)
{
	return;
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_STORE, result);
	OSS_LOG_INFO("Store", get_world_id(),
				 //<<iEventId<<iWorldId<<dtEventTime<<iUin<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iItemId<<iItemType<<iItemNum<<iItemGuid
				 eid << get_world_id(player) << make_time(result) << make_Account_string(player) << make_empty_ip_string(player) << player << item
				);
}

void oss_role_takeout(LC_ServerPlayer* player , LC_BackPackEntry* item)
{
	return;
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_TAKEOUT, result);
	OSS_LOG_INFO("TakeOut", get_world_id(),
				 //<<iEventId<<iWorldId<<dtEventTime<<iUin<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iItemId<<iItemType<<iItemNum<<iItemGuid
				 eid << get_world_id(player) << make_time(result) << make_Account_string(player) << make_empty_ip_string(player) << player << item
				);
}

void oss_role_trade_(const StringType& eid, const StringType& etime, LC_ServerPlayer* player, int64_t gp, LC_BackPackEntry* item , LC_ServerPlayer* player2 , LC_BackPackEntry* item2)
{
	CHECKNULLPTR(player);
	CHECKNULLPTR(player2);
	OSS_LOG_INFO("Trade", get_world_id(),
				 //<<iEventId<<iWorldId<<dtEventTime<<vClientIp<<iUin<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iSelfMoneyBeforeTrade<<iSelfMoneyAfterTrade<<iSelfItemId<<iSelfItemType<<iSelfItemNum<<iSelfItemGuid
				 //<<vOtherClientIp<<iOtherUin<<iOtherRoleId<<vOtherName<<iOtherJobId<<iOtherGender<<iOtherLevel
				 //<<iOtherMoneyBeforeTrade<<iOtherMoneyAfterTrade<<iOtherItemId<<iOtherItemType<<iOtherItemNum<<iOtherItemGuid<<iMoneyTransfer
				 eid << get_world_id(player) << etime << make_empty_ip_string(player) << make_Account_string(player) << player
				 << player->GetUnBindCash().GetMoney() - gp << player->GetUnBindCash().GetMoney() << item
				 << make_empty_ip_string(player2) << make_Account_string(player2) << player2
				 << (player2->GetUnBindCash().GetMoney() + gp) << player2->GetUnBindCash().GetMoney() << item2 << gp
				);
}

void oss_role_trade(LC_ServerPlayer* player, int64_t gp, GameLogic::LC_BackPackEntry items1[], int32_t itemssz1, LC_ServerPlayer* player2, int64_t gp2, GameLogic::LC_BackPackEntry items2[], int32_t itemssz2)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_TRADE, result);
	StringType etime = make_time(result);
	int64_t get_gp = gp2 - gp;

	bool hit = false;
	for (int i=0, j=0; i<itemssz1||j<itemssz2; ++i, ++j)
	{
		GameLogic::LC_BackPackEntry* pack1 = NULL;
		for (; i<itemssz1; ++i)
		{
			if (items1[i].GetBackPackValueByType(LPSFET_VALID))
			{
				pack1 = &items1[i];
				break;
			}
		}
		GameLogic::LC_BackPackEntry* pack2 = NULL;
		for (; j<itemssz2; ++j)
		{
			if (items2[j].GetBackPackValueByType(LPSFET_VALID))
			{
				pack2 = &items2[j];
				break;
			}
		}
		if (!hit || pack1 || pack2)
		{
			hit = true;
			oss_role_trade_(eid, etime, player, get_gp, pack1, player2, pack2);
		}
	}
}

void oss_role_npc_buy(LC_ServerPlayer* player, LC_BackPackEntry* pack , int point , int npcid, const StringType& npc_name , int is_buyback)
{
	return;
	CHECKNULLPTR(player);
	int64_t gp = player->GetUnBindCash().GetMoney();
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("NpcBuy", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<imoneybefore<<imoneyafter<<npcid<<npc_name<<iMapId<<iPosX<<iPosY<<iCostMoney<<iItemId<<iItemNum<<iItemGuid<<iItemType<<是否回购
				 make_event_id(OSS_GAMESVR_USER_NPCBUY, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << gp + point << gp << npcid << npc_name << player->GetMapResID() << player->GetCurrentLocation().x << player->GetCurrentLocation().y << point << pack << is_buyback
				);
}

void oss_role_npc_sell(LC_ServerPlayer* player, LC_BackPackEntry* pack , int point , int npcid, const StringType& npc_name)
{
	return;
	CHECKNULLPTR(player);
	int64_t gp = player->GetUnBindCash().GetMoney();
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("NpcSell", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<npcid<<npc_name<<iMapId<<iPosX<<iPosY<<imoneybefore<<imoneyafter<<iGetMoney<<iItemId
				 make_event_id(OSS_GAMESVR_USER_NPCSELL, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << npcid << npc_name << player->GetMapResID() << player->GetCurrentLocation().x << player->GetCurrentLocation().y << gp - point << gp << point << pack
				);
}

void oss_role_monster_die(LC_ServerPlayer* player , LC_ServerNPC* npc , int exp)
{
	return;
	CHECKNULLPTR(player);
	CHECKNULLPTR(npc);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_MONSTERDIE, result);
	CF_EXPList* pkCSVEXPList = CF_EXPList::GetSingletonPtr();
	CF_EXPList::DataEntry* pkEXPData = pkCSVEXPList->GetEntryPtr(npc->GetLevel());
	if (!pkEXPData)
	{
		return;
	}
	int lEXPBase = pkEXPData->_lBaseXP;

	OSS_LOG_INFO("MonsterDie", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iTeamFlag<<iGetExp<<iGetMoney<<iMonsterId<<vMonsterName<<iMonsterType<<iMonsterLevel<<iOrdinAttackNum<<iSkillAttackNum
				 //<<iOrdinAttackInjury<<iSkillAttackInjury<<iStandardPh<<iMonsterExp<<dtFirstAttackTime<<iMapId<<iPosX<<iPosY地图Y坐标<<iPosZ地图Z坐标
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << (player->GetPlayerGroupID() > 0 ? 0 : 1) << exp << 0 << npc->GetID() << npc->GetOwnerCharName() << npc->GetCharType() << npc->GetLevel() << 0 << 0
				 << 0 << 0 << npc->GetAttributeMap()->GetAttributeBase(ATT_MAX_HP) << lEXPBase << "" << npc->GetMapResID()
				 << npc->GetCurrentLocation().x << npc->GetCurrentLocation().y << npc->GetCurrentLocation().z
				);
}

void oss_role_monster_drop(LC_PlayerPtrList& players , LC_ServerNPC* npc , int gp , LC_BackPackEntryPtrList& items , int share)
{
	return;
	CHECKNULLPTR(npc);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_MONSTERDROP, result);
	//StringType etime = make_time(result);
	const char* tag = "MonsterDrop";
	for (LC_PlayerPtrList::iterator p = players.begin(); p != players.end(); ++p)
	{
		LC_ServerPlayer* player = *p;
		bool bAddItem = false;
		for (LC_BackPackEntryPtrList::iterator i = items.begin() ; i != items.end(); ++i)
		{
			bAddItem = true;
			LC_BackPackEntry* item = *i;
			OSS_LOG_INFO(tag, get_world_id(),
						 //<<iEventId<<iWorldId<<iMonsterId<<dtEventTime<<vClientIp<<iMonsterLevel<<iUin<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
						 //<<iDropMoney<<iItemId<<iItemType<<iItemNum<<iItemGuid<<iIsShare<<vMonsterName<<iMapId<<iPosX<<iPosY<<iPosZ<<iReason
						 eid << get_world_id(player) << npc->GetID() << make_time(result) << make_empty_ip_string(player) << npc->GetLevel() << make_Account_string(player) << player
						 << gp << item << share << npc->GetOwnerCharName() << npc->GetMapResID() << npc->GetCurrentLocation().x << npc->GetCurrentLocation().y << npc->GetCurrentLocation().z << 0
						);
		}
		if(bAddItem == false)
		{
			OSS_LOG_INFO(tag, get_world_id(),
				//<<iEventId<<iWorldId<<iMonsterId<<dtEventTime<<vClientIp<<iMonsterLevel<<iUin<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				//<<iDropMoney<<iItemId<<iItemType<<iItemNum<<iItemGuid<<iIsShare<<vMonsterName<<iMapId<<iPosX<<iPosY<<iPosZ<<iReason
				eid << get_world_id(player) << npc->GetID() << make_time(result) << make_empty_ip_string(player) << npc->GetLevel() << make_Account_string(player) << player
				<< gp << NULL << share << npc->GetOwnerCharName() << npc->GetMapResID() << npc->GetCurrentLocation().x << npc->GetCurrentLocation().y << npc->GetCurrentLocation().z << 0
				);
		}
	}
}

void oss_role_team(LC_ServerPlayer* player , int type)
{
	return;
	CHECKNULLPTR(player);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_TEAMOPT, result);
	OSS_LOG_INFO("Team", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iTeamId<<iEventType
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << player->GetPlayerGroupID() << type
				);
}

void oss_role_friend(LC_ServerPlayer* player , int type , const StringType& uid , unique_id_type cid , const StringType& cname , int job , int chartype , int level)
{
	return;
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_FRIENDOPT, result);
	OSS_LOG_INFO("Friend", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iEventType<<iOtherUin<<vOtherClientIp<<iOtherRoleId<<vOtherRoleName<<iOtherJobId<<iOtherGender<<iOtherRoleLevel
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << type << uid << "" << cid << cname << job << chartype << level
				);
}

void oss_role_send_mail(LC_ServerPlayer* player , const StringType& uid , unique_id_type cid , const StringType& cname , const StringType& title , const StringType& Context , int cost , int gp , int pay , LC_BackPackEntryPtrList& items)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_SENDMAIL, result);
	StringType etime = make_time(result);
	const char* tag = "SendMail";
	bool bAddItem = false;
	for (LC_BackPackEntryPtrList::iterator i = items.begin() ; i != items.end(); ++i)
	{
		LC_BackPackEntry* item = *i;
		if (NULL == item)
		{
			continue;
		}
		bAddItem = true;
		OSS_LOG_INFO(tag, get_world_id(),
					 eid << get_world_id(player) << make_Account_string(player) << etime << make_empty_ip_string(player) << player
					 << cid << 1 << cname << title << Context << gp << item << pay << cost
					);
	}
	if(bAddItem == false)
	{
		OSS_LOG_INFO(tag, get_world_id(),
			eid << get_world_id(player) << make_Account_string(player) << etime << make_empty_ip_string(player) << player
			<< cid << 1 << cname << title << Context << gp << (LC_BackPackEntry*)NULL << pay << cost
			);
	}
}

void oss_role_send_mail(const unique_id_impl& charid, const std::string& title, const std::string& Context, int cost, int gp, int pay, PS_ItemAssetEntry* items, int32_t count)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_SENDMAIL, result);
	StringType etime = make_time(result);
	const char* tag = "SendMail";
	bool bAddItem = false;
	static unique_id_impl senderid = GET_SYSTEM_ID();

	if (items && count>0)
	{
		for (int i = 0; i<count&&i<MAIL_ITEM_AFFIX_NUM; ++i)
		{
			PS_ItemAssetEntry& item = items[i];
			if (!item.GetItemBaseData_Valid())
			{
				continue;
			}
			bAddItem = true;
			OSS_LOG_INFO(tag, get_world_id(),
				eid << get_world_id() << senderid.detail() << etime << "" << oss_player_charid(senderid)
				<< charid << 1 << "undef" << title << Context << gp << &item << pay << cost
				);
		}
	}
	if(bAddItem == false)
	{
		OSS_LOG_INFO(tag, get_world_id(),
			eid << get_world_id() << senderid.detail() << etime << "" << oss_player_charid(senderid)
			<< charid << 1 << "undef" << title << Context << gp << (PS_ItemAssetEntry*)NULL << pay << cost
			);
	}
}

void oss_role_recv_mail(LC_ServerPlayer* player , const StringType& send_name, const StringType& title ,
						const StringType& Context , int pay , int gp , int delta, LC_BackPackEntry* item)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_RECVMAIL, result);
	OSS_LOG_INFO("RcvMail", get_world_id(),
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player)
				 << oss_player_charid((player ? player->GetInstance() : null_unique_impl))
				 << send_name << title << Context << pay << item << gp << delta
				);
}

void oss_role_enchase_equip(LC_ServerPlayer* player , LC_BackPackEntry* item , LC_BackPackEntry* socket_item)
{
	return;
	CHECKNULLPTR(item);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_ITEMENCHASE, result);
	OSS_LOG_INFO("EnchaseEquip", get_world_id(),
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << make_item_uid(item) << item->GetBackPackValueByType(LPSFET_TYPE_ID) << make_item_uid(socket_item) << socket_item->GetBackPackValueByType(LPSFET_VALID)
				);
}

void oss_role_pull_equip(LC_ServerPlayer* player , LC_BackPackEntry* item , LC_BackPackEntry* socket_item , int gp)
{
	return;
	CHECKNULLPTR(item);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_PULLEQUIP, result);
	OSS_LOG_INFO("PullEquip", get_world_id(),
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << make_item_uid(item) << item->GetBackPackValueByType(LPSFET_TYPE_ID) << gp << make_item_uid(socket_item) << socket_item->GetBackPackValueByType(LPSFET_VALID)
				);
}

void oss_role_dur_equip(LC_ServerPlayer* player , LC_BackPackEntry* item , int type , int dur , int gp)
{
	return;
	CHECKNULLPTR(item);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_DUREQUIP, result);
	OSS_LOG_INFO("DurabilityEquip", get_world_id(),
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << make_item_uid(item) << item->GetBackPackValueByType(LPSFET_TYPE_ID) << type << dur << gp
				);
}

void oss_role_decompund_equip(LC_ServerPlayer* player, LC_BackPackEntryPtrList& nitems, GameLogic::PackOPRecordSet::PackOPRecords& ditems)
{
	return;
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_EQUIPDECOMP, result);
	StringType etime = make_time(result);
	const char* tag = "DecompoundEquip";

	for (LC_BackPackEntryPtrList::iterator i = nitems.begin(); i != nitems.end(); ++i)
	{
		LC_BackPackEntry* gitem = *i;
		for (GameLogic::PackOPRecordSet::PackOPRecords::iterator i = ditems.begin(); i != ditems.end(); ++i)
		{
			LC_BackPackEntry* ditem = &(*i).m_CloneEntry;
			OSS_LOG_INFO(tag, get_world_id(),
						 eid << get_world_id(player) << make_Account_string(player) << etime << make_empty_ip_string(player) << player
						 << make_item_uid(ditem) << ditem->GetBackPackValueByType(LPSFET_TYPE_ID) << make_item_uid(gitem) << gitem->GetBackPackValueByType(LPSFET_VALID)
						);
		}

	}
}

void oss_role_change_equip(LC_ServerPlayer* player , int on_id , int off_id)
{
	return;
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_EQUIPCHANGE, result);
	OSS_LOG_INFO("EquipChange", get_world_id(),
				 //<<iEventId<<iWorldId<<dtEventTime<<iuin<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iEquipWareId<<iEquipTakeOffId
				 eid << get_world_id(player) << make_time(result) << make_Account_string(player) << make_empty_ip_string(player) << player << on_id << off_id
				);
}


void oss_role_vehicle_up(LC_ServerPlayer* player, int32_t iUpType, int32_t iSubType, int32_t iPreLevel, int32_t iPostLevel, int64_t iCash, LC_BackPackEntry* item, int32_t res, int32_t iBPDelta, int32_t iUpCount, int32_t iSkillID)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_VEHICLEUP, result);
	OSS_LOG_INFO("RefinedUpgrade", get_world_id(),
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player)
				 << oss_player_charid((player ? player->GetInstance() : null_unique_impl))
				 << iUpType << iSubType << iPreLevel << iPostLevel << iCash << item << res << iBPDelta << iUpCount << iSkillID
				);
}

void oss_role_item_refine(LC_ServerPlayer* player, int opType, int opParam, LC_BackPackEntry* aitem, LC_BackPackEntry* bitem, int pre, int post)
{
	CHECKNULLPTR(player);
	CHECKNULLPTR(aitem);

	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_EQUIPREFINE, result);
	OSS_LOG_INFO("ItemRefine", get_world_id(),
				 eid << get_world_id(player) << make_time(result) << make_empty_ip_string(player)  << make_Account_string(player)
				 << oss_player_charid((player ? player->GetInstance() : null_unique_impl)) << opType << opParam
				 << make_item_uid(aitem) << aitem->GetBackPackValueByType(LPSFET_TYPE_ID) << make_item_uid(bitem) << (bitem ? bitem->GetBackPackValueByType(LPSFET_VALID) : 0)
				 << pre << post;
				);
}


void oss_role_arena(LC_ServerPlayer* player , int rank , LC_ServerPlayer* player2 , int rank2 , int res , attr_value_type exp , MoneyType gp)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_AREA, result);
	OSS_LOG_INFO("Arena", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iRank<<iTargetUin<<vTargetClientIp<<iTargetRoleId<<vTargetRoleName<<iTargetJobId<<iTargetGender<<iTargetRoleLevel
				 //<<iTargetRank<<iResult<<iGetEXP<<iGetMoney
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << rank << make_Account_string(player2) << make_empty_ip_string(player2) << player2
				 << rank2 << res << exp << gp
				);
}

void oss_role_arena_reward(LC_ServerPlayer* player , int rank , attr_value_type exp , MoneyType gp)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_AREAREWARD, result);
	OSS_LOG_INFO("ArenaReward", get_world_id(),
				 //<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iRank<<iGetEXP<<iGetMoney
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << rank << exp << gp
				);
}

void oss_role_copy_enter(LC_ServerPlayer* player, int nMapResId)
{
	CHECKNULLPTR(player);
	CF_WorldMapList::DataEntry* entry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(nMapResId);
	if(entry == NULL || entry->_iMapLogicType < MT_TRANSCRIPTION_BEGIN || entry->_iMapLogicType > MT_TRANSCRIPTION_END)
	{
		return;
	}
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_COPYENTER, result);
	OSS_LOG_INFO("CopyEnter", get_world_id(),
				 //<<iEventId<<iworldid<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //iTeamId所属队伍ID<<iCopyID副本ID<<iCopyType副本类型<<iCoypName副本名称
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << player->GetPlayerGroupID() << nMapResId << entry->_iMapLogicType << ""
				);
}

void oss_role_copy_exit(LC_ServerPlayer* player , int score , int gp , int exp , int sp , LC_BackPackEntryPtrList& items)
{
	CHECKNULLPTR(player);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_COPYEXIT, result);
	const char* tag = "CopyExit";
	if (player->GetMap() == NULL)
	{
		return;
	}
	//CF_WorldMapList::DataEntry* entry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(player->GetMapResID());
	bool bAddItem = false;
	for (LC_BackPackEntryPtrList::iterator i = items.begin() ; i != items.end(); ++i)
	{
		LC_BackPackEntry* item = *i;
		bAddItem = true;
		OSS_LOG_INFO(tag, get_world_id(),
					 //<<iEventId<<iworldid<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
					 //<<iTeamId<<iCopyID<<iCoypName<<iGrade<<iGetEXP<<iGetMoney<<iGetPointExp<<iItemId<<iItemType<<iItemNum<<iItemGuid
					 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
					 << player->GetPlayerGroupID() << player->GetMapResID() << "" << score << exp << gp << sp << item
					);
	}
	if(bAddItem == false)
	{
		OSS_LOG_INFO(tag, get_world_id(),
			//<<iEventId<<iworldid<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
			//<<iTeamId<<iCopyID<<iCoypName<<iGrade<<iGetEXP<<iGetMoney<<iGetPointExp<<iItemId<<iItemType<<iItemNum<<iItemGuid
			eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
			<< player->GetPlayerGroupID() << player->GetMapResID() << "" << score << exp << gp << sp << NULL
			);
	}
}

void oss_role_counter_attack(LC_ServerPlayer* player , LC_ServerNPC* npc , int damage , int exp)
{
	CHECKNULLPTR(player);
	CHECKNULLPTR(npc);
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_COUNTERATTACK, result);
	OSS_LOG_INFO("CounterAttack", get_world_id(),
				 //<<iEventId<<dtEventTime<<iUin<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iMonsterId<<vMonsterName<<iMonsterLevel<<iMapId<<iPosX<<iPosY<<iPosZ<<iDamage<<iGetEXP
				 eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << npc->GetCharType() << npc->GetOwnerCharName() << npc->GetLevel() << player->GetMapResID()
				 << player->GetCurrentLocation().x << player->GetCurrentLocation().y << player->GetCurrentLocation().z << damage << exp
				);
}

StringType oss_role_moneyflow(LC_ServerPlayer* player, int64_t nBefore, int64_t nAfter, int64_t nChange, int nMoneyType, int nAction,int nFlowType, uint64_t nSrcID)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_MONEYFLOW, result);
	OSS_LOG_INFO_RET("MoneyFlow", get_world_id(),
		eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << oss_player_charid((player ? player->GetInstance() : null_unique_impl))
		<< nBefore << nAfter << nChange << nMoneyType << nAction << nFlowType << nSrcID
		);
}

// void oss_role_goodsflow(LC_ServerPlayer* player, int nOperate, GameLogic::LC_BackPackEntry * item, int identifier)
// {
// 	DEF_LOCAL_TIME(result);
// 	StringType eid = make_event_id(OSS_GAMESVR_USER_GOODSFLOW, result);
// 	//<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
// 	//<<iOperate<<iItemId<<iItemType<<iItemNum<<iItemGuid<<iIdentifier
// 	OSS_LOG_INFO("GoodsFlow", get_world_id(),
// 		eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
// 		<< nOperate << item << identifier
// 		);
// }

StringType oss_role_produceitem(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry * item, int nProductType, uint64_t nProductDetail, int nBfNum, int nEdNum, int iItemCostMoney)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_PRODUCEITEM, result);
	//<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
	//<<iProductType<<iItemId<<iItemType<<iItemNum<<iItemGuid<<iBeforeNum<<iAfterNum<<iItemCostMoney
	OSS_LOG_INFO_RET("ProduceItem", get_world_id(),
		eid << get_world_id(player) << make_Account_string(player) << make_time(result) << "" << oss_player_charid((player ? player->GetInstance() : null_unique_impl))
		<< nProductType << item << nBfNum << nEdNum << iItemCostMoney << nProductDetail << (item ? item->MakeItemAttrSummary() : StringType(""))
		);
}

StringType oss_role_consumeitem(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry * item, int nConsumeType, uint64_t nConsumeDetail, int nBfNum, int nEdNum, int iItemCostMoney)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_CONSUMEITEM, result);
	//<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
	//<<iConsumeType<<iItemId<<iItemType<<iItemNum<<iItemGuid<<iBeforeNum<<iAfterNum<<iItemCostMoney
	OSS_LOG_INFO_RET("ConsumeItem", get_world_id(),
		eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << oss_player_charid((player ? player->GetInstance() : null_unique_impl))
		<< nConsumeType<< item << nBfNum << nEdNum << iItemCostMoney << nConsumeDetail << (item ? item->MakeItemAttrSummary() : StringType(""))
		);
}
StringType oss_role_sendchat(LC_ServerPlayer* player, int nChannel, const StringType& strMessage, unique_id_type nTargetId, const StringType& strTargetName)
{
	DEF_LOCAL_TIME(result);
	StringType eid = make_event_id(OSS_GAMESVR_USER_CHATLOG, result);
	//<<iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
	//<<iChatChannel<<vChatMsg<<iTargetId<<vTargetName
	
	OSS_LOG_INFO_RET("ChatLog", get_world_id(),
		eid << get_world_id(player) << make_Account_string(player) << make_time(result) << make_ip_string(player) << oss_gbk_player(player)
		<< nChannel << strMessage << nTargetId << strTargetName << (player ? player->GetGuildID() : 0) << (player ? player->GetUserID() : 0)
		);
}
//////////////////////////////////////////////////////////////////////////
void oss_shop_buy(LC_ServerPlayer* player, int64_t yb, int64_t lijin, int itemid , int count)
{
	CHECKNULLPTR(player);
	
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("Shop", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iTradeTb1<<iTradeTb2<<iItemGuid<<iItemId<<iItemNum
				 make_event_id(OSS_GAMESVR_USER_SHOPBUY, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player) << player
				 << yb << lijin << "" << itemid << count
				);
}
//////////////////////////////////////////////////////////////////////////
void oss_act_shop_buy(LC_ServerPlayer* player, int32_t type, int32_t key, const StringType& sRewards, const StringType& sConsumes, const StringType& sCond, int32_t iDrawCount)
{
	CHECKNULLPTR(player);
	
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("ActShop", get_world_id(),
				 //iEventId<<iWorldId<<iUin<<dtEventTime<<vClientIp<<iRoleId<<vRoleName<<iJobId<<iGender<<iRoleLevel
				 //<<iType<<iKey<<sRewards<<sConsumes<<sConditions
				 make_event_id(OSS_GAMESVR_USER_ACT_SHOPBUY, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << make_empty_ip_string(player)
				 << player << type << key << sRewards << sConsumes << sCond << iDrawCount
				);
}
//////////////////////////////////////////////////////////////////////////
void oss_jump(LC_ServerPlayer* player, int32_t skilltypeid)
{
	/*
	CHECKNULLPTR(player);

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("Jump", get_world_id(),
		make_event_id(OSS_GAMESVR_USER_JUMP, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << player << skilltypeid
		);
	*/
}

void oss_role_common_stat(LC_ServerPlayer* player, int32_t type, int32_t key, uint32_t opcode, int32_t oppara)
{
	CHECKNULLPTR(player);

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("CommonStat", get_world_id(),
		make_event_id(OSS_GAMESVR_USER_COMMONSTAT, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << player
		<< type << key << opcode << oppara
		);
}

void oss_role_combat_score(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);

	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("CombatScore", get_world_id(),
		make_event_id(OSS_GAMESVR_USER_COMBATSCORE, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << player
		//<< player->GetTotalScore()
		<< player->GetControllerCombatScore()
		);
}

void oss_role_gm_command(LC_ServerPlayer* player, int64_t msgid, const StringType& cmd, const StringType& full, uint32_t rslt)
{
	DEF_LOCAL_TIME(result);
	StringType final_full;
	UT_BaseStringUtility::ReplaceAll(full, "|", "$", final_full);
	OSS_LOG_INFO("GMCommand", get_world_id(),
		make_event_id(OSS_GAMESVR_USER_GMCOMMAND, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << player
		<< (player?player->GetGMUser():"") << (player?player->GetGMPriority():INIT_GM_PRIORITY) << msgid << cmd << final_full << rslt;
		);
}

void oss_daily_data_report(const data_report& kDataReport)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("DailyDataReport", get_world_id(),
		make_event_id(OSS_GAMESVR_DAILY_DATA_REPORT, result) << get_world_id() << kDataReport.start_day << kDataReport.conflate_day
		<< kDataReport.recharge << kDataReport.bonus << kDataReport.yb_consume << kDataReport.active << kDataReport.max_online << kDataReport.avg_online
		<< kDataReport.recharge_role << kDataReport.create_char << kDataReport.world_level << kDataReport.yb_deposit);
}

void oss_resource_alert(LC_ServerPlayer* player, int32_t iTypeID, uint64_t iTotalCount, int64_t iLimit, uint32_t iAlertTime, const StringType& sSrcCount)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("ResourceAlert", get_world_id(),
		make_event_id(OSS_GAMESVR_USER_RESOURCE_ALERT, result) << get_world_id(player) << make_Account_string(player) << make_time(result) << player
		<< iTypeID << iTotalCount << iLimit << iAlertTime << sSrcCount
		);
}

//LoginStep
void oss_login_step(int server, const StringType& uid, int acttype, uint16_t platformid, int rslt)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("LoginStep", server,
		make_event_id(OSS_GAMESVR_USER_LOGINSTEP, result) << server << uid << make_time(result) 
		<< "mac" << "na" << "centos" << "linux" << 0 << 0
		<< acttype << 0 << platformid << rslt
		);
}
//
void oss_server_info(int server, uint32_t starttime, uint32_t conflatetime)
{
	DEF_LOCAL_TIME(result);
	OSS_LOG_INFO("ServerInfo", server,
		make_event_id(OSS_GAMESVR_SERVERINFO, result) << get_world_id() << make_time(result) << server << starttime << conflatetime
		);
}
StringType encode_items(LC_BackPackEntryPtrList& items)
{
	if (items.empty())
	{
		return "";
	}
	StringStreamType ss;
	ss << "id:222,";
	for (LC_BackPackEntryPtrList::iterator i = items.begin(); i != items.end(); ++i)
	{
		LC_BackPackEntry* item = *i;
		if (NULL == item)
		{
			continue;
		}
		if (!item->GetBackPackValueByType(LPSFET_VALID))
		{
			continue;
		}
		ss << "goodid:" << item->GetBackPackValueByType(LPSFET_TYPE_ID) << ",goodtype:" << 1 << ",num:" << item->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) << ";";
	}

	using namespace boost::archive::iterators;
	StringType res = ss.str();
	ss.str("");
	ss << Base64Encode(res);
	return ss.str();
}

void make_new_item_info(LC_BackPackEntryPtrList& new_items , LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);
	PackOPRecordSet* record_set = player->GetPackAsset()->GetRecordSet();
	for (PackOPRecordSet::PackOPRecords::iterator i = record_set->m_Add_Items.begin(); i != record_set->m_Add_Items.end(); ++i)
	{
		PackEntryOPRecord& rd = *i;
		LC_BackPackEntry* pack = rd.m_Entry;
		if (pack != NULL)
		{
			new_items.push_back(pack);
		}
	}
}
