#ifndef WIN32
	#include <sys/syslog.h>
#else
	#include "syslog_port.h"
#endif

#include "oss_log.h"
#include "ajson_mem.hpp"

namespace oss
{
	void oss_mukun_log(const char* tag, const char* info)
	{
		syslog(LOG_INFO | LOG_LOCAL7, "%s" , info);
	}

	void oss_360_log(const char* tag, const char* info)
	{
		syslog(LOG_INFO | LOG_LOCAL5, "%s" , info);
	}

	void oss_internal_log(const char* tag, const char* info)
	{
		syslog(LOG_INFO | LOG_LOCAL5, "%s" , info);
	}

	StringStreamType* get_stringstream()
	{
		static StringStreamType ss;
		ss.str("");
		return &ss;
	}

	typedef mem::set<StringType>  oss_config_type;

	oss_config_type& get_oss_config()
	{
		static oss_config_type obj;
		return obj;
	}

	bool  check_oss(const StringType& type)
	{
		oss_config_type& obj = get_oss_config();
		if (obj.find(type) != obj.end())
		{
			return false;
		}
		return true;
	}

	void  enable_oss(const StringType& type)
	{
		oss_config_type& obj = get_oss_config();
		obj.erase(type);
	}

	void  disable_oss(const StringType& type)
	{
		oss_config_type& obj = get_oss_config();
		obj.insert(type);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	oss_obj::oss_obj(const char* name , int wolrd_id, int nType)
		: /*m_name_(name),*/ m_world_id_(wolrd_id), m_ss_(*get_stringstream()), m_nType(nType)
	{
		m_ss_ << name;
		m_ch[oss_type_orig] = '|';
		m_ch[oss_type_360] = '&';
	}

	oss_obj::oss_obj(int world_id, int nType) 
		: m_world_id_(world_id)
		, m_nType(nType)
		, m_ss_(*get_stringstream())

	{
		m_ch[oss_type_orig] = '|';
		m_ch[oss_type_360] = '&';
	}
}

