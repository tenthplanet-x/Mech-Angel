#include "LM_LocaleManager.h"
#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_ItemProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerNPCOwnerList.h"
#include "LC_ServerNPC.h"
#include "LC_NPCShopManager.h"
#include "LC_NPCManagerBase.h"
#include "LC_ShopBase.h"
#include "MG_Common.h"
#include "LC_NPCShop.h"
#include "oss_define.h"
#include "oss_360_define.h"
#include "LC_ItemFactoryBase.h"
#include "CF_NPCShopItems.h"

using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_NPCSHOP_BUY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_NPCShopBuy reqMsg;
	if (reqMsg.Decode(pMsgBody, usBodySize))
	{
		//object_id_type lPlayerID = reqMsg.m_lPlayerID;
		object_id_type lPlayerID = pkPlayer->GetOwnerObjectID();
		object_id_type lNPCID = reqMsg.m_lNPCID;
		int iEntryId = reqMsg.m_iEntryId;
		int iGoodNum = reqMsg.m_iGoodNum;
		int lItemID = reqMsg.m_lItemID;
		int iPackType = reqMsg.m_iPackType;
		int iPackIndex = reqMsg.m_iPackIndex;

		if (iGoodNum <= 0)
		{
			return;
		}

		int NpcCharType = 0;
		if (lNPCID > 0)
		{
			pkPlayer->SetLockedNpcID(lNPCID);
			LC_NPCManagerBase* const pkNPCManager = ENGINE_GET_NPC_MANAGER();
			if (pkNPCManager == NULL)
			{
				return;
			}
			LC_NPCBase* const pkNPC = pkNPCManager->FindNPC(lNPCID);
			if (pkNPC == NULL)
			{
				return;
			}
			int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pkNPC->GetCurrentLocation());
			if (iDistance > MAX_NPC_INTERACTION_DISTANCE)
			{
				return;
			}
			//¼ì²énpcµÄ¹éÊô
			if (pkNPC->GetProperty_NeedCheckOwnerShip())
			{
				LC_ServerNPCOwnerList& rkOwnerList = ((LC_ServerNPC*)pkNPC)->GetNPCOwnerList();
				if (!rkOwnerList.HasOwnerID(lPlayerID))
				{
					MG_RLT_TalkToNPC kRltTalkToNPC;
					kRltTalkToNPC.m_lPlayerID = lPlayerID;
					kRltTalkToNPC.m_lNPCID = 0;
					kRltTalkToNPC.m_bCheckNPCOwerShipSuccess = false;
					pkPlayer->SendMsgToClient(MGPT_RLT_TALKTONPC, &kRltTalkToNPC);
					return;
				}
			}
			NpcCharType = pkNPC->GetCharType();
		}
		else
		{
			pkPlayer->SetLockedNpcID(lNPCID);
			NpcCharType = LC_ShopBase::PLAYER_SHOP_NPC_CHARTYPE;
		}

		LC_NPCShopManager* pkNPCShopManager = LC_NPCShopManager::GetSingletonPtr();
		if (pkNPCShopManager == NULL)
		{
			return;
		}

		LC_NPCShop* pkNPCShop = pkNPCShopManager->GetNPCShop(NpcCharType, LST_ITEM_SHOP);
		if (pkNPCShop == NULL)
		{
			return;
		}

		const CF_NPCShopItems::DataEntry* pkShopEntry = CF_NPCShopItems::Instance()->GetEntryPtr(iEntryId);
		if (pkShopEntry == NULL)
		{
			return;
		}

		if (lItemID != pkShopEntry->_iItemID)
		{
			return;
		}

		ResultType iResult = pkPlayer->BuyGoodsFromNPC(pkShopEntry, iGoodNum, iPackType, iPackIndex, NpcCharType);

		MG_RLT_NPCShopBuy sucMsg;
		sucMsg.m_iResultType = iResult;
		sucMsg.m_lItemID = lItemID;
		sucMsg.m_lItemCount = iGoodNum;

		pkPlayer->SendMsgToClient(MGPT_RLT_NPCSHOP_BUY, &sucMsg);
	}
}

}
