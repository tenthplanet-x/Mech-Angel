
//------------------------------------------------------------------------
void LC_ClientGamePlayer::ResponseFromLuaAfterMapChanged(int type)
{
	GameEventType eType = GameEventType(type);
	switch(eType)
	{
	case GLET_SHOW_DUNGEON_STAGE:
		{
			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = false;
			MG_REQ_TRANSCRIPTION_PROGRESS kMsg;
			kMsg.m_lPlayerID	= GetID();
			bCreateMsg			= CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TRANSCRIPTION_PROGRESS, &kMsg);
			if(bCreateMsg)
			{
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}
		break;
	}
}