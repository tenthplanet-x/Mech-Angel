#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"

#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"

#include "ACT_ActivitySchedule.h"
#include "ACT_SecretTreasure.h"
#include "ACT_Subordinate.h"
#include "ACT_Monopoly.h"
#include "ACT_SnowMan.h"

#include "UT_ServerHelper.h"
#include "LC_GameEventManager.h"

#include "MG_PACKET.h"
#include "MG_Stream.h"
#include "MG_PB.h"
#include "msg.pb.h"

using namespace Protocol;
using namespace GameLogic;
using namespace Activity;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//--------------------------------------------
void on_activ_op_successfully(LC_ServerPlayer* pkPlayer, int32_t iActiType, int32_t iCategory, uint32_t opcode, int count=1)
{
	if ( pkPlayer == NULL )
		return;
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
	if (pkGameEvent )
	{
		pkGameEvent->SetEventType(GLET_ACTI_STAT_OP);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
		pkGameEvent->SetEventParam(EP_ACTOP_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_ACTOP_ACTITYPE, iActiType);
		if(iActiType == ACT_SCHEDULE_TP_LOTTERY)
		{
			pkGameEvent->SetEventParam(EP_ACTOP_CATEGORY, ACT_SCHEDULE_TP_LOTTERY);
		}
		else
		{
			pkGameEvent->SetEventParam(EP_ACTOP_CATEGORY, iCategory);
		}

		pkGameEvent->SetEventParam(EP_ACTOP_OPCODE, opcode);
		pkGameEvent->SetEventParam(EP_ACTOP_COUNT, count);
		pkGameEvent->SetEventParam(EP_ACTOP_MAP_ID, pkPlayer->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
}
//--------------------------------------------
ON_HANDLE(MGPT_ACTIVITY_COMMON_OP_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_PB<msg_define::activity_common_op_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	const msg_define::activity_common_op_req& req = reqMsg.m_value;


	MG_PB<msg_define::activity_common_op_resp> respMsg;
	msg_define::activity_common_op_resp& resp = respMsg.m_value;

	resp.mutable_req()->CopyFrom(req);
	ResultType rslt = RE_FAIL;

	int count = 1;
	static ACT_ScheduleDataManager* pkACTDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
	ACT_ScheduleDataActivityBase* pkActivity = pkACTDataMgr->GetActivity(req.type());
	if (pkActivity)
	{
		switch(req.code())
		{
		case ACT_COMMON_OP_DRAW:
			{
				if (req.params_size() < ACT_COMMON_OP_PARAM_SIZE)
				{
					break;
				}
				GameLogic::LC_RewardItemsExtend kExtend;
				kExtend.m_iChoice = req.params(ACT_COMMON_OP_PARAM_CHOICE);
				GameLogic::LC_IDVector kExtraParams;
				kExtraParams.push_back(req.params(ACT_COMMON_OP_PARAM_EXTRA_1));
				kExtraParams.push_back(req.params(ACT_COMMON_OP_PARAM_EXTRA_2));
				kExtraParams.push_back(req.count());
				rslt = pkActivity->DrawReward(pkPlayer, req.catagory(), req.schedule(),
					req.params(ACT_COMMON_OP_PARAM_KEY), kExtend, &kExtraParams, count);
			}
			break;
		case  ACT_MONOPOLY_OP_FORWARD:
			{
				if (req.params_size() < ACT_MONOPOLY_OP_PARAM_SIZE)
				{
					break;
				}
				bool bUseYuanbao = req.params(ACT_MONOPOLY_OP_PARAM_USE_YUANBAO) > 0;
				Activity::ACT_MonopolyManager* pkActiMonopoly = dynamic_cast<Activity::ACT_MonopolyManager*>(pkActivity);
				if (NULL == pkActiMonopoly)
				{
					break;
				}
				int32_t iMoveStep = 0;
				rslt = pkActiMonopoly->MonopolyForward(pkPlayer,req.catagory(), req.schedule(),
						bUseYuanbao, iMoveStep);
				resp.add_values(iMoveStep);
			}
			break;
		case ACT_COMMON_OP_RANDOM_DRAW:
			{
				if (req.params_size() < ACT_COMMON_OP_PARAM_SIZE)
				{
					break;
				}
				GameLogic::LC_RewardItemsExtend kExtend;
				kExtend.m_iChoice = req.params(ACT_COMMON_OP_PARAM_CHOICE);
				GameLogic::LC_IDVector kExtraParams;
				Utility::UT_SIMDataList kRewards;
				kExtraParams.push_back(req.params(ACT_COMMON_OP_PARAM_EXTRA_1));
				kExtraParams.push_back(req.params(ACT_COMMON_OP_PARAM_EXTRA_2));
				rslt = pkActivity->RandomReward(pkPlayer, req.catagory(), req.schedule(),
					req.params(ACT_COMMON_OP_PARAM_KEY), kExtend, &kExtraParams, kRewards);
				Utility::UT_SIMDataList::iterator it = kRewards.begin();
				for (; it != kRewards.end(); ++it)
				{
					resp.add_values(it->IID());
					resp.add_values(it->ItemCount());
				}
			}
			break;
		case  ACT_SNOWMAN_USE_ITEM:
			{
				if (req.params_size() <= 0 || req.params_size() % 2 != 0)
				{
					break;
				}
				mem::vector<int32_t> kParams;
				kParams.reserve(req.params_size());
				for (int i = 0; i < req.params_size(); ++i)
				{
					kParams.push_back(req.params(i));
				}
				Activity::ACT_SnowManManager* pkActiSnowMan = dynamic_cast<Activity::ACT_SnowManManager*>(pkActivity);
				if (NULL == pkActiSnowMan)
				{
					break;
				}
				pkActiSnowMan->DoUseItem(pkPlayer, kParams);
			}
			break;
		case ACT_SNOWMAN_REQ_GROUPSCORE:
			{
				Activity::ACT_SnowManManager* pkActiSnowMan = dynamic_cast<Activity::ACT_SnowManManager*>(pkActivity);
				if (NULL == pkActiSnowMan)
				{
					break;
				}			
				if (req.params_size() < 2)
				{
					break;
				}
				int iGroupScore = pkActiSnowMan->QueryGroupScore(pkPlayer, req.params(ACT_EXTRA_PARAM_INDEX_1), req.params(ACT_EXTRA_PARAM_INDEX_2));
				if (iGroupScore < 0)
				{
					iGroupScore = 0;
				}
				rslt = RE_SUCCESS;
				resp.add_values(iGroupScore);
			}
			break;
		case ACT_COMMON_QUERY:
			{		
				mem::vector<int32_t> kReqVals;
				mem::vector<int32_t> kRespVals;
				kReqVals.reserve(req.params_size());
				for (int i = 0; i < req.params_size(); ++i)
				{
					kReqVals.push_back(req.params(i));
				}
				rslt = pkActivity->DoQuery(pkPlayer, req.catagory(), req.schedule(), kReqVals, kRespVals);
				if (RE_SUCCESS != rslt)
				{
					break;
				}
				for (size_t i = 0; i < kRespVals.size(); ++i)
				{
					resp.add_values(kRespVals[i]);
				}				
			}
			break;
		case ACT_COMMON_BEHAVE_SPEC:
			{
				mem::vector<int32_t> kReqVals;
				mem::vector<int32_t> kRespVals;
				kReqVals.reserve(req.params_size());
				for (int i = 0; i < req.params_size(); ++i)
				{
					kReqVals.push_back(req.params(i));
				}
				rslt = pkActivity->DoBehaveSpec(pkPlayer, req.catagory(), req.schedule(), kReqVals, kRespVals);
				if (RE_SUCCESS != rslt)
				{
					break;
				}
				for (size_t i = 0; i < kRespVals.size(); ++i)
				{
					resp.add_values(kRespVals[i]);
				}
			}
			break;
		case ACT_COMMON_TREASURE_ADD:
			{
				mem::vector<int> vecReward;
				rslt = pkPlayer->TreasureReward(req.catagory(),vecReward);
				for (int i = 0; i < vecReward.size(); ++i)
				{
					resp.add_values(vecReward[i]);
				}
			}
			break;
		default:
			break;
		}
	}
	resp.set_rslt(rslt);
	pkPlayer->SendMsgToClient(MGPT_ACTIVITY_COMMON_OP_RESP, &respMsg);

	if (RE_SUCCESS == rslt)
	{
		on_activ_op_successfully(pkPlayer, req.type(), req.catagory(), req.code(), count);
	}

	// Send message immediatly
	if (pkPlayer->GetDirtyFlag(DIRTY_FLAG_COMMON_STAT_INFO))
	{
		bool bDirty = false;
		pkPlayer->SyncCommonStatInfo(bDirty);
		if (!bDirty)
		{
			pkPlayer->SetDirtyFlag(DIRTY_FLAG_COMMON_STAT_INFO, false);
		}
	}
}
}
