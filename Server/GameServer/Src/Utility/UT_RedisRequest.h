#ifndef __UT_REDIS_ASYNC_H__
#define __UT_REDIS_ASYNC_H__

#pragma once

#ifdef WIN32
#pragma warning( disable : 4819)
#endif

#include "redispp.h"

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits.hpp>
#include <boost/lockfree/queue.hpp>

#if (defined T6_SERVER ) || (defined T6_CLINET)
#include "UT_Log.h"
#define UT_CHANNEL_LOG(tag_, file_, line_, switch_, ...) Utility::GfxWriteFmtLog(tag_, file_, line_, switch_, __VA_ARGS__)
#else
#define UT_CHANNEL_LOG(tag_, file_, line_, switch_, ...)
#endif


namespace Utility{

#define _ut_redis_safe_delete(p) if(p){delete p; p=0;}

	struct redis_reply_base
	{
		typedef int32_t error_type;
		
		redis_reply_base(void):_err(-1){}
		redis_reply_base(error_type err):_err(err){}

		inline error_type rerror(void){return _err;}
		inline void rerror(error_type err){_err = err;}
	private:
		error_type _err;  
	};

	template <typename ReplyType>
	struct redis_reply : public redis_reply_base
	{
		typedef void result_type;
		typedef ReplyType reply_type;
		typedef redis_reply<reply_type> self_type;
		typedef boost::function<void (error_type)> callback_type;

		redis_reply(void):redis_reply_base(-1){}

		template <typename command_type>
		inline void operator ()(command_type& cmd, redispp::Connection* conn)
		{
			cmd(conn);
			rerror(0);
		}

		inline void operator ()(callback_type cb)
		{
			cb(rerror());
		}
	};

	template <>
	struct redis_reply<redispp::VoidReply> : public redis_reply_base
	{
		typedef std::string result_type;
		typedef redispp::VoidReply reply_type;
		typedef redis_reply<reply_type> self_type;
		typedef boost::function<void (error_type, result_type&)> callback_type;

		redis_reply(void):redis_reply_base(-1){}

		template <typename command_type>
		inline void operator ()(command_type& cmd, redispp::Connection* conn)
		{
			_rslt = cmd(conn).result();
			rerror(0);
		}
		
		operator result_type(void){return _rslt;}
		inline void operator ()(callback_type cb)
		{
			cb(rerror(), _rslt);
		}

		result_type _rslt;
	};

	template <>
	struct redis_reply<redispp::BoolReply> : public redis_reply_base
	{
		typedef bool result_type;
		typedef redispp::BoolReply reply_type;
		typedef redis_reply<reply_type> self_type;
		typedef boost::function<void (error_type, result_type&)> callback_type;

		redis_reply(void):redis_reply_base(-1), _rslt(false) {}

		template <typename command_type>
		inline void operator ()(command_type& cmd, redispp::Connection* conn)
		{
			_rslt = cmd(conn).result();
			rerror(0);
		}

		operator result_type(void){return _rslt;}
		inline void operator ()(callback_type cb)
		{
			cb(rerror(), _rslt);
		}

		result_type _rslt;
	};

	template <>
	struct redis_reply<redispp::IntReply> : public redis_reply_base
	{
		typedef int64_t result_type;
		typedef redispp::IntReply reply_type;
		typedef redis_reply<reply_type> self_type;
		typedef boost::function<void (error_type, result_type&)> callback_type;

		redis_reply(void):redis_reply_base(-1), _rslt(0){}

		template <typename command_type>
		inline void operator ()(command_type& cmd, redispp::Connection* conn)
		{
			_rslt = cmd(conn).result();
			rerror(0);
		}

		operator result_type(void){return _rslt;}
		inline void operator ()(callback_type cb)
		{
			cb(rerror(), _rslt);
		}

		result_type _rslt;
	};

	template <>
	struct redis_reply<redispp::StringReply> : public redis_reply_base
	{
		typedef std::string result_type;
		typedef redispp::StringReply reply_type;
		typedef redis_reply<reply_type> self_type;
		typedef boost::function<void (error_type, result_type&)> callback_type;

		redis_reply(void):redis_reply_base(-1), _rslt(""){}

		template <typename command_type>
		inline void operator ()(command_type& cmd, redispp::Connection* conn)
		{
			redispp::StringReply _r = cmd(conn);
			if (_r.result())
			{
				_rslt = _r;
			}
			rerror(0);
		}

		operator result_type(void){return _rslt;}
		inline void operator ()(callback_type cb)
		{
			cb(rerror(), _rslt);
		}

		result_type _rslt;
	};

	template <>
	struct redis_reply<redispp::MultiBulkEnumerator> : public redis_reply_base
	{
		typedef std::vector<std::string> result_type;
		typedef redispp::MultiBulkEnumerator reply_type;
		typedef redis_reply<reply_type> self_type;
		typedef boost::function<void (error_type, result_type&)> callback_type;

		redis_reply(void):redis_reply_base(-1){}

		template <typename command_type>
		inline void operator ()(command_type& cmd, redispp::Connection* conn)
		{
			redispp::MultiBulkEnumerator _r = cmd(conn);
			std::string _elm;
			while(_r.next(&_elm))
			{
				_rslt.push_back(_elm);
			}
			rerror(0);
		}

		operator result_type(void){return _rslt;}
		inline void operator ()(callback_type cb)
		{
			cb(rerror(), _rslt);
		}

		result_type _rslt;
	};

	template <typename T>
	struct redis_transfer_base
	{
		virtual ~redis_transfer_base(){}

		virtual T operator ()(void) = 0;
	};

	template <typename IT, typename OT, typename Enable=void>
	struct redis_transfer : public redis_transfer_base<OT>
	{
		typedef typename boost::remove_reference<IT>::type raw_in_type;
		typedef typename boost::remove_cv<typename boost::remove_reference<OT>::type>::type raw_cache_type;

		redis_transfer(const IT& it):_in(it){}

		virtual OT operator ()(void){_cache = _in();return _cache;}
		raw_in_type _in;
		raw_cache_type _cache;
	};

	template <typename IT, typename OT>
	struct redis_transfer<IT, OT, typename boost::enable_if<boost::is_convertible<IT, OT> >::type>: public redis_transfer_base<OT>
	{
		typedef typename boost::remove_reference<OT>::type raw_in_type;
		typedef typename boost::remove_cv<typename boost::remove_reference<OT>::type>::type raw_cache_type;

		redis_transfer(const IT& it):_in(it){}

		virtual OT operator ()(void){return _in;}
		raw_in_type _in;
	};

	template <typename RT>
	struct redis_command0
	{
		typedef RT (redispp::Connection::*mem_func_type)(void);
		typedef redis_reply<RT> reply_type;
		typedef typename reply_type::result_type result_type;

		redis_command0(const std::string& fname, mem_func_type func):_fname(fname), _func(func){}
		~redis_command0(void){}

		RT operator ()(redispp::Connection* conn) {return (conn->*_func)();}

		std::string desc(void) {return _fname;}

		std::string		_fname;
		mem_func_type	_func;
	};


	template <typename RT, typename P0>
	struct redis_command1
	{
		typedef P0 p0_type;
		typedef RT (redispp::Connection::*mem_func_type)(p0_type);
		typedef redis_reply<RT> reply_type;
		typedef typename reply_type::result_type result_type;

		typedef redis_transfer_base<p0_type> p0_transfer_type;

		redis_command1(const std::string& fname, mem_func_type func):_fname(fname), _func(func), _p0_transfer(NULL){}
		redis_command1(const std::string& fname, mem_func_type func, p0_type p0)
			:_fname(fname), _func(func), _p0_transfer(new redis_transfer<p0_type, p0_type>(p0))
		{}
		~redis_command1(void)
		{
			_ut_redis_safe_delete(_p0_transfer);
		}
		
		RT operator ()(redispp::Connection* conn) {return (conn->*_func)((*_p0_transfer)());}

		std::string desc(void) 
		{
			std::stringstream _ss;
			_ss << _fname << "|" << (*_p0_transfer)();
			return _ss.str() ;
		}

		template <typename IP0>
		void bind_p0(const IP0& ip0)
		{
			_ut_redis_safe_delete(_p0_transfer);
			_p0_transfer = new redis_transfer<IP0, p0_type>(ip0);
		}

		std::string		_fname;
		mem_func_type	_func;
		p0_transfer_type* _p0_transfer;
	};

	template <typename RT, typename P0, typename P1>
	struct redis_command2
	{
		typedef P0 p0_type;
		typedef P1 p1_type;

		typedef RT (redispp::Connection::*mem_func_type)(P0, P1);
		typedef redis_reply<RT> reply_type;
		typedef typename reply_type::result_type result_type;

		typedef redis_transfer_base<p0_type> p0_transfer_type;
		typedef redis_transfer_base<p1_type> p1_transfer_type;

		redis_command2(const std::string& fname, mem_func_type func):_fname(fname), _func(func), _p0_transfer(NULL), _p1_transfer(NULL){}
		redis_command2(const std::string& fname, mem_func_type func, p0_type p0, p1_type p1)
			:_fname(fname), _func(func), _p0_transfer(new redis_transfer<p0_type, p0_type>(p0)), _p1_transfer(new redis_transfer<p1_type, p1_type>(p1))
		{}
		~redis_command2(void)
		{
			_ut_redis_safe_delete(_p0_transfer);
			_ut_redis_safe_delete(_p1_transfer);
		}

		RT operator ()(redispp::Connection* conn) {return (conn->*_func)((*_p0_transfer)(), (*_p1_transfer)());}

		std::string desc(void) 
		{
			std::stringstream _ss;
			_ss << _fname << "|" << (*_p0_transfer)() << (*_p1_transfer)();
			return _ss.str() ;
		}

		template <typename IP0>
		void bind_p0(const IP0& ip0)
		{
			_ut_redis_safe_delete(_p0_transfer);
			_p0_transfer = new redis_transfer<IP0, p0_type>(ip0);
		}

		template <typename IP1>
		void bind_p1(const IP1& ip1)
		{
			_ut_redis_safe_delete(_p1_transfer);
			_p1_transfer = new redis_transfer<IP1, p1_type>(ip1);
		}

		std::string		_fname;
		mem_func_type	_func;
		p0_transfer_type* _p0_transfer;
		p1_transfer_type* _p1_transfer;
	};

	template <typename RT, typename P0, typename P1, typename P2>
	struct redis_command3
	{
		typedef P0 p0_type;
		typedef P1 p1_type;
		typedef P2 p2_type;

		typedef RT (redispp::Connection::*mem_func_type)(P0, P1, P2);
		typedef redis_reply<RT> reply_type;
		typedef typename reply_type::result_type result_type;

		typedef redis_transfer_base<p0_type> p0_transfer_type;
		typedef redis_transfer_base<p1_type> p1_transfer_type;
		typedef redis_transfer_base<p2_type> p2_transfer_type;

		redis_command3(const std::string& fname, mem_func_type func):_fname(fname), _func(func), _p0_transfer(NULL), _p1_transfer(NULL), _p2_transfer(NULL){}
		redis_command3(const std::string& fname, mem_func_type func, P0 p0, P1 p1, P2 p2)
			:_fname(fname), _func(func), _p0_transfer(new redis_transfer<p0_type, p0_type>(p0)), _p1_transfer(new redis_transfer<p1_type, p1_type>(p1)), _p2_transfer(new redis_transfer<p2_type, p2_type>(p2))
		{}
		~redis_command3(void)
		{
			_ut_redis_safe_delete(_p0_transfer);
			_ut_redis_safe_delete(_p1_transfer);
			_ut_redis_safe_delete(_p2_transfer);
		}

		RT operator ()(redispp::Connection* conn) {return (conn->*_func)((*_p0_transfer)(), (*_p1_transfer)(), (*_p2_transfer)());}

		std::string desc(void) 
		{
			std::stringstream _ss;
			_ss << _fname << "|" << (*_p0_transfer)() << (*_p1_transfer)() << (*_p2_transfer)();
			return _ss.str() ;
		}

		template <typename IP0>
		void bind_p0(const IP0& ip0)
		{
			_ut_redis_safe_delete(_p0_transfer);
			_p0_transfer = new redis_transfer<IP0, p0_type>(ip0);
		}

		template <typename IP1>
		void bind_p1(const IP1& ip1)
		{
			_ut_redis_safe_delete(_p1_transfer);
			_p1_transfer = new redis_transfer<IP1, p1_type>(ip1);
		}

		template <typename IP2>
		void bind_p2(const IP2& ip2)
		{
			_ut_redis_safe_delete(_p2_transfer);
			_p2_transfer = new redis_transfer<IP2, p2_type>(ip2);
		}

		std::string		_fname;
		mem_func_type	_func;
		p0_transfer_type* _p0_transfer;
		p1_transfer_type* _p1_transfer;
		p2_transfer_type* _p2_transfer;
	};

	template <typename RT, typename P0, typename P1, typename P2, typename P3>
	struct redis_command4
	{
		typedef P0 p0_type;
		typedef P1 p1_type;
		typedef P2 p2_type;
		typedef P3 p3_type;

		typedef RT (redispp::Connection::*mem_func_type)(P0, P1, P2, P3);
		typedef redis_reply<RT> reply_type;
		typedef typename reply_type::result_type result_type;

		typedef redis_transfer_base<p0_type> p0_transfer_type;
		typedef redis_transfer_base<p1_type> p1_transfer_type;
		typedef redis_transfer_base<p2_type> p2_transfer_type;
		typedef redis_transfer_base<p3_type> p3_transfer_type;

		redis_command4(const std::string& fname, mem_func_type func):_fname(fname), _func(func), _p0_transfer(NULL), _p1_transfer(NULL), _p2_transfer(NULL), _p3_transfer(NULL){}
		redis_command4(const std::string& fname, mem_func_type func, P0 p0, P1 p1, P2 p2, P3 p3)
			:_fname(fname), _func(func), _p0_transfer(new redis_transfer<p0_type, p0_type>(p0)), _p1_transfer(new redis_transfer<p1_type, p1_type>(p1)), _p2_transfer(new redis_transfer<p2_type, p2_type>(p2)), _p3_transfer(new redis_transfer<p3_type, p3_type>(p3))
		{}
		~redis_command4(void)
		{
			_ut_redis_safe_delete(_p0_transfer);
			_ut_redis_safe_delete(_p1_transfer);
			_ut_redis_safe_delete(_p2_transfer);
			_ut_redis_safe_delete(_p3_transfer);
		}

		RT operator ()(redispp::Connection* conn) {return (conn->*_func)((*_p0_transfer)(), (*_p1_transfer)(), (*_p2_transfer)(), (*_p3_transfer)());}

		std::string desc(void) 
		{
			std::stringstream _ss;
			_ss << _fname << "|" << (*_p0_transfer)() << (*_p1_transfer)() << (*_p2_transfer)() << (*_p3_transfer)();
			return _ss.str() ;
		}

		template <typename IP0>
		void bind_p0(const IP0& ip0)
		{
			_ut_redis_safe_delete(_p0_transfer);
			_p0_transfer = new redis_transfer<IP0, p0_type>(ip0);
		}

		template <typename IP1>
		void bind_p1(const IP1& ip1)
		{
			_ut_redis_safe_delete(_p1_transfer);
			_p1_transfer = new redis_transfer<IP1, p1_type>(ip1);
		}

		template <typename IP2>
		void bind_p2(const IP2& ip2)
		{
			_ut_redis_safe_delete(_p2_transfer);
			_p2_transfer = new redis_transfer<IP2, p2_type>(ip2);
		}

		template <typename IP3>
		void bind_p3(const IP3& ip3)
		{
			_ut_redis_safe_delete(_p3_transfer);
			_p3_transfer = new redis_transfer<IP3, p3_type>(ip3);
		}

		std::string		_fname;
		mem_func_type	_func;
		p0_transfer_type* _p0_transfer;
		p1_transfer_type* _p1_transfer;
		p2_transfer_type* _p2_transfer;
		p3_transfer_type* _p3_transfer;
	};

	struct redis_request_base
	{
		virtual ~redis_request_base(void){}
		virtual void operator ()(redispp::Connection* conn) = 0;
		virtual void operator ()(void) = 0;
		virtual std::string desc(void) {return "a null request";}
	};

	template <typename CommandType>
	struct redis_request : public redis_request_base
	{
		typedef CommandType commond_type;
		typedef typename CommandType::reply_type reply_type;
		typedef typename CommandType::mem_func_type mem_func_type;
		typedef void result_type;
		typedef typename reply_type::callback_type callback_type;

		redis_request(const std::string& fname, mem_func_type func, callback_type cb)
			:_cmd(fname, func)
			,_cb(cb)
		{}

		virtual void operator ()(redispp::Connection* conn)
		{
			_reply(_cmd, conn);
		}

		virtual void operator ()(void){if(_cb)_reply(_cb);}
		virtual std::string desc(void){return _cmd.desc();}

		commond_type& command(void){return _cmd;}

		commond_type _cmd;
		callback_type _cb;
		reply_type _reply;
	};

	template <typename commond_type, typename callback_type>
	redis_request_base* new_redis_request(const std::string& fname, commond_type& cmd, callback_type cb)
	{
		return new redis_request<commond_type>(fname, cmd, cb);
	}

	template <typename RT, typename callback_type>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(void), callback_type cb)
	{
		return new redis_request<redis_command0<RT> >(fname, func, cb);
	}

	template <typename RT>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(void))
	{
		return new redis_request<redis_command0<RT> >(fname, func, (typename redis_request<redis_command0<RT> >::callback_type)NULL);
	}

	template <typename RT, typename P0, typename IP0, typename callback_type>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0), callback_type cb, const IP0& ip0)
	{
		redis_request<redis_command1<RT, P0> >* req = new redis_request<redis_command1<RT, P0> >(fname, func, cb);
		req->command().bind_p0(ip0);
		return req;
	}

	template <typename RT, typename P0, typename IP0>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0), const IP0& ip0)
	{
		redis_request<redis_command1<RT, P0> >* req = new redis_request<redis_command1<RT, P0> >(fname, func, (typename redis_request<redis_command1<RT, P0> >::callback_type)NULL);
		req->command().bind_p0(ip0);
		return req;
	}

	template <typename RT, typename P0, typename IP0, typename P1, typename IP1, typename callback_type>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0, P1), callback_type cb, const IP0& ip0, const IP1& ip1)
	{
		redis_request<redis_command2<RT, P0, P1> >* req =  new redis_request<redis_command2<RT, P0, P1> >(fname, func, cb);
		req->command().bind_p0(ip0);
		req->command().bind_p1(ip1);
		return req;
	}

	template <typename RT, typename P0, typename IP0, typename P1, typename IP1>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0, P1), const IP0& ip0, const IP1& ip1)
	{
		redis_request<redis_command2<RT, P0, P1> >* req =  new redis_request<redis_command2<RT, P0, P1> >(fname, func, (typename redis_request<redis_command2<RT, P0, P1> >::callback_type)NULL);
		req->command().bind_p0(ip0);
		req->command().bind_p1(ip1);
		return req;
	}

	template <typename RT, typename P0, typename IP0, typename P1, typename IP1, typename P2, typename IP2, typename callback_type>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0, P1, P2), callback_type cb, const IP0& ip0, const IP1& ip1, const IP2& ip2)
	{
		redis_request<redis_command3<RT, P0, P1, P2> >* req =  new redis_request<redis_command3<RT, P0, P1, P2> >(fname, func, cb);
		req->command().bind_p0(ip0);
		req->command().bind_p1(ip1);
		req->command().bind_p2(ip2);
		return req;
	}

	template <typename RT, typename P0, typename IP0, typename P1, typename IP1, typename P2, typename IP2>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0, P1, P2), const IP0& ip0, const IP1& ip1, const IP2& ip2)
	{
		redis_request<redis_command3<RT, P0, P1, P2> >* req =  new redis_request<redis_command3<RT, P0, P1, P2> >(fname, func, (typename redis_request<redis_command3<RT, P0, P1, P2> >::callback_type)NULL);
		req->command().bind_p0(ip0);
		req->command().bind_p1(ip1);
		req->command().bind_p2(ip2);
		return req;
	}

	template <typename RT, typename P0, typename IP0, typename P1, typename IP1, typename P2, typename IP2, typename P3, typename IP3, typename callback_type>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0, P1, P2, P3), callback_type cb, const IP0& ip0, const IP1& ip1, const IP2& ip2, const IP3& ip3)
	{
		redis_request<redis_command4<RT, P0, P1, P2, P3> >* req =  new redis_request<redis_command4<RT, P0, P1, P2, P3> >(fname, func, cb);
		req->command().bind_p0(ip0);
		req->command().bind_p1(ip1);
		req->command().bind_p2(ip2);
		req->command().bind_p3(ip3);
		return req;
	}

	template <typename RT, typename P0, typename IP0, typename P1, typename IP1, typename P2, typename IP2, typename P3, typename IP3>
	redis_request_base* new_redis_request(const std::string& fname, RT(redispp::Connection::*func)(P0, P1, P2, P3), const IP0& ip0, const IP1& ip1, const IP2& ip2, const IP3& ip3)
	{
		redis_request<redis_command4<RT, P0, P1, P2, P3> >* req =  new redis_request<redis_command4<RT, P0, P1, P2, P3> >(fname, func, (typename redis_request<redis_command4<RT, P0, P1, P2, P3> >::callback_type)NULL);
		req->command().bind_p0(ip0);
		req->command().bind_p1(ip1);
		req->command().bind_p2(ip2);
		req->command().bind_p3(ip3);
		return req;
	}

	inline void delete_redis_request(redis_request_base* req){_ut_redis_safe_delete(req);}

	typedef boost::lockfree::queue<redis_request_base*> redis_request_queue;

#define MAX_REDIS_REQ_QUEUE_SIZE 0xFFFF
#define MAX_REDIS_RESP_QUEUE_SIZE 0xFFFF
#define MAX_REDIS_REQ_HANDLE_SIZE 100
#define MAX_REDIS_RESP_HANDLE_SIZE 100

	struct scoped_redis_req_trans
	{
		scoped_redis_req_trans(redis_request_queue& tar, redis_request_base* r):target(tar), req(r){}
		~scoped_redis_req_trans()
		{
			if(!target.bounded_push(req))
			{
				_ut_redis_safe_delete(req);
			}
		}
		redis_request_queue& target;
		redis_request_base* req;
	};

	struct reris_request_container
	{
		reris_request_container(void)
			:m_requests(MAX_REDIS_REQ_QUEUE_SIZE)
			,m_responses(MAX_REDIS_RESP_QUEUE_SIZE)
		{}

		bool async_req(redis_request_base* req) 
		{ 
			if(!m_requests.bounded_push(req))
			{
				_ut_redis_safe_delete(req);
				return false;
			}
			return true;
		}

		void update_resp(void)
		{
			for (int i=0; i<MAX_REDIS_RESP_HANDLE_SIZE; ++i)
			{
				redis_request_base* resp = NULL;
				if(!m_responses.pop(resp))
				{
					break;
				}
				if (resp)
				{
					resp->operator ()();
					delete_redis_request(resp);
				}
			}
		}

		void update_req(redisconn_ptr pconn)
		{
			for (int i=0; i<MAX_REDIS_REQ_HANDLE_SIZE; ++i)
			{
				redis_request_base* req = NULL;
				if(!m_requests.pop(req))
				{
					break;
				}
				if (req)
				{
					scoped_redis_req_trans req_scp(m_responses, req);
					try{
						req->operator()(pconn.get());
					}catch(std::exception& e){
						UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, "Redis_Channel Request Failed!Err", req->desc(), e.what());
					}catch (...){
						UT_CHANNEL_LOG(Utility::LOG_TAG_ERROR, __FILE__ , __LINE__ , LOG_SWITCH_ERROR, "Redis_Channel Request Failed!Err", req->desc(), "some uncatched err happened!");
					}
				}
			}
		}

		redis_request_queue		m_requests;
		redis_request_queue		m_responses;
	};

#define ASYNC_REDIS_REQUEST(ret, channel, redis_func, cb, ...) \
	{\
	Utility::redis_request_base* req = Utility::new_redis_request(#redis_func, redis_func, cb, __VA_ARGS__);\
	if(req)	{ ret = (channel).async_req(req) ? 0 : -1;} else {ret = -1;}\
	}

#define ASYNC_REDIS_REQUEST_NCB(channel, redis_func, ...) \
	{\
	Utility::redis_request_base* req = Utility::new_redis_request(#redis_func, redis_func, __VA_ARGS__);\
	if(req)	{ (channel).async_req(req);} else {}\
	}
}


#endif //__UT_REDIS_ASYNC_H__