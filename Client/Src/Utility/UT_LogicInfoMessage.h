#pragma once

#include "MG_MessageCenter.h"
#include "TT_TextManager.h"

#include "CF_SystemError.h"
#include "CF_GameStoryMessage.h"

using namespace CSVFile;

namespace Utility
{
	//这里也有拷贝开销，后面统一修改
#define LOGIC_INFO_ASSIGN_STR(DstPtr,SrcType)\
	StringType kStr##SrcType = TypeValueToStringEx(SrcType);\
	DstPtr = ( kStr##SrcType.c_str());\


	 //多次拷贝，影响效率，后面修改
	inline StringType TypeValueToStringEx(const StringType& tValue)
	{
		return tValue;
	}


	inline StringType TypeValueToStringEx(char*  tValue)
	{
		return tValue;
	}


	inline StringType TypeValueToStringEx(const char * tValue)
	{
		return tValue;
	}

	inline StringType TypeValueToStringEx(int tValue)
	{
		return Int2String(tValue);
	}


	inline StringType TypeValueToStringEx(char tValue)
	{
		return Int2String((int)tValue);
	}


	inline StringType TypeValueToStringEx(unsigned char tValue)
	{
		return Int2String((int)tValue);
	}


	inline StringType TypeValueToStringEx(float tValue)
	{
		return Float2String(tValue);
	}



	inline StringType TypeValueToStringEx(double tValue)
	{
		return Double2String(tValue);
	}


	inline StringType TypeValueToStringEx(bool  tValue)
	{
		return Bool2String(tValue);
	}


	inline StringType TypeValueToStringEx(long  tValue)
	{
		return Long2String(tValue);
	}

	inline StringType TypeValueToStringEx(int64_t  tValue)
	{
		return Int642String(tValue);
	}

	inline StringType TypeValueToStringEx(uint32_t tValue)
	{
		return Int2String(tValue);
	}


	inline void SetMessageToAuxiliaryTab()
	{
		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();
		if(pkTextManager)
			pkTextManager->SetAddToAuxiliaryTabFlag(true);
	}

	inline bool SendNotifyMessage(long lMsgType, const StringType& kMsgText, int iLevel = PROMPT_LEVEL_NO)
	{
		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = lMsgType;
		kTextElement.m_strBaseText		= kMsgText;
		kTextElement.m_iLevel			= iLevel;

		//这个Notify消息是不会通知聊天消息的所以不会有发送者，如果这个消息显示在帮会频道，只会前缀会显示帮会公告
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],kMsgText);
		kTextElement.m_ui64Parameters[0]	= GUILD_NOTIFICATION;

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}
	inline bool SendLogicInfoMessage(long lMsgID)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	template<typename T>
	bool SendLogicInfoMessage(long lMsgID,
		const T& typeValue)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue);
	
		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	template<typename T1,typename T2>
	bool SendLogicInfoMessage(long lMsgID,
		const T1& typeValue1,
		const T2& typeValue2)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],typeValue2);

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	template<typename T1,typename T2, typename T3>
	bool SendLogicInfoMessage(long lMsgID,
		const T1& typeValue1,
		const T2& typeValue2,
		const T3& typeValue3)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],typeValue2);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[2],typeValue3);

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	template<typename T1,typename T2, typename T3, typename T4>
	bool SendLogicInfoMessage(long lMsgID,
		const T1& typeValue1,
		const T2& typeValue2,
		const T3& typeValue3,
		const T4& typeValue4)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],typeValue2);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[2],typeValue3);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[3],typeValue4);

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	template<typename T1,typename T2, typename T3, typename T4, typename T5>
	bool SendLogicInfoMessage(long lMsgID,
		const T1& typeValue1,
		const T2& typeValue2,
		const T3& typeValue3,
		const T4& typeValue4,
		const T5& typeValue5)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],typeValue2);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[2],typeValue3);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[3],typeValue4);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[4],typeValue5);

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	template<typename T1,typename T2, typename T3, typename T4, typename T5>
	bool SendLogicInfoMessage(long lMsgID,
		const T1& typeValue1,
		const T2& typeValue2,
		const T3& typeValue3,
		const T4& typeValue4,
		const T5& typeValue5,
		long lMsgType)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],typeValue2);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[2],typeValue3);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[3],typeValue4);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[4],typeValue5);
		kTextElement.m_lParameters[0]		=   lMsgType;

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

        //保持兼容性临时修改
	template<typename T1>
	bool SendTaskInfoMessage(long lMsgID,
		const T1& typeValue1)
	{
		CF_GameStoryMessage* pkCSVStoryMessage = CF_GameStoryMessage::GetSingletonPtr();
		CF_GameStoryMessage::DataEntry* pkData = pkCSVStoryMessage->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_iMsgType;
		kTextElement.m_strBaseText			= pkData->_sMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	//世界名称
	template<typename T1,typename T2>
	bool SendWorldNameInfoMessage(long lMsgID,
		const T1& strAreaName,
		const T2& strAreaType,
		long  lMapID,
		long  lAreaType,
		long  lMapLogicID)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],strAreaName);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],strAreaType);
		kTextElement.m_lParameters[0]	= lMapID;
		kTextElement.m_lParameters[1]	= lAreaType;
		kTextElement.m_lParameters[2]	= lMapLogicID;

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	//YES/NO对话框
	template<typename T1,typename T2>
	bool SendMessageDialog(long lMsgID,
		const T1& typeValue1,
		const T2& typeValue2,
		long  lMessageDialogType,
		long  lPara1,
		long  lPara2,
		long  lPara3)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],typeValue2);
		kTextElement.m_lParameters[0]	= lMessageDialogType;
		kTextElement.m_lParameters[1]	= lPara1;
		kTextElement.m_lParameters[2]	= lPara2;
		kTextElement.m_lParameters[3]	= lPara3;

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}
	
	template<typename T1,typename T2, typename T3>
	bool SendMessageDialog(long lMsgID,
		const T1& typeValue1,
		const T2& typeValue2,
		const T3& typeValue3,
		long  lMessageDialogType,
		long  lPara1,
		long  lPara2,
		long  lPara3,
		long  lPara4,
		long  lPara5)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType		= pkData->_lMsgType;
		kTextElement.m_strBaseText			= pkData->_sErrorMsg;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],typeValue1);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],typeValue2);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[2],typeValue3);
		kTextElement.m_lParameters[0]	= lMessageDialogType;
		kTextElement.m_lParameters[1]	= lPara1;
		kTextElement.m_lParameters[2]	= lPara2;
		kTextElement.m_lParameters[3]	= lPara3;
		kTextElement.m_lParameters[4]	= lPara4;
		kTextElement.m_lParameters[5]	= lPara5;

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}

	//聊天信息
	template<typename T1,typename T2,typename T3>
	bool SendChatMsg(long lMsgID,
		const T1& strChatText,
		const T2& strChatPlayerName,
		const T3& strChatTime,
		uint64_t   lChatSrcID,
		uint64_t   lChatDesID,
		uint32_t   lSrcUserID = 0,
		uint32_t   lSreGmPriority = MAX_GM_RPIORITY)
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(lMsgID);
		if(!pkData)
			return false;

		TT_TextElement kTextElement;
		kTextElement.m_lTextMessageType = pkData->_lMsgType;
		kTextElement.m_strBaseText			= /*kTextElement.m_strWildCards[0],*/strChatText;
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[0],strChatText);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[1],strChatPlayerName);
		LOGIC_INFO_ASSIGN_STR(kTextElement.m_strWildCards[2],strChatTime);
		kTextElement.m_ui64Parameters[0]	= lChatSrcID;
		kTextElement.m_ui64Parameters[1]	= lChatDesID;

		kTextElement.m_lParameters[0] = lSrcUserID;
		kTextElement.m_lParameters[1] = lSreGmPriority;

		TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();

		pkTextManager->AddTextElement(kTextElement);
		return true;
	}
}