#include "MG_Game2Group.h"
#include "MG_ArenaProtocol.h"
#include "GameServerApp.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_Dule_Manager.h"
#include "LC_RankManager.h"
#include "UT_Timer.h"
#include "LC_ServerMap.h"
#include "LC_ServerWorldManager.h"
#include "LC_ShotActivity.h"
#include "CF_FlowerTitleCfg.h"
#include "CF_FlowerGiftCfg.h"
#include "CF_FlowerTitleCfg.h"
#include "UT_ServerHelper.h"
#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"
#include "../Buddy/BuddyInstance.h"
#include "LC_ItemFactoryBase.h"
#include "GlobalSettings.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "CF_ExportRobotBuddyTemplateInfo.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "CF_ExportRobotBuddyTemplateInfo.h"
#include "LC_ServerPlayerManager.h"
#include "gsnode.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

LC_RankManager::LC_RankManager()
{
	nRankOpenTimeInfo.clear();
}

LC_RankManager::~LC_RankManager()
{
}

bool LC_RankManager::init()
{
	InitFlowerGiftMap();
	return true;
}

void LC_RankManager::fin()
{
}

void LC_RankManager::Update(float fCurrentTime)
{
}

LC_ServerPlayer* LC_RankManager::GetPlayer(const unique_id_impl& charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	return playManager ? (LC_ServerPlayer*)playManager->FindPlayer(charID) : NULL;
}

void LC_RankManager::NoticeUserDataToGroup(LC_ServerPlayer* pkPlayer)
{
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	if(pkPlayer->IsRobot())
	{
		return;
	}

	MG_GG_Update_RankUserData msg;
	msg.m_ServerID = GetGameServerID();
	FillPlayerRankValue(pkOwnerPlayer, msg.m_RankValue);
	FillPlayerData(pkOwnerPlayer, msg.m_UserInfo);
	msg.m_GuildId = pkOwnerPlayer->GetGuildID();
	SendMsgToGroup(MGPT_SS_RANK_UPDATE_USERDATA, &msg, pkOwnerPlayer->GetUserID(), pkOwnerPlayer->GetInstance());
}

void LC_RankManager::FillPlayerRankValue(LC_ServerPlayer* pkOwnerPlayer, std::vector<uint64_t>& rankValue)
{
	if(NULL == pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	rankValue.reserve(Rank_Type_End);
	for(int rankType=Rank_Type_Begin; rankType<Rank_Type_End; ++rankType)
	{
		uint64_t value = pkOwnerPlayer->GetRankValueByRankType(rankType);
		rankValue.push_back(value);
	}
}

void LC_RankManager::FillPlayerData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_User_Info& data)
{
	if(NULL == pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	FillRankPlayerBaseData(pkOwnerPlayer, data.m_nPlayerBaseData);
	FillRankAllBuddyData(pkOwnerPlayer, data.m_nBuddyData);
	FillRankPetData(pkOwnerPlayer, data.m_nPetData);
	FillRankSubData(pkOwnerPlayer, LC_SUB_TP_VEHICLE, data.m_nVehicleData);
	FillRankSubData(pkOwnerPlayer, LC_SUB_TP_DEVA_EYE, data.m_nDevaeyeData);
	FillRankSubData(pkOwnerPlayer, LC_SUB_TP_MUHUN, data.m_nWingData);
	FillBuddyCombinedData(pkOwnerPlayer, data.m_MapBuddyId);
	FillPlayerSchemeData(pkOwnerPlayer, data.m_SchemeData);
}

bool LC_RankManager::FillRobotData(uint32_t robotID, Protocol::PS_Rank_Robot_Info_New& data)
{
	CF_ExportSoloDuleRobotInfo::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotInfo>(robotID);
	if(NULL == pkDataEntry)
	{
		return false;
	}

	data.m_nScore = pkDataEntry->_score;
	data.m_nStageLevel = pkDataEntry->_level;
	std::stringstream ss;
	ss << "s" << GetGameServerEntryID() << "." << pkDataEntry->_name;
	
	TPSTR2STDSTR(ss.str().c_str(), data.m_nPlayerBaseData.m_CharName);
	data.m_nPlayerBaseData.m_CharID = pkDataEntry->_id;
	data.m_nPlayerBaseData.m_CommanderTitleID = 1;
	data.m_nPlayerBaseData.m_CommanderTitleFrameID = 1;
	data.m_nPlayerBaseData.m_CommanderScore = (uint64_t)(pkDataEntry->_controllerCombatScore);
	data.m_nPlayerBaseData.m_ProfilePictureID = pkDataEntry->_createBuddyCID;
	data.m_nPlayerBaseData.m_Level = pkDataEntry->_controllerLevel;
	data.m_nPlayerBaseData.m_Robot = true;

	FillRobotBuddyData(data.m_nBuddyData, pkDataEntry->_buddyBaseInfo);
	FillRobotSchemeData(pkDataEntry->_robotType, data.m_nSchemeData, pkDataEntry->_groupInfo);

	return true;
}

ResultType LC_RankManager::QueryRankInfo(LC_ServerPlayer* pkPlayer, Protocol::MG_REQ_RankInfo& ReqMsg)
{
	if (NULL==pkPlayer || ReqMsg.m_Begin<0 || ReqMsg.m_End<ReqMsg.m_Begin)
	{
		return RE_FAIL;
	}


	MG_GG_REQ_Query_Rank reqMsg;
	reqMsg.m_RankID = ReqMsg.m_RankID;
	reqMsg.m_Begin = ReqMsg.m_Begin;
	reqMsg.m_End = ReqMsg.m_End;
	reqMsg.m_ServerId = ReqMsg.m_ServerID;
	reqMsg.m_uCharID = pkPlayer->GetInstance();
	reqMsg.m_nType = ReqMsg.m_nType;
	reqMsg.m_nGroupId = ReqMsg.m_nGroupId;
	if(reqMsg.m_ServerId == 0)
	{
		reqMsg.m_ServerId = GetGameServerEntryID();
	}

	bool ret = SendMsgToGroup(MGPT_SS_RANK_QUERY_RANK_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	if (!ret)
	{
		return RE_SOLO_DULE_RANKSERVER_OUT;
	}
	return RE_SUCCESS;
}


ResultType LC_RankManager::QueryChallengRankInfo(LC_ServerPlayer* pkPlayer, int begin, int end, int nServerID)
{
	if (NULL==pkPlayer || begin<0 || end<begin)
	{
		return RE_FAIL;
	}

	MG_GG_REQ_QueryChalleng_Rank reqMsg;
	reqMsg.m_Begin = begin;
	reqMsg.m_End = end;
	reqMsg.m_ServerId = nServerID;
	reqMsg.m_uCharID = pkPlayer->GetInstance();
	reqMsg.m_nMapID = LC_Helper::GetSchemeIDFromMapID(SOLO_DULE_RES_ID);
	if(reqMsg.m_ServerId == 0)
	{
		reqMsg.m_ServerId = GetGameServerEntryID();
	}

	bool ret = SendMsgToGroup(MGPT_SS_QUERY_CHALLENG_RANK_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	if (!ret)
	{
		return RE_SOLO_DULE_RANKSERVER_OUT;
	}
	return RE_SUCCESS;
}


ResultType LC_RankManager::QueryUserChallenge(LC_ServerPlayer* player)
{
	if(NULL == player)
	{
		return RE_FAIL;
	}

	const unique_id_impl& playerInstance = player->GetInstance();
	MG_GG_REQ_Query_User_Challenge reqMsg;
	reqMsg.m_nRobotSize = 0;
	reqMsg.m_nRandnum = GetGlobalSetting.RandSoluDulePlayerNum;
	reqMsg.m_nRankK = GetGlobalSetting.RankSoluDuleK;
	reqMsg.m_nMapID = LC_Helper::GetSchemeIDFromMapID(SOLO_DULE_RES_ID);
	FillPlayerData(player, reqMsg.m_UserInfo);

	bool tag = SendMsgToGroup(MGPT_SS_RANK_QUERY_USER_CHALLENGE_RANK_REQ, &reqMsg, player->GetUserID(), player->GetInstance());
	if(!tag)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SendMsgToGroup-MGPT_SS_RANK_QUERY_USER_CHALLENGE_RANK_REQ error! c_cid:%d, c_citizenship:%d, c_uid:%d", playerInstance.detail(), playerInstance.catagory(), player->GetUserID());
		return RE_SOLO_DULE_RANKSERVER_OUT;
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "QueryUserChallenge! c_cid:%d, c_citizenship:%d, c_uid:%d", playerInstance.detail(), playerInstance.catagory(), player->GetUserID());
	return RE_SUCCESS;
}

void LC_RankManager::QueryUserChallengeResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Query_User_Challenge& resp)
{
	LC_ServerPlayer* pkPlayer = GetPlayer(charID);
	if (pkPlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " QueryUserChallengeResponse c_cid:%d, c_citizenship:%d is null!", charID.detail(), charID.catagory());
		return;
	}

	pkPlayer->GetArenaBattleAssert()->SetLocalIndex(resp.m_Index);
	//pkPlayer->GetCommonStatAsset().SetArenaIndex(resp.m_Index);	
	pkPlayer->SendMsgToClient(MGPT_RESP_RANK_QUERY_CHALLENGE_INFO, &resp);
}

void LC_RankManager::Add2CrossRealmRank(LC_ServerPlayer* player, int rankId)
{
	MG_GG_REQ_ADD2CROSSREALMRANK reqMsg;
	reqMsg.m_CharID = player->GetInstance();
	reqMsg.m_RankID = rankId;
	SendMsgToGroup(MGPT_SS_RANK_ADD2CROSSREALMRANK, &reqMsg);
}

void LC_RankManager::QueryBaseInfoResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Query_User_Base_Rank& resp)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
	{
		return;
	}

	LC_RankAssert* rankassert = player->GetRankAssert();
	rankassert->SetUserRank(resp.m_UserRank);
	return;
}

ResultType LC_RankManager::QueryBattleInfo(LC_ServerPlayer* player, int rankID)
{
	MG_GG_REQ_Query_User_Battle reqMsg;
	reqMsg.m_RankID = rankID;
	if (!SendMsgToGroup(MGPT_SS_RANK_QUERY_BATTLE_REQ, &reqMsg, player->GetUserID(), player->GetInstance()))
	{
		return RE_SOLO_DULE_RANKSERVER_OUT;
	}
	return RE_SUCCESS;
}

void LC_RankManager::QueryBattleResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Query_User_Battle& resp)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
	{
		return;
	}
	player->SendMsgToClient(MGPT_RESP_BATTLE_INFO, &resp);
	return;
}

ResultType LC_RankManager::SendGroupChallengeReq(LC_ServerPlayer* player, const unique_id_impl& bechallengeCharID, int bechallengeindex)
{
	MG_GG_REQ_Challenge reqMsg;
	reqMsg.m_beChallengeCharID = bechallengeCharID;
	reqMsg.m_BeChallengeIndex = bechallengeindex;
	if (!SendMsgToGroup(MGPT_SS_RANK_CHALLENGE_REQ, &reqMsg, player->GetUserID(), player->GetInstance()))
		return RE_SOLO_DULE_RANKSERVER_OUT;
	return RE_SUCCESS;
}

void LC_RankManager::ChallengeResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Challenge& resp)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
	{
		return;
	}
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->ChallengeCheckResult(charID, resp.m_beChallengeCharID, resp.m_iResult,resp.m_solofightuniqueId);
}
ResultType	LC_RankManager::SendGroupChallengeSummitTeamReq(LC_ServerPlayer* player, const unique_id_impl& bechallengeCharID,int eType)
{
	MG_Req_G2G_SummitArenaCheckChallenge reqMsg;
	reqMsg.m_nChallengeId = bechallengeCharID;
	reqMsg.m_eType = eType;
	if (!SendMsgToGroup(MGPT_SS_REQ_SUMMITARENA_CHECKCHALLENGE , &reqMsg, player->GetUserID(), player->GetInstance()))
		return RE_SOLO_DULE_RANKSERVER_OUT;
	return RE_SUCCESS;
}
void LC_RankManager::SummitTeamChallengeResponse(const unique_id_impl& charID, Protocol::MG_Resp_G2G_SummitArenaCheckChallenge& resp)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
		return;
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Peak_Team)->ChallengeCheckResult(charID, resp.m_nChallengeId, resp. m_nResult);
}
ResultType LC_RankManager::ChallengeReq_GuildOffline(LC_ServerPlayer* player, const unique_id_impl& bechallengeCharID, int bechallengeindex)
{
	return RE_SUCCESS;
}

ResultType LC_RankManager::ChallengeResponse_GuildOffline(const unique_id_impl& charID, uint64_t nBeChallengeCharID, ResultType iResult)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
		return RE_FAIL;
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	return dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_GuildOffline)->ChallengeCheckResult(charID, nBeChallengeCharID, iResult);
}

ResultType LC_RankManager::ChallengeResponse_Sky(const unique_id_impl& charID, uint64_t nBeChallengeCharID, ResultType iResult,int nScore)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
		return RE_FAIL;
	// 生成本场 唯一id
	uint64_t solofightuniqueId = GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	return dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Sky)->ChallengeCheckResult(charID, nBeChallengeCharID, iResult,solofightuniqueId,nScore);
}

ResultType LC_RankManager::SetChallengeResult(LC_ServerPlayer* pkPlayer, PS_Rank_User_Info& beChallenge, bool sucess)
{
	if (pkPlayer == NULL)
	{
		return RE_FAIL;
	}
	//发送请求
	MG_GG_REQ_Challenge_Result reqMsg;
	FillPlayerData(pkPlayer, reqMsg.m_UserInfo);
	reqMsg.m_beChallenge = beChallenge;
	reqMsg.m_sucess = sucess;
	if (!SendMsgToGroup(MGPT_SS_RANK_CHALLENGE_RESULT_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance()))
	{
		return RE_SOLO_DULE_RANKSERVER_OUT;
	}
	return RE_SUCCESS;
}

void LC_RankManager::BeChallengeNotify(const unique_id_impl& charID, Protocol::MG_GG_Rank_Notify_beChallenge& resp, GameLogic::SOLO_DULE_TYPE eType)
{
	LC_ServerPlayer* player = GetPlayer(charID);
	if (player == NULL)
	{
		return;
	}

	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	dule_Manager->GetSoloDuleManager(eType)->BeChallengeNotify(charID, resp);
}

//#define WORLDLEVEL_SKILLSTATE 38200231

void LC_RankManager::PostEnterGame(LC_ServerPlayer* pkPlayer)
{
	if (NULL == pkPlayer)
	{
		return;
	}

	NoticeUserDataToGroup(pkPlayer);
}

////////////////////////////////////////////////////////////////////////////////////////
void	LC_RankManager::InitFlowerGiftMap()
{
	{
		m_kFlowerGiftGainMap.clear();
		CF_FlowerGiftCfg::DataEntryMapExternIterator iter = CF_FlowerGiftCfg::GetSingleton().GetIterator();
		while (iter.HasMoreElements())
		{
			CF_FlowerGiftCfg::DataEntry* rkValue = iter.PeekNextValuePtr();
			{
				LC_FlowerGiftGainMap::iterator iterGain = m_kFlowerGiftGainMap.find(rkValue->_lGroup);
				if (iterGain == m_kFlowerGiftGainMap.end())
				{
					LC_FlowerGiftGain rGainNode;
					m_kFlowerGiftGainMap.insert(std::make_pair(rkValue->_lGroup, rGainNode));
					iterGain = m_kFlowerGiftGainMap.find(rkValue->_lGroup);
				}
				LC_FlowerGiftGain& rGainNode = iterGain->second;
				rGainNode.AddGainWeight(rkValue->_lGetType, rkValue->_lWeight, rkValue->_id);
			}
			iter.MoveNext();
		}
	}

	{
		m_kFlowerGiftCondition.m_accumulateTitleMap.clear();
		m_kFlowerGiftCondition.m_rankTitleMap.clear();

		CF_FlowerTitleCfg::DataEntryMapExternIterator iter = CF_FlowerTitleCfg::GetSingleton().GetIterator();
		while (iter.HasMoreElements())
		{
			CF_FlowerTitleCfg::DataEntry* pTitleCfg = iter.PeekNextValuePtr();
			{
				if (pTitleCfg->_logicType == 1)
				{
					m_kFlowerGiftCondition.m_rankTitleMap.insert(std::make_pair(pTitleCfg->_logicVal, pTitleCfg->_id));
				}
				else
				{
					LC_FlowerTitleLvlMap::iterator iterLvl = m_kFlowerGiftCondition.m_accumulateTitleMap.find(pTitleCfg->_logicType);
					if (iterLvl == m_kFlowerGiftCondition.m_accumulateTitleMap.end())
					{
						mem::vector<FlowerTitleLevel> rLevelList;
						m_kFlowerGiftCondition.m_accumulateTitleMap.insert(std::make_pair(pTitleCfg->_logicType, rLevelList));
						iterLvl = m_kFlowerGiftCondition.m_accumulateTitleMap.find(pTitleCfg->_logicType);
					}
					mem::vector<FlowerTitleLevel>& rLevelList = iterLvl->second;
					FlowerTitleLevel rNode;
					rNode.m_nID = pTitleCfg->_id;
					rNode.m_nValue = pTitleCfg->_logicVal;
					rLevelList.push_back(rNode);
				}
			}
			iter.MoveNext();
		}

		for (LC_FlowerTitleLvlMap::iterator iterLvl = m_kFlowerGiftCondition.m_accumulateTitleMap.begin();
			iterLvl != m_kFlowerGiftCondition.m_accumulateTitleMap.end(); iterLvl ++)
		{
			mem::vector<FlowerTitleLevel>& rLevelList = iterLvl->second;
			sort(rLevelList.begin(), rLevelList.end());
		}
	}
}

void	LC_RankManager::GainFlowerTitle(const unique_id_impl& nCharID, const StringType& strName, uint32_t nType, uint32_t nValue, uint32_t nLastValue)
{
	LC_FlowerTitleLvlMap::iterator iter = m_kFlowerGiftCondition.m_accumulateTitleMap.find(nType);
	if (iter == m_kFlowerGiftCondition.m_accumulateTitleMap.end())
	{
		return;
	}
	mem::vector<FlowerTitleLevel>& rLevelList = iter->second;
	size_t nSize = rLevelList.size();
	if (nSize <= 0)
	{
		return;
	}
	if (nValue < rLevelList[0].m_nValue || nLastValue >= rLevelList[nSize - 1].m_nValue)
	{
		return;
	}
	for (int i = nSize - 1; i >= 0; i--)
	{
		FlowerTitleLevel& rNode = rLevelList[i];
		if (nValue >= rNode.m_nValue && nLastValue < rNode.m_nValue)
		{
			_GainFlowerTitle(nCharID, strName, rNode.m_nID);
			break;
		}
	}
}

void LC_RankManager::_GetItemListByGroupID(int nGroupID, UT_ServerHelper::LC_ItemCntList& rItemList)
{
	LC_FlowerGiftGainMap::iterator iterGain = m_kFlowerGiftGainMap.find(nGroupID);
	if (iterGain != m_kFlowerGiftGainMap.end())
	{
		UT_ServerHelper::__ItemCnt rItemCnt;
		LC_FlowerGiftList rGiftList = iterGain->second.GainGiftID();
		for (size_t index = 0; index < rGiftList.size(); index++)
		{
			CF_FlowerGiftCfg::DataEntry* pGiftCfg = CF_FlowerGiftCfg::GetSingleton().GetEntryPtr(rGiftList[index]);
			if (pGiftCfg != NULL)
			{
				if (pGiftCfg->_ItemID > 0 &&  pGiftCfg->_ItemCnt > 0)
				{
					rItemCnt.m_nId = pGiftCfg->_ItemID;
					rItemCnt.m_nCnt = pGiftCfg->_ItemCnt;
					rItemList.push_back(rItemCnt);
				}

				if (pGiftCfg->_MoneyCnt > 0)
				{
					rItemCnt.m_nCnt = pGiftCfg->_MoneyCnt;
					rItemCnt.m_nId = 0;
					switch (pGiftCfg->_MoneyType)
					{
					case CT_UNBIND_CASH:
						{
							rItemCnt.m_nId = SPEC_ITEM_TID_CASH;
						}
						break;
					case CT_BIND_YUANBAO:
						{
							rItemCnt.m_nId = SPEC_ITEM_TID_LIJIN;
						}
						break;
					case CT_UNBIND_YUANBAO:
						{
							rItemCnt.m_nId = SPEC_ITEM_TID_YUANBAO;
						}
						break;
					default:
						break;
					}
					if (rItemCnt.m_nId > 0)
					{
						rItemList.push_back(rItemCnt);
					}
				}
			}
		}
	}
}

void	LC_RankManager::_GainFlowerTitle(const unique_id_impl& nCharID, const StringType& strName, uint32_t nTitleID, uint32_t nGuildID)
{
	CF_FlowerTitleCfg::DataEntry* pTitleCfg = CF_FlowerTitleCfg::GetSingleton().GetEntryPtr(nTitleID);
	if (pTitleCfg == NULL)
	{
		return;
	}

	stringstream rankIndex;
	rankIndex << nTitleID;
	StringType mailDesc = pTitleCfg->_MainContent + ";" + STDSTR2TPSTR(rankIndex.str()) + ";";

	//添加称号，邮件
	if (pTitleCfg->_TitleItem > 0)
	{
		UT_ServerHelper::LC_ItemCntList rItemList;
		_GetItemListByGroupID(pTitleCfg->_TitleItem, rItemList);
		if (rItemList.empty() == false)
		{
			UT_ServerHelper::SendToMail(pTitleCfg->_MailTitle, mailDesc, nCharID, strName, 0, rItemList);
		}
	}

	//给定称号奖励
	if (pTitleCfg->_ItemID > 0)
	{
		_GainFlowerGiftGroup(nCharID, strName, pTitleCfg->_ItemID, pTitleCfg->_MailTitle, mailDesc);
	}

	//给定帮会奖励
	if (pTitleCfg->_GuildItem > 0 && nGuildID > 0)
	{
		_GainFlowerGiftGroup(nCharID, strName, pTitleCfg->_GuildItem, pTitleCfg->_MailTitleGuild, pTitleCfg->_MainContentGuild, nGuildID);
	}
}

void	LC_RankManager::_GainFlowerGiftGroup(const unique_id_impl& nCharID, const StringType& strName, uint32_t nGroupID, const StringType& mailTitle, const StringType& mailContant, uint32_t nGuildID)
{
	UT_ServerHelper::LC_ItemCntList rItemList;
	_GetItemListByGroupID(nGroupID, rItemList);

	if (rItemList.empty() == false)
	{
		if (nGuildID <= 0)
		{
			UT_ServerHelper::SendToMail(mailTitle, mailContant, nCharID, strName, 0, rItemList);
		}
		else
		{
			UT_ServerHelper::SendToMailGuild(mailTitle, mailContant, nGuildID, 0, rItemList);
		}
	}
}

void LC_RankManager::FillRankPlayerBaseData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_PlayerBaseData& data)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	TPSTR2STDSTR(pkOwnerPlayer->GetGuildName(), data.m_nGuildName);
	TPSTR2STDSTR(pkOwnerPlayer->GetOwnerCharName(), data.m_CharName);
	data.m_Robot = pkOwnerPlayer->IsRobot();
	data.m_CharID = pkOwnerPlayer->GetInstance();
	data.m_CommanderTitleID = pkOwnerPlayer->GetCommanderTitle();
	data.m_CommanderTitleFrameID = pkOwnerPlayer->GetCurrentCommanderTitleFrameID();
	data.m_ProfilePictureID = pkOwnerPlayer->GetProfilePictureID();
	data.m_Level = pkOwnerPlayer->GetControllerLevel();
	data.m_CommanderScore = pkOwnerPlayer->GetControllerCombatScore();
}

void LC_RankManager::FillRankAllBuddyData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_BuddyData& data)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	data.m_CurrentBuddyID = pkOwnerPlayer->GetCurrentActivateBuddyID();
	data.m_CurrentBuddyTransformID = pkOwnerPlayer->GetCurrentActivateBuddyTransformID();
	data.m_ActiveBuddyBattle = pkOwnerPlayer->GetActivateBuddyCombatScore();
	pkOwnerPlayer->FillPlayerRuneAndEquipInfoVector(data.m_BuddyInfo);
}

void LC_RankManager::FillRankPetData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_PetData& data)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	data.m_MaxCombatScorePetID = pkOwnerPlayer->GetMaxCombatScorePetID(data.m_MaxCombatScorePetIDStarLevel);
}

void LC_RankManager::FillRankSubData(LC_ServerPlayer* pkOwnerPlayer, int32_t subType, PS_Rank_SubData& data)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	LC_SubUpgradeData* pkSubData = pkOwnerPlayer->GetSubordinateAsset().GetSubData(subType);
	if(NULL == pkSubData)
	{
		return;
	}

	data.m_nSubType = subType;
	data.m_nSubStage = pkSubData->GetStage();
	data.m_nSubTransform = pkSubData->GetSetTransform();
	data.m_nSubCombatScore = pkOwnerPlayer->GetSumSubSystemCombatScore(subType);
	pkOwnerPlayer->GetSubEquipItemInfo(subType, data.m_SubEquipInfo);
}

void LC_RankManager::FillBuddyCombinedData(LC_ServerPlayer* pkOwnerPlayer, std::map<int8_t, BuddyID_Vec>& data)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	data.clear();

	mem::vector<int32_t> vecBuddyIDs;
	pkOwnerPlayer->GetActivateBuddyIDs(vecBuddyIDs);
	Protocol::BuddyID_Vec tempBuddyIds;
	tempBuddyIds.m_buddyVec.assign(vecBuddyIDs.begin(),vecBuddyIDs.end());
	data[BuddyVectorType::UVT_CurrentBuddy] = tempBuddyIds;

	mem::vector<int32_t> & vecThreeBuddyIDs = pkOwnerPlayer->GetCommanderAttrMgr().GetThreeBuddyVec();
	Protocol::BuddyID_Vec tempThreeBuddyIds;
	tempThreeBuddyIds.m_buddyVec.assign(vecThreeBuddyIDs.begin(),vecThreeBuddyIDs.end());
	data[BuddyVectorType::UVT_ThreeBuddy] = tempThreeBuddyIds;
}

void LC_RankManager::FillPlayerSchemeData(LC_ServerPlayer* pkOwnerPlayer, PS_Player_SchemeData& data)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	pkOwnerPlayer->GetBuddySchemeMgr().GetAllSchemeData(data);
}

void LC_RankManager::FillRobotBuddyData(std::vector<PS_Buddy_Info_BuddyCS>& allBuddyData, const Utility::UT_SIMDataList& baseData)
{
	allBuddyData.reserve(baseData.size());

	//buddy指挥官数据
	for(int i=0; i<baseData.size(); ++i)
	{
		int buddyID = baseData[i].IID();
		int templateID = baseData[i].ParamA();

		CF_ExportRobotBuddyTemplateInfo::DataEntry* pkDate = SafeGetCSVFileDataEntryPtr<CF_ExportRobotBuddyTemplateInfo>(templateID);
		if(NULL==pkDate || pkDate->_buddyBaseInfo.empty())
		{
			continue;
		}

		//指挥官数据
		PS_Buddy_Info_BuddyCS buddyData;		
		buddyData.m_nBuddyID = buddyID;
		buddyData.m_nBuddyTransformID = buddyData.m_nBuddyID*100+1;
		buddyData.m_nBuddyLevel = pkDate->_buddyBaseInfo[0].IID();
		buddyData.m_nBuddyStarLevel = pkDate->_buddyBaseInfo[0].ParamA();
		buddyData.m_bActivateAllSkill = pkDate->_buddyBaseInfo[0].ParamB();
		buddyData.m_nBuddyCombatScore = LC_Helper::GetRobotCombatScore(pkDate->_buddyBaseInfo[0].ParamC());

		//宝具数据
		const Utility::UT_SIMDataList& runeInfo = pkDate->_buddyRuneInfo;
		for(int i=0; i<runeInfo.size(); ++i)
		{
			int slotIndex = runeInfo[i].IID();

			PS_RuneItemInfo_BuddyCS data;
			data.m_nRuneItemID = runeInfo[i].ParamA();
			data.m_nRuneLevel = runeInfo[i].ParamB();
			data.m_nRuneStarLevel = runeInfo[i].ParamC();

			buddyData.m_nBuddyRuneInfo.insert(std::make_pair(slotIndex, data));
		}

		//装备数据
		int totalLevel = 0;
		const Utility::UT_SIMDataList& equipInfo = pkDate->_buddyEquipSlotInfo;
		for(int i=0; i<equipInfo.size(); ++i)
		{
			int slotIndex = equipInfo[i].IID();

			PS_EquipItemInfo_BuddyCS data;
			data.m_nEquipItemID = equipInfo[i].ParamA();
			data.m_nEquipSlotLevel = equipInfo[i].ParamB();
			data.m_nEquipSlotStarLevel = equipInfo[i].ParamC();

			buddyData.m_nEquipSlotInfo.insert(std::make_pair(slotIndex, data));

			totalLevel += data.m_nEquipSlotStarLevel;
		}
		buddyData.m_nAllEquipSlotReinforceLevel = totalLevel;
		allBuddyData.push_back(buddyData);
	}
}

void LC_RankManager::FillRobotSchemeData(int32_t robotType, PS_Player_SchemeData& schemeData, const Utility::UT_SIMDataList& groupInfo)
{
	schemeData.m_vMapDefenceSchemeData.reserve(groupInfo.size());

	PS_MapFullSchemeData fullData;
	fullData.m_vSchemeData.reserve(groupInfo.size());
	for(int i=0; i<groupInfo.size(); ++i)
	{
		PS_SingleSchemeData data;
		data.m_nSchemeGroupID = groupInfo[i].IID();
		data.m_vSchemeData[0] = groupInfo[i].ParamA();
		data.m_vSchemeData[1] = groupInfo[i].ParamB();
		data.m_vSchemeData[2] = groupInfo[i].ParamC();
		fullData.m_vSchemeData.push_back(data);
	}
	
	switch(robotType)
	{
		case Robot_Type_Challenge:
			{
				fullData.m_nSchemeID = LC_Helper::GetSchemeIDFromMapID(SOLO_DULE_RES_ID);
				schemeData.m_vMapDefenceSchemeData.push_back(fullData);
			}
			break;
		case Robot_Type_Firmament:
			{
				fullData.m_nSchemeID = LC_Helper::GetSchemeIDFromMapID(SKY_SOLO_DULE_RES_ID);
				schemeData.m_vMapDefenceSchemeData.push_back(fullData);

				fullData.m_nSchemeID = LC_Helper::GetSchemeIDFromMapID(SUMMIT_TEAM_SOLO_DULE_RES_ID);
				schemeData.m_vMapDefenceSchemeData.push_back(fullData);
			}
			break;
		default:
			break;
	}
}

void LC_RankManager::SetRankOpenTime(RankTimeData& timeData)
{
	nRankOpenTimeInfo = timeData;

}


void LC_RankManager::SetSummitArenaOpenTime(SummitArenaTimeData& timeData)
{
	nSummitArenaTime = timeData;
	if(nSummitArenaTime.nStageTimeType == 1)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID( GetGlobalSetting.iSummitArenaStartNoticeId );
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
	}
}

bool LC_RankManager::IsInRankTime()
{
	int32_t time = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	return time>nRankOpenTimeInfo.nStartTime && time<nRankOpenTimeInfo.nEndime;
}


////////////////////////////////////////////////////////////////////////////////////////
LC_FlowerGiftGain::LC_FlowerGiftGain(): m_nGainType(0), m_nTotalWeight(0)
{
	m_gainWeightArr.clear();
}

LC_FlowerGiftGain::~LC_FlowerGiftGain()
{}

LC_FlowerGiftList	LC_FlowerGiftGain::GainGiftID()
{
	LC_FlowerGiftList outList;
	if (m_nGainType == 0)
	{
		//随机
		uint32_t nRand = rand() % m_nTotalWeight;
		uint32_t nValueNow = 0;
		uint32_t nLastID = 0;
		for (size_t i = 0; i < m_gainWeightArr.size(); i++)
		{
			FlowerGainWeight& rGain = m_gainWeightArr[i];
			if (nValueNow + rGain.m_nWeight >= nRand)
			{
				outList.push_back(rGain.m_nID);
				break;
			}
			nValueNow += rGain.m_nWeight;
			nLastID = rGain.m_nID;
		}
		if (outList.empty())
		{
			outList.push_back(nLastID);
		}
	}
	else
	{
		for (size_t i = 0; i < m_gainWeightArr.size(); i++)
		{
			FlowerGainWeight& rGain = m_gainWeightArr[i];
			outList.push_back(rGain.m_nID);
		}
	}
	return outList;
}

void		LC_FlowerGiftGain::AddGainWeight(uint32_t nGainType, uint32_t nWeight, uint32_t nID)
{
	m_nGainType = nGainType;
	if (m_nGainType == 0)
	{
		m_nTotalWeight += nWeight;
	}
	FlowerGainWeight rGainWeight;
	rGainWeight.m_nID = nID;
	rGainWeight.m_nWeight = nWeight;
	m_gainWeightArr.push_back(rGainWeight);
}
