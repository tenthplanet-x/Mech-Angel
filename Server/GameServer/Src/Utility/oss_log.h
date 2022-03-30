#ifndef OSS_LOG_H_DFGFD45353
#define OSS_LOG_H_DFGFD45353

#include "StringDefine.h"
class LC_ServerPlayer;
namespace oss
{
	void oss_mukun_log(const char* tag, const char* info);
	void oss_360_log(const char* tag, const char* info);
	void oss_internal_log(const char* tag, const char* info);

	StringStreamType* get_stringstream();

	bool  check_oss(const StringType& type);
	void  enable_oss(const StringType& type);
	void  disable_oss(const StringType& type);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef enum OSS_TYPE
	{
		oss_type_orig,
		oss_type_360,
		oss_type_count,
	};

	class oss_obj
	{
	public:
		oss_obj(const char* name , int wolrd_id = -1, int nType = oss_type_360);
		oss_obj(int world_id=-1, int nType = oss_type_360);

		template<typename ty>
		inline oss_obj& operator , (const ty& value)
		{
			m_ss_ << m_ch[m_nType] << value;
			return * this;
		}

		template<typename ty>
		inline oss_obj& operator << (const ty& value)
		{
			if (m_ss_.str().length() == 0 && m_nType == oss_type_360 )
				m_ss_ << value;
			else
				m_ss_ << m_ch[m_nType] << value;
				
			return * this;
		}

		inline StringType info() { return m_ss_.str(); }
		inline void do_log() { oss_mukun_log(NULL, m_ss_.str().c_str()); }
		inline void do_internal_log() { oss_internal_log(NULL, m_ss_.str().c_str()); }
		inline void do_360_log() { oss_360_log(NULL, m_ss_.str().c_str()); }

	private:
		//const char* m_name_;
		int			m_world_id_;
		StringStreamType& m_ss_;
		int m_nType;
		char m_ch[oss_type_count];
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#define OSS_LOG_INFO(key,worldid,msg)\
{\
	oss::oss_obj obj(key, worldid, oss::oss_type_orig);\
	obj<<msg;\
	obj.do_log();\
}

#define OSS_LOG_INFO_RET(key,worldid,msg)\
{\
	oss::oss_obj obj(key, worldid, oss::oss_type_orig);\
	obj<<msg;\
	obj.do_log();\
	return obj.info();\
}

#define OSS_INTERNAL_LOG_INFO(worldid,msg)\
{\
	oss::oss_obj obj(worldid, oss::oss_type_orig);\
	obj<<msg;\
	obj.do_internal_log();\
}

#define OSS_360_LOG_INFO(header, msg)\
{\
	oss::oss_obj obj;\
	obj<<header;\
	obj<<msg;\
	obj.do_360_log();\
}


#endif
