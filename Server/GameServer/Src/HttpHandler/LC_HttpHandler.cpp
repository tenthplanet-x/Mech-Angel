#include "LC_HttpHandler.h"

#include <boost/lexical_cast.hpp>

#include "GameServerApp.h"
#include "LC_CommonStatAsset.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerTools.h"
#include "MG_Common.h"
#include "MG_PB.h"
#include "MG_Stream.h"
#include "md5plusplus.h"
#include "redispp.h"
#include "StringDefine.h"
#include "SystemError.h"

#define ACT_360_PRIVI_PRIVKEY "360wlol_TWOUU84DEWSNJU"
#define ACT_360_PRIVI_RESULT_COUNT 10
static const int32_t ACT_360_PRIVI_CODES[ACT_360_PRIVI_RESULT_COUNT] =
{1, 0, -1, -2, -3, -4, -5, -6, -7, -8};
static const ResultType ACT_360_PRIVI_RESULTS[ACT_360_PRIVI_RESULT_COUNT] =
{
	RE_SUCCESS,
	RE_ACT_360_PRIVI_FAILED,
	RE_ACT_360_PRIVI_GUARD_LOGOUT,
	RE_ACT_360_PRIVI_GUARD_LEVEL_LIMIT,
	RE_ACT_360_PRIVI_PARAM_ERROR,
	RE_ACT_360_PRIVI_CLOSED,
	RE_ACT_360_PRIVI_OVER,
	RE_ACT_360_PRIVI_NOT_INIT,
	RE_ACT_360_PRIVI_NOT_SHARE,
	RE_ACT_360_PRIVI_SHARE_ON_THE_WAY
};

using namespace Protocol;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string prepareAct360PriviParam(const StringType& sUserName, int32_t level)
{
	static const int32_t aid = 3216;
	static const std::string gkey = "wlol";
	static const int32_t serverId = GetGameServerID();
	static const std::string type = "privi";

	std::string levelstr = (level == 0) ? "share" : boost::lexical_cast<std::string>(level);
	std::string qidstr = TPSTR2STDSTR(sUserName);
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	std::stringstream ss;
	ss << aid << '|' << gkey << '|' << qidstr << '|' << type << '|' << timestamp << '|'
	   << ACT_360_PRIVI_PRIVKEY;  
	std::string sign = getMd5Str(ss.str()); 

	ss.str("");
	ss << "?aid=" << aid
	   << "&gkey=" << gkey
	   << "&skey=S" << serverId
	   << "&type=" << type
	   << "&qid=" << qidstr
	   << "&time=" << timestamp
	   << "&level=" << levelstr
	   << "&sign=" << sign;
	return ss.str();
}

ResultType getAct360PriviResult(int32_t iCode)
{
	for (int32_t i = 0; i < ACT_360_PRIVI_RESULT_COUNT; ++i)
	{
		if (ACT_360_PRIVI_CODES[i] == iCode)
		{
			return ACT_360_PRIVI_RESULTS[i];
		}
	}
	return RE_FAIL;
}

struct Act360PriviResp
{
	Act360PriviResp() : code(0) {}
	int32_t code;
};
AJSON(Act360PriviResp, (code))

void HttpAct360PriviMsg::HandleRequest(int32_t iReqID, std::string& data)
{
	msg_define::common_stat_operation_req& reqMsg = m_kReqMsg;
	http_req json_req;
	json_req.req_id = iReqID;
	json_req.type = HTTP_REQ_360_PRIVI;
	json_req.server_id = GetGameServerID();
	json_req.url = GetGameServerApp()->GetAct360PriviUrl();
	json_req.method = "GET";
	json_req.param = prepareAct360PriviParam(m_sUserName, reqMsg.op_para());
	std::stringstream ss;
	boost::ajson::save_to_buff(json_req, ss);
	data = ss.str().c_str();
}

void HttpAct360PriviMsg::HandleResponse(const std::string& sRespMsg)
{
	Act360PriviResp kResp;
	std::string errorMsg;
	if (!boost::ajson::load_from_buff(kResp, sRespMsg.c_str(), errorMsg))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	LC_CommonStatManager* pkMgr = LC_CommonStatManager::GetSingletonPtr();
	if (NULL == pkMgr)
	{
		return;
	}

	msg_define::common_stat_operation_req& req_val = m_kReqMsg;
	Protocol::MG_PB<msg_define::common_stat_operation_resp> respMsg;
	msg_define::common_stat_operation_resp& resp_val = respMsg.m_value;
	resp_val.set_type(req_val.type());
	resp_val.set_op_code(req_val.op_code());
	resp_val.set_op_para(req_val.op_para());

	LC_CommonStatConfigDataEntry* pkConfig = pkMgr->GetCmnConfigDataEntry(req_val.type());
	if (NULL == pkConfig)
	{
		return;
	}

	ResultType rslt = RE_FAIL;
	do {
		rslt = getAct360PriviResult(kResp.code);
		if (rslt != RE_SUCCESS)
		{
			break;
		}
		LC_CommonStatAsset& kCmnStatAsset = pkPlayer->GetCommonStatAsset();
		LC_CommonStatEntry* pkCmnStatEntry =
			kCmnStatAsset.GetCommonStatEntry(req_val.type());
		if (!pkCmnStatEntry)
		{
			rslt = RE_FAIL;
			break;
		}
		LC_CommonStatRewardDataEntry* pkCSRewardEntry =
			pkMgr->GetCmnRewardDataEntry(req_val.type(), req_val.op_para());
		if (NULL == pkCSRewardEntry)
		{
			rslt = RE_COMMON_STAT_BAD_KEY;
			break;
		}
		rslt = RE_FAIL;
		break;
		
		//do the reward
		CommonStatOPCBArg opArg = CommonStatOPCBArg(pkPlayer->GetID(), req_val.type(),
			req_val.op_para(), pkConfig, pkCSRewardEntry, NULL,
			(1 << LC_CMN_STAT_OP_DRAW), req_val.op_para());
		opArg();
		if (RE_SUCCESS != opArg.m_uiRslt)
		{
			rslt = opArg.m_uiRslt;
			break;
		}
		rslt = RE_SUCCESS;
	} while (0);

	resp_val.set_result(rslt);
	pkPlayer->SendMsgToClient(MGPT_COMMON_STAT_OPERATION_RESP, &respMsg);
}

bool LC_HttpHandler::Init()
{
	Clear();
	return true;
}

void LC_HttpHandler::UnInit()
{
	Clear();
}

void LC_HttpHandler::Clear()
{
	mem::map<int32_t, HttpBaseMsg*>::iterator it = m_kMsgs.begin();
	for (; it != m_kMsgs.end();)
	{
		HttpBaseMsg* kMsg = it->second;
		m_kMsgs.erase(it++);
		T_DELETE_D(kMsg);
	}
}

void LC_HttpHandler::HandleRequest(unique_id_type uiCharID, const StringType& sUserName,
	const msg_define::common_stat_operation_req& kReqMsg)
{
	static GameServerApp* pkApp = GetGameServerApp();
	HttpBaseMsg* kMsg = T_NEW_D HttpAct360PriviMsg(uiCharID, sUserName, kReqMsg);
	kMsg->m_iProtocolType = MGPT_COMMON_STAT_OPERATION_REQ;

	std::string data;
	m_kMsgs[m_iReqID] = kMsg;
	kMsg->HandleRequest(m_iReqID, data);
	m_iReqID++;

	ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::rpush,
		pkApp->GetRedisHttpReqChannel(), data);
}

void LC_HttpHandler::HandleResponse(int32_t iReqID, const std::string& sRespMsg)
{
	mem::map<int32_t, HttpBaseMsg*>::iterator it = m_kMsgs.find(iReqID);
	if (it == m_kMsgs.end())
	{
		return;
	}
	HttpBaseMsg* kMsg = it->second;
	kMsg->HandleResponse(sRespMsg);
	m_kMsgs.erase(it);
	T_DELETE_D(kMsg);
}
