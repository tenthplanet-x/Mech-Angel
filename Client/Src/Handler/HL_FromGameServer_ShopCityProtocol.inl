#include "MG_GameProtocol.h"
#include "LC_ClientShopCity.h"

void HL_FromGameServer::_handlerRltMysticalShopGoodList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	LC_ClientShopCity* pkShopCity = LC_ClientShopCity::GetSingletonPtr();
	if(NULL == pkShopCity)
		return;
	pkShopCity->OnRecvMysticalGooldList(pMsg, wSize);
}

void HL_FromGameServer::_handlerRltMysticalShopOp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RTL_MysticalShopOpt msg;
	if (!msg.Decode(pMsg, wSize))
		return;

	LC_GameEvent kGameEvent;
	if(msg.nResult == RE_SUCCESS)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_MYSTICAL_SHOP_OPT);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.optType);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.itemIndex);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	SendLogicInfoMessage(msg.nResult);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltShopCityBuyGood(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Buy_Item rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientShopCity* pkShopCity = LC_ClientShopCity::GetSingletonPtr();
	if(NULL == pkShopCity)
		return;


	long		lItemID			= 0;
	long		lItemCount		= 0;
	MoneyType	llCostYuanBao	= 0;
	StringType	kItemName		= "";

	std::map<int32_t, int32_t>::iterator it = rltMsg.m_kShopIDMap.begin();
	for (; it!=rltMsg.m_kShopIDMap.end(); ++it)
	{
		//ÅÐ¶ÏÊÔÁ¶ÉÌµêÎïÆ·

		if(rltMsg.m_Result == RE_SUCCESS)
		{
			LC_CommonShopManager* pkCmnShopMgr = LC_CommonShopManager::GetSingletonPtr();
			LC_ShopDataEntry* pkDataEntry = pkCmnShopMgr->GetShopDataEntry(it->first);
			if(pkDataEntry && pkDataEntry->m_iShopType == 3)
			{
				LC_GSScoreMap pkScoreMap = pkDataEntry->m_kGSScorePriceMap;
				LC_GSScoreMap::iterator pkScore = pkScoreMap.find(GS_SCR_TP_TRIALS);
				if(pkScore != pkScoreMap.end())
					SendLogicInfoMessage(RE_DEL_TRIALPOINT, pkScore->second);
				pkScore = pkScoreMap.find(GS_SCR_TP_ROLL);
				if(pkScore != pkScoreMap.end())
					SendLogicInfoMessage(RE_DEL_SLOTPOINT, pkScore->second);
			}
		}

		LC_GameEvent kGameEvent;
		if(rltMsg.m_Result == RE_SUCCESS)
		{
			kGameEvent.SetEventType(GLET_SHOPCITY_BUY_ITEM_SUCCESS);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, it->first);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, it->second);
		}
		else
		{
			kGameEvent.SetEventType(GLET_SHOPCITY_BUY_ITEM_FAIL);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, it->first);
		}

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);


		if(rltMsg.m_Result != RE_SUCCESS)
		{
			SendLogicInfoMessage(rltMsg.m_Result);
			return;
		}

		const LC_ShopCityGoodsDescriptionEntry* pkGoodsConfig = pkShopCity->GetGoodsInfoByShopID(it->first);
		if(NULL == pkGoodsConfig)
			return;

		lItemID			= pkGoodsConfig->GetItemID();
		lItemCount		= it->second;

		CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
		CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(lItemID);
		if(pkItemData)
		{
			char numstr[100];
			ltoa(lItemID, numstr, 10);
			kItemName = kItemName + "[@" + numstr +"]";
		}
		SendLogicInfoMessage(rltMsg.m_Result, kItemName, lItemCount);
	}		
}

void HL_FromGameServer::_handleRltVIPDrawGiftDaily(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_Draw_VIP_Time_Gift rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		return;
	}
	
	if(rltMsg.m_uiRslt == RE_SUCCESS)
	{
		StringStreamType sStream;
		sStream << rltMsg.m_uiType << "#";
		UI_UIManager::GetSingletonPtr()->HandlerMsgGameEvent(MGPT_RLT_DRAW_VIP_TIME_GIFT, sStream.str());
	}
	SendLogicInfoMessage(rltMsg.m_uiRslt);
}

void HL_FromGameServer::_handleRltVIPDrawGiftOneOff(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_Draw_VIP_Oneoff_Gift rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		return;
	}

	if(rltMsg.m_uiRslt == RE_SUCCESS)
	{
		StringStreamType sStream;
		sStream << rltMsg.m_uiLevel << "#";
		UI_UIManager::GetSingletonPtr()->HandlerMsgGameEvent(MGPT_RLT_DRAW_VIP_ONEOFF_GIFT, sStream.str());
	}
	SendLogicInfoMessage(rltMsg.m_uiRslt);
}

void HL_FromGameServer::_handleRltDrawFdepositReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_Draw_FDepositReward rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		return;
	}
	if(rltMsg.m_uiRslt == RE_SUCCESS)
	{
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		StringStreamType sStream("");
		sStream << rltMsg.m_uiLevel << "#";
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_DRAW_FDEPOSIT_REWARD, sStream.str());
	}
	else
	{
		SendLogicInfoMessage(rltMsg.m_uiRslt);
	}
}

void HL_FromGameServer::_handleSyncSignInInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncSignInInfo syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
		return;


	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	
	if (syncMsg.m_nType == SIGNIN_TYPE_1)
		pkGamePlayer->GetSignInAsset()->GetDataFromProtocolStruct(syncMsg);
	else if (syncMsg.m_nType == SIGNIN_TYPE_2)
		pkGamePlayer->GetMiniSignInAsset()->GetDataFromProtocolStruct(syncMsg);
}


void HL_FromGameServer::_handleRltSignIn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SignIn_Resp rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		return;
	}

	SendLogicInfoMessage(rltMsg.m_Result);
}

void HL_FromGameServer::_handleRltReSignIn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_ReSignIn_Resp rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		return;
	}

	SendLogicInfoMessage(rltMsg.m_Result);
}
void HL_FromGameServer::_handleRespSigninRewward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SignInReward_Resp rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		return;
	}

	SendLogicInfoMessage(rltMsg.m_Result);
}

void HL_FromGameServer::_handlerRltResetShopTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ResetShopTimes rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_ResetShopTimes½âÎö´íÎó");
		return;
	}

	if(rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
	else
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SHOPCITY_RLT_GETRESETTIMES);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nShopType);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nResetTimes);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, rltMsg.m_nResult);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerRltGetShopResetTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetShopResetTimes rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetShopResetTimes½âÎö´íÎó");
		return;
	}
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SHOPCITY_RLT_GETRESETTIMES);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nShopType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nResetTimes);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, RE_FAIL);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltGetShopItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetShopItem msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetShopItem½âÎö´íÎó");
		return;
	}

	LC_ClientGameEvent kCGameEvent;
	kCGameEvent.SetClientEventType(CGET_SHOP_ITEMLIST);

	StringStreamType sStream;
	sStream << Int2String(msg.m_nShopType) << '#';

	LC_CommonShopManager* pkCmnShopMgr = LC_CommonShopManager::GetSingletonPtr();

	std::map<int32_t,int32_t>::const_iterator iter = msg.m_nItemInfo.begin();
	for(; iter!=msg.m_nItemInfo.end(); ++iter)
	{
		CF_ACTShopA::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ACTShopA>(iter->second);
		if(pkData != NULL)
		{
			sStream << Int2String(iter->first) << ':' << Int2String(pkData->_iKey) << ':' << Int2String(iter->second) << '|';
		}
	}

	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kCGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kCGameEvent);
}

void HL_FromGameServer::_handlerRltBuyInvesmentCard(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BuyInvesmentCard msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_BuyInvesmentCard½âÎö´íÎó");
		return;
	}
	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->SetInvestmentType(msg.m_nInvesmentType);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_BUYINVESMENTCARD,"");
	}
}

void HL_FromGameServer::_handlerRltAddInvesmentCardReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddInvestmentCardReward msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_AddInvestmentCardReward½âÎö´íÎó");
		return;
	}
	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
		if (msg.m_nRewardType == InvestmentReward_Free)
			pkGamePlayer->SetInvestmentFreeReward(msg.m_nID);
		else if(msg.m_nRewardType == InvestmentReward_High)
			pkGamePlayer->SetInvestmentHighReward(msg.m_nID);
		else if(msg.m_nRewardType == InvestmentReward_Both)
		{
			pkGamePlayer->SetInvestmentFreeReward(msg.m_nID);
			pkGamePlayer->SetInvestmentHighReward(msg.m_nID);
		}
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_ADDINVESTMENTCARDREWARD,"");
	}
}

void HL_FromGameServer::_handlerRltRefreshRewardTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_RefershRewardTask msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_RefershRewardTask½âÎö´íÎó");
		return;
	}
	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}