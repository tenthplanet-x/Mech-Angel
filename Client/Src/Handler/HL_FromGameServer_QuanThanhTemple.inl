//#include "LC_ClientTranscriptionInfoManager.h"
//
//
//
//void HL_FromGameServer::_handlerRltChangeElement( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//	if(!pkGamePlayer)
//		return;
//	LC_ClientTranscriptionInfoManager* pkClientTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
//	if (pkClientTranscriptionInfoManager)
//	{
//		pkClientTranscriptionInfoManager->OnRecvQuanThanhTempleMsg(pMsg,wSize,MGPT_RLT_CHANGEELEMENT,pkGamePlayer);
//	}
//}
