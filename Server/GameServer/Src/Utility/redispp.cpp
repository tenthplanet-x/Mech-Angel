#include "redispp.h"
#include <errno.h>
#include <stdio.h>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <assert.h>

using boost::asio::ip::tcp;

namespace redispp
{

class ClientSocket : boost::noncopyable
{
public:
	ClientSocket(boost::asio::io_service& io_service_, const char* host, const char* port)
	: socket_(io_service_)
	{
		sb_.set_conn(this);
		boost::system::error_code ec;
		socket_.connect(tcp::endpoint(boost::asio::ip::address_v4::from_string(host, ec), atoi(port)), ec);
		if (ec)
		{
			throw std::runtime_error(std::string("error connect: ") + host + ":" + port + " (" + boost::system::system_error(ec).what()+ ")");
		}

		socket_.set_option(boost::asio::socket_base::keep_alive(true), ec);
		if (ec)
		{
			throw std::runtime_error(std::string("error option keep_alive: ") + host + ":" + port + " (" + boost::system::system_error(ec).what()+ ")");
		}

		socket_.set_option(boost::asio::socket_base::reuse_address(true), ec);
		if (ec)
		{
			throw std::runtime_error(std::string("error option reuse_address: ") + host + ":" + port + " (" + boost::system::system_error(ec).what()+ ")");
		}
	}

	~ClientSocket(void)
	{
		boost::system::error_code ec;
		socket_.close(ec);
		if (ec)
		{
			throw std::runtime_error(std::string("error close socket: ") + boost::system::system_error(ec).what());
		}
	}

	void tcpNoDelay(bool enable)
	{
		boost::system::error_code ec;
		socket_.set_option(boost::asio::ip::tcp::no_delay(enable), ec);
		if(ec)
		{
			throw std::runtime_error(std::string("error option no_delay: ") + boost::system::system_error(ec).what());
		}
	}

	void write(const void* data, size_t len)
	{
		boost::system::error_code ec;
		boost::asio::write(socket_, boost::asio::buffer(data, len), boost::asio::transfer_at_least(len), ec);
		if(ec)
		{
			throw std::runtime_error(std::string("error writing to socket: ") + boost::system::system_error(ec).what());
		}
	}

	size_t read(void* data, size_t len)
	{
		boost::system::error_code ec;
		std::size_t ret = socket_.read_some(boost::asio::buffer(data, len), ec);
		if(ec)
		{
			throw std::runtime_error(std::string("error reading from socket: ") + boost::system::system_error(ec).what());
		}
		return ret;
	}

	inline std::streambuf* getStreamBuf(void)
	{
		return &sb_;
	}

	class StreamBuf : public std::streambuf
	{
	public:
		StreamBuf(ClientSocket* conn = NULL)
			: conn(conn)
		{
			setp(outBuffer, outBuffer + sizeof(outBuffer) - 1);
		}

		void set_conn(ClientSocket* conn)
		{
			this->conn = conn;
		}

		int_type overflow(int_type c)
		{
			if (!traits_type::eq_int_type(traits_type::eof(), c))
			{
				traits_type::assign(*pptr(), traits_type::to_char_type(c));
				pbump(1);
			}
			return sync() == 0 ? traits_type::not_eof(c): traits_type::eof();
		}

		std::streamsize xsputn(const char* buf, std::streamsize size)
		{
			sync();
			conn->write(buf, size);
			return size;
		}

		int sync()
		{
			if(pbase() != pptr())
			{
				conn->write(pbase(), pptr() - pbase());
				setp(outBuffer, outBuffer + sizeof(outBuffer) - 1);
			}
			return 1;
		}

		int_type underflow()
		{
			const size_t got = conn->read(inBuffer, sizeof(inBuffer));
			setg(inBuffer, inBuffer, inBuffer + got);
			return traits_type::to_int_type(*gptr());
		}

		std::streamsize xsgetn(char* dest, std::streamsize size)
		{
			std::streamsize numRead = 0;
			while(numRead < size)
			{
				char* const beg = gptr();
				char* const end = egptr();
				if(beg < end)
				{
					const std::streamsize avail = end - beg;
					const std::streamsize toRead = std::min<std::streamsize>(size - numRead, avail);
					char* const newBeg = beg + toRead;
					std::copy(beg, newBeg, dest + numRead);
					numRead += toRead;
					if(newBeg != end)
					{
						setg(newBeg, newBeg, end);
						return numRead;
					}
				}
				setg(inBuffer, inBuffer, inBuffer);
				const std::streamsize remaining = size - numRead;
				if(remaining > 0)
				{
					if(remaining > std::streamsize(sizeof(inBuffer)))
					{
						while(numRead < size)
						{
							numRead += conn->read(dest + numRead, size - numRead);
						}
					}
					else
					{
						underflow();
					}
				}
			}
			return numRead;
		}

	private:
		char outBuffer[4095];
		char inBuffer[4095];
		ClientSocket* conn;
	};
private:
	tcp::socket socket_;
	StreamBuf sb_;
};

class Buffer
{
public:
	Buffer(size_t bufferSize)
	: buffer(new char[bufferSize]), spot(buffer), end(buffer + bufferSize), marked(buffer)
	{}

	~Buffer(void)
	{
		delete [] buffer;
	}

	void write(char c)
	{
		checkSpace(sizeof(char));
		*spot = c;
		++spot;
	}

	void write(const char* str)
	{
		const size_t len = strlen(str);
		checkSpace(len);
		memcpy(spot, str, len);
		spot += len;
	}

	void write(const char* str, size_t len)
	{
		checkSpace(len);
		memcpy(spot, str, len);
		spot += len;
	}

	void write(const std::string& str)
	{
		write(str.c_str(), str.size());
	}

	void write(size_t i)
	{
		namespace qi = boost::spirit::qi;
		namespace karma = boost::spirit::karma;
		namespace ascii = boost::spirit::ascii;
		using karma::uint_;
		using karma::generate;
		checkSpace(11);
		generate(spot, uint_, i);
	}

	void writeArg(const char* const& arg)
	{
		const size_t len = strlen(arg);
		checkSpace(len + 1 + 11 + 4);//1 $, 11 for length, 4 for \r\n's
		writeArgLen(len);
		memcpy(spot, arg, len);
		spot += len;
		*spot++ = '\r';
		*spot++ = '\n';
	}

	void writeArg(std::string const& arg)
	{
		const size_t len = arg.length();
		checkSpace(len + 1 + 11 + 4);//1 $, 11 for length, 4 for \r\n's
		writeArgLen(len);
		memcpy(spot, &(arg[0]), len);
		spot += len;
		*spot++ = '\r';
		*spot++ = '\n';
	}

	void writeArg(int64_t const& arg)
	{
		namespace qi = boost::spirit::qi;
		namespace karma = boost::spirit::karma;
		namespace ascii = boost::spirit::ascii;
		using karma::int_;
		using karma::generate;

		char numberBuf[22];
		char* spot = numberBuf;
		generate(spot, int_, arg);
		*spot = 0;
		writeArg((const char*)numberBuf);
	}

	void mark()
	{
		marked = spot;
	}

	void reset()
	{
		spot = buffer;
		marked = buffer;
	}

	void resetToMark()
	{
		spot = marked;
	}

	void checkSpace(size_t needed)
	{
		if(spot + needed >= end)
		{
			throw std::runtime_error("buffer is full: spot + needed >= end");
		}
	}

	size_t length() const
	{
		return spot - buffer;
	}

	char* data()
	{
		return buffer;
	}

	const char* data() const
	{
		return buffer;
	}

private:
	void writeArgLen(unsigned int len)
	{
		namespace qi = boost::spirit::qi;
		namespace karma = boost::spirit::karma;
		namespace ascii = boost::spirit::ascii;
		using karma::uint_;
		using karma::generate;

		*spot++ = '$';
		generate(spot, uint_, len);
		*spot++ = '\r';
		*spot++ = '\n';
	}

	char* buffer;
	char* spot;
	char* end;
	char* marked;
};

#define EXECUTE_COMMAND_SYNC(cmd) \
	do {\
		buffer->resetToMark(); \
		_ ## cmd ## Command.execute(buffer); \
		connection->write(buffer->data(), buffer->length()); \
	} while(0)

#define EXECUTE_COMMAND_SYNC1(cmd, arg1) \
	do {\
		buffer->resetToMark(); \
		_ ## cmd ## Command.execute(arg1, buffer); \
		connection->write(buffer->data(), buffer->length()); \
	} while(0)

#define EXECUTE_COMMAND_SYNC2(cmd, arg1, arg2) \
	do {\
		buffer->resetToMark(); \
		_ ## cmd ## Command.execute(arg1, arg2, buffer); \
		connection->write(buffer->data(), buffer->length()); \
	} while(0)

#define EXECUTE_COMMAND_SYNC3(cmd, arg1, arg2, arg3) \
	do {\
		buffer->resetToMark(); \
		_ ## cmd ## Command.execute(arg1, arg2, arg3, buffer); \
		connection->write(buffer->data(), buffer->length()); \
	} while(0)

#define EXECUTE_COMMAND_SYNC4(cmd, arg1, arg2, arg3, arg4) \
	do {\
		buffer->resetToMark(); \
		_ ## cmd ## Command.execute(arg1, arg2, arg3, arg4, buffer); \
		connection->write(buffer->data(), buffer->length()); \
	} while(0)

NullReplyException::NullReplyException()
: std::out_of_range("Casting null bulk reply to string")
{
}

Command::Command(const char* cmdName, size_t numArgs)
{
	cmd = cmdName;
	header = formate_cus_head(numArgs);
}

std::string Command::formate_cus_head(size_t numArgs)
{
	std::string _header = "*";
	_header += boost::lexical_cast<std::string>(numArgs + 1);
	_header += "\r\n";
	_header += "$";
	_header += boost::lexical_cast<std::string>(cmd.length());
	_header += "\r\n";
	_header += cmd;
	_header += "\r\n";
	return _header;
}

Command::~Command()
{
}

BaseReply::BaseReply(Connection* conn)
: conn(conn)
{
	conn->outstandingReplies.push_back(*this);
	if(conn->transaction)
	{
		conn->transaction->replies.count += 1;
	}
}

BaseReply::BaseReply(const BaseReply& other)
: conn(other.conn)
{
	other.conn = NULL;
	if(conn)
		conn->outstandingReplies.insert(conn->outstandingReplies.iterator_to(other), *this);
	const_cast<BaseReply&>(other).unlink();
}

BaseReply& BaseReply::operator=(const BaseReply& other)
{
	unlink();
	conn = other.conn;
	if(conn)
		conn->outstandingReplies.insert(conn->outstandingReplies.iterator_to(other), *this);
	other.conn = NULL;
	const_cast<BaseReply&>(other).unlink();
	return *this;
}

void BaseReply::clearPendingResults()
{
	ReplyList::iterator cur = conn->outstandingReplies.begin();
	ReplyList::iterator const end = conn->outstandingReplies.iterator_to(*this);
	while(cur != end)
	{
		BaseReply& reply = *cur;
		++cur;
		reply.readResult();
	}
}

VoidReply::VoidReply(Connection* conn)
: BaseReply(conn), storedResult("")
{}

VoidReply::~VoidReply()
{
	try
	{
		result();
	}
	catch(...)
	{}
}

const std::string& VoidReply::result()
{
	if(conn)
	{
		clearPendingResults();
		Connection* const tmp = conn;
		conn = NULL;
		storedResult = tmp->readStatusCodeReply();
		unlink();
	}
	return storedResult;
}

BoolReply::BoolReply(Connection* conn)
: BaseReply(conn), storedResult(false)
{}

BoolReply::~BoolReply()
{
	try
	{
		result();
	}
	catch(...)
	{}
}

bool BoolReply::result()
{
	if(conn)
	{
		clearPendingResults();
		Connection* const tmp = conn;
		conn = NULL;
		storedResult = tmp->readIntegerReply() > 0;
		unlink();
	}
	return storedResult;
}

IntReply::IntReply(Connection* conn)
: BaseReply(conn), storedResult(0)
{}

IntReply::~IntReply()
{
	try
	{
		result();
	}
	catch(...)
	{}
}

int64_t IntReply::result()
{
	if(conn)
	{
		clearPendingResults();
		Connection* const tmp = conn;
		conn = NULL;
		storedResult = tmp->readIntegerReply();
		unlink();
	}
	return storedResult;
}

StringReply::StringReply(Connection* conn)
: BaseReply(conn)
{}

StringReply::~StringReply()
{
	try
	{
		result();
	}
	catch(...)
	{}
}

const boost::optional<std::string>& StringReply::result()
{
	if(conn)
	{
		clearPendingResults();
		Connection* const tmp = conn;
		conn = NULL;
		tmp->readBulkReply(storedResult);
		unlink();
	}
	return storedResult;
}

MultiBulkEnumerator::MultiBulkEnumerator(Connection* conn)
: BaseReply(conn), headerDone(false), count(0)
{}

MultiBulkEnumerator::~MultiBulkEnumerator()
{
	try
	{
		if(conn && count > 0)
		{
			std::string tmp;
			while(next(&tmp));
		}
	}
	catch(...)
	{}
}

bool MultiBulkEnumerator::nextOptional(boost::optional<std::string> &out)
{
	if(!pending.empty())
	{
		out = pending.front();
		pending.pop_front();
		return true;
	}
	if(!conn)
	{
		return false;
	}

	if(!headerDone)
	{
		clearPendingResults();
		headerDone = true;

		char code = 0;
		if(!(*conn->ioStream >> code))
		{
			throw std::runtime_error("error reading multi bulk response header");
		}

		if (code == '-')
		{
			std::string err;
			getline(*conn->ioStream, err);
			throw std::runtime_error(err);
		}

		if(code != '*')
		{
			throw std::runtime_error(std::string("bad multi bulk header code: ") + code);
		}

		if(!(*conn->ioStream >> count))
		{
			throw std::runtime_error("error reading multi bulk response header");
		}

		if(count < 0)
		{
			throw std::runtime_error("multi bulk reply: -1");
		}
	}
	if(count <= 0)
	{
		conn = NULL;
		unlink();
		return false;
	}
	--count;
	out = conn->readCaseReply();
	return true;
}

bool MultiBulkEnumerator::next(std::string *out) {
	bool result;
	boost::optional<std::string> optionalOut;
	result = nextOptional(optionalOut);
	if (!result) {
		return result;
	}
	if (!optionalOut) {
		throw NullReplyException();
	}
	out->swap(*optionalOut);
	return result;
}
static boost::asio::io_service st_io_service;
Connection::Connection(const std::string& host, const std::string& port, const std::string& password, size_t bufferSize)
: connection(new ClientSocket(st_io_service, host.c_str(), port.c_str())), ioStream(new std::iostream(connection->getStreamBuf())),
  buffer(new Buffer(bufferSize)), transaction(NULL)
{
	if(!password.empty())
	{
		authenticate(password.c_str());
	}
}

Connection::~Connection()
{
	ioStream.reset();//make sure this is cleared first, since it references the connection
}

static inline std::istream& getlineRN(std::istream& is, std::string& str)
{
	return std::getline(is, str, '\r');
}

std::string Connection::readStatusCodeReply()
{
	std::string ret;
	readStatusCodeReply(&ret);
	return ret;
}

void Connection::readStatusCodeReply(std::string* out)
{
	char code = 0;
	if(!(*ioStream >> code) || !getlineRN(*ioStream, *out))
	{
		throw std::runtime_error("error reading status response");
	}
	if (code == '-')
	{
		std::string err;
		getline(*ioStream, err);
		throw std::runtime_error(err);
	}

	if(code != '+')
	{
		throw std::runtime_error(std::string("read error response: ") + *out);
	}
}

int64_t Connection::readIntegerReply()
{
	char code = 0;
	int64_t ret = 0;
	if (!(*ioStream >> code))
	{
		throw std::runtime_error("error reading integer header");
	}
	
	if (code == '-')
	{
		std::string err;
		getline(*ioStream, err);
		throw std::runtime_error(err);
	}

	switch(code)
	{
	case ':':
		{
			if(!(*ioStream >> ret))
			{
				throw std::runtime_error("error reading integer response");
			}
		}
		break;
	case '$':
		{
			int count = 0;
			if(!(*ioStream >> count))
			{
				throw std::runtime_error("error reading bulk response header");
			}
			if(count < 0)
			{
				return -1;
			}
			ioStream->get();//'\r'
			ioStream->get();//'\n'
			std::string out;
			out.resize(count, '\0');
			ioStream->read((char*)out.c_str(), out.size());
			if (out.compare("nil")==0)
			{
				ret = -1;
			}
			else
			{
				ret = atoll(out.c_str());
			}
		}
		break;
	default:
		throw std::runtime_error(std::string("bad reading header code: ") + code);
		break;
	}
	return ret;
}

boost::optional<std::string> Connection::readBulkReply()
{
	boost::optional<std::string> ret;
	readBulkReply(ret);
	return ret;
}

void Connection::readBulkReply(boost::optional<std::string> &out)
{
	char code = 0;
	int count = 0;
	if (!(*ioStream >> code))
	{
		throw std::runtime_error("error reading bulk response header");
	}

	if (code == '-')
	{
		std::string err;
		getline(*ioStream, err);
		throw std::runtime_error(err);
	}

	if(code != '$')
	{
		throw std::runtime_error(std::string("bad bulk header code: ") + code);
	}

	if(!(*ioStream >> count))
	{
		throw std::runtime_error("error reading bulk response header");
	}

	if(count < 0)
	{
		out = boost::optional<std::string>();
		return;
	}
	ioStream->get();//'\r'
	ioStream->get();//'\n'
	out = std::string();
	out->resize(count, '\0');
	ioStream->read((char*)out->c_str(), out->size());
}

boost::optional<std::string> Connection::readCaseReply()
{
	boost::optional<std::string> ret;
	readCaseReply(ret);
	return ret;
}

void Connection::readCaseReply(boost::optional<std::string> &out)
{
	char code = 0;
	if (!(*ioStream >> code))
	{
		throw std::runtime_error("error reading bulk response header");
	}

	if (code == '-')
	{
		std::string err;
		getline(*ioStream, err);
		throw std::runtime_error(err);
	}

	switch(code)
	{
	case '$':
		{
			int count = 0;
			if(!(*ioStream >> count))
			{
				throw std::runtime_error("error reading bulk response header");
			}

			if(count < 0)
			{
				out = boost::optional<std::string>();
				return;
			}
			ioStream->get();//'\r'
			ioStream->get();//'\n'
			out = std::string();
			out->resize(count, '\0');
			ioStream->read((char*)out->c_str(), out->size());
		}
		break;
	case ':':
		{
			int64_t ret = 0;
			if(!(*ioStream >> ret))
			{
				throw std::runtime_error("error reading integer response");
			}
			ioStream->get();//'\r'
			ioStream->get();//'\n'
			std::stringstream _ss;
			_ss << ret;
			out = _ss.str();
		}
		break;
	case '+':
		{
			std::string _status;
			if(!getlineRN(*ioStream, _status))
			{
				throw std::runtime_error("error reading status response");
			}
			out = _status;
		}
		break;
	default:
		throw std::runtime_error(std::string("bad case header code: ") + code);
		break;
	}
}

void Connection::quit()
{
	EXECUTE_COMMAND_SYNC(Quit);
}

VoidReply Connection::authenticate(const char* password)
{
	EXECUTE_COMMAND_SYNC1(Auth, password);
	return VoidReply(this);
}

BoolReply Connection::exists(const std::string& name)
{
	EXECUTE_COMMAND_SYNC1(Exists, name);
	return BoolReply(this);
}

BoolReply Connection::del(const std::string& name)
{
	EXECUTE_COMMAND_SYNC1(Del, name);
	return BoolReply(this);
}

static std::string s_none = "none";
static std::string s_string = "string";
static std::string s_list = "list";
static std::string s_set = "set";
static std::string s_zset = "zset";
static std::string s_hash = "hash";

Type Connection::type(const std::string& name)
{
	EXECUTE_COMMAND_SYNC1(Type, name);
	std::string t = readStatusCodeReply();
	if(t == s_none)
		return None;
	if(t == s_string)
		return String;
	if(t == s_list)
		return List;
	if(t == s_set)
		return Set;
	if(t == s_zset)
		return ZSet;
	if(t == s_hash)
		return Hash;

	return None;
}

MultiBulkEnumerator Connection::keys(const std::string& pattern)
{
	EXECUTE_COMMAND_SYNC1(Keys, pattern);
	return MultiBulkEnumerator(this);
}

StringReply Connection::randomKey()
{
	EXECUTE_COMMAND_SYNC(RandomKey);
	return StringReply(this);
}

VoidReply Connection::rename(const std::string& oldName, const std::string& newName)
{
	EXECUTE_COMMAND_SYNC2(Rename, oldName, newName);
	return VoidReply(this);
}

BoolReply Connection::renameNX(const std::string& oldName, const std::string& newName)
{
	EXECUTE_COMMAND_SYNC2(RenameNX, oldName, newName);
	return BoolReply(this);
}

IntReply Connection::dbSize()
{
	EXECUTE_COMMAND_SYNC(DbSize);
	return IntReply(this);
}

BoolReply Connection::expire(const std::string& name, int seconds)
{
	EXECUTE_COMMAND_SYNC2(Expire, name, seconds);
	return BoolReply(this);
}

BoolReply Connection::expireAt(const std::string& name, int timestamp)
{
	EXECUTE_COMMAND_SYNC2(ExpireAt, name, timestamp);
	return BoolReply(this);
}

//TODO: persist

IntReply Connection::ttl(const std::string& name)
{
	EXECUTE_COMMAND_SYNC1(Ttl, name);
	return IntReply(this);
}
VoidReply Connection::ping(void)
{
	EXECUTE_COMMAND_SYNC(Ping);
	return VoidReply(this);
}
StringReply Connection::echo(const std::string& msg)
{
	EXECUTE_COMMAND_SYNC1(Echo, msg);
	return StringReply(this);
}
VoidReply Connection::select(int db)
{
	EXECUTE_COMMAND_SYNC1(Select, db);
	return VoidReply(this);
}

BoolReply Connection::move(const std::string& name, int db)
{
	EXECUTE_COMMAND_SYNC2(Move, name, db);
	return BoolReply(this);
}

VoidReply Connection::flushDb()
{
	EXECUTE_COMMAND_SYNC(FlushDb);
	return VoidReply(this);
}

VoidReply Connection::flushAll()
{
	EXECUTE_COMMAND_SYNC(FlushAll);
	return VoidReply(this);
}

VoidReply Connection::set(const std::string& name, const std::string& value)
{
	EXECUTE_COMMAND_SYNC2(Set, name, value);
	return VoidReply(this);
}

StringReply Connection::get(const std::string& name)
{
	EXECUTE_COMMAND_SYNC1(Get, name);
	return StringReply(this);
}

//TODO: mget

StringReply Connection::getSet(const std::string& name, const std::string& value)
{
	EXECUTE_COMMAND_SYNC2(GetSet, name, value);
	return StringReply(this);
}

BoolReply Connection::setNX(const std::string& name, const std::string& value)
{
	EXECUTE_COMMAND_SYNC2(SetNX, name, value);
	return BoolReply(this);
}

VoidReply Connection::setEx(const std::string& name, int time, const std::string& value)
{
	EXECUTE_COMMAND_SYNC3(SetEx, name, time, value);
	return VoidReply(this);
}

IntReply Connection::incr(const std::string& name)
{
	EXECUTE_COMMAND_SYNC1(Incr, name);
	return IntReply(this);
}

IntReply Connection::incrBy(const std::string& name, int value)
{
	EXECUTE_COMMAND_SYNC2(IncrBy, name, value);
	return IntReply(this);
}

IntReply Connection::decr(const std::string& name)
{
	EXECUTE_COMMAND_SYNC1(Decr, name);
	return IntReply(this);
}

IntReply Connection::decrBy(const std::string& name, int value)
{
	EXECUTE_COMMAND_SYNC2(DecrBy, name, value);
	return IntReply(this);
}

IntReply Connection::append(const std::string& name, const std::string& value)
{
	EXECUTE_COMMAND_SYNC2(Append, name, value);
	return IntReply(this);
}

StringReply Connection::subStr(const std::string& name, int start, int end)
{
	EXECUTE_COMMAND_SYNC3(SubStr, name, start, end);
	return StringReply(this);
}

IntReply Connection::rpush(const std::string& key, const std::string& value)
{
	EXECUTE_COMMAND_SYNC2(RPush, key, value);
	return IntReply(this);
}

IntReply Connection::lpush(const std::string& key, const std::string& value)
{
	EXECUTE_COMMAND_SYNC2(LPush, key, value);
	return IntReply(this);
}

IntReply Connection::llen(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(LLen, key);
	return IntReply(this);
}

MultiBulkEnumerator Connection::lrange(const std::string& key, int start, int end)
{
	EXECUTE_COMMAND_SYNC3(LRange, key, start, end);
	return MultiBulkEnumerator(this);
}

VoidReply Connection::ltrim(const std::string& key, int start, int end)
{
	EXECUTE_COMMAND_SYNC3(LTrim, key, start, end);
	return VoidReply(this);
}

StringReply Connection::lindex(const std::string& key, int index)
{
	EXECUTE_COMMAND_SYNC2(LIndex, key, index);
	return StringReply(this);
}

VoidReply Connection::lset(const std::string& key, int index, const std::string& value)
{
	EXECUTE_COMMAND_SYNC3(LSet, key, index, value);
	return VoidReply(this);
}

IntReply Connection::lrem(const std::string& key, int count, const std::string& value)
{
	EXECUTE_COMMAND_SYNC3(LRem, key, count, value);
	return IntReply(this);
}

StringReply Connection::lpop(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(LPop, key);
	return StringReply(this);
}

StringReply Connection::rpop(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(RPop, key);
	return StringReply(this);
}

//TODO: blpop
//TODO: brpop

StringReply Connection::rpopLpush(const std::string& src, const std::string& dest)
{
	EXECUTE_COMMAND_SYNC2(RPopLPush, src, dest);
	return StringReply(this);
}

BoolReply Connection::sadd(const std::string& key, const std::string& member)
{
	EXECUTE_COMMAND_SYNC2(SAdd, key, member);
	return BoolReply(this);
}

BoolReply Connection::srem(const std::string& key, const std::string& member)
{
	EXECUTE_COMMAND_SYNC2(SRem, key, member);
	return BoolReply(this);
}

StringReply Connection::spop(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(SPop, key);
	return StringReply(this);
}

BoolReply Connection::smove(const std::string& src, const std::string& dest, const std::string& member)
{
	EXECUTE_COMMAND_SYNC3(SMove, src, dest, member);
	return BoolReply(this);
}

IntReply Connection::scard(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(SCard, key);
	return IntReply(this);
}

BoolReply Connection::sisMember(const std::string& key, const std::string& member)
{
	EXECUTE_COMMAND_SYNC2(SIsMember, key, member);
	return BoolReply(this);
}

//TODO: sinter
//TODO: sinterstore
//TODO: sunion
//TODO: sunionstore
//TODO: sdiff
//TODO: sdiffstore

MultiBulkEnumerator Connection::smembers(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(SMembers, key);
	return MultiBulkEnumerator(this);
}

StringReply Connection::srandMember(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(SRandMember, key);
	return StringReply(this);
}

//TODO: all Z* functions

IntReply Connection::zadd(const std::string& key, int64_t score, const std::string& member)
{
	EXECUTE_COMMAND_SYNC3(ZAdd, key, score, member);
	return IntReply(this);
}
IntReply Connection::zrem(const std::string& key, const std::string& member)
{
	EXECUTE_COMMAND_SYNC2(ZRem, key, member);
	return IntReply(this);
}
IntReply Connection::zcount(const std::string& key, int64_t _min, int64_t _max)
{
	EXECUTE_COMMAND_SYNC3(ZCount, key, _min, _max);
	return IntReply(this);
}
IntReply Connection::zrank(const std::string& key, const std::string& member)
{
	EXECUTE_COMMAND_SYNC2(ZRank, key, member);
	return IntReply(this);
}
IntReply Connection::zscore(const std::string& key, const std::string& member)
{
	EXECUTE_COMMAND_SYNC2(ZScore, key, member);
	return IntReply(this);
}
IntReply Connection::zrevrank(const std::string& key, const std::string& member)
{
	EXECUTE_COMMAND_SYNC2(ZRevRank, key, member);
	return IntReply(this);
}
StringReply Connection::zincrby(const std::string& key, int increment, const std::string& member)
{
	EXECUTE_COMMAND_SYNC3(ZIncrBy, key, increment, member);
	return StringReply(this);
}
MultiBulkEnumerator Connection::zrange(const std::string& key, int start, int stop, const std::string& ws)
{
	EXECUTE_COMMAND_SYNC4(ZRange, key, start, stop, ws);
	return MultiBulkEnumerator(this);
}
MultiBulkEnumerator Connection::zrevrange(const std::string& key, int start, int stop, const std::string& ws)
{
	EXECUTE_COMMAND_SYNC4(ZRevRange, key, start, stop, ws);
	return MultiBulkEnumerator(this);
}
MultiBulkEnumerator Connection::zrangebyscore(const std::string& key, int64_t min, int64_t max, const std::string& ws)
{
	EXECUTE_COMMAND_SYNC4(ZRangeByScore, key, min, max, ws);
	return MultiBulkEnumerator(this);
}
MultiBulkEnumerator Connection::zrevrangebyscore(const std::string& key, int64_t max, int64_t min, const std::string& ws)
{
	EXECUTE_COMMAND_SYNC4(ZRevRangeByScore, key, max, min, ws);
	return MultiBulkEnumerator(this);
}
IntReply Connection::zremrangebyrank(const std::string& key, int start, int stop)
{
	EXECUTE_COMMAND_SYNC3(ZRemRangeByRank, key, start, stop);
	return IntReply(this);
}
BoolReply Connection::hset(const std::string& key, const std::string& field, const std::string& value)
{
	EXECUTE_COMMAND_SYNC3(HSet, key, field, value);
	return BoolReply(this);
}

StringReply Connection::hget(const std::string& key, const std::string& field)
{
	EXECUTE_COMMAND_SYNC2(HGet, key, field);
	return StringReply(this);
}

MultiBulkEnumerator Connection::hmget(const std::string& key, const std::vector<std::string>& fields)
{
	do
	{
		buffer->resetToMark();
		std::string head = _HMGetCommand.formate_cus_head(1 + fields.size());
		buffer->write(head);
		buffer->writeArg(key);
		{
			std::vector<std::string>::const_iterator it = fields.begin();
			for (; it!=fields.end(); ++it)
			{
				buffer->writeArg(*it);
			}
		}
		connection->write(buffer->data(), buffer->length());
	} while(0);
	return MultiBulkEnumerator(this);
}

BoolReply Connection::hsetNX(const std::string& key, const std::string& field, const std::string& value)
{
	EXECUTE_COMMAND_SYNC3(HSetNX, key, field, value);
	return BoolReply(this);
}

IntReply Connection::hincrBy(const std::string& key, const std::string& field, int value)
{
	EXECUTE_COMMAND_SYNC3(HIncrBy, key, field, value);
	return IntReply(this);
}

BoolReply Connection::hexists(const std::string& key, const std::string& field)
{
	EXECUTE_COMMAND_SYNC2(HExists, key, field);
	return BoolReply(this);
}

BoolReply Connection::hdel(const std::string& key, const std::string& field)
{
	EXECUTE_COMMAND_SYNC2(HDel, key, field);
	return BoolReply(this);
}

IntReply Connection::hlen(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(HLen, key);
	return IntReply(this);
}

MultiBulkEnumerator Connection::hkeys(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(HKeys, key);
	return MultiBulkEnumerator(this);
}

MultiBulkEnumerator Connection::hvals(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(HVals, key);
	return MultiBulkEnumerator(this);
}

MultiBulkEnumerator Connection::hgetAll(const std::string& key)
{
	EXECUTE_COMMAND_SYNC1(HGetAll, key);
	return MultiBulkEnumerator(this);
}

VoidReply Connection::save()
{
	EXECUTE_COMMAND_SYNC(Save);
	return VoidReply(this);
}

VoidReply Connection::bgSave()
{
	EXECUTE_COMMAND_SYNC(BgSave);
	return VoidReply(this);
}

VoidReply Connection::bgReWriteAOF()
{
	EXECUTE_COMMAND_SYNC(BgReWriteAOF);
	return VoidReply(this);
}

IntReply Connection::lastSave()
{
	EXECUTE_COMMAND_SYNC(LastSave);
	return IntReply(this);
}

void Connection::shutdown()
{
	EXECUTE_COMMAND_SYNC(Shutdown);
}

StringReply Connection::info()
{
	EXECUTE_COMMAND_SYNC(Info);
	return StringReply(this);
}
MultiBulkEnumerator Connection::checkSub(void)
{
	return MultiBulkEnumerator(this);
}
void Connection::subscribe(const std::string& channel)
{
	EXECUTE_COMMAND_SYNC1(Subscribe, channel);
}

void Connection::unsubscribe(const std::string& channel)
{
	EXECUTE_COMMAND_SYNC1(Unsubscribe, channel);
}

void Connection::psubscribe(const std::string& pattern)
{
	EXECUTE_COMMAND_SYNC1(PSubscribe, pattern);
}

void Connection::punsubscribe(const std::string& pattern)
{
	EXECUTE_COMMAND_SYNC1(PUnsubscribe, pattern);
}

IntReply Connection::publish(const std::string& channel, const std::string& message)
{
	EXECUTE_COMMAND_SYNC2(Publish, channel, message);
	return IntReply(this);
}
IntReply Connection::eval(const std::string& scripts, const std::vector<std::string>& keys, const std::vector<std::string>& args)
{
	do 
	{
		buffer->resetToMark();
		std::string head = _EvalCommand.formate_cus_head(2 + keys.size() + args.size());
		buffer->write(head);
		buffer->writeArg(scripts);
		buffer->writeArg(keys.size());
		{
			std::vector<std::string>::const_iterator it = keys.begin();
			for (; it!=keys.end(); ++it)
			{
				buffer->writeArg(*it);
			}
		}
		
		{
			std::vector<std::string>::const_iterator it = args.begin();
			for (; it!=args.end(); ++it)
			{
				buffer->writeArg(*it);
			}
		}
		connection->write(buffer->data(), buffer->length()); 
	} while(0);
	return IntReply(this);
}
void Connection::multi()
{
	EXECUTE_COMMAND_SYNC(Multi);
}

void Connection::exec()
{
	EXECUTE_COMMAND_SYNC(Exec);
}

void Connection::discard()
{
	EXECUTE_COMMAND_SYNC(Discard);
}

Transaction::Transaction(Connection* conn)
: conn(conn), replies(conn)
{
	if(conn->transaction)
		throw std::runtime_error("cannot start a transaction while the connection is already in one");

	conn->multi();

	conn->transaction = this;
	replies.state = Dirty;
}

Transaction::~Transaction()
{
	try
	{
		abort();
	}
	catch(...)
	{}
	conn->transaction = NULL;
}

void Transaction::commit()
{
	if(replies.state == Dirty)
	{
		replies.state = Committed;
		conn->exec();
		replies.readResult();
	}
}

void Transaction::abort()
{
	if(replies.state == Dirty)
	{
		replies.state = Aborted;
		conn->discard();
		replies.readResult();
	}
}

void QueuedReply::readResult()
{
	if(!conn)
		return;

	Connection* const tmp = conn;
	conn = NULL;
	tmp->readStatusCodeReply();//one +OK for the MULTI
	//one +QUEUED per queued request (including this QueuedReply)
	for(size_t i = 0; i < count; ++i)
	{
		tmp->readStatusCodeReply();
	}
	if(state == Committed)
	{
		const int64_t expectedCount = tmp->readIntegerReply();
		if(expectedCount != (int64_t)count)
		{
			throw std::runtime_error("transaction item count did not match");
		}
	}
	else if(state == Aborted)
	{
		tmp->readStatusCodeReply();
		//TODO: discard all remaining items to be parsed
	}
}
}; //namespace redispp

