#if !defined _LC_PLAYER_SNAPSHOT_H_
#define _LC_PLAYER_SNAPSHOT_H_

#include "ajson.hpp"
#include <string>

class _role_data
{
public:
	_role_data()
		: account_id("")
		, role_id("")
		, role_name("")
		, level(0)
		, ip("0.0.0.0")
		{
		}
	std::string account_id;
	std::string role_id;
	std::string role_name;
	int level;
	std::string ip;
};

AJSON(_role_data, (account_id)(role_id)(role_name)(level)(ip));

#endif
