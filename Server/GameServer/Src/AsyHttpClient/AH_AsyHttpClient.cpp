#include "AH_AsyHttpClient.h"
#include "UT_Func.h"
#include "UT_Log.h"

/*
static void Test(AsyHttpResponse * rsp)
{
	std::cout << Utility::Utf8Show(rsp->m_sResult.c_str())<< std::endl<<rsp->m_iReturnCode << std::endl;
} */


// 执行 HTTP 动作
void AsyHttpClient::DoHttp( AsyHttpRequest  *  req,  // 请求对象
						    AsyHttpResponse *& rsp ) // 响应对象
{
	INT32                nRetCode;
	HTTP_SESSION_HANDLE  pHTTP;
	UINT32               nSize,nTotal = 0;
	CHAR                 Buffer[HTTP_CLIENT_BUFFER_SIZE];
	BOOL l_bTotalLength = FALSE;


	if( NULL == req ) return;

	rsp = NULL;

	pHTTP = HTTPClientOpenRequest(0);

	if( ( nRetCode = HTTPClientSetVerb( pHTTP,req->m_struReqPara.Verb ) ) != HTTP_CLIENT_SUCCESS )
	{
		goto END;
	}

	if( req->m_struReqPara.AuthType != AuthSchemaNone )
	{
		if( ( nRetCode = HTTPClientSetAuth( pHTTP,
			                                req->m_struReqPara.AuthType,
											NULL ) ) != HTTP_CLIENT_SUCCESS )
		{
			goto END;
		}

		if( ( nRetCode = HTTPClientSetCredentials( pHTTP,
			                                       req->m_struReqPara.UserName,
												   req->m_struReqPara.Password ) ) !=  HTTP_CLIENT_SUCCESS)
		{
			goto END;
		}
	}

	

	if( VerbPost == req->m_struReqPara.Verb &&
		0 != req->m_struReqPara.DataLength )
	{
		l_bTotalLength = TRUE;
	}

	if( ( nRetCode = HTTPClientSendRequest( pHTTP,
		                                    req->m_struReqPara.Uri,
											req->m_struReqPara.Data,
											req->m_struReqPara.DataLength,
											l_bTotalLength,
											0,
											0 ) ) != HTTP_CLIENT_SUCCESS)
	{
		goto END;
	}

	if( ( nRetCode = HTTPClientRecvResponse( pHTTP,3 ) ) != HTTP_CLIENT_SUCCESS )
	{
		goto END;
	}

	while( ( ( nRetCode == HTTP_CLIENT_SUCCESS     ) || 
		     ( nRetCode != HTTP_CLIENT_EOS         )  ) &&
		   ( nTotal < HTTP_CLIENT_BUFFER_SIZE ) )
	{
		nSize = HTTP_CLIENT_BUFFER_SIZE - nTotal;   

		nRetCode = HTTPClientReadData( pHTTP, &(Buffer[nTotal]), nSize, 0, &nSize );
		nTotal += nSize;		
	}

END:
	if( 0 != nTotal ) {
		rsp = new AsyHttpResponse(&(Buffer[0]),HTTP_CLIENT_SUCCESS,req->m_pCallBack );
	} else {
		rsp = new AsyHttpResponse((CHAR*)(""),nRetCode,req->m_pCallBack );
	}

	//std::cout << Utility::Utf8Show(rsp->m_sResult.c_str()) << rsp->m_uReqCode<<std::endl; //bugbug


	HTTPClientCloseRequest(&pHTTP);

}


void AsyHttpClient::SendReq( CHAR * uri,
		                     HTTP_VERB verb,
			                 CHAR * name,
			                 CHAR * pass,
			                 VOID * data,
			                 UINT32 dataLength,
			                 HTTP_AUTH_SCHEMA authType,
							 ASY_HTTP_REP_CALLBACK callBack )
{
	AsyHttpRequest * req = new AsyHttpRequest( uri,
		                                       verb,
											   name,
											   pass,
											   data,
											   dataLength,
											   authType,
											   callBack );
	if(!m_kAHRequests.bounded_push(req) )
	{
		delete req;
	}
}

void AsyHttpClient::Update()
{
	AsyHttpResponse * rsp = NULL;
	ASY_HTTP_REP_CALLBACK callBack = NULL;

	if( m_kAHResponses.pop(rsp) )
	{
		if( NULL != rsp )
		{
			callBack = rsp->m_pCallBack;

			if( NULL != callBack )
			{
				try {
					callBack(rsp);
					delete rsp;
				} catch(...){
					GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_GAME, "Asy Http callback fail! " );
				}
			}
		}
	}
}

void AsyHttpClient::update(void)
{
	AsyHttpRequest  * req = NULL;
	AsyHttpResponse * rsp = NULL;

	while( m_kAHRequests.pop(req) )
	{
		if( NULL !=  req )
		{
			rsp = NULL;
			DoHttp( req, rsp );
			delete req;
			if( NULL != rsp )
			{
				m_kAHResponses.bounded_push(rsp);
			}
		}
	}
}



