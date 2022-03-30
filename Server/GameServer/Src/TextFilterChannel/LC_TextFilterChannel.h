#ifndef __LC_TEXT_FILTER_CHANNEL_H__
#define __LC_TEXT_FILTER_CHANNEL_H__

#pragma once

#include <string>

#include <boost/atomic.hpp>
#include <boost/lockfree/queue.hpp>

#include "MG_ChatProtocol.h"
#include "MG_CS_Guild.h"
#include "simple_thread.h"
#include "SystemError.h"
#include "TT_TextFilter.h"
#include "UT_Redis.h"

#define MAX_CHAT_REQ_QUEUE_SIZE 0xFFFF
#define MAX_CHAT_RESP_QUEUE_SIZE 0xFFFF
#define FORBID_LIST_SYNC_INTERVAL 600
//#define SafeDelete(ptr) if (ptr) {delete ptr; ptr=0;}

using namespace Utility;
using namespace Protocol;

class TextFilterRequest : public Memory::MM_BaseObject
{
public:
	TextFilterRequest() : m_iForbidLevel(0) {}
	virtual void HandleRequest(TT_TextFilter& kTextFilter) = 0;
	virtual void HandleResponse() = 0;

	int32_t m_iForbidLevel;
};

class ChatRequest : public TextFilterRequest
{
public:
	ChatRequest(const MG_REQ_RequestChat& kReq)
		: TextFilterRequest(), m_kReq(kReq) {}
	virtual void HandleRequest(TT_TextFilter& kTextFilter);
	virtual void HandleResponse();

	StringType _removeChatContentPrefix(const std::string& strContent);

	std::string m_sContentEncode;
	std::string m_sTargetNameEncode;
	MG_REQ_RequestChat m_kReq;
};

class CreateGuildRequest : public TextFilterRequest
{
public:
	CreateGuildRequest(const MG_REQ_CreateGuild& kReq, user_id_type uiUserID)
		: TextFilterRequest(), m_uiUserID(uiUserID), m_kReq(kReq) {}
	virtual void HandleRequest(TT_TextFilter& kTextFilter);
	virtual void HandleResponse();

	user_id_type m_uiUserID;
	MG_REQ_CreateGuild m_kReq;
};

class TextFilterChannel : public simple_thread
{
public:
	TextFilterChannel() : m_fLastUpdateTime(0), m_iForbidListLastSyncTime(0)
				  , m_kRequests(MAX_CHAT_REQ_QUEUE_SIZE)
				  , m_kResponses(MAX_CHAT_RESP_QUEUE_SIZE) {}
	bool Init();
	bool Unit();

	void Update(float fTime);
	void BounedPush(const MG_REQ_RequestChat& kReq);
	void BounedPush(const MG_REQ_CreateGuild& kReq, user_id_type uiUserID);
	
	virtual void update(void);
	static void LoadForbidWordListFromRedis(Utility::Redis_Handler* pkRedisHandler, mem::list<std::string>& keywords);

private:
	float m_fLastUpdateTime;
	int64_t m_iForbidListLastSyncTime;
	TT_TextFilter m_kTextFilter;
	Utility::Redis_Handler m_kRedisHandler;

	boost::lockfree::queue<TextFilterRequest*> m_kRequests;
	boost::lockfree::queue<TextFilterRequest*> m_kResponses;
};
#endif
