#include "TS_GMRequest.h"
#include "GameServerApp.h"
#include "ConfigManager.h"
#include "UT_Func.h"
#include "MG_Game2Guild.h"
#include "MG_GMTool.h"

#include "ajson.hpp"

using namespace Protocol;
using namespace Utility;
using namespace GameLogic;

//////////////////////////////////////////////////////////////////////////
struct guild_desc
{
	int32_t guild_id;
	int32_t guild_level;
	std::string guild_name;
	std::string master_name;
	std::string create_name;
	int64_t create_time;
	int32_t server;
	int32_t member_num;
	uint32_t ident;
	std::string guild_notice;
	int32_t guildCrystalLevel1;
};
AJSON(guild_desc, (guild_id)(guild_level)(guild_name)(master_name)(create_name)(create_time)(server)(member_num)(guild_notice))
typedef std::vector<guild_desc> guild_desc_list;
//////////////////////////////////////////////////////////////////////////
TS_Server_GM::~TS_Server_GM()
{
	Reset();
}

void TS_Server_GM::Reset()
{
	m_msg_id = 0;
	m_req.Clear();
	m_rslt.clear();
	m_data.clear();
	m_step = 0;
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}
	m_kStepTranMap.clear();
	TS_TranBase::Reset();
}

void TS_Server_GM::Response(uint32_t rslt)
{
	if (m_msg_id)
	{
		static GameServerApp* pkServerApp = GetGameServerApp();
		Utility::gm_cmd_resp json_resp;
		json_resp.channel.type = MGPT_REDIS_RESP_GM_COMMAND;
		json_resp.channel.id = m_msg_id;
		json_resp.type = m_req.type();
		json_resp.cmd = m_req.command();
		json_resp.code = rslt;
		json_resp.msg.append(m_rslt.c_str(), m_rslt.size());
		json_resp.data.append(m_data.c_str(), m_data.size());
		pkServerApp->PublishChannel(pkServerApp->GetRedisGMRespChannel(), json_resp);
	}
}
//////////////////////////////////////////////////////////////////////////
void AO_Server_GMStep::RespondRltMsg()
{
	if (m_pkTrans)
	{
		m_pkTrans->Response(GetResultID());
	}
}

emTranOperate AO_Server_QueryGuildRlt::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	if (NULL == pkMsg)
	{
		return TRAN_OP_FAILED;
	}
	if (m_pkTrans)
	{
		MG_SS_RLT_GuildList* pkRltGuild = static_cast<MG_SS_RLT_GuildList*>(pkMsg);

		guild_desc_list guilds;
		
		std::vector<PS_GuildSearchData>::iterator it = pkRltGuild->m_kResultInfo.m_kGuildSearchData.begin();
		for (; it!=pkRltGuild->m_kResultInfo.m_kGuildSearchData.end(); ++it)
		{
			PS_GuildSearchData& psGuild = *it;
			guild_desc desc;
			desc.guild_id = psGuild.uiGuildID;
			desc.guild_level = psGuild.usLevel;
			desc.guild_name = CodeConvert::MbsToUtf8(psGuild.szName.c_str()).c_str();
			desc.master_name = CodeConvert::MbsToUtf8(psGuild.szHeadName.c_str()).c_str();
			desc.create_name = CodeConvert::MbsToUtf8(psGuild.createRole.c_str()).c_str();
			desc.create_time = psGuild.createTime;
			desc.server = psGuild.server;
			desc.member_num = psGuild.memberNums;
			desc.ident = psGuild.ident;
			desc.guild_notice = CodeConvert::MbsToUtf8(psGuild.guildNotice.c_str()).c_str();
			desc.guildCrystalLevel1 = psGuild.guildCrystalLevel1;
			guilds.push_back(desc);
		}

		std::stringstream _ss_guilds;
		boost::ajson::save_to_buff(guilds, _ss_guilds);
		SetResultID(RE_SUCCESS);
		m_pkTrans->set_rslt("SUCCESS");
		m_pkTrans->set_data(_ss_guilds.str().c_str());
	}
	else
	{
		SetResultID(RE_FAIL);
	}
	RespondRltMsg();
	return TRAN_OP_NEXT;
}

emTranOperate AO_Server_RandomGuildNameRlt::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	SetResultID(RE_FAIL);
	if(pkMsg != NULL && m_pkTrans != NULL)
	{
		MG_SS_RLT_RandomGuildName* pkRltRandom = dynamic_cast<MG_SS_RLT_RandomGuildName*>(pkMsg);
		if(pkRltRandom != NULL)
		{
			if (RE_SUCCESS == pkRltRandom->m_Result)
			{
				m_pkTrans->set_rslt("SUCCESS!");
			}
			else if (RE_GUILD_GUILD_NAME_USED == pkRltRandom->m_Result)
			{
				m_pkTrans->set_rslt("Guild's name has been used!");
			}
			else
			{
				m_pkTrans->set_rslt("FAIL!");
			}
			SetResultID(pkRltRandom->m_Result);
		}
	}
	RespondRltMsg();
	return TRAN_OP_NEXT;
}

emTranOperate AO_Server_CommGMRlt::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	SetResultID(RE_FAIL);
	if(pkMsg != NULL && m_pkTrans != NULL)
	{
		MG_GMTool_CommRtl* pkRltGM = dynamic_cast<MG_GMTool_CommRtl*>(pkMsg);
		if(pkRltGM != NULL && m_pkTrans->get_msgid() == pkRltGM->m_iMsgID)
		{
			m_pkTrans->set_rslt(pkRltGM->m_sMsg);
			m_pkTrans->set_data(pkRltGM->m_sData);
			SetResultID((ResultType)pkRltGM->m_iResult);
		}
	}
	RespondRltMsg();
	return TRAN_OP_NEXT;
}

