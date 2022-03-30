#include "UT_RedisChannel.h"
#include <boost/function.hpp>

using namespace Utility;

static void sim_thread_log_func(const char* info)
{
	UT_CHANNEL_LOG(Utility::LOG_TAG_INFO, __FILE__ , __LINE__, LOG_SWITCH_INFO, info);
}
ResultType RedisSubscribe::Init(const std::string& host, const std::string& port, const std::string& pwd)
{
	return m_handler.AddPeer(host, port, pwd);
}

ResultType RedisSubscribe::Unit(void)
{
	return m_handler.Unit();
}

ResultType RedisSubscribe::Start(void)
{
	set_log_func(sim_thread_log_func);
	simple_thread::start(); 
	return RE_SUCCESS;
}

ResultType RedisSubscribe::End(void)
{	
	simple_thread::end();
	return RE_SUCCESS;
}

void RedisSubscribe::Update(float fTime)
{
	{
		redis_subscribe_channel_map::iterator it = m_channels.begin();
		while (it!=m_channels.end())
		{
			redis_subscribe_channel& rkchannel = it->second;
			if (rkchannel.last_time() + 5.0f <= fTime)
			{
				if(!rkchannel.is_valid())
				{
					if (rkchannel.is_subscribed())
					{
						DoCommand(get_redis_sub_reverse_command(rkchannel.m_command), rkchannel.m_pattern, NULL, NULL, fTime);
					}
					m_channels.erase(it++);
				}
				else
				{
					if (!rkchannel.is_subscribed())
					{
						DoCommand(rkchannel.m_command, rkchannel.m_pattern, NULL, NULL, fTime);
					}
					++it;
				}
			}
			else{++it;}
		}
	}

	for (int i=0; i<MAX_REDIS_SUBSCRIBE_HANDLE_SIZE; ++i)
	{
		redis_subscribe_msg* pmsg = NULL;
		if(!m_messages.pop(pmsg))
		{
			break;
		}
		if (pmsg)
		{
			redis_subscribe_channel_map::iterator it = m_channels.find(pmsg->key());
			if (it != m_channels.end())
			{
				redis_subscribe_channel& rkchannel = it->second;
				if (pmsg->scommand.compare("message") == 0 || pmsg->scommand.compare("pmessage") == 0)
				{
					redis_subscribe_channel::message_handler_set::iterator it = rkchannel.m_on_messages.begin();
					for (; it!= rkchannel.m_on_messages.end(); ++it)
					{
						(*it)(*pmsg);
					}
				}
				else if (pmsg->scommand.compare(get_redis_sub_command(redis_subscribe)) == 0 
					|| pmsg->scommand.compare(get_redis_sub_command(redis_psubscribe)) == 0)
				{
					rkchannel.m_subscribed = true;
					redis_subscribe_channel::subscribe_handler_set::iterator it = rkchannel.m_on_subscribes.begin();
					for (; it!= rkchannel.m_on_subscribes.end(); ++it)
					{
						(*it)(pmsg->scommand, pmsg->spattern, pmsg->smessage);
					}
				}
			}
			else
			{
				UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__, LOG_SWITCH_ERROR, "subscribe channel update unkown channel", pmsg->scommand, pmsg->schannel, pmsg->smessage);
			}
			delete pmsg;
		}
	}
}

ResultType RedisSubscribe::DoCommand(int32_t command, const std::string& pattern, redis_subscribe_func on_sub, redis_submessage_func on_msg, float ftime)
{
	if (command<0 || command>=redis_sub_max || pattern.empty())
	{
		return RE_FAIL;
	}
	redisconn_ptr conn_ = _fetchConnection();
	try
	{
		redis_subscribe_channel* pkChannel = NULL;
		switch(command)
		{
		case redis_subscribe:
			{
				pkChannel = _addChannel(command, pattern);
				if (NULL == pkChannel)
				{
					break;
				}
				pkChannel->reg_callback(on_sub, on_msg);
				if (conn_) conn_->subscribe(pattern);
			}
			break;
		case redis_psubscribe:
			{
				pkChannel = _addChannel(command, pattern);
				if (NULL == pkChannel)
				{
					break;
				}
				pkChannel->reg_callback(on_sub, on_msg);
				if (conn_) conn_->psubscribe(pattern);
			}
			break;
		case redis_unsubscribe:
			{
				pkChannel = _getChannel(command, pattern);
				if (NULL == pkChannel)
				{
					break;
				}
				pkChannel->unreg_callback(on_sub, on_msg);
				if (conn_) conn_->unsubscribe(pattern);
			}
			break;
		case redis_punsubscribe:
			{
				pkChannel = _getChannel(command, pattern);
				if (NULL == pkChannel)
				{
					break;
				}
				pkChannel->unreg_callback(on_sub, on_msg);
				if (conn_) conn_->punsubscribe(pattern);
			}
			break;
		default:
			break;
		}

		if (pkChannel)
		{
			pkChannel->last_time(ftime);
		}
	}catch(redispp::NullReplyException&){
		return RE_FAIL;
	}catch(std::exception& e){
		UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, __FUNCTION__, e.what());
		_resetConnection();
		return RE_FAIL;
	}catch(...){
		UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, __FUNCTION__, "some uncatched err happened!");
		_resetConnection();
		return RE_FAIL;
	}
	return RE_SUCCESS;
}

void RedisSubscribe::update(void)
{
	redisconn_ptr conn_ = _getConnection();
	if (conn_)
	{
		try
		{
			do
			{
				redispp::MultiBulkEnumerator kReply = conn_->checkSub();	
				std::string sinfo;	
				mem::vector<std::string> replys;
				while (kReply.next(&sinfo))
				{
					//UT_CHANNEL_LOG(Utility::LOG_TAG_INFO, __FILE__ , __LINE__, LOG_SWITCH_INFO, "subscribe test", sinfo);
					replys.push_back(sinfo);
				}

				std::size_t reply_sz = replys.size();
				if (reply_sz>=3)
				{
					const std::string& type			= replys[0];
					const std::string& channel		= replys[(reply_sz == 3)?1:2];
					const std::string& value		= replys[(reply_sz == 3)?2:3];
					const std::string& pattern		= (reply_sz == 4) ? replys[1] : channel;
					redis_subscribe_msg* pmsg		= new redis_subscribe_msg(type, pattern, channel, value);
					if(!m_messages.bounded_push(pmsg))
					{
						_ut_redis_safe_delete(pmsg);
					}
				}
				else
				{
					std::stringstream __ss_reply;
					mem::vector<std::string>::iterator itr = replys.begin();
					for (; itr!=replys.end(); ++itr)
					{
						__ss_reply << "|" << *itr;
					}

					UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__, LOG_SWITCH_ERROR, "subscribe channel bad size", replys.size(), __ss_reply.str());
				}
			}while(1);
		}
		catch (redispp::NullReplyException&)
		{
			UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__, LOG_SWITCH_ERROR, "subscribe channel null reply");
		}
		catch (std::exception& e)
		{
			UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__, LOG_SWITCH_ERROR, "subscribe channel", e.what());
			_resetConnection();
		}
		catch (...)
		{
			UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__, LOG_SWITCH_ERROR, "subscribe channel unkown exception");
			_resetConnection();
		}
	}
}
redis_subscribe_channel* RedisSubscribe::_addChannel(int32_t command, const std::string& pattern)
{
	std::string key = get_redis_subscribe_command_prefix(get_redis_sub_command(command)) + pattern;
	redis_subscribe_channel_map::iterator it = m_channels.find(key);
	redis_subscribe_channel* pkchannel = NULL;
	if (it == m_channels.end())
	{
		pkchannel = &m_channels[key];
		pkchannel->init(command, pattern);
	}
	else
	{
		pkchannel = &it->second;
	}
	return pkchannel;
}
redisconn_ptr RedisSubscribe::_fetchConnection(void)
{
	boost::unique_lock<boost::shared_mutex> _lock(m_mutex);
	redisconn_ptr conn_ = m_handler.GetConection(false);
	if (m_conn != conn_)
	{
		redis_subscribe_channel_map::iterator it = m_channels.begin();
		for (; it!=m_channels.end(); ++it)
		{
			redis_subscribe_channel& rkchannel = it->second;
			rkchannel.m_subscribed = false;
		}
		m_conn = conn_;
	}
	return m_conn;
}
ResultType Redis_Channel::Start(void)
{
	ClearChannel();//must first this
	set_log_func(sim_thread_log_func);
	simple_thread::start(); 
	m_kSubscribes.Start();
	return RE_SUCCESS;
}

ResultType Redis_Channel::End(void)
{	
	m_kSubscribes.End();
	simple_thread::end();
	return RE_SUCCESS;
}

void Redis_Channel::Update(float fTime)
{
	if (fTime < m_fLastUpdateTime + 0.1f)
	{
		return;
	}

	m_fLastUpdateTime = fTime;
	RedisChannelEntryMap::iterator it = m_kChannelMap.begin();
	for(; it!=m_kChannelMap.end(); ++it)
	{
		RedisChannelEntry& kEntry = it->second;
		redis_define::redis_channel_msg msg;
		while(kEntry.Deqeue(msg))
		{
			kEntry(msg);
		}
	}

	update_resp();

	m_kSubscribes.Update(fTime);
}

ResultType Redis_Channel::Publish(const std::string& channel, const std::string& msg)
{
	int ret = 0;
	ASYNC_REDIS_REQUEST(ret, *this, &redispp::Connection::publish, (void(*)(int32_t, int64_t&))NULL, channel, msg);
	if(0!=ret)
	{
		UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, "Redis_Channel Publish fail!may be the queue is full!!");
	}
	return ret;
}

ResultType Redis_Channel::Publish(const std::string& channel, const redis_define::redis_channel_msg& msg)
{
	int ret = 0;
	ASYNC_REDIS_REQUEST(ret, *this, &redispp::Connection::publish, (void(*)(int32_t, int64_t&))NULL, channel, msg.SerializeAsString());
	if(0!=ret)
	{
		UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, "Redis_Channel Publish fail!may be the queue is full!!");
	}
	return ret;
}

void Redis_Channel::update(void)
{
	
	Redis_Handler* pkRedisHandler = &m_kRedisHandler;
	{
		int32_t ret = 0;
		UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
		RedisChannelEntryMap::iterator it = m_kChannelMap.begin();
		for(; it!=m_kChannelMap.end(); ++it)
		{
			RedisChannelEntry& kEntry = it->second;
			redispp::StringReply kMsgReply = pkRedisConn->lpop(it->first);
			if(kMsgReply.result())
			{
				redis_define::redis_channel_msg kmsg;
				if (kmsg.ParseFromString(kMsgReply))
				{
					while(!kEntry.Enqeue(kmsg))
					{
						boost::this_thread::yield();
					}
				}
				else
				{
					UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, "Redis_Channel Bad redis_channel_msg PB!", std::string(kMsgReply));
				}
			}
		}
		//handle async redis req
		update_req(pkRedisConn);

		UT_REDIS_CMD_END(ret, pkRedisHandler);
		if (ret != 0)
		{
			UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, "Redis_Channel Update Failed!Err",  pkRedisHandler->GetLastError());
		}
	}
}

void Redis_Channel::ClearChannel(void)
{
	Redis_Handler* pkRedisHandler = &m_kRedisHandler;
	int32_t ret = 0;
	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);

	RedisChannelEntryMap::iterator it = m_kChannelMap.begin();
	for(; it!=m_kChannelMap.end(); ++it)
	{
		pkRedisConn->del(it->first);
	}
	UT_REDIS_CMD_END(ret, pkRedisHandler);
}

static void on_sub_test(const std::string& scommand, const std::string& spattern, const std::string& scount)
{
	UT_CHANNEL_LOG(Utility::LOG_TAG_INFO, __FILE__ , __LINE__ , LOG_SWITCH_INFO, __FUNCTION__, scommand, spattern, scount);
}

static void on_msg_test(const redis_subscribe_msg& smsg)
{
	UT_CHANNEL_LOG(Utility::LOG_TAG_INFO, __FILE__ , __LINE__ , LOG_SWITCH_INFO, __FUNCTION__, smsg.scommand, smsg.spattern, smsg.schannel, smsg.smessage);
}

void Utility::TestSubscribe(Redis_Channel& rkChannel, float fTime)
{
	rkChannel.SubCommand(redis_subscribe, ":0:10010000:subscribe_server_survive_check:3_20", on_sub_test, on_msg_test, fTime);
	rkChannel.SubCommand(redis_psubscribe, ":0:10010000:subscribe_server_survive_check:3_20", on_sub_test, on_msg_test, fTime);
}