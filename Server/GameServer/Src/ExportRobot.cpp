#include "LM_LocaleManager.h"

#include "GameServerApp.h"
#include "ExportRobot.h"
#include "MG_ProtocolStruct.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_RankManager.h"
#include "CF_ExportRobot.h"
#include "LC_PackEntryFactory.h"
#include "LC_ItemFactoryBase.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "CF_ExportRobotBuddyTemplateInfo.h"
#include "GlobalSettings.h"


using namespace Utility;
using namespace NetWork;
using namespace Protocol;
using namespace CSVFile;
using namespace GameLogic;

object_id_vector kRobotIDs;
void ExportRobot::Reset(void)
{
	m_Operator = 0;
	m_StartTime = 0;
	m_Step = 0;
	m_nServerID = 0;
	ClearAllPlayer();
}
void ExportRobot::Update(float fCurrentTime)
{
	if (m_StartTime && m_StartTime + 600<=fCurrentTime)
	{
		Reset();
	}
}
void ExportRobot::Export2LoginSvrd(int beginindex)
{
	if (beginindex <= 0)
	{
		ClearAllPlayer();
		beginindex = 0;
		m_Step = 0;
		m_StartTime = GET_PROCESS_TIME;
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
		CF_ExportRobot::DataEntryMapExternIterator kIter = CF_ExportRobot::GetSingletonPtr()->GetIterator();
		for (; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_ExportRobot::DataEntry* pkEntry = kIter.PeekNextValuePtr();

			for (int i = 0; i < pkEntry->_OutCount; ++i)
			{
				unique_id_impl charID = GEN_UNIQUE_ID((platform_id_type)GET_PLATFORM_ID(), 0, GET_ROBOT_CHARID(pkEntry->_iID, i));
				object_id_type objectID = GET_ROBOT_PLAYERID(charID.detail());

				LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->CreatePlayer(objectID, charID, pkEntry->_Sex, (5<<24) + PLY_DEF_INSTANCE_BITS);
				if (player == NULL)
				{
					ClearAllPlayer();
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "export robot failed! could not create a player!");
					ExportResult(false);
					return;
				}
				player->InitLogicData();
				player->SetWeaponAttribute((WeaponType)pkEntry->_School);
				player->SetLevel(pkEntry->_Level);
				player->InitGameData();
				player->SetDead(false);
				player->SetDying(false);
				//插入数据

				int SlotIdx = -1;
				player->LearnSkill(pkEntry->_SchoolSkill1, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_SchoolSkill2, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_SchoolSkill3, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_SchoolSkill4, SlotIdx);

				player->GetSubordinateAsset().GMSetLevel(LC_SUB_TP_VEHICLE, pkEntry->_Vehicle);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleSkill1, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleSkill2, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleSkill3, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleSkill4, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleSkill5, SlotIdx);
				player->GetSubordinateAsset().GMSetLevel(LC_SUB_TP_ARROW, pkEntry->_Bow);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_BowSkill1, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_BowSkill2, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_BowSkill3, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_BowSkill4, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_BowSkill5, SlotIdx);
				player->GetSubordinateAsset().GMSetLevel(LC_SUB_TP_DEVA_EYE, pkEntry->_Devaeye);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_DevaeyeSkill1, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_DevaeyeSkill2, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_DevaeyeSkill3, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_DevaeyeSkill4, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_DevaeyeSkill5, SlotIdx);
				player->GetSubordinateAsset().GMSetLevel(LC_SUB_TP_VEHICLE_FIGHTING, pkEntry->_VehicleFight);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleFightSkill1, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleFightSkill2, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleFightSkill3, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleFightSkill4, SlotIdx);
				SlotIdx = -1;
				player->LearnSkill(pkEntry->_VehicleFightSkill5, SlotIdx);
				player->GetAttributeMap()->RecomputeAll();
				player->SetHP(player->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
				player->SetMP((int)player->GetFinalAttributeMap()->GetAttribute(ATT_MAX_MP));

				player->SetDirtyFlag(DIRTY_FLAG_SCORE, true);
				//player->CheckScoreChange();

				kRobotIDs.push_back(objectID);
			}
		}
	}

	MG_GL_REQ_ExportRobot req;
	//5个一发
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	int i = 0;
	for (size_t curindex=beginindex; curindex<kRobotIDs.size() && i < 5; ++curindex)
	{
		object_id_type objectID = kRobotIDs[curindex];
		LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(objectID);
		if (player)
		{
			char_data_define::char_base_info binfo;
			char_data_define::char_full_info cinfo;
			player->SaveCharPB(req.m_Info[i].Info, binfo, cinfo);
			binfo.SerializeToString(&req.m_Info[i].BaseBlobBuffer);
			cinfo.SerializeToString(&req.m_Info[i].DetailBlobBuffer);
			++i;
		}
	}

	if (i <= 0)
	{
		return;
	}

	req.m_InfoSize = i;
	req.m_Index = beginindex;
	SendMsgToLogin(MGPT_GL_REQ_EXPORTROBOT, &req);

	if (beginindex%20)
	{
		ExportStep();
	}
}

void ExportRobot::Export2LoginSvrdResp(int32_t result, int beginindex, std::vector<std::string>& name)
{
	if (result != RE_SUCCESS)
	{
		ClearAllPlayer();
		ExportResult(false);
		return;
	}
	if(beginindex < 0)
	{
		beginindex = 0;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	size_t curindex = (size_t)beginindex;
	for (size_t i = 0; curindex < kRobotIDs.size() && i < name.size(); ++curindex, ++i)
	{
		object_id_type objectID = kRobotIDs[curindex];
		LC_PlayerBase* player = pkPlayerManager->FindPlayer(objectID);
		if (player)
		{
			player->SetCharName(name[i].c_str());
		}
	}
	Export2LoginSvrd((int)curindex);
}

void ExportRobot::ClearAllPlayer()
{
	static LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	while (!kRobotIDs.empty())
	{
		object_id_type id = kRobotIDs.back();
		kRobotIDs.pop_back();
		LC_PlayerBase* player = pkPlayerManager->FindPlayer(id);
		if (player)
		{
			pkPlayerManager->DestroyPlayer(player);
		}
	}
}

void ExportRobot::ExportResult(bool result)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(m_Operator);

	StringType msg;

	if (result)
	{
		msg = "ExportRobot success!";
	}
	else
	{
		msg = "ExportRobot fail!";
	}

	if (player)
	{
		player->SendNotifyStrToClient(msg);
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%s]: %s.\n", __FUNCTION__, msg.c_str());
}

void ExportRobot::ExportStep()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(m_Operator);

	char temp[100] = {0};
	sprintf(temp, "ExportStep:%d", m_Step);
	if (player)
	{
		player->SendNotifyStrToClient(temp);
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%s]: %s.\n", __FUNCTION__, temp);

	++m_Step;
}

void ExportRobot::InitExportRobotToGroup(int32_t exportType, int32_t exportRobotType)
{
	mem::vector<uint32_t> robotID;
	UT_ServerHelper::GetRobotIDVecByRobotType(exportRobotType, robotID);
	Export2GroupSvrd(exportType, exportRobotType, robotID);
}

void ExportRobot::Export2GroupSvrd(int32_t exportType, uint32_t robotType, mem::vector<uint32_t>& robotID)
{
	if(robotID.empty())
	{
		return;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	int count = 0;

	PS_Rank_Robot_Info_New data;

	MG_GG_REQ_ExportRobot_New resp;
	uint32_t maxNum = UT_ServerHelper::GetExportRobotMaxNum(robotType);
	if(0 == maxNum)
	{
		return;
	}

	resp.m_RobotInfo.reserve(maxNum);
	for (int i = 0; i < robotID.size(); ++i)
	{
		data.clear();

		if(count == 0)
		{
			resp.clear();
			resp.m_nExportType = exportType;
			resp.m_nRobotType = robotType;
			resp.m_IndexBegin = i;
		}

		bool tag = rank_Manager->FillRobotData(robotID[i], data);
		if(!tag)
		{
			break;
		}

		resp.m_RobotInfo.push_back(data);
		++count;
		if(count==maxNum || i+1==robotID.size())
		{
			resp.m_bEnd = i+1==robotID.size() ? true : false;
			bool bRet = SendMsgToGroup(MGPT_SS_REQ_EXPORTROBOT_NEW, &resp);
			if(bRet)
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ExportRobot exportType:%d, robotType:%d, %d-%d [%d]", exportType, robotType, resp.m_IndexBegin, resp.m_IndexBegin+resp.m_RobotInfo.size(), bRet);
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ExportRobot exportType:%d, robotType:%d, %d-%d [%d]", exportType, robotType, resp.m_IndexBegin, resp.m_IndexBegin+resp.m_RobotInfo.size(), bRet);
			}
			count = 0;
		}
	}
}