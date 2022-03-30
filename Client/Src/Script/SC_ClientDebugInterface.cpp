#include "ClientPCH.h"

#include "CA_CameraManager.h"

#include "GD_ClientAsyncModelManager.h"
#include "IM_Manager.h"

#include "LC_MapManager.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientLogicManager.h"
#include "LC_GameStoryDataDefine.h"

#include "RG_RenderManager.h"

#include "SC_ClientDebugInterface.h"

#include "SE_SceneManager.h"

#include "UI_UIManager.h"
#include "UI_AvatarManager.h"

#include "NW_ClientSessionManager.h"
#include "NW_Helper.h"
#include "UT_ClientGlobalSettings.h"
#include "WT_WidgetManager.h"
#include "UT_CutSceneManager.h"
#include "BL_AsyncOperationManager.h"
#include "UT_DebugCommandParser.h"

#include "UT_LogicInfoMessage.h"

#include "IG_ImageManager.h"
#include "UE_UiEffectManager.h"
#include <fstream>
#include <iomanip>
#include "md5.h"
#include "IG_MaterialManager.h"
#include "IG_MemoryImageManager.h"

#include "UT_GlobalSettings.h"

#include "CA_CameraManager.h"
#include "CA_CameraPathAnimation.h"
#include "ClientConfigManager.h"
#include "CF_CharType.h"
#include "CF_BuddyTransform.h"
#include "CF_BossCameraDetail.h"

#include "UT_MemTracker.h"

using namespace std;
using namespace NetWork;
using namespace Protocol;
using namespace GameLogic;
using namespace Data;
using namespace Utility;
using namespace Skill;
using namespace GUISys;
//---------------------------------------------------------------------
SC_ClientDebugInterface::SC_ClientDebugInterface()
{

}
//---------------------------------------------------------------------
SC_ClientDebugInterface::~SC_ClientDebugInterface()
{

}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetFogEnable( bool bEnable )
{
	if( SE_SceneManager::GetSingletonPtr() )
	{
		SE_SceneManager::GetSingleton().SetFogShow( bEnable );
	}
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetWaterRenderingEnable( bool bEnable )
{
	if( SE_SceneManager::GetSingletonPtr() )
	{
		SE_SceneManager::GetSingleton().SetWaterShow( bEnable   );
	}
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetModelRenderingEnable( bool bEnable )
{
	if( SE_SceneManager::GetSingletonPtr() )
	{
		SE_SceneManager::GetSingleton().SetModelShow( bEnable   );
	}
}
void SC_ClientDebugInterface::SetTerrainRenderingEnable( bool bEnable )
{
	if( SE_SceneManager::GetSingletonPtr() )
	{
		SE_SceneManager::GetSingleton().SetTerrainShow( bEnable   );
	}
}
void SC_ClientDebugInterface::SetActorRenderingEnable( bool bEnable )
{
	if( IM_Manager::GetSingletonPtr() )
	{
		IM_Manager::GetSingleton().EnableActorShow( bEnable   );
	}
}
void SC_ClientDebugInterface::SetSceneFxRenderingEnable( bool bEnable )
{
	if( SE_SceneManager::GetSingletonPtr() )
	{
		SE_SceneManager::GetSingleton().SetSceneFxShow( bEnable   );
	}
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetShadowRenderingEnable( bool bEnable )
{
	if( SE_SceneManager::GetSingletonPtr() )
	{
		SE_SceneManager::GetSingleton().SetShadowShow( bEnable   );
	}
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetGrassRenderingEnable( bool bEnable )
{
	if( SE_SceneManager::GetSingletonPtr() )
	{
		SE_SceneManager::GetSingleton().SetGrassShow( bEnable  );
	}
}

//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetCharacterUpdateEnable( bool bEnable )
{
	if ( GD_ClientAsyncModelManager::GetSingletonPtr() )
	{
		GD_ClientAsyncModelManager::GetSingleton().SetModelsUpdatable( bEnable ); 
	}
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SendToDebugConsole(const StringType &strTemp)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_RLT_DebugCommandLine msg;
	TPSTR2STDSTR(strTemp, msg.m_strMsg);
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_DEBUG_COMMANDLINE, &msg);

	if (bCreateMsg && NW_ClientSessionManager::GetSingletonPtr() != NULL )
	{
		NW_ClientSessionManager* pkManager = NW_ClientSessionManager::GetSingletonPtr();
		NW_NetSessionBase* pkSession = pkManager->GetDebugSession();
		if ( pkSession != NULL )
		{
			pkSession->Send(hMsgHandler.GetBuff(),hMsgHandler.GetMsgSize());
		}
		GfxWriteLog( LOG_TAG_GAME_LOG , LOG_SWITCH_LUABIND ,"lua debug info [%s]" , strTemp.c_str());
	}
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::LogError(const StringType &strError)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_RLT_DebugCommandLine msg;
	TPSTR2STDSTR(strError, msg.m_strMsg);
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_DEBUG_COMMANDLINE, &msg);

	if (bCreateMsg && NW_ClientSessionManager::GetSingletonPtr() != NULL )
	{
		NW_ClientSessionManager* pkManager = NW_ClientSessionManager::GetSingletonPtr();
		NW_NetSessionBase* pkSession = pkManager->GetDebugSession();
		if ( pkSession != NULL )
		{
			pkSession->Send(hMsgHandler.GetBuff(),hMsgHandler.GetMsgSize());
		}
	}
}
//---------------------------------------------------------------------
int SC_ClientDebugInterface::GetBackPackTypeID() 
{
	return 2;
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetStatEnable( bool bEnable )
{
	RG_RenderManager::GetSingleton().SetScreenConsoleVisible( bEnable );
}
//---------------------------------------------------------------------
void SC_ClientDebugInterface::SetPathFindingEnable( bool bEnable )
{
	//设置是否开启寻路的全局标记
	GfxGetWorldConfig().CFG_ENABLE_PATH_FINDING = bEnable;
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ShowProfile( const StringType& name , bool bEnable )
{
	Utility::UT_StatisticsManager* pkStat = Utility::UT_StatisticsManager::GetSingletonPtr();
	if ( pkStat == NULL )
	{
		return;
	}
	if ( name == "mainloop" )
	{
		pkStat->SetShowMainLoopStat( bEnable );
	}
	if ( name == "memory" )
	{
		pkStat->SetShowMemoryStat( bEnable );
	}
	if ( name == "dx9" )
	{
		pkStat->SetShowDx9Stat( bEnable );
	}
	if ( name == "dp" )
	{
		pkStat->SetShowDpStat( bEnable );
	}
	if ( name == "animation" )
	{
		pkStat->SetShowAnimationStat( bEnable );
	}
	if ( name == "audio" )
	{
		pkStat->SetShowAudioStat( bEnable );
	}
	if( name == "particle" )
	{
		pkStat->SetShowParticleStat( bEnable );
	}
	if ( name == "collision" )
	{
		pkStat->SetShowCollisionStat( bEnable );
	}
	if ( name == "update" )
	{
		pkStat->SetShowMainUpdateStat( bEnable );
	}
}

void SC_ClientDebugInterface::ReloadLuaFile(const StringType& fileName)
{
	LS_LuaState* pkLogicState = LS_LuaStateFactory::GetSingleton().GetLogicLuaState();
	if(pkLogicState)
	{
		if(!pkLogicState->DoFile("Data/Script/UI/UIControl/"+fileName))
		{
			GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL,"load lua file: %s failed!", fileName.c_str());
		}
	}
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ChangeVehicle(int iLevel, int lCharTypeID)
{
	CSVFile::CF_Subordinate::DataEntry *pkEntry = LC_Helper::GetCimeliaInfoEntry(LC_SUB_TP_VEHICLE, iLevel);
	if (pkEntry)
	{
		pkEntry->_CharTypeID = lCharTypeID;
			//LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
			//pkGamePlayer->SetRiderCharType(lCharTypeID);
	}
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ChangeESCVehicle(int iLevel, StringType sESCModRes)
{
	CSVFile::CF_Subordinate::DataEntry *pkEntry = LC_Helper::GetCimeliaInfoEntry(LC_SUB_TP_VEHICLE, iLevel);
	if (pkEntry)
	{
		pkEntry->_ESCModResName = sESCModRes;
	}
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::DebugCharacterAvatar(int cid, float dist, float x, float z)
{
	CSVFile::CF_BuddyTransform::DataEntry *pkEntry = CSVFile::CF_BuddyTransform::GetSingleton().GetEntryPtr(cid);
	if (pkEntry)
	{
		Utility::UT_SIMDataList::iterator cit = pkEntry->_Position.begin();
		for (; cit!=pkEntry->_Position.end(); ++cit)
		{
			switch(cit->ID())
			{
				case 1:
					{
						cit->SetItemInfo(cit->ID(), x, 0, 0);
					}
					break;
				case 2:
					{
						cit->SetItemInfo(cit->ID(), z, 0, 0);
					}
					break;
				case 3:
					{
						cit->SetItemInfo(cit->ID(), dist, 0, 0);
					}
					break;
				default:
					break;
			}
		}
	}
}

void SC_ClientDebugInterface::DebugBossDetailCamera(const StringType& sParams)
{
	TStringVector kVec = UT_BaseStringUtility::SplitString(sParams, ':');
	if(kVec.empty())
		return;

	int id = String2Int(kVec[0]);
	CSVFile::CF_BossCameraDetail::DataEntry *pkEntry = CSVFile::CF_BossCameraDetail::GetSingleton().GetEntryPtr(id);
	if(pkEntry == NULL)
		return;

	pkEntry->_fCloseUpDistance = String2Float(kVec[1]);
	pkEntry->_fTargetCloseUpDistance = String2Float(kVec[2]);
	pkEntry->_fCloseUpHeight = String2Float(kVec[3]);
	pkEntry->_fTargetCloseUpHeight = String2Float(kVec[4]);
	pkEntry->_fCloseUpAngle = String2Float(kVec[5]);
	pkEntry->_fTargetCloseUpAngle = String2Float(kVec[6]);
	pkEntry->_fSideAngle = String2Float(kVec[7]);
	pkEntry->_fTargetSideAngle = String2Float(kVec[8]);
	pkEntry->_fHorzAngle = String2Float(kVec[9]);
	pkEntry->_fTargetHorzAngle = String2Float(kVec[10]);
	pkEntry->_fTotalTime = String2Float(kVec[11]);
}

//-------------------------------------------------------------
void SC_ClientDebugInterface::ChangeUIVehicleCamera(int charType, float dist)
{
	CSVFile::CF_CharType::DataEntry *pkEntry = CSVFile::CF_CharType::GetSingleton().GetEntryPtr(charType);
	if (pkEntry)
	{
		pkEntry->_CameraDist = dist;
	}
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ChangeVehicleWeapon(int iLevel, int lItemID)
{
	CSVFile::CF_Subordinate::DataEntry *pkEntry = LC_Helper::GetCimeliaInfoEntry(LC_SUB_TP_VEHICLE_FIGHTING, iLevel);
	if (pkEntry)
	{
		pkEntry->_ItemTypeID = lItemID;
	}
	
}
//-------------------------------------------------------------
StringType SC_ClientDebugInterface::FindOwerType(object_id_type nID)
{
	StringType strOwerType = "";

	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(nID);

	if (pkNPC)
	{
		strOwerType = "npc";
		return strOwerType;
	}

	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(nID);

	if (pkPlayer)
	{
		strOwerType = "player";
		return strOwerType;
	}

	strOwerType = "errorid";
	return strOwerType;
}
//-------------------------------------------------------------
StringType SC_ClientDebugInterface::GetAniName(object_id_type nID, const StringType &strOwerType)
{
	LC_ActorBase* pkActor = NULL;

	if ( strOwerType == "player" )
	{
		LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
		pkActor = pkPlayerManager->FindPlayer( nID );
	}
	else if ( strOwerType == "npc" )
	{
		LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
		pkActor = pkNPCManager->FindNPC( nID );
	}
	
	if(pkActor)
	{
		GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(pkActor->GetID());
		if(pkVisibleData)
			return pkVisibleData->GetCurrentAnimationName();
	}
	

	return "error";
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SendDebugCommandToGameServer(const StringType& rkCommand)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
	{
		return;
	}
	MG_REQ_DebugCommandLine reqMsg;
	TPSTR2STDSTR(rkCommand, reqMsg.m_strCommandLine);
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DEBUG_COMMANDLINE, &reqMsg);
	if(bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::TestEffentForRepeat()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
	{
		return;
	}
	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();

	if(pkMessageCenter)
	{
		pkMessageCenter->SendMessage(CreateMsg_UE_FINISH_TASK(pkGamePlayer->GetID(),0));
	}

}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SetActorUpdateThread( unsigned int uiCount )
{
	//if ( GD_ClientAsyncModelManager::GetSingletonPtr() )
	//{
	//	GD_ClientAsyncModelManager::GetSingleton().SetActorUpdateThread( uiCount );
	//}
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ReloadUIScript()
{
	if(UI_UIManager::GetSingletonPtr())
	{
		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UI_RELOAD_ALL_SCRIPT() );
	}
}
//----------------------------------------------------------------------------
void SC_ClientDebugInterface::OpenMapList()
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_DEBUG_UI_MAPLIST_SHOW);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ReloadSkillScript()
{
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	if(!pkSkillFactory)
		return;

	pkSkillFactory->UnInit();
	pkSkillFactory->Init(true);
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ReloadCSV(const StringType& rkCSVFile)
{
	CSVDataSourceBase* pkCsvFile = CSVDataSourceBase::GetCSV(rkCSVFile);
	if (pkCsvFile)
	{
		pkCsvFile->Load();
	}
}
//-------------------------------------------------------------
int SC_ClientDebugInterface::GetAllUIImgAllocSize()
{
	int size = 0;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	if(!pkUIManager)
		return 0;
	size =(int) pkUIManager->GetAllImgAllocationSize();
	return size;
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SetCurrentLocation(int iMapLogicID,int nLoactionX, int nLoactionY)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	LC_MapManager* pkMapManager = (LC_MapManager*)(LC_MapManager::GetSingletonPtr());
	LC_MiniMap* pkMiniMap = pkMapManager->GetMiniMap();

	int iPosX = 0;
	int iPosY = 0;
	pkMiniMap->CalcMapPosToWorld(nLoactionY, nLoactionX, iPosX, iPosY);


	char commond[256] = {0};
	sprintf(commond,"setcurrentlocation %d %d %d %d",pkGamePlayer->GetID(),iMapLogicID,iPosX,iPosY);

	SendDebugCommandToGameServer(commond);
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ChangeCurrentLocation(int iMapInOutType,int iMapLogicType,int iMapLogicID,int iMapResID,int iX,int iY)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	LC_MapManager* pkMapManager = (LC_MapManager*)(LC_MapManager::GetSingletonPtr());
	LC_MiniMap* pkMiniMap = pkMapManager->GetMiniMap();

	int iPosX = 0;
	int iPosY = 0;
	pkMiniMap->CalcMapPosToWorld(iY, iX, iPosX, iPosY);

	char commond[256] = {0};
	sprintf(commond,"changecurrentlocation %d %d %d %d %d %d %d",pkGamePlayer->GetID(),iMapInOutType,iMapLogicType,iMapLogicID,iMapResID,iPosX,iPosY);

	SendDebugCommandToGameServer(commond);
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SetCurrentLocationDirectly(int iMapLogicID,int nLoactionX, int nLoactionY)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	char commond[256] = {0};
	sprintf(commond,"setcurrentlocationdirectly %d %d %d %d",pkGamePlayer->GetID(),iMapLogicID,nLoactionX,nLoactionY);

	SendDebugCommandToGameServer(commond);
}
//-------------------------------------------------------------
bool SC_ClientDebugInterface::AcceptTask(long lTaskID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

	if (pkGamePlayer)
	{
		pkGamePlayer->ReqAcceptTask(lTaskID);
		return true;
	}

	return false;
}
//-------------------------------------------------------------
bool SC_ClientDebugInterface::FinishTask(long lTaskID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

	if (pkGamePlayer)
	{
		pkGamePlayer->ReqFinishTask(lTaskID,-1);
		return true;
	}

	return false;
}
//-------------------------------------------------------------
bool SC_ClientDebugInterface::DeleteTask(long lTaskID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

	if (pkGamePlayer)
	{
		pkGamePlayer->ReqCancelTask(lTaskID);
		return true;
	}

	return false;
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SaveActiveMemLog()
{
	std::ofstream of;
	of.open("ActiveMemory.log",ios::app);
	of << "============================================================================"<<endl;
	size_t stSystemWorkingSet;
	size_t stSystemPeekSize;
	UT_StatisticsManager::GetSingleton().SampleMemoryInfo( stSystemWorkingSet , stSystemPeekSize );
	of<<std::setw(8)<< ( ( stSystemWorkingSet ) >> 10 ) << "(KB) "<< " Current System WorkingSet" << endl;
	of<<std::setw(8)<< ( ( stSystemPeekSize ) >> 10 ) << "(KB) "<< " Peek System WorkingSet" << endl;
#if T_MEMORY_TRACKER
	if ( Memory::UT_MemTracker::Get() )
	{
#ifdef _DEBUG
		_CrtMemState kCrtMem;
		_CrtMemCheckpoint( &kCrtMem );
		_CrtMemDumpStatistics( &kCrtMem );
#endif 
		NiMemTracker* pkT = NiMemTracker::Get();
		pkT->LogTopMemoryToFile(500); 
		Memory::UT_MemTracker* pkTracker = Memory::UT_MemTracker::Get();
		//pkTracker->LogTopMemoryToFile( 500 );
	}
#endif

	NiDX9Renderer* pDevice = NiDynamicCast(NiDX9Renderer,NiRenderer::GetRenderer());

	if(pDevice)
	{
		LPDIRECT3DDEVICE9 lpDD9 = pDevice->GetD3DDevice();

		if(lpDD9)
		{
			IDirect3DQuery9* pEventQuery = NULL;
			lpDD9->CreateQuery(D3DQUERYTYPE_RESOURCEMANAGER, &pEventQuery);

			if (pEventQuery)
			{
				// Add an end marker to the command buffer queue.
				pEventQuery->Issue(D3DISSUE_END);
				D3DDEVINFO_RESOURCEMANAGER kRes;
				pEventQuery->GetData( &kRes, sizeof(D3DDEVINFO_RESOURCEMANAGER), 0 );
				pEventQuery->Release();

				//BOOL    bThrashing;             /* indicates if thrashing */
				//DWORD   ApproxBytesDownloaded;  /* Approximate number of bytes downloaded by resource manager */
				//DWORD   NumEvicts;              /* number of objects evicted */
				//DWORD   NumVidCreates;          /* number of objects created in video memory */
				//DWORD   LastPri;                /* priority of last object evicted */
				//DWORD   NumUsed;                /* number of objects set to the device */
				//DWORD   NumUsedInVidMem;        /* number of objects set to the device, which are already in video memory */
				//// Persistent data
				//DWORD   WorkingSet;             /* number of objects in video memory */
				//DWORD   WorkingSetBytes;        /* number of bytes in video memory */
				//DWORD   TotalManaged;           /* total number of managed objects */
				//DWORD   TotalBytes;             /* total number of bytes of managed objects */
				//
				of << "============================================================================"<<endl;
				DWORD stTotalDX9SystemMemory = 0;
				DWORD stTotalVideoMemory = 0;
				for ( int i = 1 ; i < D3DRTYPECOUNT ; i++ )
				{
					if ( kRes.stats[i].TotalBytes == 0 )
					{
						continue;
					}
					stTotalDX9SystemMemory += ( kRes.stats[i].TotalBytes >> 10 ) ;
					stTotalVideoMemory += ( ( kRes.stats[i].WorkingSetBytes ) >> 10 ) ;
				}
				of<<std::setw(8)<< stTotalDX9SystemMemory << "(KB) "<< " Total DX9 Runtime ManagedPool System Memory" << endl;
				of<<std::setw(8)<< stTotalVideoMemory << "(KB) "<< " Total ManagedPool Video Memory" << endl;
				static const char* d3d_name[8] = {"Invalid","Surface","Volume","Texture","VolumeTexture","CubeTex","VB","IB"};
				for ( int i = 1 ; i < D3DRTYPECOUNT ; i++ )
				{
					if ( kRes.stats[i].TotalBytes == 0 )
					{
						continue;
					}
					of << d3d_name[i] << endl;
					of<<std::setw(8)<< ( ( kRes.stats[i].TotalBytes ) >> 10 ) 
					    << "(KB) "<< " total number of bytes of managed objects" << endl;
					of<<std::setw(8)<< ( ( kRes.stats[i].WorkingSetBytes ) >> 10 ) 
						<< "(KB) "<< " number of bytes in video memory" << endl;
					of<<std::setw(8)<< kRes.stats[i].TotalManaged
						<< " total number of managed objects" << endl;
					of<<std::setw(8)<< kRes.stats[i].WorkingSet 
						<< " number of objects in video memory" << endl;
				}
			}
		}
	}
};

void SC_ClientDebugInterface::SetFreeCameraModeEnalbe( bool bEnable )
{
	if(bEnable)
	{
		CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_FREE);
	}
	else
		CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_LOCK);
};

void SC_ClientDebugInterface::SetTianlongCameraEnable( bool bEnable )
{
	if(bEnable)
	{
		CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_DRACONIS);
	}
	else
		CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_LOCK);
}

bool SC_ClientDebugInterface::GetTianlongCameraEnable()
{
	if(CA_CameraManager::GetSingleton().GetCameraType() == CA_CameraManager::CT_DRACONIS)
	{
		return true;
	}
	return false;
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SetTrackAnimTrackKey(int trackIdx, int keyIdx)
{
	CA_CameraManager::GetSingleton().SetCameraKey(trackIdx, keyIdx);
}
//-------------------------------------------------------------
int SC_ClientDebugInterface::GetTrackAnimTrackKeyCount(int trackIdx)
{
	return CA_CameraManager::GetSingleton().GetCameraKeyCount(trackIdx);
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::ModifyTrackAnimKey(int trackIdx, int keyIdx)
{
	CA_CameraManager::GetSingleton().ModifyCameraKey(trackIdx, keyIdx);
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::AddTrackAnimKey(int trackIdx,int keyIdx)
{
	CA_CameraManager::GetSingleton().AddCameraKey(trackIdx, keyIdx);
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::RemoveTrackAnimKey(int trackIdx,int keyIdx)
{
	CA_CameraManager::GetSingleton().RemoveCameraKey(trackIdx,keyIdx);
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SaveTrackAnimToFile()
{
	Camera::CA_CameraAnimationManager::GetSingleton().Save();
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::LoadTrackAnimFromFile()
{
	Camera::CA_CameraAnimationManager::GetSingleton().Shutdown();
	Camera::CA_CameraAnimationManager::GetSingleton().Load();
}
//-------------------------------------------------------------
void SC_ClientDebugInterface::SetTrackAnimTimeSpin(int trickIdx, float ti)
{
	Camera::CA_CameraAnimationManager::GetSingleton().SetAnimationPathSpeed(trickIdx, ti);
}
//-------------------------------------------------------------
float SC_ClientDebugInterface::GetTrackAnimTimeSpin(int trickIdx)
{
	return Camera::CA_CameraAnimationManager::GetSingleton().GetAnimationPathSpeed(trickIdx);
}
//-------------------------------------------------------------
bool SC_ClientDebugInterface::ItemEnhance(long lKeepsakeID, int nItemEntryIndex)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

	if (pkGamePlayer)
	{
		//pkGamePlayer->ItemEnhance(lKeepsakeID,nItemEntryIndex);
		return true;
	}

	return false;
}

void SC_ClientDebugInterface::SetTargetDebugInfo( bool bEnable )
{
	g_settings.SHOW_TARGET_DEBUG_INFO = bEnable;
}

void SC_ClientDebugInterface::SetTempStringDump( bool bEnable , int nCount )
{
	g_settings.DUMP_STRING_OBJECT = bEnable;
}

void SC_ClientDebugInterface::SetUseLastUIRender( bool bUse )
{
	g_settings.LAST_UI_RENDERRESTULT = bUse;
}

void SC_ClientDebugInterface::SaveFbx()
{
	UT_CutSceneManager::GetSingleton().SaveFbx();
}

void SC_ClientDebugInterface::PlayCutScene()
{
	UT_CutSceneManager::GetSingleton().Play();
}

void SC_ClientDebugInterface::PauseCutScene()
{
	static bool s_enable = false;
	s_enable = !s_enable;
	UT_CutSceneManager::GetSingleton().Pause( s_enable );
}

void SC_ClientDebugInterface::StopCutScene()
{
	UT_CutSceneManager::GetSingleton().Stop();
}

void SC_ClientDebugInterface::ReloadSkyDome()
{
	UT_CutSceneManager::GetSingleton().ReloadConfig();
	SE_SceneManager::GetSingleton().ReloadSkyBox();
}

void SC_ClientDebugInterface::SetUIVisibility(bool bUIVisible)
{
    UI_UIManager::GetSingleton().SetCurrentScreenVisibility(bUIVisible);
}

void SC_ClientDebugInterface::Transform( long lCharTypeID )
{
// 	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
// 	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
// 
// 	if (pkGamePlayer)
// 	{
// 		pkGamePlayer->RequestShapeShift(lCharTypeID);
// 	}
}
//--------------------------------------------------------
void SC_ClientDebugInterface::ReqActorAIInfo(long lActorID)
{
	if (!IS_NPC_ID(lActorID))
	{
		return;
	}

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;
	
	MG_REQ_DebugActorAIInfo kReq;
	kReq.m_lPlayerID = pkGamePlayer->GetID();
	kReq.m_lTargetID = lActorID;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DEBUG_ACTOR_AI_INFO, &kReq);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//--------------------------------------------------------
void SC_ClientDebugInterface::SetUIDebugMode( bool bDebug )
{
	g_settings.UI_DEBUG_MODE = bDebug;
	 UI_UIManager::GetSingleton().RedrawAllWidget();
}
//--------------------------------------------------------
void SC_ClientDebugInterface::ReloadControlScript(bool bUseScript)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->LoadControlScript(bUseScript,true);
}
//--------------------------------------------------------
void SC_ClientDebugInterface::SetSupremacyGfxPlayInterval(float fTime)
{
}
//--------------------------------------------------------
//-------------------------------------------------------------
void SC_ClientDebugInterface::SignUpTeamPlatform(long lMapLogicType)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	char commond[256] = {0};
	sprintf(commond,"signupteamplatform %d %d",pkGamePlayer->GetID(),lMapLogicType);

	SendDebugCommandToGameServer(commond);
}

void SC_ClientDebugInterface::LogCurrentFxList(void)
{
    //FX_ClientFxManager::GetSingleton().LogCurrentFxs();
}

void SC_ClientDebugInterface::SetHooking(bool bFlag)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->SetHooking(bFlag);
}

void SC_ClientDebugInterface::SetHookingParam(const StringType &info, long lParam)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	LC_AssistAsset &kAssistAsset = pkGamePlayer->GetAssistAsset();
	if (info=="hp")
	{
		kAssistAsset.SetHPTypeID(lParam);
	}
	else if(info=="hppro")
	{
		kAssistAsset.SetHPProportion(lParam);
	}
	else if (info=="skill")
	{
		kAssistAsset.SetAutoSkillID(lParam);
	}
	else if (info == "search")
	{
		kAssistAsset.SetAutoSearchType(AssitAutoSearchType(lParam));
	}
	else if (info == "equip")
	{
		kAssistAsset.SetAutoEquipGrade(lParam);
	}
	else if (info == "flag")
	{
		kAssistAsset.SetFlag(AssistAutoFlagBit(lParam), !BOOL(kAssistAsset.IsFlagSet(AssistAutoFlagBit(lParam))));
	}
	else if (info == "time")
	{
		kAssistAsset.SetHOOKTimeLen(lParam);
	}
	else
	{

	}
	pkGamePlayer->SynHookingSettingToServer();
}

void SC_ClientDebugInterface::ParseCommand(const StringType &commandLine, uint32_t uiPri)
{
	StringType kAllCmd = commandLine;
	size_t stCmdStart = 0;
	size_t stCmdEnd = 0;
	size_t stAllCmdLen = kAllCmd.length();
	//解析多条指令
	while (true)
	{
		stCmdStart = kAllCmd.find('#', stCmdEnd);
		if (stCmdStart == StringType::npos || stCmdStart + 1 >= stAllCmdLen + 1)
		{
			break;
		}
		stCmdStart ++;
		stCmdEnd = kAllCmd.find('#', stCmdStart);
		if (stCmdEnd == StringType::npos)
		{
			break;
		}

		//找到一条指令
		StringType kCmd = kAllCmd.substr(stCmdStart, stCmdEnd - stCmdStart);

		if (kCmd.empty())
		{
			continue;
		}

		//处理一条指令
		Utility::UT_CommandLine  cmdLine;
		cmdLine.SplitLine( kCmd );
		UT_DebugCommandParser::GetSingletonPtr()->ParseCommand( cmdLine, uiPri );

		if (stCmdEnd + 1>= stAllCmdLen)
		{
			break;
		}
	}
}
StringType SC_ClientDebugInterface::GuessCommand(StringType cmdchip)
{
	UT_DebugCommandParser::DebugCommandSet cmdSet;
	UT_DebugCommandParser::GetSingletonPtr()->GuessCommand(cmdchip, cmdSet);

	StringType rslt;

	UT_DebugCommandParser::DebugCommandSet::iterator it = cmdSet.begin();
	for(; it!=cmdSet.end(); ++it)
	{
		if (rslt.size())
		{
			rslt.append("#");
		}

		rslt.append(*it);
	}

	return rslt;
}

StringType SC_ClientDebugInterface::GuessCmdFile(StringType filechip)
{
	UT_DebugCommandParser::DebugCmdFileSet files;
	UT_DebugCommandParser::GetSingletonPtr()->GuessCmdFile(filechip, files);
	
	StringType rslt;

	UT_DebugCommandParser::DebugCmdFileSet::iterator it = files.begin();
	for(; it!=files.end(); ++it)
	{
		if (rslt.size())
		{
			rslt.append("#");
		}

		rslt.append((*it)->path);
	}

	return rslt;
}

StringType SC_ClientDebugInterface::MD5String(const StringType& src)
{
	char md5_32[33] = {0};
	md5_string((const unsigned char*)src.c_str(), md5_32, src.size());
	return md5_32;
}

void SC_ClientDebugInterface::GetMyPos()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	UT_Vec3Int pos = pkGamePlayer->GetCurrentLocation();
	char buf[256];
	sprintf_s(buf,256, "恭喜, 您的当前3d坐标为：（%d, %d, %d）", pos.x,pos.y,pos.z);

	StringType st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
}

void SC_ClientDebugInterface::DumpScore()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if(!pkPlayer)
		return;

	char buf[256];


	StringType st = buf;

	//CF_LevelUp::DataEntry* pkLevelupEntry = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(pkPlayer->GetLevel());
	CF_LevelUp::DataEntry* pkLevelupEntry = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(pkPlayer->GetCommanderLevel());
	if (NULL == pkLevelupEntry)
	{
		return ;
	}
	combat_score_type sc = pkLevelupEntry->_lCombatScore;
	sprintf_s(buf,256, "等级战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//装备战斗力
	sc = pkPlayer->GetCommanderEquipScore();
	sprintf_s(buf,256, "装备战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//附加属性战斗力
	sc = 0;
	sprintf_s(buf,256, "附加属性战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//称号战斗力
	sc = pkPlayer->GetTitleFIghtScore();
	sprintf_s(buf,256, "称号战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//坐骑战斗力
	sc = pkPlayer->GetVehicleCurrentCombatScore();
	sprintf_s(buf,256, "坐骑战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//弩炮战斗力
	sc = pkPlayer->GetBowCurrentCombatScore();
	sprintf_s(buf,256, "弩炮战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//天眼战斗力
	sc = pkPlayer->GetDevaEyeCurrentCombatScore();
	sprintf_s(buf,256, "天眼战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//骑兵战斗力
	sc = pkPlayer->GetVehicleFightingCurrentCombatScore();
	sprintf_s(buf,256, "骑兵战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//神翼战斗力
	sc = pkPlayer->GetMuhunCurrentCombatScore();
	sprintf_s(buf,256, "神翼战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//法宝战斗力
	sc = pkPlayer->GetFabaoCurrentCombatScore();
	sprintf_s(buf,256, "法宝战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//灵宝战斗力
	sc = pkPlayer->GetSpiritPetCurrentCombatScore();
	sprintf_s(buf,256, "灵宝战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//灵武战斗力
	sc = pkPlayer->GetSpiritWeaponCurrentCombatScore();
	sprintf_s(buf,256, "灵武战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//法器战斗力
	if ( pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_HIDDEN_WEAPON) > 0 )
		sc = LC_Helper::GetCimeliaTotalScore(pkPlayer, LC_SUB_TP_HIDDEN_WEAPON,pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_HIDDEN_WEAPON)->GetStage());
	else
		sc = 0;
	sprintf_s(buf,256, "法器战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//天罡战斗力
	if ( pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_PLOUGH) > 0 )
		sc = LC_Helper::GetCimeliaTotalScore(pkPlayer, LC_SUB_TP_PLOUGH,pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_PLOUGH)->GetStage());
	else
		sc = 0;
	sprintf_s(buf,256, "天罡战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//神兵
	sc = LC_Helper::GetGodWeaponTotalScore(pkPlayer);
	sprintf_s(buf,256, "神兵战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//时装
	sc = LC_Helper::GetFashionTotalScore(pkPlayer);
	sprintf_s(buf,256, "时装战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//炼魂战斗力
	sc = pkPlayer->GetSoulRefineCombatScore();
	sprintf_s(buf,256, "炼魂战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//技能战斗力
	sc = LC_Helper::GetSkillTotalScore(pkPlayer);
	sprintf_s(buf,256, "技能战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//心法战斗力
	sc = LC_Helper::GetMentalTotalScore(pkPlayer);
	sprintf_s(buf,256, "心法战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//BUFF战斗力
	sc = LC_Helper::GetBufferTotalScore(pkPlayer);
	sprintf_s(buf,256, "BUFF战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 


	//秘籍战斗力
	sc = pkPlayer->GetSkillBookAsset().GetCombatScore();
	sprintf_s(buf,256, "秘籍战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//炼化炉战斗力
	sc = pkPlayer->GetMeltAsset().GetTotalCombatScore();
	sprintf_s(buf,256, "炼化炉战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//命骨炉战斗力
	sc = pkPlayer->GetBoneSoulAsset().GetTotalCombatScore();
	sprintf_s(buf,256, "命骨炉战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 

	//全服加成
	sc = LC_Helper::GetMasterBuffScore(pkPlayer);
	sprintf_s(buf,256, "全服加成战斗力：%lld", sc);
	st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", st.c_str()); 
}

void SC_ClientDebugInterface::GetACInfo()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession* pkLoginSession		= pkSessionGroup->m_pkLoginSession;

	char buf[256];
	sprintf_s(buf,256, "恭喜, 您的\nCharID为：%d \n userName为：%s \n UserID为：%d", pkSessionGroup->m_nSelectCharID.detail(), pkSessionGroup->m_strUser.c_str(), pkSessionGroup->m_dwUserID);

	StringType st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);

}

void SC_ClientDebugInterface::GetAllUnits()
{

}

void SC_ClientDebugInterface::GetAllFX()
{

}

void SC_ClientDebugInterface::WingRot(float x, float y, float z)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	GD_ClientActorModel* model =  (GD_ClientActorModel*)IM_Manager::GetSingleton().GetActor(pkGamePlayer->GetID());
	model->SetWingRot(x,y,z);
}

void SC_ClientDebugInterface::WingXYZ(float x, float y, float z)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	GD_ClientActorModel* model =  (GD_ClientActorModel*)IM_Manager::GetSingleton().GetActor(pkGamePlayer->GetID());
	model->SetWingXYZ(x,y,z);
}

void SC_ClientDebugInterface::WingLevel(int llevel)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->DoUpdateWing(llevel);
}

void SC_ClientDebugInterface::SetSkillDeadForce(int charType, int hForce, int VForce)
{
	CF_CharType::DataEntry* ccData = CF_CharType::GetSingleton().GetEntryPtr(charType);
	if ( ccData)
	{
		if ( hForce >= 0.0f )
		{
			ccData->_fHorVel = hForce;
		}

		if (VForce >= 0.0f )
		{
			ccData->_fVerVel = VForce;
		}
	}
}

void SC_ClientDebugInterface::SetCameraType(int type)
{
	CA_CameraManager::GetSingleton().SetCameraType((CA_CameraManager::CAMERA_TYPE)type);
}

void SC_ClientDebugInterface::FBUpdate(int lLevel)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->DoUpdateFB(lLevel);
}

void SC_ClientDebugInterface::LCUpdate(int lLevel)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->DoUpdateLC(lLevel);
}

void SC_ClientDebugInterface::LCWeaponUpdate(int lLevel)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->DoUpdateLCWeapon(lLevel);
}

void SC_ClientDebugInterface::LCFollowLoad()
{
	g_settings.Load(true);
}

void SC_ClientDebugInterface::AQUpdate(int iLevel)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->DoUpdateAQ(iLevel);
}

void SC_ClientDebugInterface::SetCounter(int flag)
{
	g_settings.DS_COUNTER = flag == 1;
}

void SC_ClientDebugInterface::SetSuiteCounter( int flag )
{
	g_settings.DS_SUITACK = flag == 1;
}


StringType SC_ClientDebugInterface::GetServerVersionInfo()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

	StringStreamType sStr;
	sStr << pkSessionGroup->GetServerGameVersion() << "|";
	sStr << pkSessionGroup->GetServerResVersion() << "|";
	return sStr.str();
}