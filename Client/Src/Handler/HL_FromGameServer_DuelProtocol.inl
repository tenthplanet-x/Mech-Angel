////----------------------------------------------------------------------------------------------
////添加临时函数
//const char* GfxGetPlayerNameTransformed(object_id_type lPlayerID,char* pkPlayerName)
//{
//	static char pkLocalName[] = "你";
//	static char pkError[] = "";
//
//	long lLocalPlayerID = 0;
//
//	LC_ClientLogicManager* pkClientLogicManager = 
//		static_cast<LC_ClientLogicManager*>(LC_ClientLogicManager::Instance());
//	
//	lLocalPlayerID = pkClientLogicManager->GetOperationPlayerID();
//
//	if(lPlayerID == lLocalPlayerID)
//		return pkLocalName;
//
//	if(pkPlayerName)
//	{
//		return pkPlayerName;
//	}
//	else
//	{
//		LC_ClientLogicManager* pkPlayerManager =  
//			(LC_ClientLogicManager*) LC_ClientLogicManager::Instance();
//
//		LC_PlayerBase* pkPlayerBase = 
//			pkPlayerManager->FindPlayer(lPlayerID);
//
//
//		if(pkPlayerBase)
//		{
//			return pkPlayerBase->GetCharName().c_str();
//		}
//		else
//		{
//			return pkError;
//		}	 
//	}
//	
//	return pkError;
//}
//
//
//void HL_FromGameServer::_handlerDuelRequest(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	
//}
//
////临时测试使用
//#pragma  TODO("临时使用一个局部全局变量保存客户端当前决斗旗子状态,后面修改掉")
//static LC_NPCBase* GS_FLAG = NULL;
//#define DUEL_FLAG_ID  (NPC_ID_MAX - 1)
//#define DUEL_FLAG_CHARTYPE 90000
//
//static void CleanDuelFlag()
//{
//	if(GS_FLAG)
//	{
//		LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
//		LC_ClientNPCManager* pkNPCMgr = CLIENT_GET_NPC_MANAGER();
//
//		pkWorldManager->RemoveObjectFromMap(GS_FLAG);
//		
//		if(GS_FLAG->IsCreateVisibleData())
//		{
//			GS_FLAG->DestoryVisibleData();
//		}
//
//		pkNPCMgr->DestoryNPC(GS_FLAG);
//		GS_FLAG = NULL;
//	}
//}
//
//static void SetDuelFlag(int x,int y,int nMapID)
//{
//	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
//	IM_Manager* pkIMMgr = IM_Manager::GetSingletonPtr();
//	LC_ClientNPCManager* pkNPCMgr = CLIENT_GET_NPC_MANAGER();
//
//	CleanDuelFlag();
//
//	bool bGetPathHeight = false;
//
//	Utility::UT_Vec3Int kCurrentLocation(
//		x,
//		y,
//		0);
//	long lHeight = 0;
//
//	if (SE_SceneManager::GetSingletonPtr() )
//	{
//		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
//			SE_SceneManager::GetSingleton().GetWalkableInfo(
//			Int2Float(kCurrentLocation.x) , Int2Float(kCurrentLocation.y));
//
//		lHeight = Float2Int(rkTerrainInfo._fLocalHeight);
//	}
//
//	kCurrentLocation.z = (int) lHeight;
//
//	GS_FLAG = pkNPCMgr->CreateNPC(DUEL_FLAG_ID,DUEL_FLAG_CHARTYPE);
//	GS_FLAG->InitLogicData();
//	GS_FLAG->SetCurrentLocation(kCurrentLocation);
//	GS_FLAG->SetTargetLocation(kCurrentLocation);
//	GS_FLAG->SetSpawnLocation(kCurrentLocation);
//
//	GS_FLAG->CreateVisibleData();
//
//	GD_ClientActorModel* pkActor = pkIMMgr->GetActor(DUEL_FLAG_ID);
//
//	if(pkActor)
//	{
//		pkActor->SetPickFlag(GD_ClientActorModel::PM_NONE);
//	}
//	
//	pkWorldManager->AddObjectToMap(nMapID,GS_FLAG);
//}
//
//
//
//void HL_FromGameServer::_handlerRltDuelRequest(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
//	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
//	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
//
//	////如果还未进入游戏，则退出
//	//if (eGameStage != ST_GAME_STAGE_GAME)
//	//	return;
//	if(NULL == pkSession->GetPlayer())
//		return;
//
//	MG_RLT_DuelInvite msg;
//
//	if( msg.Decode(pMsg,wSize) )
//	{
//		//怎么显示出来错误消息
//		switch(msg.m_ucType)
//		{
//
//			case MG_RLT_DuelInvite::DW_INVITE:
//			{
//				NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
//				NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
//				eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
//
//				{
//					//提示决斗邀请面板，这里可能要显示10个邀请的面版
//					//////////////////////////////////////////////////////////
//
//					LC_ClientGamePlayer* pkMainPlayer = pkSession->GetPlayer();
//
//#ifdef _DEBUG
//					T_ASSERT(pkMainPlayer && "主玩家无法获得")
//#endif
//
//						if( msg.m_lBeInvitePlayerID == pkMainPlayer->GetID() )
//						{
//							const char* acInvitor = NULL;
//							const char* acBeInvitor = NULL;
//
//							acInvitor = GfxGetPlayerNameTransformed(msg.m_lInvitePlayerID,msg.m_ucInviterPlayerName);
//							acBeInvitor = GfxGetPlayerNameTransformed(msg.m_lBeInvitePlayerID,msg.m_ucBeInviterPlayerName);
//
//							Utility::SendLogicInfoMessage(RE_DUEL_EVENT_INVITE,acInvitor,acBeInvitor);
//
//							LC_PlayerDuelState* pkDuelState = pkMainPlayer->GetDuelState();
//							pkDuelState->SetDuelInviter(msg.m_lInvitePlayerID);
//							pkDuelState->SetDuelID(msg.m_lParam);
//							pkDuelState->SetDuelState(LC_PlayerDuelState::DS_BE_INVITE);
//
//							SetDuelFlag(msg.m_lParamExtra1,msg.m_lParamExtra2,pkMainPlayer->GetMap()->GetMapLogicID());
//						}
//
//
//						if( msg.m_lInvitePlayerID == pkMainPlayer->GetID() )
//						{
//							//邀请发起玩家
//							//
//							const char* acInvitor = NULL;
//							const char* acBeInvitor = NULL;
//
//							acInvitor = GfxGetPlayerNameTransformed(msg.m_lInvitePlayerID,msg.m_ucInviterPlayerName);
//							acBeInvitor = GfxGetPlayerNameTransformed(msg.m_lBeInvitePlayerID,msg.m_ucBeInviterPlayerName);
//
//
//							LC_PlayerDuelState* pkDuelState = pkMainPlayer->GetDuelState();
//							pkDuelState->SetDuelID(msg.m_lParam);
//							pkDuelState->SetDuelState(LC_PlayerDuelState::DS_INVITE);
//							pkDuelState->SetDuelInviter(msg.m_lBeInvitePlayerID);
//
//							Utility::SendLogicInfoMessage(RE_DUEL_EVENT_INVITE,acInvitor,acBeInvitor);
//
//							SetDuelFlag(msg.m_lParamExtra1,msg.m_lParamExtra2,pkMainPlayer->GetMap()->GetMapLogicID());
//
//							return;
//						}
//
//						//其他玩家
//						//
//
//				}
//			}
//			break;
//
//			case MG_RLT_DuelInvite::DR_ACCEPT:
//			case MG_RLT_DuelInvite::DR_REFUSE:
//			case MG_RLT_DuelInvite::DR_NORESPONSE:
//				{
//					LC_ClientGamePlayer* pkMainPlayer = pkSession->GetPlayer();
//
//#ifdef _DEBUG
//					T_ASSERT(pkMainPlayer && "主玩家无法获得")
//#endif
// 
//					const char* acInvitor = NULL;
//					const char* acBeInvitor = NULL;
//
//					acInvitor = GfxGetPlayerNameTransformed(msg.m_lInvitePlayerID,msg.m_ucInviterPlayerName);
//					acBeInvitor = GfxGetPlayerNameTransformed(msg.m_lBeInvitePlayerID,msg.m_ucBeInviterPlayerName);
//
//					
//					if(msg.m_ucType == MG_RLT_DuelInvite::DR_ACCEPT)
//					{
//						LC_PlayerDuelState* pkDuelState = pkMainPlayer->GetDuelState();
//
//						if(pkDuelState)
//						{
//							pkDuelState->SetDuelState(LC_PlayerDuelState::DS_WAIT_DUEL);
//							//pkDuelState->SetDuelID(msg.m_unParam);
//						}
//
//						Utility::SendLogicInfoMessage(RE_DUEL_RESULT_ACCPET_INVITE,acBeInvitor,acInvitor);
//					}
//					
//					if(msg.m_ucType == MG_RLT_DuelInvite::DR_REFUSE)
//					{
//						LC_PlayerDuelState* pkDuelState = pkMainPlayer->GetDuelState();
//
//						if(pkDuelState)
//						{
//							pkDuelState->SetDuelState(LC_PlayerDuelState::DS_NULL);
//							CleanDuelFlag();
//							//pkDuelState->SetDuelID(0);
//						}
//
//						Utility::SendLogicInfoMessage(RE_DUEL_RESULT_REFULES_INVITE,acBeInvitor,acInvitor);
//					}
//
//					if(msg.m_ucType == MG_RLT_DuelInvite::DR_NORESPONSE)
//					{
//						LC_PlayerDuelState* pkDuelState = pkMainPlayer->GetDuelState();
//
//						if(pkDuelState)
//						{
//							pkDuelState->SetDuelState(LC_PlayerDuelState::DS_NULL);
//							CleanDuelFlag();
//							//pkDuelState->SetDuelID(0);
//						}
//
//						Utility::SendLogicInfoMessage(RE_DUEL_NO_RESPONSE,acBeInvitor,acInvitor);
//					}
//
//				}
//				break;
//
//			case MG_RLT_DuelInvite::DI_START:
//				{
//					//战斗开始
//					Utility::SendLogicInfoMessage(RE_DUEL_EVENT_START);
//
//					LC_ClientGamePlayer* pkGamePlayer = 
//						pkSession->GetPlayer();
//
//					LC_ClientLogicManager* pkPlayerManager =  
//						CLIENT_GET_LOGIC_MANAGER();
//
//					LC_PlayerBase* pkPlayerBase = 
//						pkPlayerManager->FindPlayer(msg.m_lInvitePlayerID);
//
//					if(pkPlayerBase)
//					{
//						LC_PlayerDuelState* pkPlayerDuelState = 
//							pkPlayerBase->GetDuelState();
//
//						pkPlayerDuelState->SetDuelID(msg.m_lParam);
//						pkPlayerDuelState->SetDuelState(LC_PlayerDuelState::DS_IN_DUEL);
//					}
//
//					pkPlayerBase = pkPlayerManager->FindPlayer(msg.m_lBeInvitePlayerID);
//
//					if(pkPlayerBase)
//					{
//						LC_PlayerDuelState* pkPlayerDuelState = 
//							pkPlayerBase->GetDuelState();
//
//						pkPlayerDuelState->SetDuelID(msg.m_lParam);
//						pkPlayerDuelState->SetDuelState(LC_PlayerDuelState::DS_IN_DUEL);
//					}
//
//					FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
//					if(pkFxManager)
//					{
//						int iFxID = pkFxManager->CreateFx(RES_FX_DUEL_BEGIN, true);
//						pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME, false);
//					}
//				}
//				break;
//
//			case MG_RLT_DuelInvite::DW_OUTRANGE_BELOSS:
//				{
//					Utility::SendLogicInfoMessage(RE_DUEL_WARNING_BE_LOSS,msg.m_lParam);
//				}
//				break;
//
//			case MG_RLT_DuelInvite::DI_END:
//				{	
//					//战斗结束，主动置位
//					long lWinPlayer = static_cast<long>( msg.m_lParam );					 
//
//					const char*  acWinName = NULL;
//					const char*  acLossName = NULL;
//
//					if(lWinPlayer == msg.m_lInvitePlayerID)
//					{
//						acWinName  = GfxGetPlayerNameTransformed(msg.m_lInvitePlayerID,msg.m_ucInviterPlayerName);
//						acLossName = GfxGetPlayerNameTransformed(msg.m_lBeInvitePlayerID,msg.m_ucBeInviterPlayerName);
//					}
//					else
//					{
//						acLossName = GfxGetPlayerNameTransformed(msg.m_lInvitePlayerID,msg.m_ucInviterPlayerName);
//						acWinName  = GfxGetPlayerNameTransformed(msg.m_lBeInvitePlayerID,msg.m_ucBeInviterPlayerName);
//					}
//
//
//					LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//
//					if( pkGamePlayer && (pkGamePlayer->GetID() == msg.m_lInvitePlayerID ||
//						pkGamePlayer->GetID() == msg.m_lBeInvitePlayerID) )
//					{
//						pkGamePlayer->ClearPendingSkill();
//
//						Utility::SendLogicInfoMessage(RE_DUEL_EVENT_END);
//
//						LC_PlayerDuelState* pkPlayerDuelState = 
//							pkGamePlayer->GetDuelState();
//
//						pkPlayerDuelState->SetDuelID(0);
//						pkPlayerDuelState->SetDuelState(LC_PlayerDuelState::DS_NULL);
//
//
//						CleanDuelFlag();
//
//						if(lWinPlayer)
//						{
//							Utility::SendLogicInfoMessage(RE_DUEL_RESULT_WINLOSS,acWinName,acLossName);
//
//							FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
//							if(pkFxManager)
//							{
//								int iFxID = -1;
//								if(lWinPlayer == pkGamePlayer->GetID())
//									iFxID = pkFxManager->CreateFx(RES_FX_DUEL_WIN,true);
//								else
//									iFxID = pkFxManager->CreateFx(RES_FX_DUEL_LOSE,true);
//								
//								pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME, false);
//							}
//						}
//						else
//						{
//							Utility::SendLogicInfoMessage(RE_DUEL_RESULT_DRAW);
//						}
//					}
//				}
//				break;
//
//			case MG_RLT_DuelInvite::DW_BELOSS:
//				{
//					Utility::SendLogicInfoMessage(RE_DUEL_WARNING_BE_LOSS,msg.m_lParam);
//				}
//				break;
//
//			case MG_RLT_DuelInvite::DW_BESTART:
//				{
//					Utility::SendLogicInfoMessage(RE_DUEL_WARNING_BE_START,msg.m_lParam);
//				}
//				break;
//			case MG_RLT_DuelInvite::DI_WARNING:
//				{
//					Utility::SendLogicInfoMessage(RE_DUEL_WARINING_COUNT_DOWN,msg.m_lParam);
//				}
//				break;
//			case MG_RLT_DuelInvite::DI_ERROR:
//				{
//					switch(msg.m_lParam)
//					{
//					case  RE_DUEL_ERROR_TARGET_DIE	:			     //目标已经死亡，不能进行决斗
//					case  RE_DUEL_ERROR_TARGET_FARAWAY :			 //目标过远，不能进行决斗
//					case  RE_DUEL_ERROR_INWAR :			             //你在战斗中，不能发起决斗邀请
//					case  RE_DUEL_ERROR_TARGET_INWAR :			     //对方正在战斗中，不能发起决斗邀请
//					case  RE_DUEL_ERROR_REGION :			         //当前区域不能进行决斗
//					case  RE_DUEL_ERROR_TARGET_REGION :			     //当前区域不能进行决斗
//					case  RE_DUEL_ERROR_BUSY   :                     //目标正在忙碌
//						{
//							// 错误包暂时不做处理，决斗生效以后，统一由决斗拒绝，决斗结束在同步状态，不然会出现bug
//							LC_ClientGamePlayer* pkMainPlayer = pkSession->GetPlayer();
//
//							LC_PlayerDuelState* pkDuelState = pkMainPlayer->GetDuelState();
//
//							long lMainPlayerID = pkMainPlayer->GetID();
//							if(pkDuelState)
//							{
//								if(pkDuelState->GetDuelState() == LC_PlayerDuelState::DS_BE_INVITE)
//								{
//									if(pkDuelState->GetDuelInviter() == msg.m_lInvitePlayerID && lMainPlayerID == msg.m_lBeInvitePlayerID)
//									{
//										pkDuelState->SetDuelID(0);
//										pkDuelState->SetDuelInviter(0);
//										pkDuelState->SetDuelState(LC_PlayerDuelState::DS_NULL);
//
//										CleanDuelFlag();
//									}
//								}
//								else
//								{
//									if( pkDuelState->GetDuelState() == LC_PlayerDuelState::DS_INVITE)
//									{
//
//										if(pkDuelState->GetDuelInviter() == msg.m_lBeInvitePlayerID && lMainPlayerID == msg.m_lInvitePlayerID)
//										{
//											pkDuelState->SetDuelID(0);
//											pkDuelState->SetDuelInviter(0);
//											pkDuelState->SetDuelState(LC_PlayerDuelState::DS_NULL);
//
//											CleanDuelFlag();
//										}
//									}
//								}
//							}
//							Utility::SendLogicInfoMessage(msg.m_lParam);
//						}
//						break;
//					case RE_DUEL_ERROR_REPEATINVITE:
//						{
//							Utility::SendLogicInfoMessage(msg.m_lParam);
//						}
//						break;
//
//					case  RE_DUEL_ERROR_BE_SHIELD :              //	8012			//您已经被%s屏蔽
//					case  RE_DUEL_ERROR_SHIELD:
//						{
//							//
//							// 错误包暂时不做处理，决斗生效以后，统一由决斗拒绝，决斗结束在同步状态，不然会出现bug
//							LC_ClientGamePlayer* pkMainPlayer = pkSession->GetPlayer();
//
//							LC_PlayerDuelState* pkDuelState = pkMainPlayer->GetDuelState();
//
//							long lMainPlayerID = pkMainPlayer->GetID();
//							if(pkDuelState)
//							{
//								if(pkDuelState->GetDuelState() == LC_PlayerDuelState::DS_BE_INVITE)
//								{
//									if(pkDuelState->GetDuelInviter() == msg.m_lInvitePlayerID && lMainPlayerID == msg.m_lBeInvitePlayerID)
//									{
//										pkDuelState->SetDuelID(0);
//										pkDuelState->SetDuelInviter(0);
//										pkDuelState->SetDuelState(LC_PlayerDuelState::DS_NULL);
//
//										CleanDuelFlag();
//									}
//								}
//								else
//								{
//									if( pkDuelState->GetDuelState() == LC_PlayerDuelState::DS_INVITE)
//									{
//
//										if(pkDuelState->GetDuelInviter() == msg.m_lBeInvitePlayerID && lMainPlayerID == msg.m_lInvitePlayerID)
//										{
//											pkDuelState->SetDuelID(0);
//											pkDuelState->SetDuelInviter(0);
//											pkDuelState->SetDuelState(LC_PlayerDuelState::DS_NULL);
//
//											CleanDuelFlag();
//										}
//									}
//								}
//							}
//							Utility::SendLogicInfoMessage(msg.m_lParam);
//						}
//						break;
//					}
//				}
//				break;
//		}
//	}
//}
// 
//void HL_FromGameServer::_handlerSyncDuelState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
//{
//	MG_SYNC_DuelState syncMsg;
//	if(!syncMsg.Decode(pMsg,wSize))
//	{
//		return;
//	}
//
//	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
//	if(NULL == pkLogicManager)
//		return;
//
//	LC_PlayerBase* pkInvitePlayer = pkLogicManager->FindPlayer(syncMsg.m_lInvitePlayerID);
//	LC_PlayerBase* pkBeInvitePlayer = pkLogicManager->FindPlayer(syncMsg.m_lBeInvitePlayerID);
//
//	if(pkInvitePlayer)
//	{
//		pkInvitePlayer->GetDuelState()->SetDuelID(syncMsg.m_lDuelID);
//		pkInvitePlayer->GetDuelState()->SetDuelState((LC_PlayerDuelState::PlayerDuelState)(syncMsg.m_lInvitePlayerDuelState));
//	}
//
//	if(pkBeInvitePlayer)
//	{
//		pkBeInvitePlayer->GetDuelState()->SetDuelID(syncMsg.m_lDuelID);
//		pkBeInvitePlayer->GetDuelState()->SetDuelState((LC_PlayerDuelState::PlayerDuelState)(syncMsg.m_lBeInvitePlayerDuelState));
//	}
//}