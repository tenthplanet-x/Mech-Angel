#ifndef _NW_HELPER_H
#define _NW_HELPER_H

class NW_ClientSessionGroup;

static int32_t siSequence = 0;
static uint16_t uiSerial = 0;

bool CreateMsgPacketHandler(NW_ClientSessionGroup* pkSessionGroup, uint16_t usFlag,
							Protocol::MG_PACKET_ENCODE_HANDLER& rkMsgPacket,
							uint16_t usMsgType, Protocol::MG_BODY* pBody = NULL);

bool CreateMsgPacketHandler(NetWork::NW_NetSessionBase* pkSession, uint16_t usFlag,
							Protocol::MG_PACKET_ENCODE_HANDLER& rkMsgPacket,
							uint16_t usMsgType, Protocol::MG_BODY* pBody);

bool CreateMsgPacketHandler(Protocol::MG_PACKET_ENCODE_HANDLER& rkMsgPacket,
							uint16_t usMsgType, Protocol::MG_BODY* pBody = NULL);

bool Get_All_MAC_Address(char* pMacBuff, uint16_t sBuffSize);

int  GetUserProfileIntVal(const StringType& rkValName);
void SetUserProfileIntVal(const StringType& rkValName,int iVal);

int64_t	GetUserProfileInt64Val(const StringType& rkValName);
void SetUserProfileInt64Val(const StringType& rkValName,int64_t iVal);

StringType GetIPAddress(long lIP);

void UpdateUiSerial(void);
void ResetUiSerial(void);

#ifndef _USE_SERVER
bool CreateSCMsgPacketHandler(Protocol::MG_PACKET_ENCODE_HANDLER& rkMsgPacket,
							uint16_t usMsgType, Protocol::MG_BODY* pBody = NULL);
#endif

#endif