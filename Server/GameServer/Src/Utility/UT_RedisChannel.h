#ifndef __UT_REDIS_CHANNEL_H__
#define __UT_REDIS_CHANNEL_H__

#pragma once

#include "redis.pb.h"
#include "UT_Redis.h"
#include "UT_RedisRequest.h"

#include <string>
#include <queue>
#include <map>

#include "simple_thread.h"

#include <boost/atomic.hpp>

#include "SystemError.h"
#include "ajson.hpp"


namespace Utility
{
#define MAX_REDIS_SUBSCRIBE_QUEUE_SIZE 0xFFFF
#define MAX_REDIS_SUBSCRIBE_HANDLE_SIZE 100

	typedef void (*RedisChannelHandler)(const redis_define::redis_channel_msg& kmsg);

	struct channel_msg
	{
		channel_msg(): type(0), id(0) {}

		int32_t type;
		int64_t id;
	};

	struct gm_cmd_resp
	{
		gm_cmd_resp(): type(0), code(0) {}

		int			 type;	
		channel_msg	 channel;
		std::string  cmd;
		int64_t		 code;
		std::string  msg;
		std::string  data;
	};

	struct gm_cmd_infosort
	{
		gm_cmd_infosort(): serverid(0), key("") {}

		int			 serverid;	
		std::string  key;
	};

	template <typename IT>
	struct json_op
	{
		json_op(void){}
		json_op(const IT& it):json(it){}
		const std::string operator()(void) const
		{
			std::stringstream ss;
			boost::ajson::save_to_buff(json, ss);
			return ss.str();
		}
		IT json;
	};

	struct RedisChannelEntry
	{
		std::string channel;
		RedisChannelHandler handler;

		mem::queue<redis_define::redis_channel_msg> _recv_msgs;

		RedisChannelEntry(): handler(NULL), _rcev_mutex(0) {}

		RedisChannelEntry(const RedisChannelEntry& o): channel(o.channel), handler(o.handler), _rcev_mutex(0) {}

		inline bool Enqeue(const redis_define::redis_channel_msg& msg)
		{
			int32_t i=0;
			if(false == _rcev_mutex.compare_exchange_weak(i, 1))
			{
				return false;
			}
			_recv_msgs.push(msg);
			_rcev_mutex = 0;
			return true;
		}

		inline bool Deqeue(redis_define::redis_channel_msg& msg)
		{
			int32_t i=0;
			if (false == _rcev_mutex.compare_exchange_weak(i, 1))
			{
				return false;
			}

			if (_recv_msgs.size())
			{
				msg = _recv_msgs.front();
				_recv_msgs.pop();
				_rcev_mutex = 0;
				return true;
			}
			_rcev_mutex = 0;
			return false;
		}
		
		inline void operator ()(redis_define::redis_channel_msg& kmsg)
		{
			if (handler)
			{
				handler(kmsg);
			}
		}
	protected:
		boost::atomic_int32_t _rcev_mutex;
	};
	typedef mem::map<std::string, RedisChannelEntry> RedisChannelEntryMap;

	inline std::string get_redis_subscribe_command_prefix(const std::string& command)
	{
		return command.empty() ? ("_") : ((command[0]=='p' || command[0]=='P') ? "P_" : "_");
	}

	struct redis_subscribe_msg;
	typedef void (*redis_subscribe_func)(const std::string& scommand, const std::string& spattern, const std::string& scount);
	typedef void (*redis_submessage_func)(const redis_subscribe_msg& smsg);

	enum {
		redis_subscribe = 0,
		redis_unsubscribe,
		redis_psubscribe,
		redis_punsubscribe,
		redis_sub_max,
	};
	static const std::string stc_redis_sub_commands[redis_sub_max] = {"subscribe", "unsubscribe", "psubscribe", "punsubscribe"};

	inline const std::string& get_redis_sub_command(int32_t icmd)
	{
		static std::string st_default;
		return icmd>=0&&icmd<redis_sub_max ? stc_redis_sub_commands[icmd] : st_default;
	}
	inline int32_t get_redis_sub_reverse_command(int32_t icmd)
	{
		static int32_t _st_redis_reverse_cmds[] = {redis_unsubscribe, redis_subscribe, redis_punsubscribe, redis_psubscribe};
		return icmd>=0&&icmd<redis_sub_max ? _st_redis_reverse_cmds[icmd] : -1;
	}

	struct redis_subscribe_msg
	{
		redis_subscribe_msg(const std::string& command, const std::string& pattern, const std::string& channel, const std::string& msg)
			:scommand(command), spattern(pattern), schannel(channel), smessage(msg)
		{
			sprefix = get_redis_subscribe_command_prefix(command);
		}

		inline std::string key(void) const {return sprefix + spattern;}

		std::string sprefix;
		std::string scommand;
		std::string spattern;
		std::string schannel;
		std::string smessage;
	};
	typedef boost::lockfree::queue<redis_subscribe_msg*> redis_subscribe_msg_queue;

	struct redis_subscribe_channel
	{;
		typedef mem::set<redis_subscribe_func>	subscribe_handler_set;
		typedef mem::set<redis_submessage_func>	message_handler_set;
		
		redis_subscribe_channel(void):m_subscribed(false), m_command(0), m_last_time(0){}
		
		inline void init(int32_t icommand, const std::string& spattern)
		{
			if (icommand<0 || icommand>=redis_sub_max)
			{
				return;
			}
			m_prefix = get_redis_subscribe_command_prefix(get_redis_sub_command(icommand));
			m_command = icommand;
			m_pattern = spattern;
		}
		inline std::string key(void) const {return m_prefix + m_pattern;}
		inline float last_time(void) const {return m_last_time;}
		inline void last_time(float ftime) {m_last_time = ftime;}

		inline bool is_subscribed(void) const {return m_subscribed;}
		inline bool is_valid(void) const {return !m_on_subscribes.empty() || !m_on_messages.empty();}

		void reg_callback(redis_subscribe_func sub, redis_submessage_func msg) 
		{
			if (sub)
			{
				m_on_subscribes.insert(sub);
			}
			if (msg)
			{
				m_on_messages.insert(msg);
			}
		}
		inline void unreg_callback(redis_subscribe_func sub, redis_submessage_func msg) 
		{
			m_on_subscribes.erase(sub);
			m_on_messages.erase(msg);
		}

		bool							m_subscribed;
		int32_t							m_command;
		std::string						m_prefix;
		std::string						m_pattern;
		subscribe_handler_set			m_on_subscribes;
		message_handler_set				m_on_messages;
		float							m_last_time;
	};
	typedef mem::map<std::string, redis_subscribe_channel> redis_subscribe_channel_map;

	struct RedisSubscribe : public simple_thread
	{
		RedisSubscribe(void)
			: m_messages(MAX_REDIS_SUBSCRIBE_QUEUE_SIZE)
		{}
		ResultType	Init(const std::string& host, const std::string& port, const std::string& pwd);
		ResultType	Unit(void);

		ResultType	Start(void);
		ResultType	End(void);

		void		Update(float fTime);

		ResultType	DoCommand(int32_t command, const std::string& pattern, redis_subscribe_func on_sub, redis_submessage_func on_msg, float ftime);

		virtual std::string tname(void) const {return std::string("redis subscribe");}
	
	protected:
		virtual void  update(void);

		inline redis_subscribe_channel* _getChannel(int32_t command, const std::string& pattern)
		{
			std::string key = get_redis_subscribe_command_prefix(get_redis_sub_command(command)) + pattern;
			redis_subscribe_channel_map::iterator it = m_channels.find(key);
			return (it!=m_channels.end()) ? &it->second : NULL;
		}
		redis_subscribe_channel* _addChannel(int32_t command, const std::string& pattern);

		redisconn_ptr _fetchConnection(void);

		void _resetConnection(void)
		{
			boost::unique_lock<boost::shared_mutex> _lock(m_mutex);
			m_handler.Disconnect();
			m_conn.reset();
		}

		inline redisconn_ptr _getConnection(void)
		{
			boost::shared_lock<boost::shared_mutex> _lock(m_mutex);
			return m_conn;
		}

		Redis_Handler				m_handler;
		boost::shared_mutex			m_mutex;
		redisconn_ptr				m_conn;
		redis_subscribe_channel_map	m_channels;
		redis_subscribe_msg_queue	m_messages;
	};

	class Redis_Channel : public simple_thread
		, public reris_request_container
	{
	public:
		Redis_Channel(void)
			:m_fLastUpdateTime(0)
		{}

		virtual std::string tname(void) const {return std::string("redis channel");}

		ResultType AddRedis(const std::string& host, const std::string& port, const std::string& pwd)
		{
			do 
			{
				if(RE_SUCCESS != m_kRedisHandler.AddPeer(host, port, pwd))
				{
					return RE_FAIL;
				}
				
				if (RE_SUCCESS != m_kSubscribes.Init(host, port, pwd))
				{
					return RE_FAIL;
				}
			} while (0);

			return RE_SUCCESS;
		}
		ResultType InitRedis(void)
		{
			do 
			{
				if (m_kChannelMap.empty())
				{
					break;
				}

				if(RE_SUCCESS != m_kRedisHandler.Init())
				{
					m_sError = m_kRedisHandler.GetLastError();
					return RE_FAIL;
				}
			} while (0);

			return RE_SUCCESS;
		}
		ResultType UnitRedis(void)
		{
			do 
			{
				if(RE_SUCCESS != m_kRedisHandler.Unit())
				{
					return RE_FAIL;
				}

				if (RE_SUCCESS != m_kSubscribes.Unit())
				{
					return RE_FAIL;
				}
			} while (0);

			return RE_SUCCESS;
		}

		ResultType Start(void);
		ResultType End(void);

		inline ResultType FakeSubscribe(const std::string& channel, const RedisChannelHandler& _handler)
		{
			m_kChannelMap[channel].channel = channel;
			m_kChannelMap[channel].handler = _handler;
			return RE_SUCCESS;
		}

		ResultType Publish(const std::string& channel, const std::string& msg);
		ResultType Publish(const std::string& channel, const redis_define::redis_channel_msg& msg);

		template <typename REDIS_DEF>
		ResultType Publish(const std::string& channel, int32_t msg_type, int32_t msg_id, const REDIS_DEF& msg)
		{
			redis_define::redis_channel_msg promsg;
			
			promsg.set_m_type(msg_type);
			promsg.set_m_id(msg_id);
			promsg.set_content(msg.SerializeAsString());

			return Publish(channel, promsg);
		}

		inline ResultType SubCommand(int32_t command, const std::string& pattern, redis_subscribe_func on_sub, redis_submessage_func on_msg, float ftime)
		{
			return m_kSubscribes.DoCommand(command, pattern, on_sub, on_msg, ftime);
		}

		template <typename JSON_DEF>
		ResultType Publish(const std::string& channel, const JSON_DEF& msg)
		{
			json_op<JSON_DEF> msg_op(msg);
			int ret = 0;
			ASYNC_REDIS_REQUEST(ret, *this, &redispp::Connection::publish, (void(*)(int32_t, int64_t&))NULL, channel, msg_op);
			return ret;
		}

		void			Update(float fTime);
		inline const std::string GetLastError(void) {return m_sError;}
	protected:
		virtual void	update(void);
		void			ClearChannel(void);

		Redis_Handler			m_kRedisHandler;
		RedisSubscribe			m_kSubscribes;
		RedisChannelEntryMap	m_kChannelMap;
		
		float					m_fLastUpdateTime;
		std::string				m_sError;
	};

	void TestSubscribe(Redis_Channel& rkChannel, float fTime);
}

AJSON(Utility::channel_msg, (type)(id))
AJSON(Utility::gm_cmd_resp, (type)(channel)(cmd)(code)(msg)(data))
AJSON(Utility::gm_cmd_infosort, (serverid)(key))
#endif
