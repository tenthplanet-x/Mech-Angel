#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_AssistProtocol.h"
#include "LC_AssistAsset.h"

#include "SC_ServerSkillInterface.h"
#include "SC_ServerScriptInterface.h"
#include "LC_ItemFactoryBase.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//--------------------------------------------
ON_HANDLE(MGPT_REQ_SET_ASSIST_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Assist_Info_Set_Req kmsg;
	if (!kmsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_AssistAsset& kAssistAsset = pkPlayer->GetAssistAsset();
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	ResultType rslt = RE_SUCCESS;
	do
	{
		long lItemTypeID = kmsg.m_AssistInfo.m_iHPTypeID;
		if (lItemTypeID > 0)
		{
			LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemTypeID);
			if (!pkItem)
			{
				rslt = RE_ITEM_INVALID;
				break;
			}
		}

		kAssistAsset.GetDataFromProtocolStruct(kmsg.m_AssistInfo);
	}
	while (0);

	MG_Assist_Info_Set_Rlt rltMsg;
	rltMsg.m_uiRslt = rslt;
	kAssistAsset.SetDataToProtocolStruct(rltMsg.m_AssistInfo);
	pkPlayer->SendMsgToClient(MGPT_RLT_SET_ASSIST_INFO, &rltMsg);
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_GET_HOOKING_GUIDE_LOCATION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Hooking_Guide_Location_Req kmsg;
	if (!kmsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_Hooking_Guide_Location_Rlt rltMsg;
	rltMsg.m_bFind = false;

	LC_MapBase* pkMap = pkPlayer->GetMap();
	LC_ActorBase* pkActor = NULL;

	if (pkMap)
	{
		pkActor = pkMap->GetHookGuideTarget(pkPlayer);
	}

	if (pkActor)
	{
		rltMsg.m_bFind = true;
		const UT_Vec3Int& loc = pkActor->GetCurrentLocation();
		rltMsg.m_iX = loc.x;
		rltMsg.m_iY = loc.y;
		rltMsg.m_iZ = loc.z;
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_GET_HOOKING_GUIDE_LOCATION, &rltMsg);
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_RETURN_TO_CITY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_ReturnToCity kmsg;
	if (!kmsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SC_ServerSkillInterface* pkSkillInterface = SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
	pkSkillInterface->ReturnToCity(pkPlayer, true);
}

}
