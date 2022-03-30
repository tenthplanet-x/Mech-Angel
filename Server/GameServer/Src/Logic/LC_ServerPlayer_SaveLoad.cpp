#include "zlib.h"
#include "LM_LocaleManager.h"
#include "LC_ServerPlayer.h"
#include "LC_Define.h"
#include "NW_Helper.h"
#include "CF_WorldMapList.h"
#include "LC_ServerTaskMap.h"
#include "UT_Timer.h"
#include "LC_ServerWarManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_ServerTaskManager.h"
#include "LC_ServerPackEntryFactory.h"
#include "CF_CreateCharParam.h"
#include "LC_ServerLogicManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_GameStory_Manager.h"
#include "LC_ShortcutBar.h"
#include "LC_FashionAsset.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "CF_Buddy.h"

#include "char_data.pb.h"
#include "ConfigManager.h"
#include "oss_define.h"
#include "string-util.h"

#include "LC_ServerWorldManager.h"
#include "../Buddy/BuddyInstance.h"
#include "../Buddy/BuddyChain.h"
#include "CF_CollectionReward.h"
#include "CF_CollectionActivation.h"
#include "CF_SkillBookUnlock.h"
#include "oss_internal_define.h"
#include "oss_360_define.h"
#include "md5plusplus.h"
using namespace GameLogic;
using namespace StateMachine;
using namespace Protocol;
using namespace CSVFile;
using namespace Utility;
using namespace Skill;


//#define		SQL_SIZE_ANALYZE		1

static const int32_t gsnDynamicMsgBuffSize = 	DYNAMIC_MSG_BUFF_SIZE;
//static const int32_t gsnDynamicMsgBuffSize = 	0x00FF;
//#define		SKILL_CD_NEW_IMPL

static bool CheckDuplicatedBuddy( int32_t * nDataArray, int32_t nData )
{
	for ( int i = 0; i < MAX_ACTIVE_BUDDY_NUM; i++ )
	{
		if ( nData == nDataArray[i] )
			return true;
	}
	return false;
}

const int32_t DATA_STAT_SKILL_MAX_COUNT = 16;
const skill_id_type SKILL_IDS_FOR_DATA_STAT[DATA_STAT_SKILL_MAX_COUNT] = {
	21718001, 21711001, 25803001, 25804001, 26002001, 25902001, 25401001, 21722001,
	21720001, 21719001, 25703001, 25704001, 25707001, 25716001, 25713001, 25712001
};

void LC_ServerPlayer::_Learn_openAblitySkill(int SkillID)
{
	if (IS_SKILL_TYPE(SkillID))
	{
		int32_t iSlotIndex = -1;
		if (LearnSkill(SkillID, iSlotIndex) == false)
		{
			return;
		}
	}
}

static void init_char_data(LC_ServerPlayer* player , int version)
{
	if (version < 1)
	{
		CF_CreateCharParam::DataEntry* entry = SafeGetCSVFileDataEntryPtr<CF_CreateCharParam>(1);
		if (NULL == entry)
			return;
		LC_PackAsset* pkPackAsset = player->GetPackAsset();
		if (NULL == pkPackAsset)
			return;

		PackOperateInfoList objOPList;
		objOPList.clear();

		LC_PackEntryFactory* pkPackEntryFactory = LC_ServerPackEntryFactory::GetSingletonPtr();
		LC_BackPackEntryList rkBackPackEntryList;

		int item_id[3] = {entry->_WareHouseItemID1, entry->_WareHouseItemID2, entry->_WareHouseItemID3};
		int item_count[3] = {entry->_WareHouseItemCount1, entry->_WareHouseItemCount2, entry->_WareHouseItemCount3};

		for (int i = 0 ; i < 3 ; ++i)
		{
			if (0 >= item_id[i] || 0 >= item_count[i])
			{
				continue;
			}
			PackOperateInfo objData;
			objData.Reset();

			objData.m_iOPType = OPT_ADD_ITEM;
			objData.m_iPackType = PACK_TYPE_WAREHOUSE;
			objData.m_bOPEntry = false;
			pkPackEntryFactory->CreateItemEntry(item_id[i], item_count[i], CIET_NONE, rkBackPackEntryList);
			objData.m_kPackEntry.Clone(rkBackPackEntryList[i]);
			objData.m_kPackEntry.SetBackPackValueByType(LPSFET_BINDMODE, true);
			objData.m_lItemID   = item_id[i];
			objData.m_bMerger = true;
			objData.m_lCount    = 1;
			objOPList.push_back(objData);
		}

		pkPackAsset->ExecutePackOP(objOPList, true);
		player->SyncBackPackToClient(PACK_TYPE_BASIC);
		player->OnInitBuddyTemplateId();
		player->LoadBuddyData_Locked();
		player->SetOwnerMaxMP(GetGlobalSetting.nMaxOwnerMP);
		player->SetOwnerMP(GetGlobalSetting.nInitOwnerMP);
		//oss_360_log_game_enter(player);
		oss_internal_log_create_role(player);
		oss_360_log_create_role(player);
	}
}

void test_reply(int32_t err, bool& ret)
{
	if (err)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RegisterCharInfo Err:%d Ret:%d", err, int(ret));
	}
}

void LC_ServerPlayer::RegisterCharInfo()
{
	GameServerApp* pServerApp =  GetGameServerApp();
	if(pServerApp == NULL)
	{
		return;
	}

	Utility::Redis_Handler* pkRedisHandler = pServerApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return;
	}

	std::stringstream ss;
	uint32_t ip = htonl(GetLoginIP());
	const char* sex = (GetCharType() == 1) ? "\"m\"" : "\"f\"";
	ss << "{\"nickname\":\"" << GetOwnerCharName_utf8()
	   << "\",\"charid\":" << GetInstance()
	   << ",\"userid\":\"" << GetUserName().c_str()
	   << "\",\"accountid\":\"" << GetUserID()
	   << "\",\"sex\":" << sex
	   << ",\"last_login\":" << time(0)
	   << ",\"loginlong\":" << m_TotalOnlineTime
	   << ",\"group\":\"" << GetGuildName_utf8()
	   << "\",\"level\":" << GetLevel()
	   << ",\"is_valid\":1"
	   << ",\"exp\":" << GetEXP()
	   << ",\"createtime\":" << GetRegTime()
	   << ",\"ip\":\"" << inet_ntoa(*(struct in_addr*)(&(ip)))
	   << "\",\"totalscore\":" << GetControllerCombatScore()
	   << ",\"vigor\":" << GetVigor()
	   << ",\"yb\":" << GetUnBindYuanBao().GetMoney()
	   << ",\"lj\":" << GetBindYuanBao().GetMoney()
	   << ",\"jb\":" << GetUnBindCash().GetMoney()
	   << ",\"zs\":" << GetGameStoryAsset().GetScore(GS_SCR_TP_ROLL)
	   << ",\"bhb\":" << GetGuildMoney()
	   << ",\"yw\":" << GetCouragePoint()
	   << ",\"ly\":" << GetGameStoryAsset().GetScore(GS_SCR_TP_TRIALS)
	   << ",\"vip\":" << GetVIPLevel()
	   << ",\"jy\":" << GetGagTime()
	   << ",\"internal\":" << GetInternal()
	   << ",\"freezeAcc\":" << GetFreezeAcc()
	   << ",\"freezeRol\":" << GetFreezeRol()
	   << ",\"online\":" << !IsOffline()
	   << ",\"recharge\":" << GetVIPAsset().GetCredit()
	   << ",\"consume\":" << GetVIPAsset().GetCreditConsume()
	   << ",\"last_recharge_time\":" << GetLastRechargeTime();

	const RaidInfo* pkRaid = GetRaidAsset()->GetRaidCatTop(RD_CAT_TP_MJBOSS);
	int32_t iBossRecord = 0;
	if (pkRaid)
	{
		iBossRecord = (pkRaid->m_iRaidId - 7000) * 1000 + pkRaid->m_iClearRecord;
	}
	ss << ",\"boss\":" << iBossRecord;

	
	int32_t iPriviSilver = 0;
	LC_ActorStateEntry* pkStateEntry = m_kSkillStateMap->GetStateEntryByID(PRIVI_SILVER);
	iPriviSilver = (NULL != pkStateEntry) ? 1 : 0;
	ss << ",\"silver\":" << iPriviSilver;

	int32_t iPriviGolden = 0;
	pkStateEntry = m_kSkillStateMap->GetStateEntryByID(PRIVI_GOLDEN);
	iPriviGolden = (NULL != pkStateEntry) ? 1 : 0;
	ss << ",\"golden\":" << iPriviGolden;

	bool first = true;
	ss << ",\"sub\":[";
	for (int i = LC_SUB_TP_BEGIN; i < LC_SUB_TP_END; ++i)
	{
		int32_t iLevel = GetSubordinateStage(i);
		if (iLevel <= 0)
		{
			continue;
		}
		if (!first)
		{
			ss << ",";
		}
		first = false;
		ss << "\"" << i << "-" << iLevel << "\"";
	}
	for (int i = LC_SUB_TP_ARTIFACT_BEGIN; i < LC_SUB_TP_ARTIFACT_END; ++i)
	{
		int32_t iLevel = GetSubordinateStage(i);
		if (iLevel <= 0)
		{
			continue;
		}
		ss << ",\"" << i << "-" << iLevel << "\"";
	}
	ss << "],\"transform\":[";
	first = true;
	LC_SubUpgradeDataMap subs = GetSubordinateAsset().GetSubDataMap();
	for (LC_SubUpgradeDataMap::iterator it = subs.begin(); it != subs.end(); ++it)
	{
		LC_SubUpgradeData::TransformMap trans = it->second.GetTransformMap();
		LC_SubUpgradeData::TransformMap::iterator sit = trans.begin();
		for (; sit != trans.end(); ++sit)
		{
			if (!first)
			{
				ss << ",";
			}
			first = false;
			ss << sit->first;
		}
	}
	ss << "],\"title\":[";
	first = true;
	LC_TitleAsset::SystemTitleIDList& titleList = GetTitleAsset()->GetSystemTitleList();
	for (LC_TitleAsset::SystemTitleIDList::iterator it = titleList.begin(); it != titleList.end(); ++it)
	{
		if (!first)
		{
			ss << ",";
		}
		first = false;
		ss << *it;
	}
	first = true;
	ss << "],\"fashion\":[";
	mem::map<uint32_t, GameLogic::ST_Fashion>& fashions = GetFashionBagAsset().GetFashions();
	for (mem::map<uint32_t, GameLogic::ST_Fashion>::iterator it = fashions.begin(); it != fashions.end(); ++it)
	{
		if (!first)
		{
			ss << ",";
		}
		first = false;
		ss << it->first;
	}

	first = true;
	ss << "],\"grade\":[";
	LC_PlayerBase::LC_PlayerEquipSuitPtrMap& kPlayerEquipSuitMap = GetPlayerEquipSuitMap();
	for (int32_t i = ITEM_SUIT_FUNC_ID_GRADE_BEGIN; i < ITEM_SUIT_FUNC_ID_GRADE_MAX; ++i)
	{
		LC_PlayerBase::LC_PlayerEquipSuitPtrMapIter it = kPlayerEquipSuitMap.find(i);
		if (it != kPlayerEquipSuitMap.end())
		{
			if (!first)
			{
				ss << ",";
			}
			first = false;
			int32_t iGrade = it->first % ITEM_SUIT_FUNC_ID_GRADE_MASK;
			ss << "\"" << it->first << "-" << it->second << "\"";
		}
	}

	first = true;
	ss << "], \"starsRune\":{";
	LC_StarsRuneAsset& kRuneAsset = GetStarsRuneAsset();
	const StarsRuneInfo& kRuneInfos = kRuneAsset.GetStarsRuneMap();
	for (StarsRuneInfo::const_iterator cit = kRuneInfos.begin(); cit != kRuneInfos.end(); ++cit)
	{
		if (!first)
		{
			ss << ",";
		}
		first = false;
		const StarsRuneIndexKey& key = cit->first;
		ss << "\"" << key._iCategory << "_" << key._iIndex << "\":" << cit->second;
	}

	first = true;
	ss << "}, \"gemReinforce\":{";
	LC_GemReinforceAsset& kGemAsset = GetGemReinforceAsset();
	for (int32_t i = ET_BEGIN; i < ET_MAX_COUNT; ++i)
	{
		LC_GemReinforceData* pkGemData = kGemAsset.GetGemReinforceData((EquipType)i);
		if (NULL == pkGemData)
		{
			continue;
		}
		if (pkGemData->GetLevel() < 1)
		{
			continue;
		}
		if (!first)
		{
			ss << ",";
		}
		first = false;
		ss << "\"" << i << "\":" << pkGemData->GetLevel();
	}

	first = true;
	ss << "}, \"skillBook\":{";
	const SkillBookDataMap& kSkillBookMap = GetSkillBookAsset().GetSkillBookDatas();
	for (SkillBookDataMap::const_iterator cit = kSkillBookMap.begin(); cit != kSkillBookMap.end(); ++cit)
	{
		const LC_SkillBookData& kSkillBook = cit->second;
		if (kSkillBook.GetLevel() < 1)
		{
			continue;
		}
		if (!first)
		{
			ss << ",";
		}
		first = false;
		ss << "\"" << kSkillBook.GetID() << "\":" << kSkillBook.GetLevel();
	}

	first = true;
	ss << "}, \"skill\":{";
	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	if (NULL != pkSkillAsset)
	{
		for (int32_t i = 0; i < DATA_STAT_SKILL_MAX_COUNT; ++i)
		{
			skill_id_type iSkillTypeId = SKILL_IDS_FOR_DATA_STAT[i];
			int32_t iLevel = pkSkillAsset->GetSkillLevel(iSkillTypeId);
			if (iLevel <= 0)
			{
				continue;
			}
			if (!first)
			{
				ss << ",";
			}
			first = false;
			ss << "\"" << iSkillTypeId << "\":" << iLevel;
		}
	}
	ss << "}}";

	std::stringstream ssIds;
	ssIds << "{\"charid\":" << GetInstance()
		  << ",\"userid\":\"" <<  GetUserName().c_str()
		  << "\",\"accountid\":" << GetUserID()
		  << "}";
	
	Utility::Redis_Channel& kRedisChannel = pServerApp->GetRedisChannel();

	int32_t ret = 0;
	int32_t nServerID = GetCitizenship();
	std::string strUserName = TPSTR2STDSTR(m_kUserName);
	char keyRedis[512] = {0};

	sprintf_s(keyRedis, sizeof(keyRedis), "%s_%d", pServerApp->GetGameUserInfoKey().c_str(), nServerID);
	ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::hset, test_reply, keyRedis, strUserName, ss.str());

	sprintf_s(keyRedis, sizeof(keyRedis), "%s_%d", pServerApp->GetGameAccountIDsKey().c_str(), nServerID);
	ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::hset, test_reply, keyRedis, strUserName, ssIds.str());

	{
		json_op<Utility::gm_cmd_infosort> json_resp;
		json_resp.json.serverid = nServerID;
		json_resp.json.key = strUserName;
		ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::publish, (void(*)(int32_t, int64_t&))NULL, pServerApp->GetRedisGMSortChannel(), json_resp);
	}

	sprintf_s(keyRedis, sizeof(keyRedis), "%s_%d" , pServerApp->GetGameNameInfoKey().c_str(), nServerID);
	ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::hset, test_reply, keyRedis, TPSTR2STDSTR(m_kCharName_utf8), strUserName);

	sprintf_s(keyRedis, sizeof(keyRedis), "%s_%d" , pServerApp->GetGameChar2UserKey().c_str(), nServerID);
	ASYNC_REDIS_REQUEST(ret, kRedisChannel, &redispp::Connection::hset, test_reply, keyRedis, toString(GetInstance()), strUserName);

	//战斗力
	oss_role_combat_score(this);
	SetCharInfoDirty(false);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::WriteCharbaseTest(int32_t size)
{
	int i = 0;

	char* szBuf = (char*)malloc(size);
	memset( szBuf, 'a', size );
	szBuf[size - 1] = '\0';
	m_nTest = szBuf;

	free(szBuf);

	SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
}

void LC_ServerPlayer::SetBaseDataStr( int32_t nIndex, const StringType& strInput )
{
	m_nTest = strInput.c_str();
}

void LC_ServerPlayer::EchoBaseDataStr( int32_t nIndex)
{
	std::stringstream strOutput;
	strOutput<<"Str: Idx: " << nIndex << " Value: " << m_nTest;
	SendNotifyStrToClient(strOutput.str().c_str());
}

void LC_ServerPlayer::SetBaseDataInt( int32_t nIndex, int32_t nValue )
{
}

void LC_ServerPlayer::EchoBaseDataInt( int32_t nIndex)
{
}

void LC_ServerPlayer::LoadCharPB(const Protocol::PS_CharBaseInfoLite& linfo, const std::string& binfostr, const std::string& cinfostr, bool clonePlayer)
{
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%d>LC_ServerPlayer::LoadCharPB: %u begin", UT_ServerHelper::GetServerId(), m_iID);
	SetOwnerPlayerId(m_iID);  // 玩家加载数据
	m_PetInstanceMgr.Init(m_iID);
	//m_BotInstanceMgr.Init(m_iID);
	m_CombatRecorderMgr.Init(m_iID);
	m_NewVIPReward.Init(m_iID);
	m_nResourceRetrieveMgr.Init(m_iID);
	m_kAttributeMap.SetAttributeBase(ATT_PICK_CRASH_RNG, 400);
	m_kAttributeMap.SetAttributeBase(ATT_MAX_VIGOR, 100);
	m_kAttributeMap.SetAttributeBase(ATT_SP_RECOVER_RATE, 10000);

	//这里指的是比率的改变
	m_kAttributeMap.SetAttributeBase(ATT_MP_COST_RATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_HP_COST_RATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_MP_RECOVER_RATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_HP_RECOVER_RATE, 0);
	//这里指的是百分比*100（存成整数）
	m_kAttributeMap.SetAttributeBase(ATT_EXP_RATE, 10000);
	m_kAttributeMap.SetAttributeBase(ATT_MF_RATE,	0);
	m_kAttributeMap.SetAttributeBase(ATT_CASH_RATE, 10000);
	m_kAttributeMap.SetAttributeBase(ATT_HATRED, 1);

	char_data_define::char_base_info binfo;
	char_data_define::char_full_info cinfo;


	int sizeBefore = cinfostr.size();

	bool bBaseDataParse = false;
	bool bFullDataParse = true;

	static std::string _unzipbuf;
	int32_t nBaseSize = binfostr.size();
	if ( linfo.m_bBaseDataCompressed )
	{
		size_t length = 0x3F000;
		_unzipbuf.resize((size_t)length);

		if (Z_OK != uncompress((Bytef*)(Utility::get_buff_ptr(_unzipbuf)), (uLongf*)&length, (Bytef*)binfostr.c_str(), (uLong)binfostr.size())) 
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LoadCharPB UNZIP FAILED", GetID(), GetInstance());
			//return;
			bBaseDataParse = binfo.ParseFromString(binfostr);
		}
		else
		{
			_unzipbuf.resize(length);
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadCharPB UNZIP", GetID(), GetInstance(), binfostr.size(), length);
			bBaseDataParse = binfo.ParseFromString(_unzipbuf);
		}
	}
	else
	{
		bBaseDataParse = binfo.ParseFromString(binfostr);
	}
	if(!bBaseDataParse)
    {
        GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ParseFromString FAILED", GetID(), GetInstance());
		return;
    }

	//if (binfo.data_flag())
	if ( linfo.m_bFullDataCompressed || binfo.data_flag() )
	{
		size_t length = 0x3F000;
		_unzipbuf.resize((size_t)length);

		if (Z_OK != uncompress((Bytef*)(Utility::get_buff_ptr(_unzipbuf)), (uLongf*)&length, (Bytef*)cinfostr.c_str(), (uLong)cinfostr.size())) 
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadCharPB UNZIP FAILED", GetID(), GetInstance());
			return;
		}
		_unzipbuf.resize(length);
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadCharPB UNZIP", GetID(), GetInstance(), cinfostr.size(), length);
		bFullDataParse = cinfo.ParseFromString(_unzipbuf);
	}
	else
	{
		bFullDataParse = cinfo.ParseFromString(cinfostr);
	}

	int sizeAfter = cinfo.ByteSize();
	if ( !bBaseDataParse || !bFullDataParse )
	{
		LC_ServerPlayerEnterLeaveManager* pkPlayerEnterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		if(pkPlayerEnterLeaveManager)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LoadDataFail: ", GetID(), GetInstance(), bBaseDataParse, bFullDataParse);
			pkPlayerEnterLeaveManager->UserLeaveServer((LC_ServerPlayer*)this,LMIOT_KICKOFF);
		}
		return;
	}

	LoadCharLiteInfo(linfo, clonePlayer);
	PBLoadCharBaseInfo(binfo);
	PBLoadCharDetailInfo(cinfo.detail_info());
	PBLoadPKStateInfo(cinfo.pk_state_info());
	PBLoadTitleNameInfo(cinfo.title_info());
	
	GetAchievementAsset().LoadData(cinfo.achievements());

	PBLoadGoalInfo(cinfo.goalinfo());
	PBLoadResourceRetrieveInfo(cinfo.resourceretrieveinfo());

	PBLoadCharItemCDInfo(cinfo.item_cd_info());
	//技能不做存储，上线后，所有有技能的外围系统都重新跑一遍learnSkill
	//PBLoadCharSkillInfo(cinfo.skill_info());
	PBLoadCharShortcutBarInfo(cinfo.shortcut_info());
	PBLoadCharSkillStateInfo(cinfo.skill_state_info());

	PBLoadTaskTypeInfo(cinfo.task_type_info());
	PSLoadCharFinishedTaskInfo(cinfo.finished_task_info());
	PBLoadProcessingTaskInfo(cinfo.processing_task_info());
	PBLoadTaskChapterRewardInfo(cinfo.chapter_reward_info());
	PBLoadFailTaskInfo(cinfo.fail_task_info());
	PBLoadLivenessInfo(cinfo);
	PBLoadAssignData(cinfo);

	PBLoadGameStoryInfo(cinfo.game_story_info());
	PBLoadChar_Raid_Transport_AssetInfo(cinfo.transport_raid_info());
	PBLoadCharShortcutKeyAssetInfo(cinfo.shortcut_key_info());
	PBLoadCharWorldLocationInfo(cinfo.world_location_info());
	PBLoadVIPInfo(cinfo.vip_info());
	PBLoadAssistInfo(cinfo.assis_info());
	PBLoadArenaBattleAssertInfo(cinfo.arena_info());
	PBLoadPortalAssertInfo(cinfo.portal_info());
	PBLoadSignInAssertInfo(cinfo.signin_data());
	GetRotaryTableAsset().LoadData(cinfo.rotarytabledata());
	PBLoad360MicroSignInAssertInfo(cinfo.micro_signin_data());
	GetNoviceTutorialAsset().LoadData(cinfo.novicetutorial());
	GetPlayerPlatformInfoAsset().LoadData(cinfo.platformnew());
	GetIDAsset().LoadData(cinfo.idasset());
	GetNonpareilAsset().LoadData(cinfo.nonpareilreturnasset());
	PBLoadDomain(cinfo);
	PBLoadRankGoodRecord(cinfo.rank_good_record());
	PBLoadGuildWarRecord(cinfo.guildwar_data());

	//PBLoadPromoteInfo(cinfo.promote_info());
	//PBLoadGreatKungFuInfo(cinfo.great_kunfu_info());
	PBLoadOpenAbilityInfo(cinfo.open_ability_info());
	PBLoadCommonStatInfo(cinfo.common_stat_info());
	PBLoadTreasureChestsInfo(cinfo.treasure_chests_info());
	PBLoadCommonShopInfo(cinfo.common_shop_info());
	PBLoadActivationCodeInfo(cinfo.activation_code_info());
	PBLoadAttributePlusInfo(cinfo.attr_plus());
	PBLoadChatMonitorInfo(cinfo.chat_monitor_info());
	PBLoadCommonItemRecordSet( binfo.commonitemrecorder() );

	PBLoadCombatRecorder( cinfo.combat_recorder_data() );

	for (int i = 0; i < cinfo.custom_cashs_size(); ++i)
	{
		const char_data_define::cash_entry& kentry = cinfo.custom_cashs(i);
		SetCash(kentry.value(), (CashType)kentry.type());
	}

	PBLoadCrossRealmTeam(cinfo.crossrealmteammap_info());
	m_kFashionBagAsset.Load(cinfo.fashion_bag());

	GetOpenBagAssert().GetDataFromPB(cinfo.detail_info());
	m_uiHeadPic		= 0;
	m_eCareerType = CT_NONE;
	m_lLevel = 0;
	//_applyVehicle(GetVehicleLevel(),m_lCharLevel);
	//_applyBow(GetBowLevel());
	m_nCharDataVersion = cinfo.detail_info().init_char_data_version();
	if ( cinfo.detail_info().init_char_data_version() < 1 )
		InitUnlockedActivateBuddyCount();
	SetLevel(m_lCharLevel, true);
	GameServerManager* servermanager = GetGameServerApp()->GetServerManager();
	SetActiveServer(servermanager->GetActiveId(GetCitizenship()));
	SetGuildGroup(servermanager->GetActiveGroupId(GetCitizenship()));
	init_char_data(this, cinfo.detail_info().init_char_data_version());
	UpdateOpenBag(0.0f, 0.0f);

	for (int i = 0; i < cinfo.dailycleanlist_size(); ++i)
	{
		const char_data_define::char_daily_clean_entry& rEntry = cinfo.dailycleanlist(i);
		SetDailyCleanValue(rEntry.type(), rEntry.value(), false);
	}
	m_dailyCleanStramp = cinfo.dailycleantime();

	for (int i = 0; i < cinfo.flowersendcnt_size(); ++i)
	{
		m_flowerSendCnt.push_back(cinfo.flowersendcnt(i));
	}

	const char_data_define::char_offline_exp& offlineExp = cinfo.offlineexp();
	int exp = offlineExp.exp();
	int time = offlineExp.offtime();
	SetOffLineExp(exp, time);
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Load OffLineExp and OfflineExpTime id:%d, OffLineExp:%d, OfflineExpTime:%d\n", m_iID, exp, time);

	const char_data_define::char_mystical_shop_buy& mysticalShopBuy = cinfo.mysticalshopbuy();
	m_MySyicalShopMark.m_nTimeStamp = mysticalShopBuy.timestamp();
	m_MySyicalShopMark.m_buyCntMark.clear();
	m_MySyicalShopMark.m_resetCntMark.clear();
	m_MySyicalShopMark.m_itemidlist.clear();
	for (int i = 0; i < mysticalShopBuy.buymark_size(); i++)
	{
		const char_data_define::mystical_shop_mark rMark = mysticalShopBuy.buymark(i);
		m_MySyicalShopMark.m_buyCntMark[rMark.type()] = rMark.count();
	}
	for (int i = 0; i < mysticalShopBuy.resetmark_size(); i++)
	{
		const char_data_define::mystical_shop_mark rMark = mysticalShopBuy.resetmark(i);
		m_MySyicalShopMark.m_resetCntMark[rMark.type()] = rMark.count();
	}
	m_MySyicalShopMark.m_itemidlist.reserve(mysticalShopBuy.itemidlist_size());
	for (int i = 0; i < mysticalShopBuy.itemidlist_size(); i++)
	{
		m_MySyicalShopMark.m_itemidlist.push_back(mysticalShopBuy.itemidlist(i));
	}

	PBLoadSecretTreasureInfo(cinfo.secret_treasure());
	PBLoadActivityInfo(cinfo.activity_info());
	PBLoadArenaInfo(cinfo.global_arena());

	if (1 == GetLoginCount())
	{
		RegisterCharInfo();
	}

	PBLoadNewVIPData(cinfo.new_vip_reward_data());
	
	// 强制切到0
	// Finally call this for compatibility of old data.
	// 这里不要做任何上线 业务逻辑。放在 InitGameData 中处理

	// 这行必须在这个函数最末，确保load数据成功
	// 成功的标准不是dbserver发送EOF，而是确保数据解析完成
	SetLoadDataSuccess(true);
	int enterId =  UT_ServerHelper::GetServerId();
	const unique_id_impl& instance = GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO," LoadCharPB succesfully enterId: %d, c_cid:%d, c_citizenship:%d, c_uid:%d", enterId, instance.detail(), instance.catagory(), GetUserID());
}

bool LC_ServerPlayer::SaveAllDataToDB( const Protocol::MG_GD_Save_Char_PB& data ,ResultType nReason)
{
	//SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB, &msg, m_dwUserID, GetInstance());
	int32_t nRetCode = false;
	nRetCode = PreSaveCharData_Lite( data.lite_info );
	if ( nRetCode == false )
		return false;
	nRetCode = PreSaveCharData_Base( data.baseinfo );
	if ( nRetCode == false )
		return false;
	nRetCode = PreSaveCharData_Full( data.fullinfo );
	if ( nRetCode == false )
		return false;
	nRetCode = PreSaveCharData_Pack( data.pack );
	if ( nRetCode == false )
		return false;
	nRetCode = PreSaveCharData_Hero( data.hero );
	if ( nRetCode == false )
		return false;
	nRetCode = PreSaveCharData_PlayerBase( data.playerbase );
	if ( nRetCode == false )
		return false;
	nRetCode = PreSaveCharData_Offline( data.offline );
	if ( nRetCode == false )
		return false;
	nRetCode = PreSaveCharData_Status( data.isLeave ,nReason);
	if ( nRetCode == false )
		return false;

	//std::string BaseMD5 = getMd5Str(data.baseinfo); 
	//std::string FullMD5 = getMd5Str(data.fullinfo); 
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"SaveAllDataToDB id:%lld,charId: %d  isLeave %d  BaseMD5  %s   FullMD5  %s  \n", GetInstance(),GetInstanceCharID(),(int)data.isLeave , BaseMD5.c_str(),FullMD5.c_str());

	return true;
}

bool LC_ServerPlayer::PreSaveCharData_Lite(const Protocol::PS_CharBaseInfoLite& linfo)
{
	MG_GD_Save_Char_PB_Lite data;
	data.data = linfo;
	return SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_LITE, &data, m_dwUserID, GetInstance());
}

bool LC_ServerPlayer::PreSaveCharData_Base(const std::string& strInfo)
{
	// Base
	std::string strValue = strInfo;
	int32_t nRetCode = 0;
	//const char* pBaseOffset = pData->baseinfo.c_str();
	int32_t nStrLen = strValue.length();
	int32_t nBegin = 0;
	for ( ; nStrLen > 0; )
	{
		int nLen = gsnDynamicMsgBuffSize;
		int32_t nRemain = nStrLen - gsnDynamicMsgBuffSize;
		if ( nRemain < 0 )
			nLen = nStrLen;

		MG_GD_Save_Char_PB_Base data;
		data.data = strValue.substr( nBegin, nLen );

		nBegin += nLen;
		nStrLen -= nLen;

		data.isEnd = nStrLen == 0 ? true : false;
		nRetCode = SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_BASE, &data, m_dwUserID, GetInstance());
		if ( nRetCode != true )
			return nRetCode;
	}
	return true;
}

bool LC_ServerPlayer::PreSaveCharData_Full(const std::string& strInfo)
{
	// Base
	std::string strValue = strInfo;
	int32_t nRetCode = 0;
	//const char* pBaseOffset = pData->baseinfo.c_str();
	int32_t nStrLen = strValue.length();
	int32_t nBegin = 0;
	for ( ; nStrLen > 0; )
	{
		int nLen = gsnDynamicMsgBuffSize;
		int32_t nRemain = nStrLen - gsnDynamicMsgBuffSize;
		if ( nRemain < 0 )
			nLen = nStrLen;

		MG_GD_Save_Char_PB_Full data;
		data.data = strValue.substr( nBegin, nLen );

		nBegin += nLen;
		nStrLen -= nLen;

		data.isEnd = nStrLen == 0 ? true : false;
		nRetCode = SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_FULL, &data, m_dwUserID, GetInstance());
		if ( nRetCode != true )
			return nRetCode;
	}
	return true;
}

bool LC_ServerPlayer::PreSaveCharData_Pack(const std::string& strInfo)
{
	std::string strValue = strInfo;
	int32_t nRetCode = 0;
	int32_t nStrLen = strValue.length();
	int32_t nBegin = 0;
	for ( ; nStrLen > 0; )
	{
		int nLen = gsnDynamicMsgBuffSize;
		int32_t nRemain = nStrLen - gsnDynamicMsgBuffSize;
		if ( nRemain < 0 )
			nLen = nStrLen;

		MG_GD_Save_Char_PB_Base data;
		data.data = strValue.substr( nBegin, nLen );

		nBegin += nLen;
		nStrLen -= nLen;

		data.isEnd = nStrLen == 0 ? true : false;
		nRetCode = SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_Pack, &data, m_dwUserID, GetInstance());
		if ( nRetCode != true )
			return nRetCode;
	}
	return true;
}

bool LC_ServerPlayer::PreSaveCharData_Development(const std::string& strInfo)
{
	return true;
}

bool LC_ServerPlayer::PreSaveCharData_Hero(const std::string& strInfo)
{
	std::string strValue = strInfo;
	int32_t nRetCode = 0;
	int32_t nStrLen = strValue.length();
	int32_t nBegin = 0;
	for ( ; nStrLen > 0; )
	{
		int nLen = gsnDynamicMsgBuffSize;
		int32_t nRemain = nStrLen - gsnDynamicMsgBuffSize;
		if ( nRemain < 0 )
			nLen = nStrLen;

		MG_GD_Save_Char_PB_Base data;
		data.data = strValue.substr( nBegin, nLen );

		nBegin += nLen;
		nStrLen -= nLen;

		data.isEnd = nStrLen == 0 ? true : false;
		nRetCode = SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_Hero, &data, m_dwUserID, GetInstance());
		if ( nRetCode != true )
			return nRetCode;
	}
	return true;
}

bool LC_ServerPlayer::PreSaveCharData_PlayerBase(const std::string& strInfo)
{
	std::string strValue = strInfo;
	int32_t nRetCode = 0;
	int32_t nStrLen = strValue.length();
	int32_t nBegin = 0;
	for ( ; nStrLen > 0; )
	{
		int nLen = gsnDynamicMsgBuffSize;
		int32_t nRemain = nStrLen - gsnDynamicMsgBuffSize;
		if ( nRemain < 0 )
			nLen = nStrLen;

		MG_GD_Save_Char_PB_Base data;
		data.data = strValue.substr( nBegin, nLen );

		nBegin += nLen;
		nStrLen -= nLen;

		data.isEnd = nStrLen == 0 ? true : false;
		nRetCode = SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_PlayerBase, &data, m_dwUserID, GetInstance());
		if ( nRetCode != true )
			return nRetCode;
	}
	return true;
}

bool LC_ServerPlayer::PreSaveCharData_Offline(const std::string& strInfo)
{
	std::string strValue = strInfo;
	int32_t nRetCode = 0;
	int32_t nStrLen = strValue.length();
	int32_t nBegin = 0;
	for ( ; nStrLen > 0; )
	{
		int nLen = gsnDynamicMsgBuffSize;
		int32_t nRemain = nStrLen - gsnDynamicMsgBuffSize;
		if ( nRemain < 0 )
			nLen = nStrLen;

		MG_GD_Save_Char_PB_Base data;
		data.data = strValue.substr( nBegin, nLen );

		nBegin += nLen;
		nStrLen -= nLen;

		data.isEnd = nStrLen == 0 ? true : false;
		nRetCode = SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_Offline, &data, m_dwUserID, GetInstance());
		if ( nRetCode != true )
			return nRetCode;
	}
	return true;
}

bool LC_ServerPlayer::PreSaveCharData_Status(int32_t nFlag,ResultType nReason)
{
	MG_GD_Save_Char_PB_Status data;
	data.isLeave = nFlag;
	data.nReason = nReason;
	return SendMsgToGameDB(MGPT_GD_SAVE_CHAR_PB_STATUS, &data, m_dwUserID, GetInstance());
}

void LC_ServerPlayer::SaveCharPB(bool isleave,ResultType nReason)
{
	if ( GetGlobalSetting.nSaveRoleData == 0 || GetSoloDulePlay() || NULL == m_pAssignedBuddyInstance)
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SaveToDb GetSoloDulePlayid %d ,userId %d instance %lld isleve %d nReason %d GetGlobalSetting.nSaveRoleData %d GetSoloDulePlay() %d NULL == m_pAssignedBuddyInstance %d \n", GetID(),GetUserID(), GetInstance(),  isleave,nReason,GetGlobalSetting.nSaveRoleData,GetSoloDulePlay(),NULL == m_pAssignedBuddyInstance);
		return;
	}

	// Test scope
#if defined SQL_SIZE_ANALYZE
	char_data_define::char_base_info base_info;
	char_data_define::char_full_info full_info;
	std::string strBase;
	std::string strFull;

	base_info.SerializeToString(&strBase);
	full_info.SerializeToString(&strFull);

	int nBaseSize = strBase.size();
	int nBaseLen = strBase.length();
	int nFullSize = strFull.size();
	int nFullLen = strFull.length();
#endif
	

	SetLastOfflineTime(GET_CURRENT_TIMESTAMP_IN_SECONDS());
	MG_GD_Save_Char_PB msg;
	msg.isLeave = isleave;
	char_data_define::char_base_info binfo;
	char_data_define::char_full_info cinfo;
	char_data_define::PBPackData pack;
	char_data_define::PBHeroData hero;
	char_data_define::PBPlayerBaseData playerbase;
	char_data_define::PBOfflineData offline;

	SaveCharPB(msg.lite_info, binfo, cinfo);
	SaveCharPB_Pack( pack );
	SaveCharPB_Hero( hero );
	SaveCharPB_PlayerBase( playerbase );
	SaveCharPB_Offline( offline );

	static std::string _zipbuf;
	cinfo.SerializeToString(&msg.fullinfo);
	{
		uLongf length = ::compressBound((uLong)msg.fullinfo.size());
		_zipbuf.resize((size_t)length);

		if (Z_OK == compress((Bytef*)(Utility::get_buff_ptr(_zipbuf)), (uLongf*)&length, (Bytef*)msg.fullinfo.c_str(), (uLong)msg.fullinfo.size())) 
		{
			_zipbuf.resize(length);
			//GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SaveCharPB_full ZIP", GetID(), GetInstance(), msg.fullinfo.size(), length);
			binfo.set_data_flag(1);
			msg.lite_info.m_bFullDataCompressed = true;
			msg.fullinfo = _zipbuf;
		}
	}

	binfo.SerializeToString(&msg.baseinfo);
	{
		uLongf length = ::compressBound((uLong)msg.baseinfo.size());
		_zipbuf.resize((size_t)length);

		if (Z_OK == compress((Bytef*)(Utility::get_buff_ptr(_zipbuf)), (uLongf*)&length, (Bytef*)msg.baseinfo.c_str(), (uLong)msg.baseinfo.size())) 
		{
			_zipbuf.resize(length);
			//GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SaveCharPB_base ZIP", GetID(), GetInstance(), msg.baseinfo.size(), length);
			msg.lite_info.m_bBaseDataCompressed = true;
			msg.baseinfo = _zipbuf;
		}
	}

	pack.SerializeToString(&msg.pack);
	hero.SerializeToString(&msg.hero);
	playerbase.SerializeToString(&msg.playerbase);
	offline.SerializeToString(&msg.offline);

	//GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SaveCharPB", GetID(), GetInstance(), GetMapResID(), isleave, msg.baseinfo.size(), msg.fullinfo.size(),nReason);

	
	SaveAllDataToDB(msg,nReason);
	if (isleave || CharInfoDirty())
	{
		RegisterCharInfo();
	}
}

void LC_ServerPlayer::SaveCharPB(Protocol::PS_CharBaseInfoLite& info, char_data_define::char_base_info& binfo, char_data_define::char_full_info& cinfo)
{
#if defined SQL_SIZE_ANALYZE
	std::string strDebug;
	int nDebugSize = 0;
	int nLastDebugSize = 0;
	//int nDeltaDebugSize = 0;
	vector<int> nDeltaDebugSize;
#endif
	// 公会积分 异步流程  这里初始化
	//binfo.clear_personalguildscore();
	//binfo.mutable_robbednum()->set_ukey(0);
	//uint32_t featuretime = (uint32_t)LC_Helper::GetTimeStampInCurDay(23, 59, 59);
	
	//binfo.mutable_robbednum()->set_uvalue(featuretime);
	SaveCharLiteInfo(info);
	PBSaveCharBaseInfo(binfo);
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCharDetailInfo(*cinfo.mutable_detail_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveNewVIPData(*cinfo.mutable_new_vip_reward_data());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSavePKStateInfo(*cinfo.mutable_pk_state_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCharShortcutBarInfo(*cinfo.mutable_shortcut_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveTitleNameInfo(*cinfo.mutable_title_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	GetAchievementAsset().SaveData(cinfo.mutable_achievements());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveGoalInfo(*cinfo.mutable_goalinfo());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveResourceRetrieveInfo(*cinfo.mutable_resourceretrieveinfo());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif	
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif	
	PBSaveCharItemCDInfo(*cinfo.mutable_item_cd_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	//技能不做存储，上线后，所有有技能的外围系统都重新跑一遍learnSkill
	//PBSaveCharSkillInfo(*cinfo.mutable_skill_info());
	PBSaveCharSkillStateInfo(*cinfo.mutable_skill_state_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveTaskTypeInfo(*cinfo.mutable_task_type_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PSSaveCharFinishedTaskInfo(*cinfo.mutable_finished_task_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveProcessingTaskInfo(*cinfo.mutable_processing_task_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveTaskChapterRewardInfo(*cinfo.mutable_chapter_reward_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveFailTaskInfo(*cinfo.mutable_fail_task_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveLivenessInfo(cinfo);
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveAssignData(cinfo);
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveGameStoryInfo(*cinfo.mutable_game_story_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveChar_Raid_Transport_AssetInfo(*cinfo.mutable_transport_raid_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCharShortcutKeyAssetInfo(*cinfo.mutable_shortcut_key_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCharWorldLocationInfo(*cinfo.mutable_world_location_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveVIPInfo(*cinfo.mutable_vip_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveAssistInfo(*cinfo.mutable_assis_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveArenaBattleAssertInfo(*cinfo.mutable_arena_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSavePortalAssertInfo(*cinfo.mutable_portal_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	GetRotaryTableAsset().SaveData(cinfo.mutable_rotarytabledata());

	PBSaveSignInAssertInfo(*cinfo.mutable_signin_data());
	GetNoviceTutorialAsset().SaveData(cinfo.mutable_novicetutorial());
	GetPlayerPlatformInfoAsset().SaveData(cinfo.mutable_platformnew());
	GetIDAsset().SaveData(cinfo.mutable_idasset());
	GetNonpareilAsset().SaveData(cinfo.mutable_nonpareilreturnasset());

#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveDomain(cinfo);
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveRankGoodRecord(*cinfo.mutable_rank_good_record());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveGuildWarRecord(*cinfo.mutable_guildwar_data());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	//PBSavePromoteInfo(*cinfo.mutable_promote_info());
	//PBSaveGreatKungFuInfo(*cinfo.mutable_great_kunfu_info());
	PBSaveOpenAbilityInfo(*cinfo.mutable_open_ability_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCommonStatInfo(*cinfo.mutable_common_stat_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveTreasureChestsInfo(*cinfo.mutable_treasure_chests_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCommonShopInfo(*cinfo.mutable_common_shop_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveActivationCodeInfo(*cinfo.mutable_activation_code_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveAttributePlusInfo(*cinfo.mutable_attr_plus());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveChatMonitorInfo(*cinfo.mutable_chat_monitor_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCommonItemRecordSet(*binfo.mutable_commonitemrecorder());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif	
	PBSaveCombatRecorder( *cinfo.mutable_combat_recorder_data() );
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for (int i = 0; i < CT_CASH_COUNT - CT_JUSTICE_PT; ++i)
	{
		char_data_define::cash_entry* pkentry = cinfo.add_custom_cashs();
		if (pkentry)
		{
			pkentry->set_type(CT_JUSTICE_PT + i);
			pkentry->set_value(m_kCustomCash[i].GetMoney());
		}
	}
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveCrossRealmTeam(*cinfo.mutable_crossrealmteammap_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	GetOpenBagAssert().SetDataToPB(*cinfo.mutable_detail_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for (mem::map<uint32_t, int32_t>::iterator iter = m_dailyCleanMap.begin(); iter != m_dailyCleanMap.end(); iter++)
	{
		if (iter->second != 0)
		{
			char_data_define::char_daily_clean_entry* pEntry = cinfo.add_dailycleanlist();
			if (pEntry != NULL)
			{
				pEntry->set_type(iter->first);
				pEntry->set_value(iter->second);
			}
		}
	}

#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for (mem::vector<uint32_t>::iterator iter = m_flowerSendCnt.begin(); iter != m_flowerSendCnt.end(); iter++)
	{
		cinfo.add_flowersendcnt(*iter);
	}
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	char_data_define::char_offline_exp* pOfflineExp = cinfo.mutable_offlineexp();
	if (pOfflineExp != NULL)
	{
		int exp = GetOffLineExp();
		int time = GetOfflineExpTime();
		//GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO,"Save OffLineExp and OfflineExpTime id:%d, OffLineExp:%d, OfflineExpTime:%d\n", m_iID, exp, time);
		pOfflineExp->set_exp(exp);
		pOfflineExp->set_offtime(time);
	}
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	char_data_define::char_mystical_shop_buy* pMysticalShopBuy = cinfo.mutable_mysticalshopbuy();
	if (pMysticalShopBuy != NULL)
	{
		pMysticalShopBuy->set_timestamp(m_MySyicalShopMark.m_nTimeStamp);
		for (mem::map<uint32_t, uint32_t>::iterator iter = m_MySyicalShopMark.m_buyCntMark.begin(); iter != m_MySyicalShopMark.m_buyCntMark.end(); iter++)
		{
			char_data_define::mystical_shop_mark* pMark = pMysticalShopBuy->add_buymark();
			if (pMark != NULL)
			{
				pMark->set_type(iter->first);
				pMark->set_count(iter->second);
			}
		}
		for (mem::map<uint32_t, uint32_t>::iterator iter = m_MySyicalShopMark.m_resetCntMark.begin(); iter != m_MySyicalShopMark.m_resetCntMark.end(); iter++)
		{
			char_data_define::mystical_shop_mark* pMark = pMysticalShopBuy->add_resetmark();
			if (pMark != NULL)
			{
				pMark->set_type(iter->first);
				pMark->set_count(iter->second);
			}
		}
		for (mem::vector<uint32_t>::iterator iter = m_MySyicalShopMark.m_itemidlist.begin(); iter != m_MySyicalShopMark.m_itemidlist.end(); iter++)
		{
			pMysticalShopBuy->add_itemidlist(*iter);
		}
	}
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	m_kFashionBagAsset.Save(*cinfo.mutable_fashion_bag());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveSecretTreasureInfo(*cinfo.mutable_secret_treasure());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveActivityInfo(*cinfo.mutable_activity_info());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	PBSaveArenaInfo(*cinfo.mutable_global_arena());
#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	cinfo.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	PBSave360MicroSignInAssertInfo(*cinfo.mutable_micro_signin_data());
	return;
}

void LC_ServerPlayer::PreLoadCharData_Lite(const Protocol::PS_CharBaseInfoLite& linfo)
{
	m_PreLoad_Lite = linfo;
}

void LC_ServerPlayer::PreLoadCharData_Base(const std::string& strInfo, bool bIsEnd)
{
	m_PreLoad_BaseData += strInfo;
	if ( bIsEnd )
	{

	}
}

void LC_ServerPlayer::PreLoadCharData_Full(const std::string& strInfo, bool bIsEnd)
{
	m_PreLoad_FullData += strInfo;
	if ( bIsEnd )
	{

	}
}

void LC_ServerPlayer::PreLoadCharData_Pack(const std::string& strInfo, bool bIsEnd)
{
	m_PreLoad_Pack += strInfo;
	if ( bIsEnd )
	{

	}
}

void LC_ServerPlayer::PreLoadCharData_Development(const std::string& strInfo, bool bIsEnd)
{
}

void LC_ServerPlayer::PreLoadCharData_Hero(const std::string& strInfo, bool bIsEnd)
{
	m_PreLoad_Hero += strInfo;
	if ( bIsEnd )
	{

	}
}

void LC_ServerPlayer::PreLoadCharData_PlayerBase(const std::string& strInfo, bool bIsEnd)
{
	m_PreLoad_PlayerBase += strInfo;
	if ( bIsEnd )
	{

	}
}

void LC_ServerPlayer::PreLoadCharData_Offline(const std::string& strInfo, bool bIsEnd)
{
	m_PreLoad_Offline += strInfo;
	if ( bIsEnd )
	{

	}
}

void LC_ServerPlayer::ParseCharDataFromPB(bool clonePlayer)
{
	int enterId =  UT_ServerHelper::GetServerId();
	const unique_id_impl& instance = GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO," receive db info  ParseCharDataFromPB enterId: %d, c_cid:%d, c_citizenship:%d, c_uid:%d", enterId, instance.detail(), instance.catagory(), GetUserID());
	LoadCharPB( m_PreLoad_Lite, m_PreLoad_BaseData, m_PreLoad_FullData, clonePlayer);
	LoadCharPB_Pack( m_PreLoad_Pack );
	LoadCharPB_Hero( m_PreLoad_Hero );
	LoadCharPB_PlayerBase( m_PreLoad_PlayerBase );
	LoadCharPB_Offline( m_PreLoad_Offline );
}

void LC_ServerPlayer::LoadCharLiteInfo(const Protocol::PS_CharBaseInfoLite& info, bool clonePlayer)
{
	SetBornCharType(info.m_byCharType);
	m_kCharName_gb = info.m_szCharName;
	m_kCharName_utf8 = m_kCharName_gb;//CodeConvert::MbsToUtf8(m_kCharName_gb);
	m_eWeaponAttribute = (WeaponType)info.m_byWeapon;
	m_lCharLevel = info.m_usCharLevel;
	m_lEXP		= info.m_llCharExp;
	if (IS_VALID_CHAR_MONEY(info.m_llYuanBaoPiao))
	{
		GetBindYuanBao().SetMoney(info.m_llYuanBaoPiao);
	}
	if (IS_VALID_CHAR_MONEY(info.m_llCash))
	{
		GetUnBindCash().SetMoney(info.m_llCash);
	}
	GetUnBindYuanBao().SetMoney(info.m_llUnbindYuanBao);
	m_byDeleteFlag = info.m_byDeleteFlag;		//值为0表示角色正常，否则表示当前角色已被删除
	m_iDeleteTime = info.m_iDeleteTime;
	m_iServerID = UT_ServerHelper::GetActiveId(info.m_iCitizenship);
	m_iRegTime = info.m_iRegTime;
	m_iLastOfflineTime = info.m_iLastOfflineTime;
	m_iFreezeRol = info.m_tmFreezeRole;
	SetGMLevel(info.m_nGMLevel);
	SetGlobalMailID(info.m_nGlobalMailId);
	if(clonePlayer)
	{
		m_uiGuildID = info.m_guildId;
	}
}

void LC_ServerPlayer::SaveCharLiteInfo(Protocol::PS_CharBaseInfoLite& info)
{
	info.m_nGlobalMailId = GetGlobalMailID();
	info.m_nGMLevel = GetGMLevel();
	info.m_uiCharID = GetInstanceCharID();
	//info.m_byCharType = (uint8_t)m_lCharType;
	info.m_byCharType = (uint8_t)GetBornCharType();
	STRNCPY(info.m_szCharName, m_kCharName_gb.c_str(), sizeof(info.m_szCharName));
	info.m_byWeapon = (uint8_t)m_eWeaponAttribute;
	info.m_usCharLevel = (uint16_t) m_lLevel;
	info.m_llCharExp = m_lEXP;
	info.m_llYuanBaoPiao = (int64_t)GetBindYuanBao().GetMoney();
	info.m_llCash = (int64_t)GetUnBindCash().GetMoney();
	info.m_llUnbindYuanBao = (int64_t)GetUnBindYuanBao().GetMoney();
	info.m_byDeleteFlag = m_byDeleteFlag;
	info.m_iDeleteTime = m_iDeleteTime;
	info.m_iRegTime = m_iRegTime;
	info.m_iCitizenship = GetCitizenship();
	info.m_iLastOfflineTime = m_iLastOfflineTime;
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"Save lastOfflineTime id:%lld,charId: %d  lastOfflineTime:%d\n", GetInstance(),GetInstanceCharID(), m_iLastOfflineTime);
	info.m_tmFreezeRole = m_iFreezeRol;
	info.m_guildId = m_uiGuildID;
	//info.m_iRemainEquipReinforceExp = m_iRemainEquipReinforceExp;
	STRNCPY(info.m_guildName, m_kGuildName.c_str(), sizeof(info.m_guildName));
}

void LC_ServerPlayer::PBSaveCharLiteInfo(char_data_define::char_lite_info& info)
{
	info.set_charid(GetInstance());
	info.set_chartype(this->GetCharType());
	info.set_charname(m_kCharName_gb.c_str());
	info.set_career(m_eWeaponAttribute);
	info.set_level(m_lLevel);
	info.set_guildid(m_uiGuildID);
	info.set_guildname(m_kGuildName.c_str());
}

void LC_ServerPlayer::PBLoadCharBaseInfo(const char_data_define::char_base_info& info)
{
	m_DailyReward = info.dailyreward();
	for(int i = 0; i < info.treasurenum_size();++i)
	{
		sTreasureNumStruct treasure_info;
		treasure_info.uType = info.treasurenum(i).ukey();
		treasure_info.uCount = info.treasurenum(i).uvalue();
		m_vecTreasureNum.push_back(treasure_info);
	}

	m_nTreasureMapID = info.treasure_mapid();
	m_nTreasurePos = UT_Vec3Int(info.treasure_x(), info.treasure_y(), info.treasure_z());

	m_nShowGuildPresentName = info.showguildpresentname();
	m_SkillLevelupPoints = info.skill_levelup_point();
	if (m_SkillLevelupPoints < 0)
	{
		m_SkillLevelupPoints = 100000000;
	}

	byte* tutorial_info = m_kTutorialAsset.GetDataBuff();
	for (int i = 0; i < info.tutorial_info_size() && i < MG_TUTORIALDATA_LENGTH; i++)
	{
		int value = info.tutorial_info(i);
		tutorial_info[i] = (byte)value;
	}
	m_bIsShowFashion = info.is_show_fashion() == 0 ? false : true;
	SetMapRaodmap(info.last_map_res_id());
	if (info.last_map_res_id() == 0)
	{
		MapRoadmapManager* inst = SafeGetSingleton<MapRoadmapManager>();
		SetMapRaodmap(inst->GetFirstMap());
	}
	m_LoginCount = info.login_count() + 1;
	m_TotalOnlineTime = info.total_onlinetime();

	m_nLotteryConsumeUnBindYuanBaoCount = info.lotteryconsumeunbindyuanbaocount();

	m_bNewVipState = info.newvipstate();
	m_nNewVipLevel = info.newviplevel();
	m_NewVIPReward.LoadLevel(m_nNewVipLevel);
	for(int i=0; i<info.viprewardrecord_size(); ++i)
	{
		m_nVipRewardRecord.insert(info.viprewardrecord(i));
	}

	m_bGuildRewardFlag = info.guildrewardflag();
	m_bGuildRewardIsVaild = info.guildrewardisvaild();
	m_nDailyActivity = info.dailyactivity();
	for(int i=0; i<info.dailyactivityrewardid_size(); ++i)
	{
		m_nDailyActivityRewardID.push_back(info.dailyactivityrewardid(i));
	}

	for(int i=0; i<info.activatestarveinid_size(); ++i)
	{
		m_nActivateStarVeinID.push_back(info.activatestarveinid(i));
	}

	for(int i=0; i<info.starveingoalid_size(); ++i)
	{
		m_nStarVeinGoalID.push_back(info.starveingoalid(i));
	}

	m_nSevenDayTrailFlag = info.sevendaytrailflag();

	m_nFinishMainTaskID = info.finishmaintaskid();
	m_fixMainTaskBug = info.fixmaintaskbug();
	for(int i=0; i<info.finishgamestorymapid_size(); ++i)
	{
		mem::set<int32_t> mapIDs;
		int storyID = info.finishgamestorymapid(i).storyid();
		for(int j=0; j<info.finishgamestorymapid(i).mapid_size(); ++j)
		{
			int mapID = info.finishgamestorymapid(i).mapid(j);
			mapIDs.insert(mapID);
		}
		
		m_nFinishGameStoryMapID.insert(std::make_pair(storyID, mapIDs));
	}

	const char_data_define::first_daily_charge_info& FDInfo = info.firstdailychargeinfo();
	m_nFDFlag = FDInfo.fdday();
	for(int i=0; i<FDInfo.rewardinfo().size(); ++i)
	{
		char_data_define::reward_info info = FDInfo.rewardinfo(i);
		if(info.discountrewardid().size() != info.discountrewardtime().size())
			continue;

		PS_FirstDailyChargeReward reward;
		reward.tag = info.tag();
		reward.day = info.day();
		reward.freeReward =info.freerewardid();
		for(int j=0; j<info.discountrewardid().size(); ++j)
		{
			PS_RewardRecord record;
			record.disCountRewardID = info.discountrewardid(j);
			record.disCountRewardTimes =  info.discountrewardtime(j);

			reward.disCountRewards.push_back(record);
		}

		m_firstDailyChargeRewards.insert(std::make_pair(info.day(), reward));
	}
	
	CF_CollectionActivation::DataEntry* pkActivationData = NULL;
	for(int i=0; i<info.collectiondata().collectionactivationid_size(); ++i)
	{
		pkActivationData = SafeGetCSVFileDataEntryPtr<CF_CollectionActivation>(info.collectiondata().collectionactivationid(i));
		if(NULL != pkActivationData)
		{
			m_collectionActivationIDMap.insert(std::make_pair(pkActivationData->id, pkActivationData->CollectionID));
		}
	}

	for(int i=0; i<info.collectiondata().collectionrewardid_size(); ++i)
	{
		m_collectionRewardSet.insert(info.collectiondata().collectionrewardid(i));
	}

	for(int i=0; i<info.collectiondata().collectionachievementid_size(); ++i)
	{
		m_collectionAchievementIDSet.insert(info.collectiondata().collectionachievementid(i));
	}

	for(int i=0; i<info.biographyrewardid().size(); ++i)
	{
		m_biographytRewardPartSet.insert(info.biographyrewardid(i));
	}

	for(int i=0; i<info.assignbiographyinfo().size(); ++i)
	{
		int biographyID = info.assignbiographyinfo(i).biographyid();
		mem::set<int> biographyPartIDs;		
		for(int j=0; j<info.assignbiographyinfo(i).biographypartid_size(); ++j)
		{
			int biographyPartID = info.assignbiographyinfo(i).biographypartid(j);
			biographyPartIDs.insert(biographyPartID);
		}
		
		m_nAssignBiographySet.insert(std::make_pair(biographyID, biographyPartIDs));
	}

	for(int i=0; i<info.finishbiographypartid().size(); ++i)
	{
		m_nFinishBiographyPartSet.insert(info.finishbiographypartid(i));
	}
	
	for(int i=0; i<info.nextassignbiographypartid().size(); ++i)
	{
		m_nNextAssignBiographyPartSet.insert(info.nextassignbiographypartid(i));
	}

	m_nCheckBiography = info.checkbiography();

	m_bACTSubSave = info.actsubsave();
	m_bACTSubSave_Plus = info.actsubsave_plus();
	m_nRefreshRewardTaskTimes = info.refershrewardtasktimes();
	m_nAcceptRewardTaskTimes = info.acceptrewardtasktimes();
	m_nRewardTaskEndTime = info.rewardtaskendtime();

	m_QuickRaidAsset.LoadData(info.quickraiddata());
	
	LoadRongGuangYuanZhengData(info.rongguangyuanzheng());
	
	for(int i=0; i<info.yijituxistagerewardrecord_size(); ++i)
	{
		m_nYiJiTuXiStageRewardRecord.push_back(info.yijituxistagerewardrecord(i));
	}

	m_nYiJiTuXiEndTime = info.yijituxiendtime();
	m_nYiJiTuXiSendFlag = info.yijituxisendflag();

	for(int i=0; i<info.yijixunbaostagerewardrecord_size(); ++i)
	{
		m_nYiJiXunBaoStageRewardRecord.push_back(info.yijixunbaostagerewardrecord(i));
	}
	m_nYiJiXunBaoEndTime = info.yijixunbaoendtime();
	m_nYiJiXunBaoSendFlag = info.yijixunbaosendflag();

	for(int i=0; i<info.petislandsstagerewardrecord_size(); ++i)
	{
		m_nPetIslandsStageRewardRecord.push_back(info.petislandsstagerewardrecord(i));
	}
	m_nPetIslandsEndTime = info.petislandsendtime();
	m_nPetIslandsSendFlag = info.petislandssendflag();
	
	for(int i=0; i<info.resetallcommonstatid_size(); ++i)
	{
		m_nResetAllCommonStatID.insert(info.resetallcommonstatid(i));
	}	

	for(int i=0; i<info.rankdamagereward_size(); ++i)
	{
		mem::map<int32_t, bool> data;
		for(int j=0; j<info.rankdamagereward(i).rewardinfo_size(); ++j)
		{
			data.insert(std::make_pair(info.rankdamagereward(i).rewardinfo(j).rankindex(), info.rankdamagereward(i).rewardinfo(j).flag()));
		}
		m_nDamageRankReward.insert(std::make_pair(info.rankdamagereward(i).mapid(), data));
	}

	LoadBestGuildAchievementInfo(info.bestguildachievementinfo());

	LoadHuanJingData(info.huanjingdata());

	m_bKillNPCExpFlag = info.killnpcexpflag();

	for(int i=0; i<info.timelimitedinfo().size(); ++i)
	{
		TimeLimitedInfo data;
		data.id = info.timelimitedinfo(i).id();
		data.endTimeStamp = info.timelimitedinfo(i).endtimestamp();
		data.state = (ActivityState)(info.timelimitedinfo(i).state());
		data.shopTimeLimitedID = info.timelimitedinfo(i).shoptimelimitedid(); 
		m_nTimeLimitedRecord.push_back(data);
	}

	for(int i=0; i<info.shopresetrecoder().size(); ++i)
	{
		m_ShopTimes.insert(std::make_pair(info.shopresetrecoder(i).shoptypeid(), info.shopresetrecoder(i).times()));
	}

	for(int i=0; i<info.shopstarttime().size(); ++i)
	{
		m_ShopStartTime.insert(std::make_pair(info.shopstarttime(i).shoptypeid(), info.shopstarttime(i).starttime()));
	}

	for(int i=0; i<info.unbindyuanbaoshopiteminfo().size(); ++i)
	{
		m_nUnbindYuanbaoShopItemInfo.insert(std::make_pair(info.unbindyuanbaoshopiteminfo(i).indexid(), info.unbindyuanbaoshopiteminfo(i).shopaid()));
	}

	for(int i=0; i<info.guildshopiteminfo().size(); ++i)
	{
		m_nGuildShopItemInfo.insert(std::make_pair(info.guildshopiteminfo(i).indexid(), info.guildshopiteminfo(i).shopaid()));
	}

	for(int i=0; i<info.moneycourageshopiteminfo().size(); ++i)
	{
		m_nMoneyCourageShopItemInfo.insert(std::make_pair(info.moneycourageshopiteminfo(i).indexid(), info.moneycourageshopiteminfo(i).shopaid()));
	}

	for(int i=0; i<info.bossjpshopiteminfo().size(); ++i)
	{
		m_nBossJPShopItemInfo.insert(std::make_pair(info.bossjpshopiteminfo(i).indexid(), info.bossjpshopiteminfo(i).shopaid()));
	}

	for(int i=0; i<info.limitshopiteminfo().size(); ++i)
	{
		m_nLimitShopItemInfo.insert(std::make_pair(info.limitshopiteminfo(i).indexid(), info.limitshopiteminfo(i).shopaid()));
	}

	m_nCollectBossJP = info.bossjpcollectcount();

	for(int i=0; i<info.holydeedinfo().size(); ++i)
	{
		m_nHolyDeedRecords.insert(std::make_pair(info.holydeedinfo(i).id(), info.holydeedinfo(i).count()));
	}

	mem::vector<unique_id_type> BeFriendIDs;
	for(int i=0; i<info.befriendid_size(); ++i)
	{
		BeFriendIDs.push_back(info.befriendid(i));
	}

	LoadSocietyBeID(ST_FRIEND, BeFriendIDs);

	m_bDecompositionInit = info.decompositioninit();
	if(m_bDecompositionInit)
	{
		m_bDecompositionController = info.decompositioncontroller();
		m_nControllerItemlevel = info.controlleritemlevel();
		m_nControllerItemGrade = info.controlleritemgrade();
		m_bDecompositionVehicle = info.decompositionvehicle();
		m_nVehicleItemLevel = info.vehicleitemlevel();
		m_nVehicleItemGrade = info.vehicleitemgrade();
		m_bDecompositionDevaeye = info.decompositiondevaeye();
		m_nDevaeyeItemLevel = info.devaeyeitemlevel();
		m_nDevaeyeItemGrade = info.devaeyeitemgrade();
		m_bDecompositionWing = info.decompositionwing();
		m_nWingItemLevel = info.wingitemlevel();
		m_nWingItemGrade = info.wingitemgrade();
	}
	else
	{
		m_bDecompositionController = GetGlobalSetting.DecompositionController;
		m_nControllerItemlevel = GetGlobalSetting.ControllerItemlevel;
		m_nControllerItemGrade = GetGlobalSetting.ControllerItemGrade;
		m_bDecompositionVehicle = GetGlobalSetting.DecompositionVehicle;
		m_nVehicleItemLevel = GetGlobalSetting.VehicleItemLevel;
		m_nVehicleItemGrade = GetGlobalSetting.VehicleItemGrade;
		m_bDecompositionDevaeye = GetGlobalSetting.DecompositionDevaeye;
		m_nDevaeyeItemLevel = GetGlobalSetting.DevaeyeItemLevel;
		m_nDevaeyeItemGrade = GetGlobalSetting.DevaeyeItemGrade;
		m_bDecompositionWing = GetGlobalSetting.DecompositionWing;
		m_nWingItemLevel = GetGlobalSetting.WingItemLevel;
		m_nWingItemGrade = GetGlobalSetting.WingItemGrade;
	}
	
	m_bAutoDecomposition = info.autodecomposition();

	m_nFirstChargeTime = info.firstchargetime();
	for(int i=0; i<info.firstchargerewardstate_size(); ++i)
	{
		m_nFirstChargeRewardState.insert(std::make_pair(info.firstchargerewardstate(i).id(), info.firstchargerewardstate(i).state()));
	}

	m_nUsedVIPExtraRewardTimes = info.usedvipextrarewardtimes();

	if(info.skillbookslotdata().size() == 0)
	{
		m_nSkillBookSlotDataInfo[0].clear();
		m_nSkillBookSlotDataInfo[1].clear();
		m_nSkillBookSlotDataInfo[2].clear();
		CF_SkillBookUnlock::DataEntryMapExternIterator it = CF_SkillBookUnlock::GetSingletonPtr()->GetIterator();
		while(it.HasMoreElements())
		{
			CF_SkillBookUnlock::DataEntry* pkData = it.PeekNextValuePtr();
			if(0<=pkData->_index && pkData->_index<3)
			{
				PS_SkillBookSlotData data;
				data.m_nSlotID = pkData->_slotID;
				m_nSkillBookSlotDataInfo[pkData->_index].push_back(data);
			}
			it.MoveNext();
		}
	}

	for(int i=0; i<info.skillbookslotdata().size(); ++i)
	{
		int buddyIndex = info.skillbookslotdata(i).buddyindex();

		PS_SkillBookSlotData data;
		data.m_nSlotID = info.skillbookslotdata(i).slotid();
		data.m_nSlotState = info.skillbookslotdata(i).slotstate();
		data.m_nSkillBookID = info.skillbookslotdata(i).skillbookid();
		m_nSkillBookSlotDataInfo[buddyIndex].push_back(data);
	}

	for(int i=0; i<info.investmentcardrecord().size(); ++i)
	{
		int32_t typeId = info.investmentcardrecord(i).investmentcardtype();
		m_nInvestmentCardRecord.insert(typeId);
	}

	for(int i=0; i<info.investmentcardstarttime().size(); ++i)
	{
		int32_t typeId = info.investmentcardstarttime(i).investmentcardtype();
		int32_t startTime = info.investmentcardstarttime(i).starttime();
		m_nInvestmentCardStartTime.insert(std::make_pair(typeId, startTime));
	}

	for(int i=0; i<info.freerewardachievementid().size(); ++i)
	{
		int id = info.freerewardachievementid(i);
		m_nInvestmentCardFreeRewardID.insert(id);
	}

	for(int i=0; i<info.highrewardachievementid().size(); ++i)
	{
		int id = info.highrewardachievementid(i);
		m_nInvestmentCardHighRewardID.insert(id);
	}

	for(int i=0; i<info.strewardrecord_size(); ++i)
	{
		m_nSTRewardRecord.insert(info.strewardrecord(i));
	}

	for(int i=0; i<info.subinfo_size(); ++i)
	{
		m_nSubBInfo.insert(std::make_pair(info.subinfo(i).groupid(), info.subinfo(i).value()));
	}
	
	for(int i=0; i<info.sendsubinfo_size(); ++i)
	{
		m_nSendSubInfo.insert(std::make_pair(info.sendsubinfo(i).groupid(), info.sendsubinfo(i).value()));
	}

	for(int i=0; i<info.rankgroupid_size(); ++i)
	{
		m_nRankGroupID.insert(info.rankgroupid(i));
	}

	int bornCharTypeID = GetBornCharType();
	int profilePictureID = info.profilepictureid();
	m_nProfilePictureID = profilePictureID==0 ? bornCharTypeID : profilePictureID;

	for(int i=0; i<info.profilepicturerecords_size(); ++i)
	{
		int id = info.profilepicturerecords(i).pictureid();
		bool tag = info.profilepicturerecords(i).tag();
		m_nProfilePictureRecord.insert(std::make_pair(id, tag));
	}
	
	m_nCommanderTitle = info.commandertitle();

	LoadSubSkillInfo(info.subskillinfo());

	m_nNewVipDropTime = info.newvipdroptime();

	for(int i=0; i<info.vipdropitem_size(); ++i)
	{
		m_nVIPDropItemInfo.insert(std::make_pair(info.vipdropitem(i).itemid(), info.vipdropitem(i).itemnum()));
	}

	m_nCurrentCommanderTitleFrameID = info.currentcommandertitleid();
	for(int i=0; i<info.activatecommandertitlerecords_size(); ++i)
	{
		int id = info.activatecommandertitlerecords(i).titleid();
		bool tag = info.activatecommandertitlerecords(i).tag();
		m_nCommanderTitleFrameIDRecord[id] = tag;
	}

	for(int i=0; i<info.storyassigntasktime_size(); ++i)
	{
		m_nStoryAssignTaskTime.insert(std::make_pair(info.storyassigntasktime(i).storyid(), info.storyassigntasktime(i).time()));
	}

	for(int i=0; i<info.bestguildactivityendtime_size(); ++i)
	{
		m_nBestGuildActivityEndTime.insert(std::make_pair(info.bestguildactivityendtime(i).storyid(), info.bestguildactivityendtime(i).endtime()));
	}

	for(int i=0; i<info.assignbestguildtaskendtime_size(); ++i)
	{
		m_nAssignBestGuildTaskEndTime.insert(std::make_pair(info.assignbestguildtaskendtime(i).scheduleid(), info.assignbestguildtaskendtime(i).endtime()));
	}

	for(int i=0; i<info.bestguildlastrankinfo_size(); ++i)
	{
		m_nPlayerLastRankIndex.insert(std::make_pair(info.bestguildlastrankinfo(i).key(), info.bestguildlastrankinfo(i).index()));
	}

	for(int i=0; i<info.bestguildcurrentrankinfo_size(); ++i)
	{
		m_nPlayerCurrentRankIndex.insert(std::make_pair(info.bestguildcurrentrankinfo(i).key(), info.bestguildcurrentrankinfo(i).index()));
	}	
	


	m_nChainFlag = info.chainflag();
	m_nEquipSuitLevel = info.equipsuitlevel();

	m_nRegularBossGroupID = info.regularbossgroupid();
	m_nRegularBossGroupCDTime = info.regularbossgroupcdtime();

	m_nNewAchievementStarNum = info.newachievementstarnum();
	for(int i=0; i<info.newachievementrewardrecord_size(); ++i)
	{
		m_nNewAchievementRewardRecord.insert(info.newachievementrewardrecord(i));
	}

	for(int i=0; i<info.firstkillbossrewardrecord_size(); ++i)
	{
		int npcID = info.firstkillbossrewardrecord(i);
		m_nFirstKillBossRewardRecord.insert(npcID);
	}

	for(int i=0; i<info.killbossrecord_size(); ++i)
	{
		int npcID = info.killbossrecord(i).npcid();
		bool state = info.killbossrecord(i).state();
		m_nKillBossRecord.insert(std::make_pair(npcID, state));
	}

	m_nTaskRankIndex = info.taskrankinfo().taskrankindex();
	m_nTaskHonorPoints = info.taskhonorpoint();

	m_nMainTaskFinishCount = info.maintaskfinishcount();
	for(int i=0; i<info.maintaskreachrewardrecord().size(); ++i)
	{
		m_nMainTaskReachRewardRecord.insert(info.maintaskreachrewardrecord(i));
	}

	m_nKillControllerAccountDailyCount = info.killcontrolleraccountdailycount();

	for(int i=0; i<info.gamestoryjointimes_size(); ++i)
	{
		m_nGameStoryJoinTimes.insert(std::make_pair(info.gamestoryjointimes(i).storyid(), info.gamestoryjointimes(i).value()));
	}
	m_nJoinTimeStamp = info.jointimestamp();

	m_nAssignEscortTaskTime = info.assignescorttasktime();
 	m_nEscortTimes = info.escorttimes();
	m_nRobbingTimes = info.robbingtimes();

	m_nYiZhongQinXiState = info.yizhongqinxistate();
	m_nYiZhongQinXiEndTime = info.yizhongqinxiendtime();
	
	LoadAnswerData(info.answerdata());
	LoadSkyArenaData(info.skyarenadata());
	LoadAtvChargeRedPointData(info.atvchargerp());	GetDropTimesAsset().LoadData(info.bossdroptimes());
	GetStarShipTimesAsset().LoadData(info.starshiptimes());	LC_PlayerBase::SetOwnerMaxMP(GetGlobalSetting.nMaxOwnerMP);
	int32_t nDefaultOwnerMP = info.owner_mp();
	if (nDefaultOwnerMP == -1)
		nDefaultOwnerMP = GetGlobalSetting.nInitOwnerMP;
	LC_PlayerBase::SetOwnerMP(nDefaultOwnerMP);


	m_GuildRobInfo.LoadGuildInfo(this, info.db_guild_fight_info());
	m_CollectEquipRecord.LoadCollectEquip(info);

	m_EntrustTask.LoadData( info.entrusttask() );
	m_Lottery.LoadData( info.lotterydata() );
	m_ChargeProfit.LoadData( info.chargeprofit() );

	for( int i = 0 ; i < info.themestatus_size(); ++i )
	{
		int id = info.themestatus(i).key();
		int status = info.themestatus(i).value();
		m_ThemeStatusMap[id] = status;
	}
}

void LC_ServerPlayer::PBSaveCharBaseInfo(char_data_define::char_base_info& info)
{
#if defined SQL_SIZE_ANALYZE
	std::string strDebug;
	int nDebugSize = 0;
	int nLastDebugSize = 0;
	//int nDeltaDebugSize = 0;
	vector<int> nDeltaDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	m_CollectEquipRecord.SaveCollectEquip(info);
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	LC_ActorAttributeMap* pAtrrMap = GetFinalAttributeMap();
	object_id_type nOwnerPlayerId = m_nOwnerPlayerId;
	if ( pAtrrMap == NULL )
	{
		const unique_id_impl& instance = GetInstance();
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "PBSaveCharBaseInfo error! c_cid:%d, c_citizenship:%d, c_uid:%d, attrmap:0x%X", instance.detail(), instance.catagory(), GetUserID(), pAtrrMap);
		return;
	}
	info.set_dailyreward(m_DailyReward);
	for(uint32_t i = 0; i < m_vecTreasureNum.size(); ++i)
	{
		char_data_define::sKeyValueStruct*p_treasurenum = info.add_treasurenum();
		if(p_treasurenum)
		{
			p_treasurenum->set_ukey(m_vecTreasureNum[i].uType);
			p_treasurenum->set_uvalue(m_vecTreasureNum[i].uCount);
		}
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_treasure_mapid(m_nTreasureMapID);
	info.set_treasure_x(m_nTreasurePos.x);
	info.set_treasure_y(m_nTreasurePos.y);
	info.set_treasure_z(m_nTreasurePos.z);

	info.set_showguildpresentname(m_nShowGuildPresentName);
	info.set_skill_levelup_point(m_SkillLevelupPoints);
	info.set_guildrewardisvaild(m_bGuildRewardIsVaild);
	info.set_guildrewardflag(m_bGuildRewardFlag);
	info.set_lotteryconsumeunbindyuanbaocount(m_nLotteryConsumeUnBindYuanBaoCount);
	info.set_dailyactivity(m_nDailyActivity);
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(int i=0;i<m_nDailyActivityRewardID.size(); ++i)
	{
		info.add_dailyactivityrewardid(m_nDailyActivityRewardID[i]);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(int i=0;i<m_nActivateStarVeinID.size(); ++i)
	{
		info.add_activatestarveinid(m_nActivateStarVeinID[i]);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(int i=0; i<m_nStarVeinGoalID.size(); ++i)
	{
		info.add_starveingoalid(m_nStarVeinGoalID[i]);
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_newvipstate(m_bNewVipState);
	info.set_newviplevel(m_nNewVipLevel);
	for(mem::set<int32_t>::iterator it=m_nVipRewardRecord.begin(); it!=m_nVipRewardRecord.end(); ++it)
	{
		info.add_viprewardrecord(*it);
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_finishmaintaskid(m_nFinishMainTaskID);
	info.set_fixmaintaskbug(m_fixMainTaskBug);
	for(mem::map<int32_t, mem::set<int32_t> >::iterator it=m_nFinishGameStoryMapID.begin(); it!=m_nFinishGameStoryMapID.end(); ++it)
	{
		::char_data_define::char_FinishGameStoryMapID* data = info.add_finishgamestorymapid();
		data->set_storyid(it->first);
		for(mem::set<int32_t>::iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
		{
			data->add_mapid(*kIt);
		}	
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_sevendaytrailflag(m_nSevenDayTrailFlag);

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	char_data_define::first_daily_charge_info* FDInfo = info.mutable_firstdailychargeinfo();
	FDInfo->set_fdday(m_nFDFlag);
	for(FirstDailyChargeRewardsMap::iterator it=m_firstDailyChargeRewards.begin(); it!=m_firstDailyChargeRewards.end(); ++it)
	{
		char_data_define::reward_info* record = FDInfo->add_rewardinfo();
		record->set_day(it->second.day);
		record->set_tag(it->second.tag);
		record->set_freerewardid(it->second.freeReward);

		std::vector<PS_RewardRecord>& reward = it->second.disCountRewards;
		for(int i=0; i<reward.size(); ++i)
		{
			record->add_discountrewardid(reward[i].disCountRewardID);
			record->add_discountrewardtime(reward[i].disCountRewardTimes);
		}
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(CollectionIDMap::iterator it1=m_collectionActivationIDMap.begin(); it1!=m_collectionActivationIDMap.end(); ++it1)
	{
		info.mutable_collectiondata()->add_collectionactivationid(it1->first);
	}
	
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(CollectionRewardSet::iterator it2=m_collectionRewardSet.begin(); it2!=m_collectionRewardSet.end(); ++it2)
	{
		info.mutable_collectiondata()->add_collectionrewardid(*it2);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(CollectionRewardSet::iterator it=m_collectionAchievementIDSet.begin(); it!=m_collectionAchievementIDSet.end(); ++it)
	{
		info.mutable_collectiondata()->add_collectionachievementid(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(BiographyRewardSet::iterator it3=m_biographytRewardPartSet.begin(); it3!=m_biographytRewardPartSet.end(); ++it3)
	{
		info.add_biographyrewardid(*it3);
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	/*
	//传记章节完成情况不做存储，每次上线，根据上线时表格数据重新判断
	for(BiographySet::iterator it4=m_biographySet.begin(); it4!=m_biographySet.end(); ++it4)
	{
		info.add_biographyid(*it4);
	}
	*/

	for(AssignBiographySet::iterator it=m_nAssignBiographySet.begin(); it!=m_nAssignBiographySet.end(); ++it)
	{
		::char_data_define::char_assignBiographyInfo* data = info.add_assignbiographyinfo();
		data->set_biographyid(it->first);
		for(mem::set<int>::iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
		{
			data->add_biographypartid(*kIt);
		}
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(FinishBiographyPartSet::iterator it=m_nFinishBiographyPartSet.begin(); it!=m_nFinishBiographyPartSet.end(); ++it)
	{
		info.add_finishbiographypartid(*it);
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(nextAssignBiographyPartSet::iterator it=m_nNextAssignBiographyPartSet.begin(); it!=m_nNextAssignBiographyPartSet.end(); ++it)
	{
		info.add_nextassignbiographypartid(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_checkbiography(m_nCheckBiography);
	info.set_actsubsave(m_bACTSubSave);
	info.set_actsubsave_plus(m_bACTSubSave_Plus);
	info.set_refershrewardtasktimes(m_nRefreshRewardTaskTimes);
	info.set_acceptrewardtasktimes(m_nAcceptRewardTaskTimes);
	info.set_rewardtaskendtime(m_nRewardTaskEndTime);	

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(int i=0; i<m_nYiJiTuXiStageRewardRecord.size(); ++i)
	{
		info.add_yijituxistagerewardrecord(m_nYiJiTuXiStageRewardRecord[i]);
	}
	info.set_yijituxiendtime(m_nYiJiTuXiEndTime);
	info.set_yijituxisendflag(m_nYiJiTuXiSendFlag);

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(int i=0; i<m_nYiJiXunBaoStageRewardRecord.size(); ++i)
	{
		info.add_yijixunbaostagerewardrecord(m_nYiJiXunBaoStageRewardRecord[i]);
	}
	info.set_yijixunbaoendtime(m_nYiJiXunBaoEndTime);
	info.set_yijixunbaosendflag(m_nYiJiXunBaoSendFlag);

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(int i=0; i<m_nPetIslandsStageRewardRecord.size(); ++i)
	{
		info.add_petislandsstagerewardrecord(m_nPetIslandsStageRewardRecord[i]);
	}
	info.set_petislandsendtime(m_nPetIslandsEndTime);
	info.set_petislandssendflag(m_nPetIslandsSendFlag);
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::set<int>::iterator it=m_nResetAllCommonStatID.begin(); it!=m_nResetAllCommonStatID.end(); ++it)
	{
		info.add_resetallcommonstatid(*it);
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, mem::map<int32_t, bool> >::iterator it=m_nDamageRankReward.begin(); it!=m_nDamageRankReward.end(); ++it)
	{
		::char_data_define::char_RankDamageReward* data = info.add_rankdamagereward();
		data->set_mapid(it->first);

		for(mem::map<int32_t, bool>::iterator kIt = it->second.begin(); kIt!=it->second.end(); ++kIt)
		{
			::char_data_define::char_RewardInfo* pInfo = data->add_rewardinfo();
			pInfo->set_rankindex(kIt->first);
			pInfo->set_flag(kIt->second);
		}
	}

	SaveBestGuildAchievementInfo(info.mutable_bestguildachievementinfo());

	SaveHuanJingData(info.mutable_huanjingdata());

	info.set_killnpcexpflag(m_bKillNPCExpFlag);
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	m_QuickRaidAsset.SaveData(info.mutable_quickraiddata());

	SaveRongGuangYuanZhengData(info.mutable_rongguangyuanzheng());
	
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	LC_SocietyAsset* pkSociety = GetSocietyAsset();
	mem::vector<unique_id_type> BeFriendIDs;
	pkSociety->GetBeFriendID(BeFriendIDs);
	for(int i=0; i<BeFriendIDs.size(); ++i)
	{
		info.add_befriendid(BeFriendIDs[i]);
	}

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(int i=0; i<m_nTimeLimitedRecord.size(); ++i)
	{
		char_data_define::char_timelimitedinfo* data = info.add_timelimitedinfo();
		data->set_id(m_nTimeLimitedRecord[i].id);
		data->set_endtimestamp(m_nTimeLimitedRecord[i].endTimeStamp);
		data->set_state(m_nTimeLimitedRecord[i].state);
		data->set_shoptimelimitedid(m_nTimeLimitedRecord[i].shopTimeLimitedID);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int, int>::iterator it=m_ShopTimes.begin(); it!=m_ShopTimes.end(); ++it)
	{
		char_data_define::char_shopResetRecoder* data = info.add_shopresetrecoder();
		data->set_shoptypeid(it->first);
		data->set_times(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int, int>::iterator it=m_ShopStartTime.begin(); it!=m_ShopStartTime.end(); ++it)
	{
		char_data_define::char_shopStartTime* data = info.add_shopstarttime();
		data->set_shoptypeid(it->first);
		data->set_starttime(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, int32_t>::iterator it=m_nUnbindYuanbaoShopItemInfo.begin(); it!=m_nUnbindYuanbaoShopItemInfo.end(); ++it)
	{
		char_data_define::char_shopItemInfo* data = info.add_unbindyuanbaoshopiteminfo();
		data->set_indexid(it->first);
		data->set_shopaid(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_bossjpcollectcount(m_nCollectBossJP);

	for(mem::map<int32_t, int32_t>::iterator it=m_nGuildShopItemInfo.begin(); it!=m_nGuildShopItemInfo.end(); ++it)
	{
		char_data_define::char_shopItemInfo* data = info.add_guildshopiteminfo();
		data->set_indexid(it->first);
		data->set_shopaid(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, int32_t>::iterator it=m_nMoneyCourageShopItemInfo.begin(); it!=m_nMoneyCourageShopItemInfo.end(); ++it)
	{
		char_data_define::char_shopItemInfo* data = info.add_moneycourageshopiteminfo();
		data->set_indexid(it->first);
		data->set_shopaid(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, int32_t>::iterator it=m_nBossJPShopItemInfo.begin(); it!=m_nBossJPShopItemInfo.end(); ++it)
	{
		char_data_define::char_shopItemInfo* data = info.add_bossjpshopiteminfo();
		data->set_indexid(it->first);
		data->set_shopaid(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, int32_t>::iterator it=m_nLimitShopItemInfo.begin(); it!=m_nLimitShopItemInfo.end(); ++it)
	{
		char_data_define::char_shopItemInfo* data = info.add_limitshopiteminfo();
		data->set_indexid(it->first);
		data->set_shopaid(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	for(mem::set<int32_t>::iterator it=m_nInvestmentCardRecord.begin(); it!=m_nInvestmentCardRecord.end(); ++it)
	{
		::char_data_define::char_investmentCardRecord* data = info.add_investmentcardrecord();
		data->set_investmentcardtype(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, int32_t>::iterator it=m_nInvestmentCardStartTime.begin(); it!=m_nInvestmentCardStartTime.end(); ++it)
	{
		::char_data_define::char_investmentCardStartTime* data = info.add_investmentcardstarttime();
		data->set_investmentcardtype(it->first);
		data->set_starttime(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::set<int32_t>::iterator it=m_nInvestmentCardFreeRewardID.begin(); it!=m_nInvestmentCardFreeRewardID.end(); ++it)
	{
		info.add_freerewardachievementid(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::set<int32_t>::iterator it=m_nInvestmentCardHighRewardID.begin(); it!=m_nInvestmentCardHighRewardID.end(); ++it)
	{
		info.add_highrewardachievementid(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::set<int32_t>::iterator it=m_nSTRewardRecord.begin(); it!=m_nSTRewardRecord.end(); ++it)
	{
		info.add_strewardrecord(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int, uint64_t>::iterator it=m_nSubBInfo.begin(); it!=m_nSubBInfo.end(); ++it)
	{
		::char_data_define::char_subBInfo* data = info.add_subinfo();
		data->set_groupid(it->first);
		data->set_value(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::set<int>::iterator it=m_nRankGroupID.begin(); it!=m_nRankGroupID.end(); ++it)
	{
		info.add_rankgroupid(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int, uint64_t>::iterator it=m_nSendSubInfo.begin(); it!=m_nSendSubInfo.end(); ++it)
	{
		::char_data_define::char_subBInfo* data = info.add_sendsubinfo();
		data->set_groupid(it->first);
		data->set_value(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif


	info.set_profilepictureid(m_nProfilePictureID);

	for(mem::map<int32_t, bool>::iterator it=m_nProfilePictureRecord.begin(); it!=m_nProfilePictureRecord.end(); ++it)
	{
		::char_data_define::char_profilePictureRecord* data = info.add_profilepicturerecords();
		data->set_pictureid(it->first);
		data->set_tag(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_commandertitle(m_nCommanderTitle);

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	SaveSubSkillInfo(info.mutable_subskillinfo());

	info.set_newvipdroptime(m_nNewVipDropTime);
	for(mem::map<int, int>::iterator it=m_nVIPDropItemInfo.begin(); it!=m_nVIPDropItemInfo.end(); ++it)
	{
		 ::char_data_define::char_vipdropitem* data = info.add_vipdropitem();
		 data->set_itemid(it->first);
		 data->set_itemnum(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_currentcommandertitleid(m_nCurrentCommanderTitleFrameID);
	for(mem::map<int32_t, bool>::iterator it=m_nCommanderTitleFrameIDRecord.begin(); it!=m_nCommanderTitleFrameIDRecord.end(); ++it)
	{
		::char_data_define::char_activateCommanderTitleRecord* data = info.add_activatecommandertitlerecords();
		data->set_titleid(it->first);
		data->set_tag(it->second);
	}
	
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif


	//info.set_bestguildendtime(m_nBestGuildEndTime);
	//info.set_bestguildstageendtime(m_nBestGuildStageEndTime);
	//info.set_bestguildgamestoryid(m_nBestGuildGameStoryID);

	

	for(mem::map<int32_t, uint32_t>::iterator it=m_nStoryAssignTaskTime.begin(); it!=m_nStoryAssignTaskTime.end(); ++it)
	{
		::char_data_define::char_StoryAssignTaskTime* data = info.add_storyassigntasktime();
		data->set_storyid(it->first);
		data->set_time(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<uint64_t, int32_t>::iterator it=m_nPlayerLastRankIndex.begin(); it!=m_nPlayerLastRankIndex.end(); ++it)
	{
		::char_data_define::char_BestGuildRankInfo* data = info.add_bestguildlastrankinfo();
		data->set_key(it->first);
		data->set_index(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<uint64_t, int32_t>::iterator it=m_nPlayerCurrentRankIndex.begin(); it!=m_nPlayerCurrentRankIndex.end(); ++it)
	{
		::char_data_define::char_BestGuildRankInfo* data = info.add_bestguildcurrentrankinfo();
		data->set_key(it->first);
		data->set_index(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, uint32_t>::iterator it=m_nBestGuildActivityEndTime.begin(); it!=m_nBestGuildActivityEndTime.end(); ++it)
	{
		::char_data_define::char_BestGuildActivityEndTime* data = info.add_bestguildactivityendtime();
		data->set_storyid(it->first);
		data->set_endtime(it->second);
	}

	for(mem::map<int32_t, int32_t>::iterator it=m_nAssignBestGuildTaskEndTime.begin(); it!=m_nAssignBestGuildTaskEndTime.end(); ++it)
	{
		::char_data_define::char_AssignBestGuildTaskEndTime* data = info.add_assignbestguildtaskendtime();
		data->set_scheduleid(it->first);
		data->set_endtime(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_chainflag(m_nChainFlag);
	info.set_equipsuitlevel(m_nEquipSuitLevel);

	info.set_regularbossgroupid(m_nRegularBossGroupID);
	info.set_regularbossgroupcdtime(m_nRegularBossGroupCDTime);

	for(mem::set<int32_t>::iterator it=m_nFirstKillBossRewardRecord.begin(); it!=m_nFirstKillBossRewardRecord.end(); ++it)
	{
		info.add_firstkillbossrewardrecord(*it);
	}

	for(mem::map<int32_t, bool>::iterator it=m_nKillBossRecord.begin(); it!=m_nKillBossRecord.end(); ++it)
	{
		::char_data_define::char_KillBossRecord* data = info.add_killbossrecord();
		data->set_npcid(it->first);
		data->set_state(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	for(mem::map<int32_t, int32_t>::iterator it=m_nHolyDeedRecords.begin(); it!=m_nHolyDeedRecords.end(); ++it)
	{
		char_data_define::char_holydeedinfo* data = info.add_holydeedinfo();
		data->set_id(it->first);
		data->set_count(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_newachievementstarnum(m_nNewAchievementStarNum);
	for(mem::set<int>::iterator it=m_nNewAchievementRewardRecord.begin(); it!=m_nNewAchievementRewardRecord.end(); ++it)
	{
		info.add_newachievementrewardrecord(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_autodecomposition(m_bAutoDecomposition);
	info.set_decompositioninit(true);
	info.set_decompositioncontroller(m_bDecompositionController);
	info.set_controlleritemlevel(m_nControllerItemlevel);
	info.set_controlleritemgrade(m_nControllerItemGrade);
	info.set_decompositionvehicle(m_bDecompositionVehicle);
	info.set_vehicleitemlevel(m_nVehicleItemLevel);
	info.set_vehicleitemgrade(m_nVehicleItemGrade);
	info.set_decompositiondevaeye(m_bDecompositionDevaeye);
	info.set_devaeyeitemlevel(m_nDevaeyeItemLevel);
	info.set_devaeyeitemgrade(m_nDevaeyeItemGrade);
	info.set_decompositionwing(m_bDecompositionWing);
	info.set_wingitemlevel(m_nWingItemLevel);
	info.set_wingitemgrade(m_nWingItemGrade);
	for(int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
	{
		for(int j=0; j<m_nSkillBookSlotDataInfo[i].size(); ++j)
		{
			char_data_define::skillbook_slotdata* data  = info.add_skillbookslotdata();
			data->set_buddyindex(i);
			data->set_slotid(m_nSkillBookSlotDataInfo[i][j].m_nSlotID);
			data->set_slotstate(m_nSkillBookSlotDataInfo[i][j].m_nSlotState);
			data->set_skillbookid(m_nSkillBookSlotDataInfo[i][j].m_nSkillBookID);
		}
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_firstchargetime(m_nFirstChargeTime);
	for(mem::map<int, bool>::iterator it=m_nFirstChargeRewardState.begin(); it!=m_nFirstChargeRewardState.end(); ++it)
	{
		::char_data_define::char_FirstChargeRewardState* data = info.add_firstchargerewardstate();
		data->set_id(it->first);
		data->set_state(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_usedvipextrarewardtimes(m_nUsedVIPExtraRewardTimes);

	info.mutable_taskrankinfo()->set_taskrankindex(m_nTaskRankIndex);
	info.set_taskhonorpoint(m_nTaskHonorPoints);

	info.set_maintaskfinishcount(m_nMainTaskFinishCount);
	for(mem::set<int32_t>::iterator it=m_nMainTaskReachRewardRecord.begin(); it!=m_nMainTaskReachRewardRecord.end(); ++it)
	{
		info.add_maintaskreachrewardrecord(*it);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_killcontrolleraccountdailycount(m_nKillControllerAccountDailyCount);

	for(mem::map<int, int>::iterator it=m_nGameStoryJoinTimes.begin(); it!=m_nGameStoryJoinTimes.end(); ++it)
	{
		::char_data_define::char_GameStoryJoinTimes* data = info.add_gamestoryjointimes();
		data->set_storyid(it->first);
		data->set_value(it->second);
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_jointimestamp(m_nJoinTimeStamp);

	info.set_assignescorttasktime(m_nAssignEscortTaskTime);
	info.set_escorttimes(m_nEscortTimes);
	info.set_robbingtimes(m_nRobbingTimes);


	info.set_yizhongqinxistate(m_nYiZhongQinXiState);
	info.set_yizhongqinxiendtime(m_nYiZhongQinXiEndTime);
	
	SaveAnswerData(info.mutable_answerdata());
	SaveSkyArenaData(info.mutable_skyarenadata());
	SaveAtvChargeRedPointData(info.mutable_atvchargerp());	
	GetDropTimesAsset().SaveData(info.mutable_bossdroptimes());
	GetStarShipTimesAsset().SaveData(info.mutable_starshiptimes());
#if defined SQL_SIZE_ANALYZE
	 info.SerializeToString(&strDebug);
	 nDebugSize = strDebug.size();
	 nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	 nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	byte* tutor_buff = m_kTutorialAsset.GetDataBuff();
	for (int i = 0; i < MG_TUTORIALDATA_LENGTH; ++i)
	{
		info.mutable_tutorial_info()->Add(tutor_buff[i]);
	}
	info.set_is_show_fashion(m_bIsShowFashion);
	info.set_last_map_res_id(this->GetMapRaodmap());
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	info.set_pk_value(GetPKRecorder()->GetPKValue());
	info.set_owner_mp(LC_PlayerBase::GetOwnerMP());
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

#if 0
	// Anandon code: use SkillAsset storage.
	const LC_SkillAssetEntryMap& kSkillAssetEntryMap = GetSkillAsset()->GetSkillAssetData()->GetSkillAssetEntryMap();
	LC_SkillAssetEntryMap::const_iterator cit = kSkillAssetEntryMap.begin();
	for (; cit != kSkillAssetEntryMap.end(); ++cit)
	{
		if (!cit->second.GetValid())
		{
			continue;
		}
		char_data_define::skill_entry_summary* psummary = info.add_skills();

		psummary->set_type_id(cit->second.GetTypeID());
		psummary->set_slot_idx(cit->second.GetSlotIndex());
	}
#endif
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	//保存盟主信息
	//int position = 0;

	//info.set_powerguild(0);
	//info.set_crossrealmpowerguild(0);
//
	//LC_ServerLogicManager* logicMgr = SERVER_GET_LOGIC_MANAGER();
	////if(logicMgr != NULL)
	//{
		//LC_WorldGuildWarAsset* pWorldGuildWarAsset = logicMgr->GetGuildWarAssert();
		//if(pWorldGuildWarAsset != NULL)
		//{
			//Protocol::PS_PowerGuildInfo* power = pWorldGuildWarAsset->GetPlayerPowerGuild(this->GetInstance(), position);
			//if (power != NULL && position == GUILD_MEMBER_MASTER)
			//{
				//info.set_powerguild(1);
			//}
//
			//Protocol::PS_CrossRealm_PowerGuildInfo* globalpower = pWorldGuildWarAsset->GetPlayerGlobalPowerGuild(this->GetInstance());
			//if (globalpower != NULL)
			//{
				//info.set_crossrealmpowerguild(1);
			//}
		//}
	//}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	info.set_login_count(m_LoginCount);
	info.set_total_onlinetime(m_TotalOnlineTime);

#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif
	/*
	*/

	LC_WorldLocationAssetEntry* pkPermanentEntry = m_kWorldLocationAsset.GetWorldLocationAssetEntryByIndex(WLT_PERMANENT);
	if (pkPermanentEntry && pkPermanentEntry->GetValid())
	{
		info.set_cur_map_res_id(pkPermanentEntry->GetMapResID());
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	WeaponType iWeaponType = GetCurrentBandWeapon();
	for (int i = 0; i < FT_MAX_COUNT; ++i)
	{
		info.mutable_show_fashions()->Add(m_kFashionBagAsset.GetShowFashionItems(i, iWeaponType));
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	const platform_vip_info& pf_info = GetPlatformVipInfo();
	for(platform_vip_info::const_iterator iter = pf_info.begin(); iter != pf_info.end(); iter++)
	{
		const platform_node& rPfNode = iter->second;
		char_data_define::platform_info* pkentry = info.add_platforms();
		if (pkentry)
		{
			pkentry->set_name(iter->first.c_str());
			pkentry->set_detail(rPfNode.format_vip_info());
		}
	}
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	m_GuildRobInfo.SaveGuildInfo(this, *info.mutable_db_guild_fight_info());
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	m_EntrustTask.SaveData( info.mutable_entrusttask() );
	m_Lottery.SaveData( info.mutable_lotterydata() );
	m_ChargeProfit.SaveData( info.mutable_chargeprofit() );
	
#if defined SQL_SIZE_ANALYZE
	info.SerializeToString(&strDebug);
	nDebugSize = strDebug.size();
	nDeltaDebugSize.push_back( nDebugSize - nLastDebugSize);
	nLastDebugSize = nDebugSize;
#endif

	{
		mem::map<int,int>::iterator it = m_ThemeStatusMap.begin();
		while( it != m_ThemeStatusMap.end() )
		{
			char_data_define::PBKeyValue* k = info.add_themestatus();
			if( k )
			{
				k->set_key( it->first );
				k->set_value( it->second );
			}
			++it;
		}
	}
#if defined SQL_SIZE_ANALYZE
	int32_t nTotalSize = 0;
	for ( int i = 0; i < nDeltaDebugSize.size(); i++ )
	{
		nTotalSize += nDeltaDebugSize[i];
	}
	printf( "base total: %d\n", nTotalSize );
#endif
	return;
}


void LC_ServerPlayer::PBLoadCharDetailInfo(const char_data_define::char_detail_info& info)
{
	//info.verison();?

	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	m_DailyTimeStamp = info.daily_timestamp();
	//m_DailyTimeStamp = 0;

	SetEnterServerMode(info.enter_server_mode());

	SetSpawnMapID(info.spawn_location().id());
	UT_Vec3Int pos(info.spawn_location().x(), info.spawn_location().y(), info.spawn_location().z());
	SetSpawnLocation(pos);
	SetCurrentLocation(pos);
	SetTargetLocation(pos);
	UT_Vec3Int vecDefaultPos = UT_Vec3Int(info.default_location().x(), info.default_location().y(), info.default_location().z());
	SetDefaultTransferPos(info.default_location().id(), vecDefaultPos);
	//if (timestamp >= (uint32_t) m_iLastOfflineTime + 5 * ANTIWALLOW_INTERVAL)
	if (timestamp >= (uint32_t) m_iLastOfflineTime + GetGlobalSetting.nWallowTimeout_None)
	{
		SetAnitWallowOnlineTime(0);
	}
	else
	{
		SetAnitWallowOnlineTime(info.antiwallow_online_time());
	}

	SetAntiWallowIntervalTime((int)GET_PROCESS_TIME);
// 	for (int i = 0 ; i < info.vehicle_buffer_seq_size() && i < 8; ++i)
// 	{
// 		m_VehicleBufferSeq[i] = (unsigned char)info.vehicle_buffer_seq(i);
// 	}

	SetCharMood(info.char_mood().c_str());
	SetGuildMoney(info.guild_money());
	SetTotalGuildMoney(info.guild_totalmoney());
	SetCouragePoint(info.couragepoint());
	SetGuildAutoAdd(info.guild_autoadd());
	m_VigorStamp = info.vigor_timestamp();
	//SetGlobalMailID(info.globalmailid());
	SetGagTime(info.gagtime());
	SetInternal(info.binternal());
	m_killelitecount = info.elitecount();

	// 跳闪点数
	m_JumpDodgePoint = info.jumpdodgepoint();

	// 创角返还记录
	m_userGiftReturn = info.user_return_gift();

	for (int i = 0; i < info.oss_res_cache_size(); ++i)
	{
		LC_ResourceRecord record;
		record.Load(info.oss_res_cache(i));
		mem::map<int64_t, LC_ResourceRecord>::iterator it = m_kOssResourceCache.find(record.GetUniqueID());
		if (it != m_kOssResourceCache.end())
		{
			it->second.AddCount(record.GetCount());
		}
		else
		{
			m_kOssResourceCache[record.GetUniqueID()] = record;
		}
	}

	m_kRechargeRecord.m_iLastUpdateTime = info.recharge_return().last_update_time();
	m_kRechargeRecord.m_iRecharge = info.recharge_return().recharge();

	//资源预警
	m_kResourceAlert.Load(info.alert_info());

	m_iBulletinTime = info.bulletin_time();

	m_kFacebookRecord.Load(info);

	m_iLastRechargeTime = info.last_recharge_time();
}

void LC_ServerPlayer::PBSaveCharDetailInfo(char_data_define::char_detail_info& info)
{
	info.set_daily_timestamp(m_DailyTimeStamp);
	
	char_data_define::map_location* location = info.mutable_spawn_location();
	location->set_id(m_pkMap ? m_pkMap->GetMapResID() : 1);
	location->set_x(m_kCurrentLocation.x);
	location->set_y(m_kCurrentLocation.y);
	location->set_z(m_kCurrentLocation.z);

	location = info.mutable_default_location();
	location->set_id(m_iDefaultTransferMapID);
	location->set_x(m_vecDefaultTransferPos.x);
	location->set_y(m_vecDefaultTransferPos.y);
	location->set_z(m_vecDefaultTransferPos.z);

	info.set_antiwallow_online_time(GetAntiWallowOnlineTime());
	info.set_init_char_data_version(1);
	m_nCharDataVersion = 1;
// 	for (int i = 0; i < 8; ++i)
// 	{
// 		info.mutable_vehicle_buffer_seq()->Add(m_VehicleBufferSeq[i]);
// 	}

	info.set_enter_server_mode( GetEnterServerMode() );
	info.set_char_mood(GetCharMood().c_str());
	info.set_guild_money(GetGuildMoney());
	info.set_guild_totalmoney(GetTotalGuildMoney());
	info.set_guild_autoadd(IsGuildAutoAdd());
	info.set_couragepoint((int32_t)GetCouragePoint());
	info.set_vigor_timestamp(m_VigorStamp);
	//info.set_globalmailid(GetGlobalMailID());
	info.set_gagtime(GetGagTime());
	info.set_binternal(GetInternal());
	info.set_elitecount(m_killelitecount);

	// 跳闪点数
	info.set_jumpdodgepoint(m_JumpDodgePoint);

	// 用户创角返还记录
	info.set_user_return_gift(m_userGiftReturn);

	for (mem::map<int64_t, LC_ResourceRecord>::iterator it = m_kOssResourceCache.begin();
		it != m_kOssResourceCache.end(); ++it)
	{
		char_data_define::resource_record* record_info = info.add_oss_res_cache();
		it->second.Save(*record_info);
	}

	//充值返还
	char_data_define::recharge_record* recharge = info.mutable_recharge_return();
	recharge->set_last_update_time(m_kRechargeRecord.m_iLastUpdateTime);
	recharge->set_recharge(m_kRechargeRecord.m_iRecharge);

	//资源预警
	m_kResourceAlert.Save(*info.mutable_alert_info());

	info.set_bulletin_time(m_iBulletinTime);

	m_kFacebookRecord.Save(info);

	info.set_last_recharge_time(m_iLastRechargeTime);
}

//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadCharWorldLocationInfo(const char_data_define::world_location_asset& info)
{
	m_kWorldLocationAsset.GetDataFromPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBSaveCharWorldLocationInfo(char_data_define::world_location_asset& info)
{
	m_kWorldLocationAsset.SetDataToPB(info);
}

void LC_ServerPlayer::PBSaveBuddyAutoRespawnTimer( ::google::protobuf::RepeatedField< float > & vecData )
{
	for ( int i = 0; i < MAX_ACTIVE_BUDDY_NUM; i++ )
	{
		vecData.Add(m_fBuddyAutoRespawnTimer[i]);
	}
}

void LC_ServerPlayer::PBLoadBuddyAutoRespawnTimer( const ::google::protobuf::RepeatedField< float > & vecData )
{
	int nSize = vecData.size();
	if (nSize > MAX_ACTIVE_BUDDY_NUM)
		nSize = MAX_ACTIVE_BUDDY_NUM;
	for ( int i = 0; i < nSize; i++ )
	{
		m_fBuddyAutoRespawnTimer[i] = vecData.Get(i);
	}
}

//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadCharSkillInfo(const char_data_define::skill_asset_info& info)
{
	//return;
	//获取技能信息
	LC_SkillAsset* pkSkillAsset = GetControllerSkillAsset();
	if (NULL != pkSkillAsset)
	{
		pkSkillAsset->GetDataFromPB(info);
	}

	//加属性
	LC_SkillAssetData* pkSkillAssetData = pkSkillAsset->GetSkillAssetData();
	const LC_SkillAssetEntryMap& SkillAssetEntry = pkSkillAssetData->GetSkillAssetEntryMap();
	for(LC_SkillAssetEntryMap::const_iterator it=SkillAssetEntry.begin(); it!=SkillAssetEntry.end(); ++it)
	{
		LC_SkillAssetEntry* pkSkillEntry = const_cast<LC_SkillAssetEntry*>(&(it->second));
		ModifyControllerAttributeBySkill(pkSkillEntry, it->first, true);
	}

#if !defined SKILL_CD_NEW_IMPL
	//根据技能信息构造CD表
	BuildSkillCoolDownMap();
	BuildTransformSkillCoolDownMap();
	//获取技能CD信息
	LC_SkillCoolDownMap* pkSkillCDMap = GetSkillCoolDownMap();
	if (NULL != pkSkillCDMap)
	{
		pkSkillCDMap->GetCDFromPB(info);
	}
#endif
}
//----------------------------------------------------------------------------
BuddyInstance* LC_ServerPlayer::LoadBuddyData_SingleInstance( const CSVFile::CF_Buddy::DataEntry* pTemplate, bool bCreate)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return NULL;

	BuddyInstance* pInstance = T_NEW_D BuddyInstance(m_nOwnerPlayerId);
	pInstance->InitTemplateData(pTemplate, bCreate);

	return pInstance;
}

void LC_ServerPlayer::LoadBuddyData_Usable()
{
#if 0
	CF_Buddy* pBuddyConfig = CF_Buddy::GetSingletonPtr();
	if (pBuddyConfig == NULL)
		return;

	CF_Buddy::DataEntryMapExternIterator it = pBuddyConfig->GetIterator();
	while( it.HasMoreElements() )
	{
		const CF_Buddy::DataEntry* pkData = it.PeekNextValuePtr();
		int32_t nTemplateId = pkData->_cID;
		if (!CheckActivateBuddyByTemplateID(nTemplateId))
		{
			SingleBuddyData data;
			if (nTemplateId < 5)
				data.bUnlock = true;
			else
				data.bUnlock = false;
			data.nLevel = 1;
			data.nBuddyTemplateId = nTemplateId;
			m_ppUsableBuddyData.push_back(data);
		}

		it.MoveNext();
	}
#endif
	return;
}

void LC_ServerPlayer::LoadBuddyData_Activate()
{
	CF_Buddy* pBuddyConfig = CF_Buddy::GetSingletonPtr();
	int32_t nDefaultBuddyTID[MAX_ACTIVE_BUDDY_NUM] = {};

	nDefaultBuddyTID[0] = this->GetCharType();
	int32_t nPrepareBuddyTID[4] = {1,2,3,4};
	int nCount = 0;
	int nStepResult = -1;
	for (int i = 1; i < MAX_ACTIVE_BUDDY_NUM; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			for (int j = 0; j < MAX_ACTIVE_BUDDY_NUM; j++)
			{
				nStepResult = nPrepareBuddyTID[k];
				if ( nPrepareBuddyTID[k] == nDefaultBuddyTID[j] )
					nStepResult = -1;

				if  (nStepResult == -1)
					break;
			}
			if (nStepResult != -1)
				break;
		}

		if (nStepResult != -1)
			nDefaultBuddyTID[i] = nStepResult;
	}

	for (int32_t i = 0; i < m_nUnlockedActivateBuddyCount; i++)
	{
		CF_Buddy::DataEntry* pData = pBuddyConfig->GetEntryPtr(nDefaultBuddyTID[i]);
		if (pData == NULL)
			continue;
		m_ppActivateBuddyTemplateId[i] = nDefaultBuddyTID[i];
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_SYSTEM, "Buddy Load: <%d> %s", pData->_cID, pData->_roleName);
	}

	return;
}

//----------------------------------------------------------------------------
void LC_ServerPlayer::PBSaveCharSkillInfo(char_data_define::skill_asset_info& info)
{
	//return;
	LC_SkillAsset* pkSkillAsset = GetControllerSkillAsset();
	LC_SkillCoolDownMap* pkSkillCDMap = GetSkillCoolDownMap();
	if (NULL != pkSkillAsset && NULL != pkSkillCDMap)
	{
		pkSkillAsset->SetDataToPB(info);
		pkSkillCDMap->SetCDToPB(info);
	}
}

void LC_ServerPlayer::PBSavePetDetailData(char_data_define::pet_detail_data & detail_data)
{
	m_PetInstanceMgr.SaveAll( detail_data );
}

void LC_ServerPlayer::PBLoadPetDetailData(const char_data_define::pet_detail_data & detail_data)
{
	m_PetInstanceMgr.LoadAll( detail_data );
}

void LC_ServerPlayer::PBSaveBotDetailData(char_data_define::bot_detail_data & detail_data)
{
	//m_BotInstanceMgr.SaveAll( detail_data );
}

void LC_ServerPlayer::PBLoadBotDetailData(const char_data_define::bot_detail_data & detail_data)
{
	//m_BotInstanceMgr.LoadAll( detail_data );
}

void LC_ServerPlayer::PBSaveNewVIPData(char_data_define::new_vip_reward & detail_data)
{
	m_NewVIPReward.Save(detail_data);
}

void LC_ServerPlayer::PBLoadNewVIPData(const char_data_define::new_vip_reward & detail_data)
{
	m_NewVIPReward.Load(detail_data);
}

void LC_ServerPlayer::PBSaveBuddyChain(char_data_define::buddy_chain_data & detail_data)
{
	m_BuddyChain.Save(detail_data);
}

void LC_ServerPlayer::PBLoadBuddyChain(const char_data_define::buddy_chain_data & detail_data)
{
	m_BuddyChain.Load( detail_data );
	//OnBuddyChainEvent_Load();
}

void LC_ServerPlayer::PBSaveBuddySummaryData(char_data_define::buddy_summary_data & info)
{
	info.set_current_index( m_nCurrentBuddyIndex );
	for (int i = 0; i < MAX_ACTIVE_BUDDY_NUM; i++)
	{
		char_data_define::buddy_summary_single_data* pData = info.add_activate_data();
		LC_ServerPlayer* pResultPlayer = GetActivateBuddyByIndex(i);
		IF_A_NA_PLAYER(pResultPlayer) 
		{
			pData->set_is_alive(0);
			pData->set_buddy_id(INVALID_ACTOR_ID);
			pData->set_level(0);
		}
		else
		{
			bool bIsAlive = !pResultPlayer->GetDead();
			pData->set_is_alive(bIsAlive);
			int nBuddyId = pResultPlayer->GetBuddyID();
			pData->set_buddy_id(nBuddyId);
			int nLevel = pResultPlayer->GetBuddyLevel();
			pData->set_level(nLevel);
		}
	}
	for (int i = 0; i < m_ppUsableBuddyData.size(); i++)
	{
		if (m_ppUsableBuddyData[i].bUnlock == false || m_ppUsableBuddyData[i].nBuddyTemplateId == 0)
			continue;
		char_data_define::buddy_summary_single_data* pData = info.add_usable_data();
		pData->set_is_alive(true);
		int nBuddyId = m_ppUsableBuddyData[i].nBuddyTemplateId;
		pData->set_buddy_id(nBuddyId);
		int nLevel = m_ppUsableBuddyData[i].nLevel;
		pData->set_level(nLevel);
	}

}

void LC_ServerPlayer::PBLoadBuddySummaryData(const char_data_define::buddy_summary_data & summary, const char_data_define::buddy_detail_data & detail )
{
	m_nCurrentBuddyIndex = summary.current_index();
	if (m_nCurrentBuddyIndex < 0)
		m_nCurrentBuddyIndex = 0;
	PBLoadBuddyData_Activate(summary);
	PBLoadBuddyData_Usable(summary);
	PBLoadBuddyData_Detail( summary, detail );
	RecoverBuddyListData();
	LoadBuddyData_Locked();
}

void LC_ServerPlayer::PBLoadBuddyData_Usable(const char_data_define::buddy_summary_data& info)
{
	for (int i=0; i<info.usable_data_size(); ++i)
	{
		const char_data_define::buddy_summary_single_data& data = info.usable_data(i);
		CF_Buddy::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(data.buddy_id());
		if ( pkData == NULL )
			continue;
		if (data.buddy_id() == INVALID_ACTOR_ID)
			continue;
		SingleBuddyData single_data;
		single_data.nLevel = data.level();
		single_data.nBuddyTemplateId = data.buddy_id();
		single_data.bUnlock = true;
		//m_ppUsableBuddyData.push_back(single_data);
		m_ppUsableBuddyData.insert(std::make_pair(single_data.nBuddyTemplateId, single_data));
	}
	return;
}

void LC_ServerPlayer::PBLoadBuddyData_Activate(const char_data_define::buddy_summary_data& info)
{
	LC_ServerPlayerEnterLeaveManager* pkPlayerEnterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (info.activate_data_size() == 0 && m_ppActivateBuddyTemplateId[0] == INVALID_ACTOR_ID)
	{
		if(pkPlayerEnterLeaveManager)
		{
			pkPlayerEnterLeaveManager->UserLeaveServer((LC_ServerPlayer*)this,LMIOT_KICKOFF);
		}
		return;
	}
	for (int i=0; i<info.activate_data_size(); ++i)
	{
		const char_data_define::buddy_summary_single_data& data = info.activate_data(i);
		if ( i > 0 && CheckDuplicatedBuddy(&m_ppActivateBuddyTemplateId[0], data.buddy_id()) )
		{
			continue;
		}
		m_ppActivateBuddyTemplateId[i] = data.buddy_id();
		bool bAlive = data.is_alive();
		int16_t nBuddyId = data.buddy_id();
		int16_t nBuddyLevel = data.level();
	}

	m_nBuddyTemplateId = m_ppActivateBuddyTemplateId[0];
	// Recover logic
	bool bDataValidate = true;
	if (m_ppActivateBuddyTemplateId[0] == INVALID_ACTOR_ID)
	{
		bDataValidate = false;
		int nLoop = 0;
		for ( ; nLoop < MAX_ACTIVE_BUDDY_NUM; nLoop++)
		{
			if (m_ppActivateBuddyTemplateId[nLoop] != INVALID_ACTOR_ID)
			{
				bDataValidate = true;
				m_ppActivateBuddyTemplateId[0] = m_ppActivateBuddyTemplateId[nLoop];
				m_ppActivateBuddyTemplateId[nLoop] = INVALID_ACTOR_ID;
				m_nBuddyTemplateId = m_ppActivateBuddyTemplateId[0];
				m_nCurrentBuddyIndex = 0;
				break;
			}
		}
	}
	m_kCharType.set(m_nBuddyTemplateId);
	if ( m_ppActivateBuddyTemplateId[m_nCurrentBuddyIndex] == INVALID_ACTOR_ID )
		m_nCurrentBuddyIndex = 0;
	if ( bDataValidate == false )
	{
		if(pkPlayerEnterLeaveManager)
		{
			pkPlayerEnterLeaveManager->UserLeaveServer((LC_ServerPlayer*)this,LMIOT_KICKOFF);
		}
		return;
	}
	if (info.activate_data_size() == 0)
	{
		m_ppActivateBuddyTemplateId[0] = this->GetCharType();
	}
}

void LC_ServerPlayer::PBLoadBuddyData_Detail(const char_data_define::buddy_summary_data & summary, const char_data_define::buddy_detail_data & detail )
{
	CF_Buddy* pBuddyConfig = CF_Buddy::GetSingletonPtr();
	int nDataIndex = 0;
	for ( int i = 0; i < m_ppUsableBuddyData.size(); i++ )
	{
		if (m_ppUsableBuddyData[i].bUnlock == false)
			continue;
		if (m_ppUsableBuddyData[i].nBuddyTemplateId == INVALID_ACTOR_ID)
			continue;
		const CF_Buddy::DataEntry* pkData = pBuddyConfig->GetEntryPtr(m_ppUsableBuddyData[i].nBuddyTemplateId);
		if (pkData == NULL)
			continue;
		bool bInit = false;
		if ( m_nCharDataVersion == 0 )
			bInit = true;
		BuddyInstance* pBuddy = CreateSingleBuddyInstance(pkData, bInit);
		if (pBuddy == NULL)
			continue;

		pBuddy->LoadDBData(summary.usable_data(nDataIndex));//, detail.data(nDataIndex));
		nDataIndex++;
	}

	nDataIndex = 0;
	int nSummarySize = summary.activate_data_size();
	//m_ppActivateBuddyTemplateId
	for (int i = 0; i < m_ppActivateBuddyTemplateId.size(); i++)
	{
		int32_t nBuddyId = m_ppActivateBuddyTemplateId[i];
		if (nBuddyId == INVALID_ACTOR_ID)
			continue;
		BuddyInstance* pInstance = GetBuddyInstanceByID(nBuddyId);
		if ( pInstance )
			continue;
		const CF_Buddy::DataEntry* pkData = pBuddyConfig->GetEntryPtr(nBuddyId);
		if (pkData == NULL)
			continue;

		bool bInit = false;
		if ( m_nCharDataVersion == 0 )
			bInit = true;

		BuddyInstance* pBuddy = CreateSingleBuddyInstance(pkData, bInit);
		if (pBuddy == NULL)
			continue;

		if (nSummarySize > nDataIndex)
		{
			pBuddy->LoadDBData(summary.activate_data(nDataIndex));//, detail.data(nDataIndex));
			nDataIndex++;
		}
	}
}

void LC_ServerPlayer::PBSaveBuddyDetailData(char_data_define::buddy_detail_data & detail)
{
	mem::map<int32_t, BuddyInstance*>:: iterator it = m_ppUsableBuddyInstance.begin();
	for ( ; it != m_ppUsableBuddyInstance.end(); ++it )
	{
		BuddyInstance* pBuddy = it->second;
		pBuddy->SaveData(* detail.add_data());
	}
}

void LC_ServerPlayer::PBLoadBuddyDetailData(const char_data_define::buddy_detail_data & detail )
{
	int nSize = detail.data_size();
	for (int i = 0; i < nSize; i++)
	{
		//load detail
		const char_data_define::buddy_single_instance_data& buddy = detail.data(i);
		int nBuddyId = buddy.buddyid();
		BuddyInstance* pBuddy = GetBuddyInstanceByID(nBuddyId);
		if (pBuddy == NULL)
		{
			continue;
		}
		pBuddy->LoadData(buddy);
	}
}

int LC_ServerPlayer::LoadBuddyData_Locked()
{
	int nResult = 0;
	CF_Buddy* pBuddyConfig = CF_Buddy::GetSingletonPtr();

	CF_Buddy::DataEntryMapExternIterator it = pBuddyConfig->GetIterator();
	while( it.HasMoreElements() )
	{
		const CF_Buddy::DataEntry* pkData = it.PeekNextValuePtr();
		int32_t nTemplateId = pkData->_cID;
		//if ( !CheckUnlockedBuddyByTemplateID(nTemplateId) )
		{
			SingleBuddyData data;
			data.bUnlock = false;
			data.nLevel = 1;
			data.nBuddyTemplateId = nTemplateId;
			//m_ppUsableBuddyData.push_back(data);
			mem::map<int32_t, SingleBuddyData>::iterator it = m_ppUsableBuddyData.find(nTemplateId);
			if (it == m_ppUsableBuddyData.end())
				m_ppUsableBuddyData.insert(std::make_pair(nTemplateId, data));
			else
			{
				SingleBuddyData& d = it->second;
				if (d.nBuddyTemplateId == INVALID_ACTOR_ID)
				{
					d.nBuddyTemplateId = nTemplateId;
					d.nLevel = 1;
					d.bUnlock = false;
				}
			}
			nResult++;
		}

		it.MoveNext();
	}

	return nResult;
}

void LC_ServerPlayer::PBLoadTaskTypeInfo(const char_data_define::task_type_asset_info& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	pkTaskMap->GetDataFromPB(info);
}
void LC_ServerPlayer::PBSaveTaskTypeInfo(char_data_define::task_type_asset_info& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	pkTaskMap->SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadProcessingTaskInfo(const char_data_define::processing_task_asset_info& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	pkTaskMap->PBLoadProcessingTaskInfo(info);
}

void LC_ServerPlayer::PBSaveProcessingTaskInfo(char_data_define::processing_task_asset_info& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	pkTaskMap->PBSaveProcessingTaskInfo(info);
}

void  LC_ServerPlayer::PBLoadTaskChapterRewardInfo(const ::google::protobuf::RepeatedField<::google::protobuf::uint32>& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	pkTaskMap->PBLoadTaskChapterRewardInfo(info);
}

void LC_ServerPlayer::PBSaveTaskChapterRewardInfo(::google::protobuf::RepeatedField<::google::protobuf::uint32>& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	pkTaskMap->PBSaveTaskChapterRewardInfo(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PSLoadCharFinishedTaskInfo(const char_data_define::finished_task_asset_info& info)
{
	LC_TaskMap* pkTaskMap = GetTaskMap();
	if (NULL == pkTaskMap)
		return;

	LC_FinishedTaskRecord* pkRecord = pkTaskMap->GetFinishedTaskRecord();
	pkRecord->GetDataFromPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PSSaveCharFinishedTaskInfo(char_data_define::finished_task_asset_info& info)
{
	LC_TaskMap* pkTaskMap = GetTaskMap();
	if (NULL == pkTaskMap)
		return;

	LC_FinishedTaskRecord* pkFinishTaskRecord = pkTaskMap->GetFinishedTaskRecord();
	pkFinishTaskRecord->SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadFailTaskInfo(const char_data_define::fail_task_asset_info& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	if (NULL == pkTaskMap)
		return;
	pkTaskMap->PBLoadFailTaskInfo(info);
}

void LC_ServerPlayer::PBSaveFailTaskInfo(char_data_define::fail_task_asset_info& info)
{
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)GetTaskMap();
	if (NULL == pkTaskMap)
		return;
	pkTaskMap->PBSaveFailTaskInfo(info);
}

void LC_ServerPlayer::PBLoadLivenessInfo(const char_data_define::char_full_info& info)
{
	LC_TaskMap* pkTaskMap = GetTaskMap();
	if (NULL == pkTaskMap)
		return;

	LC_LivenessEntry& entry = pkTaskMap->GetLivenessEntry(STT_DAILY_CHALLENGE);
	for (int i = 0; i < info.daily_liveness_size(); i++)
	{
		const char_data_define::char_liveness_entry& pbentry = info.daily_liveness(i);
		if (entry.GetType() == (uint32_t)pbentry.type())
		{
			entry.GetDataFromPB(pbentry);
		}
	}
}
void LC_ServerPlayer::PBSaveLivenessInfo(char_data_define::char_full_info& info)
{
	LC_TaskMap* pkTaskMap = GetTaskMap();
	if (NULL == pkTaskMap)
		return;

	LC_LivenessEntry& entry = pkTaskMap->GetLivenessEntry(STT_DAILY_CHALLENGE);
	char_data_define::char_liveness_entry* pbentry = info.add_daily_liveness();

	entry.SetDataToPB(*pbentry);

}
void LC_ServerPlayer::PBLoadAssignData(const char_data_define::char_full_info& info)
{
	LC_TaskMap* pkTaskMap = GetTaskMap();
	if (NULL == pkTaskMap)
		return;

	LC_AssignTaskInfo* pkAssignTaskInfo =  pkTaskMap->GetAssignTaskInfo();

	pkAssignTaskInfo->Clear();

	for (int i = 0; i < info.assign_datas_size(); ++i)
	{
		const char_data_define::task_assign_data& assign = info.assign_datas(i);
		LC_AssignTaskData* pkAssignTaskData = pkAssignTaskInfo->AddAssignType(assign.type(), assign.timestamp());

		pkAssignTaskData->GetDataFromPB(assign);
	}


}
void LC_ServerPlayer::PBSaveAssignData(char_data_define::char_full_info& info)
{
	LC_TaskMap* pkTaskMap = GetTaskMap();
	if (NULL == pkTaskMap)
		return;

	LC_AssignTaskInfo* pkAssignTaskInfo =  pkTaskMap->GetAssignTaskInfo();

	LC_AssignTaskDataMap::iterator cit = pkAssignTaskInfo->GetAssignDataMap().begin();
	for (; cit != pkAssignTaskInfo->GetAssignDataMap().end(); ++cit)
	{
		char_data_define::task_assign_data* pkAssignData = info.add_assign_datas();

		cit->second.SetDataToPB(*pkAssignData);
	}
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadCharShortcutBarInfo(const char_data_define::shortcutbar_info& info)
{
	return;
	LC_ShortcutBar* pkShortBar = GetShortcutBar();
	if (!pkShortBar)
	{
		return;
	}

	pkShortBar->GetDataFromPB(info);
}
//-------------------------------------------------------
void LC_ServerPlayer::PBSaveCharShortcutBarInfo(char_data_define::shortcutbar_info& info)
{
	return;
	LC_ShortcutBar* pkShortcutBar = GetShortcutBar();
	if (NULL == pkShortcutBar)
	{
		return;
	}

	pkShortcutBar->SetDataToPB(info);
}

//------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadTitleNameInfo(const char_data_define::titlename_info& info)
{
	m_kTitleAsset.SetCurrentTitle((GameLogic::TitleNameType)info.current_typeid(), info.current_index());

	GameLogic::LC_TitleAsset::SystemTitleIDList&	rSysTitleIDList = m_kTitleAsset.GetSystemTitleList();
	rSysTitleIDList.clear();
	GameLogic::LC_TitleAsset::SystemTitleIDList&	rInbornPeriodTitleIDList = m_kTitleAsset.GetInbornPeriodTitleList();

	GameLogic::LC_TitleAsset::RemoveTitleIDList&	rRemSysTitleIDList = m_kTitleAsset.GetRemoveTitleList();
	rRemSysTitleIDList.clear();
	GameLogic::LC_TitleAsset::PeriodSystemTitleList&	rPeriodSystemTitleList = m_kTitleAsset.GetPeriodSystemTitleList();
	rPeriodSystemTitleList.clear();

	int32_t bitNumber = 0;
	for (int i = 0; i < info.infos_size(); ++i)
	{
		bitNumber = info.infos(i);
		rSysTitleIDList.insert(bitNumber);
	}

	for (int i = 0; i < info.inborn_period_titles_size(); ++i)
	{
		bitNumber = info.inborn_period_titles(i);
		rInbornPeriodTitleIDList.insert(bitNumber);
	}

	int recordsize = info.del_titles_size();
	for (int i = 0; i < recordsize; ++i)
	{
		const char_data_define::del_title_info& titleinfo = info.del_titles(i);
		rRemSysTitleIDList[titleinfo.id()] = (uint32_t)titleinfo.time();
	}
	recordsize = info.period_titles_size();
	for (int i = 0; i < recordsize; ++i)
	{
		const char_data_define::period_title_info& titleinfo = info.period_titles(i);
		rPeriodSystemTitleList[titleinfo.periodid()] = (uint32_t)titleinfo.periodtime();
	}
}

void LC_ServerPlayer::PBSaveTitleNameInfo(char_data_define::titlename_info& info)
{
	info.set_current_typeid((int)m_kTitleAsset.GetCurrentTitleType());
	info.set_current_index(m_kTitleAsset.GetCurrentTitle());

	GameLogic::LC_TitleAsset::SystemTitleIDList&	rSysTitleIDList = m_kTitleAsset.GetSystemTitleList();
	int32_t bitNumber = 0;
	for (LC_TitleAsset::SystemTitleIDList::iterator it = rSysTitleIDList.begin(); it != rSysTitleIDList.end(); ++it)
	{
		bitNumber = *it;
		info.add_infos(bitNumber);
	}

	GameLogic::LC_TitleAsset::SystemTitleIDList& rInbornPeriodTitleIDList = m_kTitleAsset.GetInbornPeriodTitleList();
	for (LC_TitleAsset::SystemTitleIDList::iterator it = rInbornPeriodTitleIDList.begin(); it != rInbornPeriodTitleIDList.end(); ++it)
	{
		bitNumber = *it;
		info.add_inborn_period_titles(bitNumber);
	}

	GameLogic::LC_TitleAsset::RemoveTitleIDList&	rRemSysTitleIDList = m_kTitleAsset.GetRemoveTitleList();
	LC_TitleAsset::RemoveTitleIDList::iterator beg = rRemSysTitleIDList.begin();
	for (; beg != rRemSysTitleIDList.end(); ++beg)
	{
		char_data_define::del_title_info* titleinfo = info.add_del_titles();
		titleinfo->set_id(beg->first);
		titleinfo->set_time(beg->second);
	}
	GameLogic::LC_TitleAsset::PeriodSystemTitleList&	rPeriodSystemTitleList = m_kTitleAsset.GetPeriodSystemTitleList();
	LC_TitleAsset::PeriodSystemTitleList::iterator piter = rPeriodSystemTitleList.begin();
	for (; piter != rPeriodSystemTitleList.end(); ++piter)
	{
		char_data_define::period_title_info* titleinfo = info.add_period_titles();
		titleinfo->set_periodid(piter->first);
		titleinfo->set_periodtime(piter->second);
	}
}

void LC_ServerPlayer::PBLoadResourceRetrieveInfo(const char_data_define::char_ResourceRetrieve& info)
{
	m_nResourceRetrieveMgr.Load(info);
}

void LC_ServerPlayer::PBSaveResourceRetrieveInfo(char_data_define::char_ResourceRetrieve& info)
{
	m_nResourceRetrieveMgr.Save(info);
}

void LC_ServerPlayer::PBLoadPKStateInfo(const char_data_define::pk_state_info& info)
{
	GetPKRecorder()->SetPKMode(info.pk_mode());
	GetPKRecorder()->SetPKValue(info.pk_value());
}

void LC_ServerPlayer::PBSavePKStateInfo(char_data_define::pk_state_info& info)
{
	info.set_pk_mode(GetPKRecorder()->GetPKMode());
	info.set_pk_value(GetPKRecorder()->GetPKValue());
}

//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadCharItemCDInfo(const char_data_define::item_cd_info& info)
{
	LC_ItemCoolDownMapBase* cdMap = GetControllerItemCoolDownMap();
	if(cdMap != NULL)
	{
		cdMap->GetCDFromPB(this, info);
	}
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBSaveCharItemCDInfo(char_data_define::item_cd_info& info)
{
	LC_ItemCoolDownMapBase* cdMap = GetControllerItemCoolDownMap();
	if(cdMap != NULL)
	{
		cdMap->SetCDToPB(this, info);
	}
}

//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadCharSkillStateInfo(const char_data_define::save_skill_state_info& info)
{
	//m_kSkillStateMap->GetSaveStateFromPB(info);
	m_kControllerSkillStateMap->GetSaveStateFromPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBSaveCharSkillStateInfo(char_data_define::save_skill_state_info& info)
{
	//m_kSkillStateMap->SetSaveStateToPB(info);
	m_kControllerSkillStateMap->SetSaveStateToPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadChar_Raid_Transport_AssetInfo(const char_data_define::transport_raid_asset_info& info)
{
	GetTransportAsset()->GetDataFromPB(info);
	GetRaidAsset()->GetDataFromPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBSaveChar_Raid_Transport_AssetInfo(char_data_define::transport_raid_asset_info& info)
{
	GetTransportAsset()->SetDataToPB(info);
	GetRaidAsset()->SetDataToPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadCharShortcutKeyAssetInfo(const char_data_define::shortcut_key_asset_info& info)
{
	GetShortcutKeyAsset()->GetDataFromPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBSaveCharShortcutKeyAssetInfo(char_data_define::shortcut_key_asset_info& info)
{
	GetShortcutKeyAsset()->SetDataToPB(info);
}

//----------------------------------------------------------------------------
void LC_ServerPlayer::PBLoadGameStoryInfo(const char_data_define::game_story_asset_info& info)
{
	//SERVER_GET_GAMESTORY_MANAGER->CheckReset(this);
	GetGameStoryAsset().GetDataFromPB(info);
}
//----------------------------------------------------------------------------
void LC_ServerPlayer::PBSaveGameStoryInfo(char_data_define::game_story_asset_info& info)
{
	GetGameStoryAsset().SetDataToPB(info);
}
void LC_ServerPlayer::PBLoadVIPInfo(const char_data_define::char_vip_info& info)
{
	m_kVipAsset.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveVIPInfo(char_data_define::char_vip_info& info)
{
	m_kVipAsset.SetDataToPB(info);
}


void LC_ServerPlayer::PBLoadAssistInfo(const char_data_define::char_assist_info& info)
{
	m_kAssistAsset.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveAssistInfo(char_data_define::char_assist_info& info)
{
	m_kAssistAsset.SetDataToPB(info);
}


void LC_ServerPlayer::PBLoadSubordinateInfo(const char_data_define::char_subordinate_info& info)
{
	m_kSubordinateAsset.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveSubordinateInfo(char_data_define::char_subordinate_info& info)
{
	m_kSubordinateAsset.SetDataToPB(info);
}

void LC_ServerPlayer::PBSaveArenaBattleAssertInfo(char_data_define::char_arena_info& info)
{
	m_ArenaBattleAssert.SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadArenaBattleAssertInfo(const char_data_define::char_arena_info& info)
{
	m_ArenaBattleAssert.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSavePortalAssertInfo(char_data_define::char_portal_info& info)
{
	m_portalAssert.SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadPortalAssertInfo(const char_data_define::char_portal_info& info)
{
	m_portalAssert.GetDataFromPB(info);
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	//更新坐标
	mem::vector<int> delindex;
	int portalsize = m_portalAssert.GetSize();
	for (int i = 0; i < portalsize; ++i)
	{
		LC_PortalEntry* entry = m_portalAssert.GetPortal(i);
		CF_WorldMapList::DataEntry* data = pkCSVWorldMapList->GetEntryPtr(entry->m_MapResId);
		if (data == NULL)
		{
			delindex.push_back(i);
			continue;
		}
		entry->m_Pos = data->_iReturnBackPos;
	}
	mem::vector<int>::iterator beg = delindex.begin();
	for (; beg != delindex.end(); ++beg)
	{
		m_portalAssert.DelPortal(*beg);
	}
}

void LC_ServerPlayer::PBSaveSignInAssertInfo(char_data_define::char_signin_info& info)
{
	m_signinAssert.SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadSignInAssertInfo(const char_data_define::char_signin_info& info)
{
	m_signinAssert.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSave360MicroSignInAssertInfo(char_data_define::char_signin_info& info)
{
	m_n360MicroSignInAssert.SetDataToPB(info);
}

void LC_ServerPlayer::PBLoad360MicroSignInAssertInfo(const char_data_define::char_signin_info& info)
{
	m_n360MicroSignInAssert.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveRankGoodRecord(char_data_define::char_rank_good_record& info)
{
	m_RankAssert.SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadRankGoodRecord(const char_data_define::char_rank_good_record& info)
{
	m_RankAssert.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveDomain(char_data_define::char_full_info& info)
{
	m_PlayerGuildAssert.SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadDomain(const char_data_define::char_full_info& info)
{
	m_PlayerGuildAssert.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveGuildWarRecord(char_data_define::char_guildwar_info& info)
{
	LC_GuildWarAssert* pkAsset = GetGuildWarAssert();
	if (pkAsset)
	{
		pkAsset->SetDataToPB(info);
	}
}

void LC_ServerPlayer::PBLoadGuildWarRecord(const char_data_define::char_guildwar_info& info)
{
	LC_GuildWarAssert* pkAsset = GetGuildWarAssert();
	if (pkAsset)
	{
		pkAsset->GetDataFromPB(info);
	}
}

//void LC_ServerPlayer::PBSavePromoteInfo(char_data_define::char_promote_info& info)
//{
//	m_kPromoterAsset.SetDataToPB(info);
//}
//void LC_ServerPlayer::PBLoadPromoteInfo(const char_data_define::char_promote_info& info)
//{
//	m_kPromoterAsset.GetDataFromPB(info);
//}
//void LC_ServerPlayer::PBSaveGreatKungFuInfo(char_data_define::char_great_kungfu_info& info)
//{
//	m_kGreatKungFuAsset.SetDataToPB(info);
//}
//void LC_ServerPlayer::PBLoadGreatKungFuInfo(const char_data_define::char_great_kungfu_info& info)
//{
//	m_kGreatKungFuAsset.GetDataFromPB(info);
//}
void LC_ServerPlayer::PBSaveOpenAbilityInfo(char_data_define::char_open_ability_info& info)
{
	m_kOpenAbilityAsset.SetDataToPB(info);
}
void LC_ServerPlayer::PBLoadOpenAbilityInfo(const char_data_define::char_open_ability_info& info)
{
	m_kOpenAbilityAsset.GetDataFromPB(info);
}
void LC_ServerPlayer::PBSaveCommonStatInfo(char_data_define::char_common_stat_info& info)
{
	m_kCommonStatAsset.SetDataToPB(info);
}
void LC_ServerPlayer::PBLoadCommonStatInfo(const char_data_define::char_common_stat_info& info)
{
	m_kCommonStatAsset.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveCommonItemRecordSet(::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& info)
{
	CommonItemRecordSet::iterator it = m_CommonItemRecordSet.begin();
	for ( ; it != m_CommonItemRecordSet.end(); ++it )
	{
		uint32_t nValue = *it;
		info.Add(nValue);
	}
}

void LC_ServerPlayer::PBLoadCommonItemRecordSet(const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& info)
{
	for ( int i = 0; i < info.size(); i++ )
	{
		m_CommonItemRecordSet.insert(info.Get(i));
	}
}

void LC_ServerPlayer::PBSaveTreasureChestsInfo(char_data_define::char_treasure_chests_info& info)
{
	m_kTreasureChestsAsset.SetDataToPB(info);
}
void LC_ServerPlayer::PBLoadTreasureChestsInfo(const char_data_define::char_treasure_chests_info& info)
{
	m_kTreasureChestsAsset.GetDataFromPB(info);
}
void LC_ServerPlayer::PBSaveCommonShopInfo(char_data_define::char_common_shop_info& info)
{
	m_kCommonShopAsset.SetDataToPB(info);
}
void  LC_ServerPlayer::PBLoadCommonShopInfo(const char_data_define::char_common_shop_info& info)
{
	m_kCommonShopAsset.GetDataFromPB(info);
}
void LC_ServerPlayer::PBSaveActivationCodeInfo(char_data_define::char_activation_code_info& info)
{
	m_kActivationCodeAsset.SetDataToPB(info);
}
void  LC_ServerPlayer::PBLoadActivationCodeInfo(const char_data_define::char_activation_code_info& info)
{
	m_kActivationCodeAsset.GetDataFromPB(info);
}
void LC_ServerPlayer::PBSaveAttributePlusInfo(char_data_define::attribute_plus_info& info)
{
	//m_kAttributePlus.SetDataToPB(info);
}
void LC_ServerPlayer::PBSaveChatMonitorInfo(char_data_define::char_chat_monitor_info& info)
{
	m_kChatChannelController.SetDataToPB(info);
}
void LC_ServerPlayer::PBLoadAttributePlusInfo(const char_data_define::attribute_plus_info& info)
{
	//m_kAttributePlus.GetDataFromPB(info);
}
void LC_ServerPlayer::PBLoadChatMonitorInfo(const char_data_define::char_chat_monitor_info& info)
{
	m_kChatChannelController.GetDataFromPB(info);
}

void LC_ServerPlayer::PBSaveCombatRecorder(char_data_define::combat_recorder& info)
{
	m_CombatRecorderMgr.Save( info );
}

void LC_ServerPlayer::PBLoadCombatRecorder(const char_data_define::combat_recorder& info)
{
	m_CombatRecorderMgr.Load( info );
}

void LC_ServerPlayer::PBSaveCrossRealmTeam(char_data_define::crossrealmteam_map_assert& info)
{
	m_rTeamAssert.GetCrossRealmTeamAsset().SetDataToPB(info);
}

void LC_ServerPlayer::PBLoadCrossRealmTeam(const char_data_define::crossrealmteam_map_assert& info)
{
	m_rTeamAssert.GetCrossRealmTeamAsset().GetDataFromPB(info);
}

void LC_ServerPlayer::PBLoadGoalInfo(const char_data_define::goal_info& info)
{
	uint32_t& time = m_nGoalAsset.GetGoalResetTime();
	time = info.goalresettime();

	GameLogic::GoalAsset::GoalMap& record1 = m_nGoalAsset.GetGoalProgressRecord();
	record1.clear();
	for(int i=0; i<info.goalprocessingrecord_size(); ++i)
	{
		record1.insert(std::make_pair(info.goalprocessingrecord(i).id(), info.goalprocessingrecord(i).num()));
	}

	GameLogic::GoalAsset::GoalMap& record2 = m_nGoalAsset.GetGoalAccountRecord();
	record2.clear();
	for(int i=0; i<info.goalaccount_size(); ++i)
	{		
		record2.insert(std::make_pair(info.goalaccount(i).id(), info.goalaccount(i).account()));
	}
}

void LC_ServerPlayer::PBSaveGoalInfo(char_data_define::goal_info& info)
{
	uint32_t time = m_nGoalAsset.GetGoalResetTime();
	info.set_goalresettime(time);

	GameLogic::GoalAsset::GoalMap& record1 = m_nGoalAsset.GetGoalProgressRecord();
	for(GameLogic::GoalAsset::GoalMap::iterator it=record1.begin(); it!=record1.end(); ++it)
	{
		::char_data_define::char_goalProcessingRecord* data = info.add_goalprocessingrecord();
		data->set_id(it->first);
		data->set_num(it->second);
	}

	GameLogic::GoalAsset::GoalMap& record2 = m_nGoalAsset.GetGoalAccountRecord();
	for(GameLogic::GoalAsset::GoalMap::iterator it=record2.begin(); it!=record2.end(); ++it)
	{
		::char_data_define::char_goalAccount* data = info.add_goalaccount();
		data->set_id(it->first);
		data->set_account(it->second);
	}
}

void LC_ServerPlayer::PBSaveSecretTreasureInfo(char_data_define::secret_treasure_info& info)
{
	m_kSecretTreasureAsset.SetDataToPB(info);
}
void LC_ServerPlayer::PBLoadSecretTreasureInfo(const char_data_define::secret_treasure_info& info)
{
	m_kSecretTreasureAsset.GetDataFromPB(info);
}
void LC_ServerPlayer::PBSaveActivityInfo(char_data_define::char_activity_info& info)
{
	m_kACTScheduleAsset.SetDataToPB(info, true);
}
void LC_ServerPlayer::PBLoadActivityInfo(const char_data_define::char_activity_info& info)
{
	m_kACTScheduleAsset.GetDataFromPB(info);
}
void LC_ServerPlayer::PBSaveArenaInfo(char_data_define::char_global_arena_info& info)
{
	m_kArenaAsset.SetDataToPB(info);
}
void LC_ServerPlayer::PBLoadArenaInfo(const char_data_define::char_global_arena_info& info)
{
	m_kArenaAsset.GetDataFromPB(info);
}

void LC_ServerPlayer::LoadSocietyBeID(int iSocietyType, mem::vector<unique_id_type>& id)
{
	if(id.empty())
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	if(!IsMainController())
	{
		return pkOwnerPlayer->LoadSocietyBeID(iSocietyType, id);
	}

	LC_SocietyAsset* pkSociety = GetSocietyAsset();

	for(int i=0; i<id.size(); ++i)
	{
		pkSociety->AddBeSociety(iSocietyType, id[i]);
	}	
}

///////////////////////////-------LoadData----------///////////////////////////////
void LC_ServerPlayer::LoadCharPB_Pack( const std::string& _infoStr )
{
	char_data_define::PBPackData data;
	bool b = data.ParseFromString( _infoStr );
	if( !b )
	{
		return;
	}

	GetPackAsset()->GetBasicPack().GetFromPB(data.basic_pack_info(), true);
	GetPackAsset()->GetWarehousePack().GetFromPB(data.warehouse_pack_info(), true);
	GetPackAsset()->ValidItems();
}

void LC_ServerPlayer::LoadCharPB_Development( const std::string& _infoStr )
{

}

void LC_ServerPlayer::LoadCharPB_Hero( const std::string& _infoStr )
{
	char_data_define::PBHeroData data;
	bool b = data.ParseFromString( _infoStr );
	if( !b )
	{
		return;
	}

	PBLoadBuddySummaryData( data.buddy_summary(), data.buddy_detail() );
	PBLoadBuddyDetailData(data.buddy_detail());
	GetBuddySchemeMgr().LoadData(data.buddy_activate_scheme());
	PBLoadBuddyAutoRespawnTimer(data.buddy_respawn_timer());
	PBLoadBuddyChain(data.buddychaindata());
}

void LC_ServerPlayer::LoadCharPB_PlayerBase( const std::string& _infoStr )
{
	char_data_define::PBPlayerBaseData data;
	bool b = data.ParseFromString( _infoStr );
	if( !b )
	{
		return;
	}

	m_CommandEquipPack.LoadData( data.playerequip() );
	m_nCommanderSubEquipPack_Vehicle.LoadData(LC_SUB_TP_VEHICLE, data.playerequip());
	m_nCommanderSubEquipPack_DevaEye.LoadData(LC_SUB_TP_DEVA_EYE, data.playerequip());
	m_nCommanderSubEquipPack_Muhun.LoadData(LC_SUB_TP_MUHUN, data.playerequip());
	PBLoadSubordinateInfo(data.subordinate_info());
	PBLoadPetDetailData(data.pet_detail());
	m_BuddyEquipData.LoadData(data.buddyequipdata());
	m_RuneData.LoadData(data.runedata());
	m_kSkillBookAsset.Load(data.skill_book_info(), true);
	m_RedPoint.LoadData(data.redpoint());
	m_TaskExtraRewardAsset.LoadData(data.taskextrareward());
	m_PlayerMapInfoAsset.LoadData(data.playermapinfo());
}

void LC_ServerPlayer::LoadCharPB_Offline( const std::string& _infoStr )
{
	char_data_define::PBOfflineData data;
	bool b = data.ParseFromString( _infoStr );
	if( !b )
	{
		return;
	}
}

///////////////////////////-------SaveData----------///////////////////////////////
void LC_ServerPlayer::SaveCharPB_Pack( char_data_define::PBPackData& _info )
{
	GetPackAsset()->GetBasicPack().SetToPB(*_info.mutable_basic_pack_info());
	GetPackAsset()->GetWarehousePack().SetToPB(*_info.mutable_warehouse_pack_info());
}

void LC_ServerPlayer::SaveCharPB_Development( char_data_define::PBDevelopmentData& _info )
{

}

void LC_ServerPlayer::SaveCharPB_Hero( char_data_define::PBHeroData& _info )
{
	PBSaveBuddySummaryData(*_info.mutable_buddy_summary());
	PBSaveBuddyDetailData(*_info.mutable_buddy_detail());
	GetBuddySchemeMgr().SaveData(_info.mutable_buddy_activate_scheme());
	PBSaveBuddyAutoRespawnTimer(*_info.mutable_buddy_respawn_timer());
	PBSaveBuddyChain(*_info.mutable_buddychaindata());
}

void LC_ServerPlayer::SaveCharPB_PlayerBase( char_data_define::PBPlayerBaseData& _info )
{
	m_CommandEquipPack.SaveData( _info.mutable_playerequip() );
	m_nCommanderSubEquipPack_Vehicle.SaveData(_info.mutable_playerequip());
	m_nCommanderSubEquipPack_DevaEye.SaveData(_info.mutable_playerequip());
	m_nCommanderSubEquipPack_Muhun.SaveData(_info.mutable_playerequip());
	PBSaveSubordinateInfo(*_info.mutable_subordinate_info());
	PBSavePetDetailData(*_info.mutable_pet_detail());
	m_BuddyEquipData.SaveData(_info.mutable_buddyequipdata());
	m_RuneData.SaveData(_info.mutable_runedata());
	m_kSkillBookAsset.Save(*_info.mutable_skill_book_info(), true);
	m_RedPoint.SaveData(_info.mutable_redpoint());

	m_TaskExtraRewardAsset.SaveData(_info.mutable_taskextrareward());

	m_PlayerMapInfoAsset.SaveData(_info.mutable_playermapinfo());
}

void LC_ServerPlayer::SaveCharPB_Offline( char_data_define::PBOfflineData& _info )
{
	char_data_define::char_OfflineData* pkSaveData = _info.mutable_offlineinfo();
	if(NULL == pkSaveData)
	{
		return;
	}

	pkSaveData->set_charid(GetInstance());
	pkSaveData->set_charname(TPSTR2STDSTR(GetOwnerCharName()));
	pkSaveData->set_level(GetLevel());

	pkSaveData->set_guildname(TPSTR2STDSTR(GetGuildName()));

	pkSaveData->set_borntype(GetProfilePictureID());
	pkSaveData->set_controllercombatscore(GetControllerCombatScore());
	pkSaveData->set_commandertitleid(GetCommanderTitle());
	pkSaveData->set_commandertitleframeid(GetCurrentCommanderTitleFrameID());

	std::map<int32_t,int64_t> controllerAttrMap;
	FillAttrMap(controllerAttrMap, GetCommanderAttrMgr().GetCommanderAllAttributeMap());
	for(std::map<int32_t,int64_t>::iterator it=controllerAttrMap.begin(); it!=controllerAttrMap.end(); ++it)
	{
		char_data_define::mapKey_Value_int32* pkData = pkSaveData->add_controllerattrmap();
		if(NULL != pkData)
		{
			pkData->set_key(it->first);
			pkData->set_value(it->second);
		}
	}

	std::map<int32_t,int64_t> controllerCoreAttrMap;
	FillAttrMap(controllerCoreAttrMap, GetShowCoreAttrMap());
	for(std::map<int32_t,int64_t>::iterator it=controllerCoreAttrMap.begin(); it!=controllerCoreAttrMap.end(); ++it)
	{
		char_data_define::mapKey_Value_int32* pkData = pkSaveData->add_controllercoreattrmap();
		if(NULL != pkData)
		{
			pkData->set_key(it->first);
			pkData->set_value(it->second);
		}
	}

	std::map<int32_t,PS_ControllerEquipSlotItemInfo> controllerEquipSlotItemInfo;
	GetCommandEquipPack().GetControllerEquipSlotlnfo(controllerEquipSlotItemInfo);
	for(std::map<int32_t,PS_ControllerEquipSlotItemInfo>::iterator it=controllerEquipSlotItemInfo.begin(); it!=controllerEquipSlotItemInfo.end(); ++it)
	{
		char_data_define::PBControllerEquipSlotItemInfo* pkData = pkSaveData->add_controllerequipslotiteminfo();
		if(NULL != pkData)
		{
			pkData->set_slotindex(it->first);
			pkData->set_itemid(it->second.m_nEquipItem);
			pkData->set_slotlevel(it->second.m_nEquipSlotReinfroce);
			pkData->set_slotstarlevel(it->second.m_nEquipSlotRefine);
			pkData->set_slotmoldingspiritlevel(it->second.m_nEquipSlotMoldingSpirit);
		}
	}

	std::vector<int32_t> activateBuddyID;
	GetActivateBuddyIDs(activateBuddyID);
	for(std::vector<int32_t>::iterator it=activateBuddyID.begin(); it!=activateBuddyID.end(); ++it)
	{
		pkSaveData->add_activatebuddyid(*it);
	}

	mem::vector<BuddyInstance*> vecAllBuddys;
	GetAllBuddyInstnace(vecAllBuddys);
	for(int i=0; i<vecAllBuddys.size(); ++i)
	{
		BuddyInstance* pkBuddy = vecAllBuddys[i];
		if(NULL != pkBuddy)
		{
			uint64_t buddyCombatScore = pkBuddy->GetBuddyAttrMgr().GetBuddyCombatScoreByType(BCST_FinalBuddy);
			char_data_define::mapKey_Value_uint64* pkData = pkSaveData->add_buddycombatscore();
			if(NULL != pkData)
			{
				pkData->set_key(pkBuddy->GetTemplateID());
				pkData->set_value(buddyCombatScore);
			}
		}
	}

	std::map<int32_t,PS_Buddy_Info_BuddyCS> buddyInfo;
	FillPlayerRuneAndEquipInfoMap(buddyInfo);
	for(std::map<int32_t,PS_Buddy_Info_BuddyCS>::iterator it=buddyInfo.begin(); it!=buddyInfo.end(); ++it)
	{
		char_data_define::buddy_info* pkData = pkSaveData->add_buddyinfo();
		if(NULL != pkData)
		{
			pkData->set_buddyid(it->second.m_nBuddyID);
			pkData->set_buddylevel(it->second.m_nBuddyLevel);
			pkData->set_buddycombatscore(it->second.m_nBuddyCombatScore);
			pkData->set_buddystarlevel(it->second.m_nBuddyStarLevel);
			pkData->set_activateallskill(it->second.m_bActivateAllSkill);
			pkData->set_allequipslotreinforcelevel(it->second.m_nAllEquipSlotReinforceLevel);
			pkData->set_buddycurrenttransformid(it->second.m_nBuddyTransformID);
			for(std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator it1=it->second.m_nBuddyRuneInfo.begin(); it1!=it->second.m_nBuddyRuneInfo.end(); ++it1)
			{
				char_data_define::PB_RuneItemInfo_BuddyCS* pkInfo = pkData->mutable_buddyruneinfo();
				if(NULL != pkInfo)
				{
					char_data_define::RuneItemInfo_BuddyCS* pkRune = pkInfo->add_buddyrunedata();
					if(NULL != pkRune)
					{
						pkRune->set_index(it1->first);
						pkRune->set_runeitemid(it1->second.m_nRuneItemID);
						pkRune->set_runelevel(it1->second.m_nRuneLevel);
						pkRune->set_runestarlevel(it1->second.m_nRuneStarLevel);
					}
				}
			}
			for(std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator it2=it->second.m_nEquipSlotInfo.begin(); it2!=it->second.m_nEquipSlotInfo.end(); ++it2)
			{
				char_data_define::PB_EquipItemInfo_BuddyCS* pkInfo = pkData->mutable_buddyequipinfo();
				if(NULL != pkInfo)
				{
					char_data_define::EquipItemInfo_BuddyCS* pkEquip = pkInfo->add_buddyequipdata();
					if(NULL != pkEquip)
					{
						pkEquip->set_index(it2->first);
						pkEquip->set_equipitemid(it2->second.m_nEquipItemID);
						pkEquip->set_equipslotlevel(it2->second.m_nEquipSlotLevel);
						pkEquip->set_equipslotstarlevel(it2->second.m_nEquipSlotStarLevel);
					}
				}
			}
		}
	}

	const LC_SubUpgradeDataMap& subMap = GetSubordinateAsset().GetSubDataMap();
	for (LC_SubUpgradeDataMap::const_iterator it = subMap.begin(); it != subMap.end(); ++it)
	{
		LC_SubUpgradeData::TransformMap trans = it->second.GetTransformMap();

		char_data_define::PBSub_Info* pkData = pkSaveData->add_subinfo();
		if(NULL != pkData)
		{
			pkData->set_subtype(it->first);
			for(LC_SubUpgradeData::TransformMap::iterator kIt=trans.begin(); kIt!=trans.end(); ++kIt)
			{
				pkData->add_subtransform(kIt->first);
			}
		}
	}

	pkSaveData->set_activatepetid(GetEnabledPetInstanceResId());

	int nResult = false;
	const mem::map<int16_t, PetInstance*>& pkPetMap = GetPetRawData(nResult);
	for(mem::map<int16_t, PetInstance*>::const_iterator it=pkPetMap.begin(); it!=pkPetMap.end(); ++it)
	{
		PetInstance* pkPet = it->second;
		if(NULL != pkPet)
		{
			char_data_define::mapKey_Value_int32* pkData = pkSaveData->add_petinfo();
			if(NULL != pkData)
			{
				pkData->set_key(pkPet->GetTemplateId());
				pkData->set_value(pkPet->GetStarLevel());
			}
		}
	}
}
