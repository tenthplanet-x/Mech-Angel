#include "LC_TextFilterChannel.h"

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include "CF_ChatMonitor.h"
#include "GameServerApp.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "MG_Game2Guild.h"
#include "StringDefine.h"
#include "ConfigManager.h"
#include "publish_define.h"
#include "oss_define.h"

using namespace CSVFile;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ChatRequest::HandleRequest(TT_TextFilter& kTextFilter)
{
	//sub thread
	if (m_kReq.m_strChatContent.size() > MAX_CLIENT_CHAT_MESSAGE_SIZE)
	{
		LC_Helper::ResizeStr(m_kReq.m_strChatContent, MAX_CLIENT_CHAT_MESSAGE_SIZE);
	}
	StringType strContent = _removeChatContentPrefix(m_kReq.m_strChatContent);
	kTextFilter.SetText(strContent);
	if (kTextFilter.FindForbiddenText())
	{
		m_iForbidLevel += 1;
	}
	m_sContentEncode = UrlEncode(m_kReq.m_strChatContent);
	m_sTargetNameEncode = UrlEncode(m_kReq.m_strTargetPlayerName);
}

void ChatRequest::HandleResponse()
{
	//main thread
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer =	(LC_ServerPlayer*)pkPlayerManager->FindPlayer(m_kReq.m_lCharID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ChatChannelController* pkChatController = pkPlayer->GetChatChannelController();
	pkChatController->IncrChatMonitorCount(m_iForbidLevel > 0 ? true : false);

	bool bResetMonitor = false;
	int32_t iCurSecond = (int32_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
	do {
		CF_ChatMonitor* pkCSVChatMonitor = CF_ChatMonitor::GetSingletonPtr();
		CF_ChatMonitor::DataEntry* pkEntry = pkCSVChatMonitor->GetEntryPtr(pkChatController->GetChatMonitorLevel());
		if (NULL == pkEntry)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR, "无效的监控类型等级[%d]", pkChatController->GetChatMonitorLevel());
			break;
		}
		bResetMonitor = pkEntry->_bResetLevel;
		if (pkEntry->_iChatBaseCount > pkChatController->GetChatTotalCount())
		{
			break;
		}
		double dCurForbidRate = pkChatController->GetChatForbidRate();
		pkChatController->ResetChatMonitorCount();
		double dUpgradeRate = pkEntry->_iUpgradeRate * 0.0001f;
		double dDowngradeRate = pkEntry->_iDowngrade * 0.0001f;
		if (pkEntry->_iUpgradeRate > 0 && dCurForbidRate >= dUpgradeRate)
		{
			//level up
			int iNextLevel = pkChatController->GetChatMonitorLevel() + 1;
			CF_ChatMonitor::DataEntry* pkNextEntry = pkCSVChatMonitor->GetEntryPtr(iNextLevel); 
			if (pkNextEntry)
			{
				pkChatController->AddChatMonitorLevel(1);
				pkChatController->AddSilentGagTime(pkNextEntry->_iSilentGagTime);
			}
			else
			{
				pkChatController->AddSilentGagTime(pkEntry->_iSilentGagTime);
			}

			break;
		}
		if (pkEntry->_iDowngrade <= 0 || dCurForbidRate > dDowngradeRate)
		{
			//level unchange
			pkChatController->AddSilentGagTime(pkEntry->_iSilentGagTime);
		}
		else
		{
			//level down
			int iPreLevel = pkChatController->GetChatMonitorLevel() - 1;
			CF_ChatMonitor::DataEntry* pkPreEntry = pkCSVChatMonitor->GetEntryPtr(iPreLevel);
			if (!pkPreEntry)
			{
				break;
			}
			pkChatController->SetChatMonitorLevel(iPreLevel);
		}
	} while(0);

	MG_REQ_RequestChat& reqMsg = m_kReq;
	MG_RLT_RequestChat rltMsg;
	rltMsg.m_lTargetCharID = reqMsg.m_lTargetCharID;
	rltMsg.m_iChatChannel = reqMsg.m_iChatChannel;

	if (pkChatController->GetSilentGagTime() > 0 && iCurSecond >= pkChatController->GetSilentGagTime())
	{
		pkChatController->SetSilentGagTime(0);
		if (bResetMonitor)
		{
			pkChatController->ResetChatMonitorLevel();
		}
	}

	bool bSlientGag = false;
	if (pkChatController->GetSilentGagTime() > iCurSecond)
	{
		bSlientGag = true;
	}

	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_ChatMessageManager* pkChatMessageManager = pkServerLogicManager->GetChatMessageManager();
	if (!pkChatMessageManager)
	{
		return;
	}
	
	rltMsg.m_iResult = pkChatMessageManager->TranslateChatMessage(pkPlayer, reqMsg, bSlientGag, reqMsg.m_autoConsume);
	if (reqMsg.m_iChatChannel == CCT_TRUMPET ||	reqMsg.m_iChatChannel == CCT_SUPER_TRUMPET)
	{
		TranslateChatInfo& rInfo = pkChatMessageManager->GetTranslateChatInfo();
		rltMsg.m_iShowTime = rInfo.m_iShowTime;
		//聊天喇叭需要同步...
		pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_CHAT_REQUEST, &rltMsg);
}

StringType ChatRequest::_removeChatContentPrefix(const std::string& strContent)
{
	size_t pos = 0;
	std::string firstFlag = "#)";
	if ((pos = strContent.find(firstFlag)) == std::string::npos)
	{
		return STDSTR2TPSTR(strContent);
	}
	size_t nextPos = 0;
	std::string secondFlag = "(@";
	std::string thirdFlag = "@)";
	if ((nextPos = strContent.find(thirdFlag)) != std::string::npos &&
		nextPos <= pos + 6 && strContent.substr(pos+firstFlag.size(), secondFlag.size()) == secondFlag)
	{
		return STDSTR2TPSTR(strContent.substr(nextPos + thirdFlag.size()));
	}
	return STDSTR2TPSTR(strContent.substr(pos + firstFlag.size()));
}

void CreateGuildRequest::HandleRequest(TT_TextFilter& kTextFilter)
{
	StringType strContent = STDSTR2TPSTR(m_kReq.m_szName);
	kTextFilter.SetText(strContent);
	if (kTextFilter.FindForbiddenText())
	{
		m_iForbidLevel += 1;
	}
}

void CreateGuildRequest::HandleResponse()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(m_kReq.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return;
	}

	if (m_iForbidLevel > 0)
	{
		MG_RLT_CreateGuild kCSmsg;
		kCSmsg.m_uiResult = RE_CREATE_GUILD_FAIL_NAME_IS_WRONG;
		pkPlayer->SendMsgToClient(MGPT_RLT_CREATE_GUILD, &kCSmsg);
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (pkActionForbid->IsActionForbid(LAFT_GUILD, LAF_GUILD_MAX_COUNT))
	{
		MG_RLT_CreateGuild kCSmsg;
		kCSmsg.m_uiResult = RE_ACTION_FORBID_GUILD;
		pkPlayer->SendMsgToClient(MGPT_RLT_CREATE_GUILD, &kCSmsg);
		return;
	}

	MG_REQ_CreateGuild& kMsg = m_kReq;
	//是否可以创建门派
	ResultType result = pkPlayer->CanCreateGuild(CodeConvert::Utf8ToWcs(kMsg.m_szName.c_str()), pCreateCfgMgr->getLimit());
	if (result != RE_SUCCESS)
	{
		MG_RLT_CreateGuild kCSmsg;
		kCSmsg.m_uiResult = result;
		pkPlayer->SendMsgToClient(MGPT_RLT_CREATE_GUILD, &kCSmsg);
		return;
	}
	//预创建门派	
	MG_SS_REQ_PreCreate_Guild kSGMsg;
	kSGMsg.m_CostType = kMsg.m_CostType;
	kSGMsg.m_szGuildName = kMsg.m_szName;
	kSGMsg.m_serverID = pkPlayer->GetServerID();
	kSGMsg.m_szGuildDeclaration = kMsg.m_szGuildDeclaration;
	SendMsgToSociety(MGPT_GGU_REQ_PRECREATE_GUILD, &kSGMsg, m_uiUserID, kMsg.m_uiCharID);
}

bool TextFilterChannel::Init()
{
	redis_info& kSvrInfo = GetGameServerApp()->GetGlobalRedisInfo();
	m_kRedisHandler.AddPeer(kSvrInfo.ip, boost::lexical_cast<std::string>(kSvrInfo.port).c_str(), kSvrInfo.auth);
	if (0 != m_kRedisHandler.Init())
	{
		std::cout << Utility::Utf8Show("ChatMonitor redis init fail!") << Utility::Utf8Show(m_kRedisHandler.GetLastError().c_str()) << std::endl;
		return false;
	}
	return true;
}

bool TextFilterChannel::Unit()
{
	if (RE_SUCCESS != m_kRedisHandler.Unit())
	{
		return false;
	}
	return true;
}

void TextFilterChannel::Update(float fTime)
{
	if (fTime < m_fLastUpdateTime + 0.3f)
	{
		return;
	}

	m_fLastUpdateTime = fTime;

	for (int i = 0; i < MAX_CHAT_RESP_QUEUE_SIZE; ++i)
	{
		TextFilterRequest* resp = NULL;
		if (!m_kResponses.pop(resp))
		{
			break;
		}
		if (resp)
		{
			resp->HandleResponse();
			T_SAFE_DELETE(resp);
		}
	}
}

void TextFilterChannel::BounedPush(const MG_REQ_RequestChat& kReq)
{
	TextFilterRequest* pReq = T_NEW_D ChatRequest(kReq);
	if(!m_kRequests.bounded_push(pReq))
	{
		T_SAFE_DELETE(pReq);
	}
}

void TextFilterChannel::BounedPush(const MG_REQ_CreateGuild& kReq, user_id_type uiUserID)
{
	TextFilterRequest* pReq = T_NEW_D CreateGuildRequest(kReq, uiUserID);
	if(!m_kRequests.bounded_push(pReq))
	{
		T_SAFE_DELETE(pReq);
	}
}

void TextFilterChannel::update(void)
{
	int64_t iCurSecond = (int64_t)time(NULL);
	if (FORBID_LIST_SYNC_INTERVAL < iCurSecond - m_iForbidListLastSyncTime)
	{
		mem::list<std::string> keywords;
		LoadForbidWordListFromRedis(&m_kRedisHandler, keywords);
		m_kTextFilter.ClearKeyWordlist();
		m_kTextFilter.InitKeyWordlist(keywords);
		m_iForbidListLastSyncTime = iCurSecond;
	}
	for (int i=0; i<MAX_CHAT_REQ_QUEUE_SIZE; ++i)
	{
		TextFilterRequest* req = NULL;
		if (!m_kRequests.pop(req))
		{
			break;
		}
		if (req)
		{
			req->HandleRequest(m_kTextFilter);
			if(!m_kResponses.bounded_push(req))
			{
				T_SAFE_DELETE(req);
			}
		}
	}
}

void TextFilterChannel::LoadForbidWordListFromRedis(Utility::Redis_Handler* pkRedisHandler, mem::list<std::string>& wordlist)
{
	int ret = 0;
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	
	redispp::MultiBulkEnumerator replys;

	replys = pkRedisConn->lrange(GetGameServerApp()->GetRedisGlobalPrefix() + CHAT_FORBID_WORD_LIST, 0, -1);
	std::string str;
	while (replys.next(&str))
	{
		if (str.empty())
		{
			continue;
		}
		wordlist.push_back(str);
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
}
