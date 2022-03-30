#pragma once

#include "CommonDef.h"
#include "StringDefine.h"
#include <sstream>
#include "ajson.hpp"

//////////////////////////////////////////////////////////////////
template<typename T>
std::string getJsonInfo(const T& rNode)
{
	std::stringstream strInfo;
	boost::ajson::save_to_buff_(rNode, strInfo);
	return strInfo.str();
}
//////////////////////////////////////////////////////////////////
class publish_node
{
public:
	uint32_t serverid;
	uint32_t messageid;
	mem::vector<std::string> plattype;
	std::string data;
	uint32_t result;

public:
	publish_node(uint32_t nMsg);
	publish_node& setServerId(uint32_t nId = 0);
	publish_node& setDataStr(const std::string& sData);
	publish_node& setResult(uint32_t nRel);

	template<typename T>
	publish_node& setData(const T& sData)
	{
		return setDataStr(getJsonInfo(sData));
	}

	template<typename T>
	publish_node(uint32_t nMsg, const T& sData): messageid(nMsg), result(0)
	{
		setServerId();
		setData(sData);
	}

	template<typename T>
	publish_node(uint32_t nMsg, int nType, const T& sData): messageid(nMsg), result(0)
	{
		setServerId();
		setData(sData);
	}
};
std::string getPublishInfoJoin(const publish_node& rNode);

#ifndef AJSON_PUBLISH_NODE
#define AJSON_PUBLISH_NODE
AJSON(publish_node, (serverid)(messageid)(plattype)(data)(result))
#endif
//////////////////////////////////////////////////////////////////

struct chatData_Info
{
	std::string account;
	std::string actor;
	uint64_t	actor_id;
	std::string to_account;
	std::string to_actor;
	std::string ip;
	int			type;
	int			level;
	int			gold;
};

struct chatData_Node
{
	std::string info;
	std::string content;
};

#ifndef AJSON_COM_CHATDATA
#define AJSON_COM_CHATDATA
AJSON(chatData_Info, (account)(actor)(actor_id)(to_account)(to_actor)(ip)(type)(level)(gold))
#endif
//////////////////////////////////////////////////////////////////
