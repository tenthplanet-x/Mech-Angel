#ifndef __AH_ASY_HTTP_CLIENT_H__
#define __AH_ASY_HTTP_CLIENT_H__


#define HTTP_CLIENT_BUFFER_SIZE   8192
#define HTTP_CLIENT_URI_SIZE      1024
#define HTTP_CLIENT_NAME_SIZE     64
#define HTTP_CLIENT_PASS_SIZE     64
#define MAX_HTTP_REQ_QUEUE_SIZE   1024
#define MAX_HTTP_RSP_QUEUE_SIZE   1024

#include <stdio.h>
#include "SystemInclude.h"
#include "HttpApi/HTTPClient.h"
#include "simple_thread.h"
#include "StringDefine.h"
#include <boost/lockfree/queue.hpp>



using namespace Utility;


typedef struct _HTTPParameters
{
    CHAR               Uri[HTTP_CLIENT_URI_SIZE];        
    HTTP_VERB          Verb;
    CHAR               UserName[HTTP_CLIENT_NAME_SIZE];
    CHAR               Password[HTTP_CLIENT_PASS_SIZE];
	CHAR *             Data;
    UINT32             DataLength;
    HTTP_AUTH_SCHEMA   AuthType;
} HTTPParameters;

class AsyHttpResponse;

typedef boost::function<void(AsyHttpResponse*)> ASY_HTTP_REP_CALLBACK;

class AsyHttpResponse
{
public:
	AsyHttpResponse( CHAR * s,
		             INT32 returnCode,
		             ASY_HTTP_REP_CALLBACK callBack ):
	  m_sResult(s),
	  m_iReturnCode(returnCode),
	  m_pCallBack(callBack)
	{};
	StringType            m_sResult;
	INT32                 m_iReturnCode;
	ASY_HTTP_REP_CALLBACK m_pCallBack;
};


class AsyHttpRequest
{
public:
	AsyHttpRequest( CHAR * uri,
		            HTTP_VERB verb,
					CHAR * name,
					CHAR * pass,
					VOID * data,
					UINT32 dataLength,
					HTTP_AUTH_SCHEMA authType,
					ASY_HTTP_REP_CALLBACK callBack ) 
	{
		if( NULL != uri && strlen(uri) < HTTP_CLIENT_URI_SIZE ) 
			strcpy( m_struReqPara.Uri,uri );
		else strcpy( m_struReqPara.Uri,"");

		m_struReqPara.Verb = verb;

		if( NULL != name && strlen(name) < HTTP_CLIENT_NAME_SIZE ) 
			strcpy( m_struReqPara.UserName,name);
		else strcpy( m_struReqPara.UserName,"");

		if( NULL != pass && strlen(pass) < HTTP_CLIENT_PASS_SIZE ) 
			strcpy( m_struReqPara.Password,pass );
		else strcpy( m_struReqPara.Password,"");

		if( NULL != data && 0 != dataLength ) {
			m_struReqPara.Data = new CHAR[dataLength];
			memcpy( m_struReqPara.Data,data,dataLength);
			m_struReqPara.DataLength = dataLength;
		} else {
			m_struReqPara.Data      = NULL;
			m_struReqPara.DataLength = 0;
		}

		m_struReqPara.AuthType = authType;
		m_pCallBack = callBack;

	};

	~AsyHttpRequest() 
	{
		if( NULL != m_struReqPara.Data ) {
			delete m_struReqPara.Data;
		}
	}

	HTTPParameters        m_struReqPara;
	ASY_HTTP_REP_CALLBACK m_pCallBack;
};

class AsyHttpClient : public simple_thread
{
public:
	AsyHttpClient() :
	  m_kAHRequests(MAX_HTTP_REQ_QUEUE_SIZE),
	  m_kAHResponses(MAX_HTTP_RSP_QUEUE_SIZE)
	{};
	bool Init() { return true; };
	bool Unit() { return true; };

	// 发送 http 请求,多线程安全
	void SendReq( CHAR * uri,                      // uri和端口,比如 http://news.sina.com.cn:80 
		          HTTP_VERB verb,                  // 操作类型 Get/Head/Post
			      CHAR * name,                     // 用户名
				  CHAR * pass,                     // 用户口令
				  VOID * data,                     // 要张贴的数据
				  UINT32 dataLength,               // 要张贴的数据长度
				  HTTP_AUTH_SCHEMA authType,       // 认证类型 Basic/Digest/Kerberos
				  ASY_HTTP_REP_CALLBACK callBack );// 结果回调函数，参数为 AsyHttpResponse 对象指针，函数调用完成后对象将自动释放	

	void Update();
	virtual void update(void);
private:
	void DoHttp( AsyHttpRequest *req,AsyHttpResponse *&rsp );
	boost::lockfree::queue<AsyHttpRequest*>  m_kAHRequests;
	boost::lockfree::queue<AsyHttpResponse*> m_kAHResponses;
};

#endif
