#pragma once

#include <string>

#include "msg.pb.h"
#include "IdDefine.h"
#include "MM_MemoryAllocatorConfig.h"
#include "ajson.hpp"
#include "UT_RedisChannel.h"

#define MAX_HTTP_RESP_QUEUE_SIZE 0xFFFF

enum HTTP_REQ_TYPE {
	HTTP_REQ_360_PRIVI = 0,
};

struct http_req
{
	http_req() : req_id(0), type(0), server_id(0) {}
	int32_t		req_id;
	int32_t		type;
	int32_t		server_id;
	std::string	url;
	std::string	method;
	std::string	param;
};
AJSON(http_req, (req_id)(type)(server_id)(url)(method)(param));

struct http_resp
{
	http_resp() : req_id(0) {}
	int32_t		req_id;
	std::string	result;
};
AJSON(http_resp, (req_id)(result));

class HttpBaseMsg : public Memory::MM_BaseObject
{
public:
	HttpBaseMsg(unique_id_type uiCharID = 0, const StringType sUserName = "")
		: m_iProtocolType(0), m_uiCharID(uiCharID), m_sUserName(sUserName) {}

	virtual void HandleRequest(int32_t iReqID, std::string& data) = 0;
	virtual void HandleResponse(const std::string& sRespMsg) = 0;

	int32_t			m_iProtocolType;
	unique_id_type	m_uiCharID;
	StringType		m_sUserName;
};

class HttpAct360PriviMsg : public HttpBaseMsg
{
public:
	HttpAct360PriviMsg(unique_id_type uiCharID, const StringType sUserName,
		const msg_define::common_stat_operation_req& kReqMsg)
		: HttpBaseMsg(uiCharID, sUserName), m_kReqMsg(kReqMsg) {}

	virtual void HandleRequest(int32_t iReqID, std::string& data);
	virtual void HandleResponse(const std::string& sRespMsg);

private:
	msg_define::common_stat_operation_req m_kReqMsg;
};

class LC_HttpHandler : public Memory::MM_BaseObject
{
public:
	LC_HttpHandler() : m_iReqID(0) {}

	bool Init();
	void UnInit();
	void Clear();

	void HandleRequest(unique_id_type uiCharID, const StringType& sUserName,
		const msg_define::common_stat_operation_req& kReqMsg);

	void HandleResponse(int32_t iReqID, const std::string& sRespMsg);

private:
	int32_t m_iReqID;
	mem::map<int32_t, HttpBaseMsg*> m_kMsgs;
};
