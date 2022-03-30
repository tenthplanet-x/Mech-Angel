#ifndef _HL_FROM_CLIENT_H
#define _HL_FROM_CLIENT_H

class NW_Login2ClientSession;

class HL_FromLoginServer
{
public:
	HL_FromLoginServer(){};
	~HL_FromLoginServer(){};

	static VOID ParsePacket(NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize );

protected:

	//处理 与 loginserver 之间的逻辑
	static void _handlerRltBeforeEnterGameServer(NW_Login2ClientSession* pkSession,char* pMsg, uint16_t wSize);
	static void _handlerMsg(NW_Login2ClientSession* pkSession,char* pMsg, uint16_t wSize);
	static void _handlerRltLogin(NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize , uint32_t uid);
	static void _handlerServerList(NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCharList(NW_Login2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	static void _handlerRltCreateChar(NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDeleteChar(NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize);
	static void _handlerRltRecoverChar(NW_Login2ClientSession* pkClientSession,char* pMsg, uint16_t wSize);
	static void _handlerRltServerBusy(NW_Login2ClientSession* pkClientSession,char* pkMsg,uint16_t wSize);
	static void _handlerRltGameTime(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize);
	static void _handlerRltRandomCharName(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize);
	static void _handlerRltSignatureResponse(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize);
	static void _handlerSSKickPlayer(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize);
};


#endif