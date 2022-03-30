
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "TT_TextManager.h"
#include "MG_MessageBase.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "UT_LogicInfoMessage.h"
#include "UI_UIManager.h"
#include "tinyxml.h"
#include "CF_TextColor.h"
#include "LC_ClientNetPlayerManager.h"

extern BOOL MakeDir(char*   szDir);

using namespace Memory;
using namespace Utility;
using namespace GameLogic;

//--------------------------------------------------------------
TT_TextElementContainer::TT_TextElementContainer()
{
	Reset();
}
//--------------------------------------------------------------
TT_TextElementContainer::~TT_TextElementContainer()
{
	Reset();
}
//--------------------------------------------------------------
void TT_TextElementContainer::Reset()
{
	m_iTextElementIncreaseNum = 0;
	m_iContainerMaxSize = 0;
	m_kTextElementList.clear();
}
//--------------------------------------------------------------
int TT_TextElementContainer::AddTextElement(TT_TextElement& rkTextElement,int& iRemoveTextCount)
{
	iRemoveTextCount = 0;

	if(m_kTextElementList.size() >= m_iContainerMaxSize)
	{
		int iRemoveCount = UT_MathBase::MinInt(UT_MathBase::MaxInt(m_iContainerMaxSize / 2, 1),m_kTextElementList.size());

		for(int i = 0; i < iRemoveCount ; i++)
			m_kTextElementList.pop_front();
		
		iRemoveTextCount = iRemoveCount;
	}
	else
		iRemoveTextCount = 0;

	++m_iTextElementIncreaseNum;
	m_kTextElementList.push_back(rkTextElement);
	return m_kTextElementList.size() - 1;
}
//------------------------------------------------------------------------
TT_TextElement* TT_TextElementContainer::GetTextElement(int iIndex)
{	
	if(iIndex >= m_kTextElementList.size()||iIndex <0)
		return NULL;

	return &m_kTextElementList[iIndex];
}
//--------------------------------------------------------------
int TT_TextElementContainer::GetTextElementCount()
{
	return m_kTextElementList.size();
}
//--------------------------------------------------------------
//TT_ChannelTextDataManager
//--------------------------------------------------------------
TT_ChannelTextDataManager::TT_ChannelTextDataManager()
{
	Reset();
}
//--------------------------------------------------------------
TT_ChannelTextDataManager::~TT_ChannelTextDataManager()
{
	TT_ChannelTabMapIter Iter;
	TRAV(m_kChannelTabMap,Iter)
	{
		TT_ChannelTabData* pkChannelTabData = Iter->second;
		T_SAFE_DELETE(pkChannelTabData);
	}

	m_kChannelTabMap.clear();
	m_kChannelTextDataContainer.Reset();
}
//--------------------------------------------------------------
void TT_ChannelTextDataManager::Reset()
{
	TT_ChannelTabMapIter Iter;
	TRAV(m_kChannelTabMap,Iter)
	{
		TT_ChannelTabData* pkChannelTabData = Iter->second;
		if (pkChannelTabData != NULL)
		{
			pkChannelTabData->m_bOrderDirtyFlag = true;
			pkChannelTabData->m_kList.clear();
		}
	}

	int iMax = m_kChannelTextDataContainer.m_iContainerMaxSize;
	m_kChannelTextDataContainer.Reset();
	m_kChannelTextDataContainer.SetContainerMaxSize(iMax);
}
//--------------------------------------------------------------
void TT_ChannelTextDataManager::RegisterTab(ChannelTabType eChannelTabType,int iTabMaxTextCount)
{
	TT_ChannelTabData* pkChannelTabData = T_NEW_D TT_ChannelTabData;
	pkChannelTabData->iTabMaxTextCount = iTabMaxTextCount;
	pkChannelTabData->m_bOrderDirtyFlag = false;
	pkChannelTabData->m_kList.clear();

	m_kChannelTabMap[eChannelTabType] = pkChannelTabData;
}
//--------------------------------------------------------------
void TT_ChannelTextDataManager::SetChannelMaxTextCount(int iCount)
{
	m_kChannelTextDataContainer.SetContainerMaxSize(iCount);
}
//--------------------------------------------------------------
int TT_ChannelTextDataManager::AddTextElement(TT_TextElement& rkTextElement,int& iRemoveTextCount)
{
	return m_kChannelTextDataContainer.AddTextElement(rkTextElement,iRemoveTextCount);
}
//--------------------------------------------------------------
void TT_ChannelTextDataManager::AddTextElementIndexToTab(int iTabType,int iIndex)
{
	TT_ChannelTabMapIter IterFind = m_kChannelTabMap.find(iTabType);
	if(IterFind == m_kChannelTabMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"提示消息类型错误[%d]", iTabType);
		return;
	}

	TT_ChannelTabData* pkChannelTabData = IterFind->second;
	TT_TextIndexList& rkList = pkChannelTabData->m_kList;
	if(rkList.size() >= pkChannelTabData->iTabMaxTextCount)
	{
		int iRemoveCount = UT_MathBase::MinInt(UT_MathBase::MaxInt(pkChannelTabData->iTabMaxTextCount / 2, 1),rkList.size());
		
		for(int i = 0; i < iRemoveCount; i++)
			rkList.pop_front();
	
		pkChannelTabData->m_bOrderDirtyFlag = true;
	}

	rkList.push_back(iIndex);
}
void TT_ChannelTextDataManager::AddTrumpetTextElementIndexToTab(int iTabType,int iIndex)
{
	TT_ChannelTabMapIter IterFind = m_kChannelTabMap.find(iTabType);
	if(IterFind == m_kChannelTabMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"提示消息类型错误[%d]", iTabType);
		return;
	}

	TT_ChannelTabData* pkChannelTabData = IterFind->second;
	TT_TextIndexList& rkList = pkChannelTabData->m_kList;

	pkChannelTabData->m_bOrderDirtyFlag = true;

	rkList.push_back(iIndex);
}
//--------------------------------------------------------------
void TT_ChannelTextDataManager::RemoveTextElementIndexInTabs(int iIndex)
{
	TT_ChannelTabMapIter Iter;
	TRAV(m_kChannelTabMap,Iter)
	{
		TT_ChannelTabData* pkChannelTabData = Iter->second;
		pkChannelTabData->m_bOrderDirtyFlag = true;

		TT_TextIndexListIter IterBe = pkChannelTabData->m_kList.begin();
		for(; IterBe != pkChannelTabData->m_kList.end();)
		{
			if(*IterBe == iIndex)
			{
				 IterBe = pkChannelTabData->m_kList.erase(IterBe);
			}
			else
			{
				if(*IterBe > iIndex)
					*IterBe = *IterBe - 1;	

				++IterBe;
			}
		}
	}
}
//--------------------------------------------------------------
void TT_ChannelTextDataManager::Update(float fCurrentTime)
{
	TT_TextElementList& rkTextList = m_kChannelTextDataContainer.m_kTextElementList;
	TT_TextElementListIter Iter;
	int iIndex = 0;
	TRAV(rkTextList,Iter)
	{
		TT_TextElement& rkTextElement = *Iter;
		if(rkTextElement.GetTextLifeTime() > 0 && rkTextElement.GetTextLifeTime() <= fCurrentTime)
		{
			rkTextList.erase(Iter);
			RemoveTextElementIndexInTabs(iIndex);
			return;
		}
		iIndex++;
	}
}
//--------------------------------------------------------------
int TT_ChannelTextDataManager::GetTabTextCount(int iTabType)
{
	TT_ChannelTabMapIter IterFind = m_kChannelTabMap.find(iTabType);
	if(IterFind == m_kChannelTabMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"提示消息类型错误");
		return 0;
	}

	return IterFind->second->m_kList.size();
}
//--------------------------------------------------------------
void TT_ChannelTextDataManager::ClearTabText(int iTabType)
{
	TT_ChannelTabMapIter IterFind = m_kChannelTabMap.find(iTabType);
	if(IterFind == m_kChannelTabMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"提示消息类型错误");
		return;
	}

	IterFind->second->m_kList.clear();
	IterFind->second->m_bOrderDirtyFlag = true;
}
//--------------------------------------------------------------
bool TT_ChannelTextDataManager::GetAndClearTabOderDirtyFlag(int iTabType)
{
	TT_ChannelTabMapIter IterFind = m_kChannelTabMap.find(iTabType);
	if(IterFind == m_kChannelTabMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"提示消息类型错误");
		return false;
	}

	bool bResult = IterFind->second->m_bOrderDirtyFlag;
	IterFind->second->m_bOrderDirtyFlag = false;
	return bResult;
}
//--------------------------------------------------------------
int TT_ChannelTextDataManager::GetAndClearTextIncreaseNum()
{
	int iIncreaseNum = m_kChannelTextDataContainer.m_iTextElementIncreaseNum;
	m_kChannelTextDataContainer.m_iTextElementIncreaseNum = 0;
	return iIncreaseNum;
}
//--------------------------------------------------------------
TT_TextElement* TT_ChannelTextDataManager::GetTabTextElement(int iTabType,int iIndex)
{
	TT_ChannelTabMapIter IterFind = m_kChannelTabMap.find(iTabType);
	if(IterFind == m_kChannelTabMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"提示消息类型错误");
		return NULL;
	}
	
	TT_ChannelTabData* pkChannelTabData = IterFind->second;
	TT_TextIndexList& rkList = pkChannelTabData->m_kList;

	if(iIndex >= rkList.size())
		return NULL;

	int iTextIndex = rkList[iIndex];

	return m_kChannelTextDataContainer.GetTextElement(iTextIndex);
}
//--------------------------------------------------------------
TT_TextElement* TT_ChannelTextDataManager::GetTextElement(int iTextIndex)
{
	return m_kChannelTextDataContainer.GetTextElement(iTextIndex);
}
//--------------------------------------------------------------
//TT_TextManager
//--------------------------------------------------------------
TT_TextManager::TT_TextManager()
: m_bAddToAuxiliaryTab(false)
{
	Utility::SetOnErrFunc(_logErrFunc);
}
//--------------------------------------------------------------
TT_TextManager::~TT_TextManager()
{

}
//--------------------------------------------------------------
void TT_TextManager::Init()
{
	//系统公告频道，只有1个TAB，保留1条信息
	m_kChannel_Announce.SetChannelMaxTextCount(20);
	m_kChannel_Announce.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,20);

	//通用系统提示频道，只有一个TAB，保留3条信息
	m_kChannel_Tip.SetChannelMaxTextCount(1);
	m_kChannel_Tip.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,1);

	//副本信息提示频道，1个TAB，3条信息
	m_kChannel_Transcription.SetChannelMaxTextCount(3);
	m_kChannel_Transcription.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,3);

	//聊天频道，5个TAB，每个TAB 50条信息，总共可保持1000条信息
	m_kChannel_Chat.SetChannelMaxTextCount(1600);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_NORMAL, 200);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_SYSTEM, 200);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_NEARBY, 200);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_PRIVATE, 200);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_TEAM, 200);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_GUILD, 200);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_WORLD, 200);
	m_kChannel_Chat.RegisterTab(CHAT_CHANNEL_TAB_TYPE_TRUMPET, 200);

	//副聊天框
	m_kChannel_SubChat.SetChannelMaxTextCount(4);
	m_kChannel_SubChat.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL, 4);

	//怒气值上面
	m_kChannel_RageUP.SetChannelMaxTextCount(5);
	m_kChannel_RageUP.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL, 5);

	//密室宝藏全服通知
	m_kChannel_Treasure.SetChannelMaxTextCount(3);
	m_kChannel_Treasure.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,3);


	//小喇叭频道，1个TAB，保留1条信息
	m_kChannel_Trumpet.SetChannelMaxTextCount(1);
	m_kChannel_Trumpet.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,1);

	m_kChannel_SuperTrumpet.SetChannelMaxTextCount(1);
	m_kChannel_SuperTrumpet.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,1);
	
	//好友私聊频道，1个TAB，100条信息
	m_kChannel_Friend.SetChannelMaxTextCount(100);
	m_kChannel_Friend.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,100);

	//无模式窗口频道，1个TAB，1条信息
	m_kChannel_MessagePrompt.SetChannelMaxTextCount(1);
	m_kChannel_MessagePrompt.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,1);

	//OK模式窗口频道，1个TAB，1条信息
	m_kChannel_MessageBox.SetChannelMaxTextCount(1);
	m_kChannel_MessageBox.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,1);

	//YES/NO模式窗口频道，1个TAB，1条信息
	m_kChannel_MessageDialog.SetChannelMaxTextCount(1);
	m_kChannel_MessageDialog.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,1);
	
	//聊天泡泡频道，1个TAB，20条信息
	m_kChannel_BubbleTalking.SetChannelMaxTextCount(20);
	m_kChannel_BubbleTalking.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,20);

	//世界名称频道，1个TAB，1条信息
	m_kChannel_WorldName.SetChannelMaxTextCount(1);
	m_kChannel_WorldName.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,1);

	//活动信息提示， 1个TAB，3条信息
	m_kChannel_ActivityTip.SetChannelMaxTextCount(3);
	m_kChannel_ActivityTip.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL, 1);

	//跟随鼠标提示嘻嘻， 1个TAB，100条信息
	m_kChannel_MouseTip.SetChannelMaxTextCount(1);
	m_kChannel_MouseTip.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL, 1);

	//跟随窗口提示，1个TAB, 1条信息
	m_kChannel_WidgetTip.SetChannelMaxTextCount(1);
	m_kChannel_WidgetTip.RegisterTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL, 1);


	m_bAddToAuxiliaryTab = false;
}
//--------------------------------------------------------------
void TT_TextManager::UnInit()
{
	Reset();
}
//--------------------------------------------------------------
void TT_TextManager::Reset()
{
	m_kChannel_Announce.Reset();
	m_kChannel_Tip.Reset();
	m_kChannel_Chat.Reset();
	m_kChannel_SubChat.Reset();
	m_kChannel_Trumpet.Reset();
	m_kChannel_SuperTrumpet.Reset();
	m_kChannel_Friend.Reset();
	m_kChannel_MessagePrompt.Reset();
	m_kChannel_MessageBox.Reset();
	m_kChannel_MessageDialog.Reset();
	m_kChannel_BubbleTalking.Reset();
	m_kChannel_WorldName.Reset();
	m_kChannel_Transcription.Reset();
	m_kChannel_ActivityTip.Reset();
	m_kChannel_MouseTip.Reset();
	m_kChannel_WidgetTip.Reset();
	m_kChannel_RageUP.Reset();
	m_kChannel_Treasure.Reset();
	m_bAddToAuxiliaryTab = false;
}
//--------------------------------------------------------------
bool TT_TextManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK,_event_TimerTick)
	END_MESSAGE_HANDLE
	return true;
}
//--------------------------------------------------------------
void TT_TextManager::AddTextElement(TT_TextElement& rkTextElement)
{
	int iTextMessageType = rkTextElement.GetTextMessageType();
	CF_TextConfig* pkCSVTextConfig = CF_TextConfig::GetSingletonPtr();
	if(!pkCSVTextConfig)
		return;
	CF_TextConfig::DataEntry* pkData = pkCSVTextConfig->GetEntryPtr(iTextMessageType);
	if(!pkData)
		return;

	if (rkTextElement.GetBaseText().empty())
		return ;

	int		iTextIndexInChannel = 0;
	int		iRemoveTextCount = 0;
	
	//系统公告频道
	if(pkData->_bChannelAnnounce)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_ANNOUNCE);
		
		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_Announce.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Announce.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_Announce.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//系统提示频道
	if(pkData->_bChannelTip)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_TIP);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + TEXT_CHANNEL_TIP_TIME);

		iTextIndexInChannel = m_kChannel_Tip.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Tip.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_Tip.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//跟随鼠标提示信息
	if(pkData->_bChannelMouseTip)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_MOUSE);

		//TT_TextElement kAddTextElement;
		//kAddTextElement = rkTextElement;
		//kAddTextElement.m_strText = strPrompText;
		//kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + 5.0f);

		//iTextIndexInChannel = m_kChannel_MouseTip.AddTextElement(kAddTextElement,iRemoveTextCount);
		//if(iRemoveTextCount > 0)
		//{
		//	for(int i = 0; i < iRemoveTextCount; i++)
		//		m_kChannel_MouseTip.RemoveTextElementIndexInTabs(0);
		//}
		//
		//m_kChannel_MouseTip.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_MOUSE_TIP);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(strPrompText);
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	//跟随窗口提示信息
	if(pkData->_bChannelWgtTip)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData, TEXT_CHANNEL_TYPE_TIP);

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_WIDGET_TIP);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(strPrompText);
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	//副本信息提示频道
	if (pkData->_bChannelTranscription)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_TRANSCRIPTION);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + TEXT_CHANNEL_TRANSCRIPTION_TIME);

		iTextIndexInChannel = m_kChannel_Transcription.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Transcription.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_Transcription.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//UI右下角副聊天框
	if(pkData->_bChannelChat_Tab_Auxiliary)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData, TEXT_CHANNEL_TYPE_SUBCHAT);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + TEXT_CHANNEL_SUBCHAT_TIME);

		iTextIndexInChannel = m_kChannel_SubChat.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_SubChat.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_SubChat.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);		
	}
	//怒气值上方
	if(pkData->_bChannelRageUP)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData, TEXT_CHANNEL_TYPE_RAGEUP);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + TEXT_CHANNEL_RAGEUP_TIME);

		iTextIndexInChannel = m_kChannel_RageUP.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_RageUP.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_RageUP.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);		
	}
	//密室藏宝全服通知
	if(pkData->_bChannelTreasure)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData, TEXT_CHANNEL_TYPE_TREASURE);

		/*TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + TEXT_CHANNEL_TREASURE_TIME);

		iTextIndexInChannel = m_kChannel_Treasure.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Treasure.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_Treasure.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);		
		*/
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_TREASURE_TIP);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(strPrompText);
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	
	}
	//聊天频道
	if(pkData->_bChannelChat)
	{
		StringType strPrompText;
		//if (pkData->_bChannelChat_Tab_Nearby)
		//{	
			//LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			//srcID
			//LC_PlayerBase* playerbase = pkNetPlayerManager->FindPlayer(rkTextElement.m_ui64Parameters[0]);
			
			//如果playerbase->IsClone()为真，则表示为好友喊话
			//if (playerbase )
			//{
			//	strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_CHAT, playerbase->IsClone() );
			//}
			//else
			//{
				//strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_CHAT);
			//}
		//}
		//else
		//{
			strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_CHAT);
		//}
		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_Chat.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Chat.RemoveTextElementIndexInTabs(0);
		}

		if(pkData->_bChannelChat_Tab_Normal)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
		if(pkData->_bChannelChat_Tab_System)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_SYSTEM,iTextIndexInChannel);
#ifndef _SHIPPING
		if(pkData->_bChannelChat_Tab_Fight)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_SYSTEM, iTextIndexInChannel);
#endif
		if(pkData->_bChannelChat_Tab_Nearby)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_NEARBY,iTextIndexInChannel);
		if(pkData->_bChannelChat_Tab_Private)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_PRIVATE,iTextIndexInChannel);
		if(pkData->_bChannelChat_Tab_Team)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_TEAM,iTextIndexInChannel);
		if(pkData->_bChannelChat_Tab_Guild)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_GUILD, iTextIndexInChannel);
		if(pkData->_bChannelChat_Tab_World)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_WORLD, iTextIndexInChannel);
		if(pkData->_bChannelChat_Tab_Trumpet)
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_TRUMPET, iTextIndexInChannel);
		
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GameLogic::GlET_NEW_CHAT_MSG);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, pkData->_iID);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, pkData->_bChannelChat_Tab_Guild);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
		/*if (IsAddToAuxiliaryTab())
		{
			m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_AUXILIARY,iTextIndexInChannel);
			SetAddToAuxiliaryTabFlag(false);
		}*/
	}
	
	//小喇叭
	if(pkData->_bChannelTrumpet)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_TRUMPET);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + SMALLTRUMPT_VIEW_TIME_UPPER);

		iTextIndexInChannel = m_kChannel_Trumpet.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Trumpet.RemoveTextElementIndexInTabs(0);
		}
		
		m_kChannel_Trumpet.AddTrumpetTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//大喇叭
	if(pkData->_bChannelSuperTrumpet)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_SUPER_TRUMPET);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + SUPERTRUMPT_VIEW_TIME_UPPER);

		iTextIndexInChannel = m_kChannel_SuperTrumpet.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_SuperTrumpet.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_SuperTrumpet.AddTrumpetTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//好友私聊
	if(pkData->_bChannelFriend)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_FRIEND);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_strServerTime = GetServerTimeString();

		iTextIndexInChannel = m_kChannel_Friend.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Friend.RemoveTextElementIndexInTabs(0);
		}
	
		m_kChannel_Friend.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//无模式窗口
	if(pkData->_bChannelMessagePrompt)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_MESSAGE_PROMPT);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_MessagePrompt.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_MessagePrompt.RemoveTextElementIndexInTabs(0);
		}
		
		m_kChannel_MessagePrompt.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//OK模式窗口
	if(pkData->_bChannelMessageBox)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_MESSAGE_BOX);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_MessageBox.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_MessageBox.RemoveTextElementIndexInTabs(0);
		}
		
		m_kChannel_MessageBox.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//YES/NO模式窗口
	if(pkData->_bChannelMessageDialog)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_MESSAGE_DIALOG);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_MessageDialog.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_MessageDialog.RemoveTextElementIndexInTabs(0);
		}
	
		m_kChannel_MessageDialog.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//泡泡聊天
	if(pkData->_bChannelBubbleTalking)
	{
		StringType strPrompText;
		//if (pkData->_bChannelChat_Tab_Nearby)
		//{	
			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			//srcID
			LC_PlayerBase* playerbase = pkNetPlayerManager->FindPlayer(rkTextElement.m_ui64Parameters[0]);

			//如果playerbase->IsClone()为真，则表示为好友喊话
			//if (playerbase )
			//{
			//	strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_BUBBLE_TALKING, playerbase->IsClone() );
			//}
			//else
			//{
			//	strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_BUBBLE_TALKING);
			//}
		//}
		//else
		//{
		strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_BUBBLE_TALKING);
		//}

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_BubbleTalking.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_BubbleTalking.RemoveTextElementIndexInTabs(0);
		} 
	
		m_kChannel_BubbleTalking.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//世界名称
	if(pkData->_bChannelWorldName)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_WORLD_NAME);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_WorldName.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_WorldName.RemoveTextElementIndexInTabs(0);
		}
	
		m_kChannel_WorldName.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//屏幕下方
	if (pkData->_bChannelDownTip)
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData, TEXT_CHANNEL_TYPE_ACTIVITY);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;
		kAddTextElement.m_lTextLifeTime = Float2Long(GET_PROCESS_TIME + TEXT_CHANNEL_ACTIVITY_TIME);

		iTextIndexInChannel = m_kChannel_ActivityTip.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_ActivityTip.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_ActivityTip.AddTextElementIndexToTab(GENERAL_CHANNEL_TAB_TYPE_NORMAL,iTextIndexInChannel);
	}

	//右下角提示信息
	if(pkData->_bGameMainShowTips)
	{
		StringType strPrompText = rkTextElement.GetBaseText();

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(CGET_GAMEMAINNOTICE_TIP);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(strPrompText);
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	/*if (IsAddToAuxiliaryTab())
	{
		StringType strPrompText = _getTextContent(rkTextElement, pkData,TEXT_CHANNEL_TYPE_CHAT);

		TT_TextElement kAddTextElement;
		kAddTextElement = rkTextElement;
		kAddTextElement.m_strText = strPrompText;

		iTextIndexInChannel = m_kChannel_Chat.AddTextElement(kAddTextElement,iRemoveTextCount);
		if(iRemoveTextCount > 0)
		{
			for(int i = 0; i < iRemoveTextCount; i++)
				m_kChannel_Chat.RemoveTextElementIndexInTabs(0);
		}

		m_kChannel_Chat.AddTextElementIndexToTab(CHAT_CHANNEL_TAB_TYPE_AUXILIARY,iTextIndexInChannel);
		SetAddToAuxiliaryTabFlag(false);
	}*/
}
//--------------------------------------------------------------
int TT_TextManager::GetChannelTabTextCount(int iChannelType,int iTabType)
{
	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		return m_kChannel_Announce.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_TIP:
		return m_kChannel_Tip.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		return m_kChannel_SubChat.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_RAGEUP:
		return m_kChannel_RageUP.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_TREASURE:
		return m_kChannel_Treasure.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_CHAT:
		return m_kChannel_Chat.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_TRUMPET:
		return m_kChannel_Trumpet.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		return m_kChannel_SuperTrumpet.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_FRIEND:
		return m_kChannel_Friend.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		return m_kChannel_MessagePrompt.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		return m_kChannel_MessageBox.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		return m_kChannel_MessageDialog.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		return m_kChannel_BubbleTalking.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		return m_kChannel_WorldName.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		return m_kChannel_Transcription.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		return m_kChannel_ActivityTip.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_MOUSE:
		return m_kChannel_MouseTip.GetTabTextCount(iTabType);
	case TEXT_CHANNEL_TYPE_WIDGET:
		return m_kChannel_WidgetTip.GetTabTextCount(iTabType);
	default:
		return 0;
	}
}
//--------------------------------------------------------------
int TT_TextManager::GetChannelTextElementCount(int iChannelType)
{
	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		return m_kChannel_Announce.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_TIP:
		return m_kChannel_Tip.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		return m_kChannel_SubChat.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_RAGEUP:
		return m_kChannel_RageUP.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_TREASURE:
		return m_kChannel_Treasure.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_CHAT:
		return m_kChannel_Chat.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_TRUMPET:
		return m_kChannel_Trumpet.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		return m_kChannel_SuperTrumpet.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_FRIEND:
		return m_kChannel_Friend.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		return m_kChannel_MessagePrompt.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		return m_kChannel_MessageBox.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		return m_kChannel_MessageDialog.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		return m_kChannel_BubbleTalking.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		return m_kChannel_WorldName.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		return m_kChannel_Transcription.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		return m_kChannel_ActivityTip.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_MOUSE:
		return m_kChannel_MouseTip.GetTextElementCount();
	case TEXT_CHANNEL_TYPE_WIDGET:
		return m_kChannel_WidgetTip.GetTextElementCount();
	default:
		return 0;
	}
}
//--------------------------------------------------------------
void TT_TextManager::ClearChannelTabText(int iChannelType,int iTabType)
{
	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		m_kChannel_Announce.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_TIP:
		m_kChannel_Tip.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		m_kChannel_SubChat.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_RAGEUP:
		m_kChannel_RageUP.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_TREASURE:
		m_kChannel_Treasure.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_CHAT:
		m_kChannel_Chat.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_TRUMPET:
		m_kChannel_Trumpet.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		m_kChannel_SuperTrumpet.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_FRIEND:
		m_kChannel_Friend.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		m_kChannel_MessagePrompt.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		m_kChannel_MessageBox.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		m_kChannel_MessageDialog.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		m_kChannel_BubbleTalking.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		m_kChannel_WorldName.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		m_kChannel_Transcription.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		m_kChannel_ActivityTip.ClearTabText(iTabType);
		break;
	case  TEXT_CHANNEL_TYPE_MOUSE:
		m_kChannel_MouseTip.ClearTabText(iTabType);
		break;
	case TEXT_CHANNEL_TYPE_WIDGET:
		m_kChannel_WidgetTip.ClearTabText(iTabType);
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------
bool TT_TextManager::GetAndClearChannelTabOderDirtyFlag(int iChannelType,int iTabType)
{
	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		return m_kChannel_Announce.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_TIP:
		return m_kChannel_Tip.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		return m_kChannel_SubChat.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_RAGEUP:
		return m_kChannel_RageUP.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_TREASURE:
		return m_kChannel_Treasure.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_CHAT:
		return m_kChannel_Chat.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_TRUMPET:
		return m_kChannel_Trumpet.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		return m_kChannel_SuperTrumpet.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_FRIEND:
		return m_kChannel_Friend.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		return m_kChannel_MessagePrompt.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		return m_kChannel_MessageBox.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		return m_kChannel_MessageDialog.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		return m_kChannel_BubbleTalking.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		return m_kChannel_WorldName.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		return m_kChannel_Transcription.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		return m_kChannel_ActivityTip.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_MOUSE:
		return m_kChannel_MouseTip.GetAndClearTabOderDirtyFlag(iTabType);
	case TEXT_CHANNEL_TYPE_WIDGET:
		return m_kChannel_WidgetTip.GetAndClearTabOderDirtyFlag(iTabType);
	default:
		return false;
	}
}
//--------------------------------------------------------------
int TT_TextManager::GetAndClearTextIncreaseNum(int iChannelType)
{
	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		return m_kChannel_Announce.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_TIP:
		return m_kChannel_Tip.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		return m_kChannel_SubChat.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_RAGEUP:
		return m_kChannel_RageUP.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_TREASURE:
		return m_kChannel_Treasure.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_CHAT:
		return m_kChannel_Chat.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_TRUMPET:
		return m_kChannel_Trumpet.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		return m_kChannel_SuperTrumpet.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_FRIEND:
		return m_kChannel_Friend.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		return m_kChannel_MessagePrompt.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		return m_kChannel_MessageBox.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		return m_kChannel_MessageDialog.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		return m_kChannel_BubbleTalking.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		return m_kChannel_WorldName.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		return m_kChannel_Transcription.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		return m_kChannel_ActivityTip.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_MOUSE:
		return m_kChannel_MouseTip.GetAndClearTextIncreaseNum();
	case TEXT_CHANNEL_TYPE_WIDGET:
		return m_kChannel_WidgetTip.GetAndClearTextIncreaseNum();
	default:
		return 0;
	}
}

//--------------------------------------------------------------
TT_TextElement* TT_TextManager::GetChannelTabTextElement(int iChannelType,int iTabType,int iIndex)
{
	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		return m_kChannel_Announce.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_TIP:
		return m_kChannel_Tip.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		return m_kChannel_SubChat.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_RAGEUP:
		return m_kChannel_RageUP.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_TREASURE:
		return m_kChannel_Treasure.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_CHAT:
		return m_kChannel_Chat.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_TRUMPET:
		return m_kChannel_Trumpet.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		return m_kChannel_SuperTrumpet.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_FRIEND:
		return m_kChannel_Friend.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		return m_kChannel_MessagePrompt.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		return m_kChannel_MessageBox.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		return m_kChannel_MessageDialog.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		return m_kChannel_BubbleTalking.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		return m_kChannel_WorldName.GetTabTextElement(iTabType,iIndex);
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		return m_kChannel_Transcription.GetTabTextElement(iTabType, iIndex);
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		return m_kChannel_ActivityTip.GetTabTextElement(iTabType, iIndex);
	case TEXT_CHANNEL_TYPE_MOUSE:
		return m_kChannel_MouseTip.GetTabTextElement(iTabType, iIndex);
	case TEXT_CHANNEL_TYPE_WIDGET:
		return m_kChannel_WidgetTip.GetTabTextElement(iTabType, iIndex);
	default:
		return NULL;
	}
}
TT_TextElement* TT_TextManager::GetChannelTextElement(int iChannelType, int iTextIndex)
{
	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		return m_kChannel_Announce.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_TIP:
		return m_kChannel_Tip.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		return m_kChannel_SubChat.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_RAGEUP:
		return m_kChannel_RageUP.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_TREASURE:
		return m_kChannel_Treasure.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_CHAT:
		return m_kChannel_Chat.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_TRUMPET:
		return m_kChannel_Trumpet.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		return m_kChannel_SuperTrumpet.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_FRIEND:
		return m_kChannel_Friend.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		return m_kChannel_MessagePrompt.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		return m_kChannel_MessageBox.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		return m_kChannel_MessageDialog.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		return m_kChannel_BubbleTalking.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		return m_kChannel_WorldName.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		return m_kChannel_Transcription.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		return m_kChannel_ActivityTip.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_MOUSE:
		return m_kChannel_MouseTip.GetTextElement(iTextIndex);
	case TEXT_CHANNEL_TYPE_WIDGET:
		return m_kChannel_WidgetTip.GetTextElement(iTextIndex);
	default:
		return NULL;
	}
}
//--------------------------------------------------------------
void TT_TextManager::EncodeXMLString( const StringType& rkString, StringType& rOutString )
{
	std::string DstTidy;
	std::string srcTidy = rkString.c_str();
	TiXmlBase::EncodeString(srcTidy,&DstTidy);
	rOutString = DstTidy.c_str();
}
//--------------------------------------------------------------
StringType TT_TextManager::GetServerTimeString()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (pkGamePlayer)
	{
		time_t uiServerTime = (time_t)pkGamePlayer->GetServerTimeVal();
		tm pkCurLocalTime =	* localtime(&uiServerTime);

		StringStreamType kString;
		kString << pkCurLocalTime.tm_yday;
		kString << ":";
		kString << pkCurLocalTime.tm_year;
		kString << ":";
		kString << pkCurLocalTime.tm_mon;
		kString << ":";
		kString << pkCurLocalTime.tm_mday;
		kString << ":";
		kString << pkCurLocalTime.tm_hour;
		kString << ":";
		kString << pkCurLocalTime.tm_min;
		kString << ":";
		kString << pkCurLocalTime.tm_sec;

		return kString.str();
	}

	return "";
}
//--------------------------------------------------------------
bool TT_TextManager::IsInChannel(int iMessageType, int iChannelType, int iTabType)
{
	bool bResult = false;
	CF_TextConfig::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_TextConfig>(iMessageType);
	if(NULL == pkData)
		return bResult;

	switch(iChannelType)
	{
	case TEXT_CHANNEL_TYPE_CHAT:
		{
			if(iTabType == CHAT_CHANNEL_TAB_TYPE_SYSTEM && pkData->_bChannelChat_Tab_System)
				bResult = true;
		}
		break;

	}

	return bResult;
}
//--------------------------------------------------------------
void TT_TextManager::_event_TimerTick(MG_MessageBase& rkMessage)
{
	T_PROFILE("Update::TT_TextManager");
	float fCurrentTime = rkMessage.m_fParam1;
	m_kChannel_Announce.Update(fCurrentTime);
	m_kChannel_Tip.Update(fCurrentTime);
	m_kChannel_Chat.Update(fCurrentTime);
	m_kChannel_SubChat.Update(fCurrentTime);
	m_kChannel_RageUP.Update(fCurrentTime);
	m_kChannel_Treasure.Update(fCurrentTime);
	m_kChannel_Trumpet.Update(fCurrentTime);
	m_kChannel_SuperTrumpet.Update(fCurrentTime);
	m_kChannel_Friend.Update(fCurrentTime);
	m_kChannel_MessagePrompt.Update(fCurrentTime);
	m_kChannel_MessageBox.Update(fCurrentTime);
	m_kChannel_MessageDialog.Update(fCurrentTime);
	m_kChannel_BubbleTalking.Update(fCurrentTime);
	m_kChannel_WorldName.Update(fCurrentTime);
	m_kChannel_Transcription.Update(fCurrentTime);
	m_kChannel_ActivityTip.Update(fCurrentTime);
	m_kChannel_MouseTip.Update(fCurrentTime);
	m_kChannel_WidgetTip.Update(fCurrentTime);
}
//--------------------------------------------------------------
StringType TT_TextManager::_getTextContent(TT_TextElement& rkTextElement,CSVFile::CF_TextConfig::DataEntry* pkConfigData,TextChannelType eChannelType)
{
	long lBaseColor = 0xFFFFFFFF;
	long lParamColors[5];
	
	_fillColor(rkTextElement, pkConfigData, eChannelType, lBaseColor, lParamColors);
	
	StringType strBaseColor;
	StringType strParamColor[5];

	CF_TextColor* pkCSVTextColor = CF_TextColor::GetSingletonPtr();
	if(!pkCSVTextColor)
		return "";

	CF_TextColor::DataEntry* pkData = NULL;
	
	pkData = pkCSVTextColor->GetEntryPtr(lBaseColor);
	if(pkData)
		strBaseColor = pkData->_kColorValue;
	else
		strBaseColor = "0xFF000000";

	unique_id_impl iSrcCharID(rkTextElement.m_ui64Parameters[0]);
	//bool bSrcPlayrer = IS_CHAR_ID(iSrcCharID.detail()) && iSrcCharID.catagory() > 0;
	bool bSrcPlayrer = iSrcCharID.detail() > 0 && iSrcCharID.catagory() > 0;
	StringType kResult = rkTextElement.m_strBaseText;
	if (!bSrcPlayrer)
	{
		for(int i = 0; i < 5; i++)
		{
			pkData = pkCSVTextColor->GetEntryPtr(lParamColors[i]);
			if(pkData)
				strParamColor[i] = pkData->_kColorValue;
			else
				strParamColor[i] = "0xFF000000";
		}

		const char* byParam[5];
		byParam[0] = rkTextElement.m_strWildCards[0].c_str();
		byParam[1] = rkTextElement.m_strWildCards[1].c_str();
		byParam[2] = rkTextElement.m_strWildCards[2].c_str();
		byParam[3] = rkTextElement.m_strWildCards[3].c_str();
		byParam[4] = rkTextElement.m_strWildCards[4].c_str();

		char strbuffer[1024];

		for(int i = 0; i < 5; i++)
		{
			_snprintf_s(strbuffer, _countof(strbuffer), _TRUNCATE, "<color val=%s>%s</color>",strParamColor[i].c_str(),byParam[i]);
			rkTextElement.m_strFormatWildCards[i] = strbuffer;
		}

		const StringType *pStrFormat = rkTextElement.m_strFormatWildCards;
		char kResultBuf[2048];
		_snprintf(kResultBuf, _countof(kResultBuf), rkTextElement.m_strBaseText.c_str(),pStrFormat[0].c_str(),pStrFormat[1].c_str(),pStrFormat[2].c_str(),pStrFormat[3].c_str(),pStrFormat[4].c_str());
		kResultBuf[_countof(kResultBuf)-1] = '\0';
		kResult = StringType(kResultBuf);
	}
	StringType strBaseColorFmt = "<color val=" + strBaseColor + ">";
	kResult.insert(0,strBaseColorFmt);
	kResult.append("</color>");
	

	return kResult;
}

void TT_TextManager::_fillColor(const TT_TextElement& rkTextElement, CSVFile::CF_TextConfig::DataEntry* pkConfigData,TextChannelType eChannelType, long &lBaseColor, long (&lParamColors)[5])
{
	switch(eChannelType)
	{
	case TEXT_CHANNEL_TYPE_ANNOUNCE:
		{
			lBaseColor	 = pkConfigData->_lChannelAnnounce_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelAnnounce_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelAnnounce_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelAnnounce_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelAnnounce_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelAnnounce_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_TIP:
		{
			lBaseColor     = pkConfigData->_lChannelTip_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelTip_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelTip_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelTip_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelTip_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelTip_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_SUBCHAT:
		{
			lBaseColor     = pkConfigData->_lChannelChat_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelChat_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelChat_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelChat_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelChat_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelChat_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_RAGEUP:
		{
			lBaseColor     = pkConfigData->_lChannelChat_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelChat_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelChat_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelChat_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelChat_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelChat_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_TREASURE:
		{
			lBaseColor     = pkConfigData->_lChannelChat_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelChat_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelChat_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelChat_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelChat_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelChat_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_CHAT:
		{
			lBaseColor     = pkConfigData->_lChannelChat_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelChat_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelChat_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelChat_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelChat_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelChat_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_TRUMPET:
		{
			lBaseColor     = pkConfigData->_lChannelTrumpet_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelTrumpet_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelTrumpet_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelTrumpet_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelTrumpet_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelTrumpet_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_FRIEND:
		{
			lBaseColor     = pkConfigData->_lChannelFriend_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelFriend_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelFriend_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelFriend_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelFriend_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelFriend_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_MESSAGE_PROMPT:
		{
			lBaseColor     = pkConfigData->_lChannelMessagePrompt_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelMessagePrompt_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelMessagePrompt_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelMessagePrompt_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelMessagePrompt_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelMessagePrompt_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_MESSAGE_BOX:
		{
			lBaseColor     = pkConfigData->_lChannelMessageBox_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelMessageBox_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelMessageBox_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelMessageBox_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelMessageBox_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelMessageBox_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_MESSAGE_DIALOG:
		{
			lBaseColor     = pkConfigData->_lChannelMessageDialog_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelMessageDialog_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelMessageDialog_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelMessageDialog_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelMessageDialog_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelMessageDialog_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_BUBBLE_TALKING:
		{
			lBaseColor     = pkConfigData->_lChannelBubbleTalking_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelBubbleTalking_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelBubbleTalking_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelBubbleTalking_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelBubbleTalking_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelBubbleTalking_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_WORLD_NAME:
		{
			lBaseColor     = pkConfigData->_lChannelWorldName_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelWorldName_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelWorldName_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelWorldName_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelWorldName_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelWorldName_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_TRANSCRIPTION:
		{
			lBaseColor     = pkConfigData->_lChannelTranscription_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelTranscription_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelTranscription_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelTranscription_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelTranscription_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelTranscription_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_SUPER_TRUMPET:
		{
			lBaseColor     = pkConfigData->_lChannelSuperTrumpet_BaseColor;
			lParamColors[0] = pkConfigData->_lChannelSuperTrumpet_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelSuperTrumpet_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelSuperTrumpet_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelSuperTrumpet_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelSuperTrumpet_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_ACTIVITY:
		{
			if (rkTextElement.GetLevel() != PROMPT_LEVEL_NO)
			{
				switch(rkTextElement.GetLevel())
				{
				case PROMPT_LEVEL_LOW:
					lBaseColor		=  pkConfigData->_lChannelDownTip_BaseColor1;
					break;
				case PROMPT_LEVEL_NORMAL:
					lBaseColor		=  pkConfigData->_lChannelDownTip_BaseColor2;
					break;
				case PROMPT_LEVEL_HIGH:
					lBaseColor		=  pkConfigData->_lChannelDownTip_BaseColor3;
					break;
				}
			}

			lParamColors[0] = pkConfigData->_lChannelDownTip_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelDownTip_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelDownTip_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelDownTip_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelDownTip_WildcardColor5;
		}
		break;
	case TEXT_CHANNEL_TYPE_MOUSE:
		{
			lBaseColor     = pkConfigData->_lChannelMouseTip_BaseColor1;
			lParamColors[0] = pkConfigData->_lChannelMouseTip_WildcardColor1;
			lParamColors[1] = pkConfigData->_lChannelMouseTip_WildcardColor2;
			lParamColors[2] = pkConfigData->_lChannelMouseTip_WildcardColor3;
			lParamColors[3] = pkConfigData->_lChannelMouseTip_WildcardColor4;
			lParamColors[4] = pkConfigData->_lChannelMouseTip_WildcardColor5;
		}
		break;
	}
}

int TT_TextManager::_logErrFunc(const char* sMsg)
{
	if ( GetGlobalSetting.GetnGMLevel() > 0 )
	{
		SendLogicInfoMessage(RE_LOG_ERROR_MESSAGE, CodeConvert::MbsToUtf8_fast(sMsg));
	}

	return 1;
}