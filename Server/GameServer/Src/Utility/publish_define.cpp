#include "publish_define.h"
#include "UT_ServerHelper.h"
//////////////////////////////////////////////////////////////////
publish_node::publish_node(uint32_t nMsg): serverid(UT_ServerHelper::GetServerId()), messageid(nMsg), plattype(), data(""), result(0)
{}

publish_node& publish_node::setServerId(uint32_t nId)
{
	if(nId == 0)
	{
		nId = UT_ServerHelper::GetServerId();
	}
	serverid = nId;
	return *this;
}


publish_node& publish_node::setDataStr(const std::string& sData)
{
	data = sData;
	return *this;
}

publish_node& publish_node::setResult(uint32_t nRel)
{
	result = nRel;
	return *this;
}

std::string getPublishInfoJoin(const publish_node& rNode)
{
	std::stringstream strInfo;
	strInfo << rNode.serverid << "|" << rNode.messageid << "|";
	for(size_t i = 0; i < rNode.plattype.size(); i++)
	{
		strInfo << rNode.plattype[i] << ",";
	}
	strInfo << "|" << rNode.result << "|" << rNode.data;
	return strInfo.str();
}
