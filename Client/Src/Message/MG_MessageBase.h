#ifndef _MG_MESSAGE_BASE_H
#define _MG_MESSAGE_BASE_H

#include "ClientDefine.h"
#include "GD_ClientActorModel.h"
#include "FX_ClientFxManager.h"
#include "MG_MessageDefine.h"
#include "GD_ClientAsyncModelManager.h"
#include "IG_Material.h"
//消息基类定义
//网络消息可根据其定义封装Wrapper
//本地消息为节省效率,无需封装
class MG_MessageBase
{
public:
	MG_MessageBase();
	MG_MessageBase(const MG_MessageBase& rkMessage);
	~MG_MessageBase();

	void Reset();
	
public:
	MessageType m_eMsgType;
	bool		m_bMsgHandled;

	union
	{
		LONG		 m_lParam1;
		float		 m_fParam1;
		unsigned int m_uiParam1;
		BYTE*        m_pbyParam1;
	};

	union
	{
		LONG		 m_lParam2;
		float		 m_fParam2;
		unsigned int m_uiParam2;
        BYTE*        m_pbyParam2;
	};

	union
	{
		LONG		 m_lParam3;
		float		 m_fParam3;
		unsigned int m_uiParam3;
        BYTE*        m_pbyParam3;
	};

	union
	{
		LONG		 m_lParam4;
		float		 m_fParam4;
		unsigned int m_uiParam4;
		BYTE*        m_pbyParam4;
	};

	union
	{
		LONG		 m_lParam5;
		float		 m_fParam5;
		unsigned int m_uiParam5;
		BYTE*        m_pbyParam5;
	};

	union
	{
		LONG		 m_lParam6;
		float		 m_fParam6;
		unsigned int m_uiParam6;
		BYTE*        m_pbyParam6;
	};

	int64_t			m_llParam;
};

typedef mem::list<MG_MessageBase> MG_MessageList;
typedef MG_MessageList::iterator  MG_MessageListIter;
#define MARK_MESSAGE_HANDLED(kMsg)	  (kMsg.m_bMsgHandled = true)
#define TEST_MESSAGE_HANDLED(kMsg)	  (kMsg.m_bMsgHandled)

//-----------------------------------------------------------------------------
#define HIGH_DWORD(q)		((long)((q >> 32) & 0x00ffffffff))
#define LOW_DWORD(q)		((long)(q & 0x00ffffffff))
#define MAKE_DWORD(h, l)	((((long long)h) << 32) | (long long)l)

//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_NO_ARG_MSG(MessageType msg)
{
	MG_MessageBase m;
	m.m_eMsgType = msg;
	return m;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_WIN32_MESSGAE(unsigned int uiWinMsgType,unsigned int wParam,unsigned int lParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_WIN32_MESSAGE;
	msg.m_uiParam1 = uiWinMsgType;
	msg.m_uiParam2 = wParam;
	msg.m_uiParam3 = lParam;

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_TIMER_TICK(float fCurrentTime,float fDeltaTime)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_TIMER_TICK;
	msg.m_fParam1 = fCurrentTime;
	msg.m_fParam2 = fDeltaTime;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_TIMER_START()
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_TIMER_START;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_KEY_MSG(MessageType msg)
{
	return CreateMsg_NO_ARG_MSG(msg);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_MOUSE_MSG(MessageType msg, long x, long y, long wParam)
{
	MG_MessageBase m;
	m.m_eMsgType = msg;
	m.m_uiParam1 = x;
	m.m_uiParam2 = y;
	m.m_lParam3 = wParam;

	return m;

	//Indicates the mouse buttons and keys that the user pressed. The following table shows the possible values, which can be combined.

	//	Value  Description  
	//	MK_CONTROL
	//	The user pressed the CTRL key.

	//	MK_LBUTTON
	//	The user pressed the left mouse button.

	//	MK_MBUTTON
	//	The user pressed the middle mouse button.

	//	MK_RBUTTON
	//	The user pressed the right mouse button.

	//	MK_SHIFT
	//	The user pressed the SHIFT key.
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_TOGGLE_BACKLOADING( long bEnable , long lR )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_TOGGLE_BACKLOADING;
	msg.m_lParam1 = bEnable;
	msg.m_lParam2 = lR;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_CAMERA_RESET()
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_CAMERA_RESET;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_CAMERA_ROTATE(float fYaw , float fPitch )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_CAMERA_ROTATE;
	msg.m_fParam1 = fYaw;
	msg.m_fParam2 = fPitch;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_DEBUG_TERRAIN_CHUNK_LOAD(long iChunkWorldIndexX,long iChunkWorldIndexY)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_DEBUG_TERRAIN_CHUNK_LOAD;
	msg.m_lParam1 = iChunkWorldIndexX;
	msg.m_lParam2 = iChunkWorldIndexY;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_DEBUG_TERRAIN_CHUNK_UNLOAD(long iChunkWorldIndexX,long iChunkWorldIndexY)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_DEBUG_TERRAIN_CHUNK_UNLOAD;
	msg.m_lParam1 = iChunkWorldIndexX;
	msg.m_lParam2 = iChunkWorldIndexY;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_SCENE_L_BUTTON_PICK_TERRAIN(float fTerrainLocationX,float fTerrainLocationY,float fTerrainLocationZ, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_SCENE_L_BUTTON_PICK_TERRAIN;
	msg.m_fParam1 = fTerrainLocationX;
	msg.m_fParam2 = fTerrainLocationY;
	msg.m_fParam3 = fTerrainLocationZ;
	msg.m_lParam4 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_SCENE_L_BUTTON_DOUBLE_PICK_TERRAIN(float fTerrainLocationX,float fTerrainLocationY,float fTerrainLocationZ, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_SCENE_L_BUTTON_DOUBLE_PICK_TERRAIN;
	msg.m_fParam1 = fTerrainLocationX;
	msg.m_fParam2 = fTerrainLocationY;
	msg.m_fParam3 = fTerrainLocationZ;
	msg.m_lParam4 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_SCENE_L_BUTTON_RELEASE_TERRAIN(float fTerrainLocationX,float fTerrainLocationY,float fTerrainLocationZ, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_SCENE_L_BUTTON_RELEASE_TERRAIN;
	msg.m_fParam1 = fTerrainLocationX;
	msg.m_fParam2 = fTerrainLocationY;
	msg.m_fParam3 = fTerrainLocationZ;
	msg.m_lParam4 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_SCENE_R_BUTTON_PICK_TERRAIN(float fTerrainLocationX,float fTerrainLocationY,float fTerrainLocationZ, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_SCENE_R_BUTTON_PICK_TERRAIN;
	msg.m_fParam1 = fTerrainLocationX;
	msg.m_fParam2 = fTerrainLocationY;
	msg.m_fParam3 = fTerrainLocationZ;
	msg.m_lParam4 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_SCENE_R_BUTTON_RELEASE_TERRAIN(float fTerrainLocationX,float fTerrainLocationY,float fTerrainLocationZ, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_SCENE_R_BUTTON_RELEASE_TERRAIN;
	msg.m_fParam1 = fTerrainLocationX;
	msg.m_fParam2 = fTerrainLocationY;
	msg.m_fParam3 = fTerrainLocationZ;
	msg.m_lParam4 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_SCENE_MOUSE_OVER(float fTerrainLocationX,float fTerrainLocationY,float fTerrainLocationZ, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_SCENE_MOUSE_OVER;
	msg.m_fParam1 = fTerrainLocationX;
	msg.m_fParam2 = fTerrainLocationY;
	msg.m_fParam3 = fTerrainLocationZ;
	msg.m_lParam4 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_MODEL_LOADING_TASK(GD_ClientAsyncModelManager::ASYNC_MODEL_TYPE eModelType,
                                                       Data::GD_ModelBase* pkModel, 
                                                       const StringType& strModFileName, 
                                                       int iModelIndex = 0, unsigned int iShaderIdentifier = 0 )
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_ADD_MODEL_LOADING_TASK;
    msg.m_lParam1    = eModelType;
    msg.m_pbyParam2  = (BYTE*)pkModel;
    msg.m_pbyParam3  = (BYTE*)strModFileName.c_str();
    msg.m_lParam4    = (long)iModelIndex;
	msg.m_lParam5	 = (long)iShaderIdentifier;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_MODEL_UNLOADING_TASK(Data::GD_ModelBase* pkModel)
{
	MG_MessageBase msg;
	msg.m_eMsgType    = MGT_ADD_MODEL_UNLOADING_TASK;
	msg.m_pbyParam1   = (BYTE*)pkModel;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_CHANGE_AVATAR_TASK(GD_ClientActorModel* pkActorModel)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_ADD_ACTOR_CHANGE_AVATAR_TASK;
	msg.m_pbyParam1  = (BYTE*)pkActorModel;
	return msg;
}

inline MG_MessageBase CreateMsg_MODEL_LOADING_TASK_FINISH( long nLogicID )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_MODEL_LOADING_TASK_FINISH;
	msg.m_lParam1 = nLogicID;
	return msg;
}
inline MG_MessageBase CreateMsg_ADD_UIEFFECT_TASK( long eType , long nID )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_UIEFFECT_TASK;
	msg.m_lParam1 = eType;
	msg.m_lParam2 = nID;
	return msg;
}
inline MG_MessageBase CreateMsg_ADD_UIEFFECT_UNLOADING_TASK( BYTE* pkObj )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_UIEFFECT_UNLOADING_TASK;
	msg.m_pbyParam1 = pkObj;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_GFX_TASK(FX_ClientFxManager::FxObject& rkFxObject)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_GFX_TASK;
	msg.m_pbyParam1 = (BYTE*)(&rkFxObject);
	return msg;
}
inline MG_MessageBase CreateMsg_ADD_GFX_UNLOADING_TASK(FX_ClientFxManager::FxObject& rkFxObject)
{
    MG_MessageBase msg;
    msg.m_eMsgType = MGT_ADD_GFX_UNLOAD_TASK;
    msg.m_pbyParam1 = (BYTE*)(&rkFxObject);;
    return msg;
}

inline MG_MessageBase CreateMsg_ADD_UI_UNLOADING_TASK( GUISys::IG_Material* pMaterial , int iErase = 0)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_UI_UNLOAD_UIIMG;
	msg.m_lParam1 = (long)iErase;
	msg.m_pbyParam2 = (BYTE*)pMaterial;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_UIIMG_TASK( const StringType& strName ,int iErase = 0 )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_UI_LOAD_UIIMG;
	msg.m_lParam1 = (long)iErase;
	msg.m_pbyParam2 = (BYTE*)strName.c_str();
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_CANCEL_UIIMG_CLUSTER_TASK( const StringType& strName )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_UI_CANCEL_LOAD_UIIMG_CLUSTER;
	msg.m_pbyParam1 = (BYTE*)strName.c_str();
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_UIIMG_CLUSTER_TASK( const StringType& strName )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_UI_LOAD_UIIMG_CLUSTER;
	msg.m_pbyParam1 = (BYTE*)strName.c_str();
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_PATH_UNLOAD_TASK( long lIndex )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_PATH_UNLOAD_TASK;
	msg.m_lParam1 = lIndex;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_PATH_LOAD_TASK( long lIndex )
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_PATH_LOAD_TASK;
	msg.m_lParam1 = lIndex;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_PLAY_VIDEO_LOAD_TASK(int eType,
													   const StringType& strImageName, 
													   const StringType& strFileName, 
													   unsigned int uLoops)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_ADD_PLAY_VIDEO_LOAD_TASK;
	msg.m_lParam1    = eType;
	msg.m_pbyParam2  = (BYTE*)strImageName.c_str();
	msg.m_pbyParam3  = (BYTE*)strFileName.c_str();
	msg.m_uiParam4   = uLoops;
	return msg;
}

class  AudioLoadArg;
inline MG_MessageBase CreateMsg_ADD_PLAY_AUDIO_LOAD_TASK(int iID, AudioLoadArg * pkArg, void * pkSoundInfo)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_ADD_PLAY_AUDIO_LOAD_TASK;
	msg.m_lParam1    = iID;
	msg.m_pbyParam2  = (BYTE*)pkArg;
	msg.m_pbyParam3  = (BYTE*)pkSoundInfo;
	return msg;
}

inline MG_MessageBase CreartMsg_ADD_REPORT_OPERATION_TASK(const char * strWebSite, const char * strFileName, bool bUpLoad, int64_t iTransId, unsigned int iTimeOutMs, const char * strPassWord)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_ADD_REPORT_OPERATION_TASK;
	msg.m_pbyParam1 = (BYTE*)strWebSite;
	msg.m_pbyParam2 = (BYTE*)strFileName;
	msg.m_lParam3	= bUpLoad;
	msg.m_llParam	= iTransId;
	msg.m_uiParam5	= iTimeOutMs;
	msg.m_pbyParam6 = (BYTE*)strPassWord;
	return msg;
}

inline MG_MessageBase CreartMsg_CALLBACK_REPORT_OPERATION_TASK(int64_t iTransId, int iResultType, int iResultCode, unsigned int iSuccessTime)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_CALLBACK_REPORT_OPERATION_TASK;
	msg.m_llParam   = iTransId;
	msg.m_lParam2	= iResultType;
	msg.m_lParam3	= iResultCode;
	msg.m_uiParam4	= iSuccessTime;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_TMAP_LOAD_TASK(const StringType& strFileName)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_ADD_TMAP_LOAD_TASK;
	msg.m_pbyParam1  = (BYTE*)strFileName.c_str();
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_RELOAD_CONFIG_TASK(TStringVector * lst, uint32_t timestamp)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_ADD_RELOAD_CONFIG_TASK;
	msg.m_pbyParam1  = (BYTE*)lst;
	msg.m_uiParam2  = timestamp;
	return msg;
}

//-----------------------------------------------------------------------------
//可交互模型管理模块发出消息
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_LBUTTON_PICK_ACTOR(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_LBUTTON_PICK_ACTOR;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_LBUTTON_PICK_INTERACTIVE_BUTTON(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_LBUTTON_PICK_INTERACTIVE_BUTTON;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_LBUTTON_DOUBLE_PICK_ACTOR(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_LBUTTON_DOUBLE_PICK_ACTOR;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_RBUTTON_PICK_ACTOR(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_RBUTTON_PICK_ACTOR;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_MOUSE_OVER_ACTOR(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_MOUSE_OVER_ACTOR;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_MOUSE_OVER_INTERACTIVE_BUTTON(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_MOUSE_OVER_INTERACTIVE_BUTTON;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_LBUTTON_PICK_CHEST(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_LBUTTON_PICK_CHEST;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_RBUTTON_PICK_CHEST(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_RBUTTON_PICK_CHEST;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_MOUSE_OVER_CHEST(long lID, long wParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_MOUSE_OVER_CHEST;
	msg.m_lParam1 = lID;
	msg.m_lParam2 = wParam;	//CreateMsg_MOUSE_MSG m_lParam3

	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_IM_PLAY_ANIMATION(long lID,const StringType& rkAnim,bool bForce,float fFrequence)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_IM_PLAY_ANIMATION;
	msg.m_lParam1	= lID;
	msg.m_pbyParam2 = (BYTE*)rkAnim.c_str();
	msg.m_lParam3	= bForce;
	msg.m_fParam4	= fFrequence;
	return msg;
}
//-----------------------------------------------------------------------------
//逻辑模块发出消息
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_PLAYER_MOVE(float fPlayerLocationX,float fPlayerLocationY,float fPlayerLocationZ)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_LOGIC_PLAYER_MOVE;
	msg.m_fParam1 = fPlayerLocationX;
	msg.m_fParam2 = fPlayerLocationY;
	msg.m_fParam3 = fPlayerLocationZ;
	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_CAN_SELECT()
{
	return CreateMsg_NO_ARG_MSG(MGT_LOGIC_CAN_SELECT);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_CAN_TALK()
{
	return CreateMsg_NO_ARG_MSG(MGT_LOGIC_CAN_TALK);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_CAN_MOVETO()
{
	return CreateMsg_NO_ARG_MSG(MGT_LOGIC_CAN_MOVETO);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_NOT_MOVETO()
{
	return CreateMsg_NO_ARG_MSG(MGT_LOGIC_NOT_MOVETO);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_BEG_LOCK_LOCATION()
{
	return CreateMsg_NO_ARG_MSG(MGT_LOGIC_BEG_LOCK_LOCATION);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_END_LOCK_LOCATION()
{
	return CreateMsg_NO_ARG_MSG(MGT_LOGIC_END_LOCK_LOCATION);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_BEG_CAST_SKILL(long lCastTime)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_LOGIC_BEG_CAST_SKILL;
	msg.m_lParam1 = lCastTime;

	return msg;
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_END_CAST_SKILL()
{
	return CreateMsg_NO_ARG_MSG(MGT_LOGIC_END_CAST_SKILL);
}
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_LOGIC_AREA_SOUND_NAME( int nAreaID , int mapResID)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_LOGIC_AREA_SOUND_NAME;
	msg.m_uiParam1 = nAreaID;
	msg.m_lParam2 = mapResID;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UI_MAP_CHANGE(int lMapLogicID, long lMapResID, long lMapLogicType, long lPreMapResID)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_UI_MAP_CHANGE;
	msg.m_lParam1 = lMapLogicID;
	msg.m_lParam2 = lMapResID;
	msg.m_lParam3 = lMapLogicType;
	msg.m_lParam4 = lPreMapResID;

	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UI_RELOAD_ALL_SCRIPT()
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_UI_RELOAD_ALL_SCRIPT;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UI_MINIMAP_CLICK(float fX, float fY)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_UI_MINIMAP_CLICK;
	msg.m_fParam1 = fX;
	msg.m_fParam2 = fY;

	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PLAYER_HP_NORMAL_COST(long lTargetID, int64_t lHPCost, int iDirection, long lMapID, int damageResultType)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_PLAYER_HP_NORMAL_COST;
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lHPCost;
    //msg.m_lParam3   = rand() % 4 + 1;
	msg.m_lParam3	= iDirection;
	msg.m_lParam4	= lMapID;
	msg.m_lParam5  = damageResultType;  // 克制关系
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PLAYER_HP_CRITICAL_COST(long lTargetID, int64_t lHPCost, int iDirection, int damageResultType)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_PLAYER_HP_CRITICAL_COST;
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lHPCost;
    //msg.m_lParam3   = rand() % 4 + 1;
	msg.m_lParam3   = iDirection;
	msg.m_lParam5  = damageResultType;  // 克制关系
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_MONSTER_HP_NORMAL_COST(long lTargetID, int64_t lHPCost, int iDirection, int iDamageType, int damageResultType)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_MONSTER_HP_NORMAL_COST;
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lHPCost;
	msg.m_lParam3   = iDirection;		// 方向：1、2、3、4
	msg.m_lParam4   = iDamageType;      // 类型：0（普通）、1（技能、DOT、反弹）、2（弩炮）
	msg.m_lParam5  = damageResultType;  // 克制关系
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_MONSTER_HP_CRITICAL_COST(long lTargetID, int64_t lHPCost, int iDirection, int iDamageType, int damageResultType)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_MONSTER_HP_CRITICAL_COST;
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lHPCost;
	msg.m_lParam3   = iDirection;		// 方向：1、2、3、4
	msg.m_lParam4   = iDamageType;		// 类型：0（普通）、1（技能、DOT、反弹）、2（弩炮）
	msg.m_lParam5  = damageResultType;  // 克制关系
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase Create_UE_PLAYER_COURAGE_POINT(long lTargetID, long lNumber)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_PLAYER_COURAGE_POINT;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= lNumber;
	return msg;
};
//------------------------------------------------------------------------------
inline MG_MessageBase Create_UE_Player_GameStoryTrials(long lTargetID, long lNumber)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_PLAYER_GAMESTORY_TRIALS;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= lNumber;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PLAYER_MISS(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_PLAYER_MISS;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//***
inline MG_MessageBase CreateMsg_UE_PLAYER_STATE_CHANGE(long lTargetID, long lStateType)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MST_UE_PLAYER_STATE_CHANGE;
	msg.m_lParam1	= lTargetID;	
	msg.m_lParam2	= lStateType;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PLAYER_JUMP_DODGE(long lTargetID, long lMapID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_PLAYER_JUMP_DODGE;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= lMapID;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_MONSTER_MISS(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_MONSTER_MISS;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_ENTER_FIGHT(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_ENTER_FIGHT;
	msg.m_lParam1	= lTargetID;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_LEAVE_FIGHT(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_LEAVE_FIGHT;
	msg.m_lParam1	= lTargetID;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PICK_GOLD(long lTargetID, int iNumber)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_PLAYER_PICK_GOLD;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= iNumber;
	return msg;
}

inline MG_MessageBase CreateMsg_UE_PICK_UNBINDYUANBAO(long lTargetID, int iNumber)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_PLAYER_PICK_UNBINDYUANBAO;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= iNumber;
	return msg;
}

inline MG_MessageBase CreateMsg_UE_PICK_SP(long lTargetID, int iNumber)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_PLAYER_PICK_SP;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= iNumber;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PLAYER_SHOW_COMBO(long lTargetID, int iNumber, int iType, int iDouble)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_PLAYER_SHOW_COMBO;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= iNumber;
	msg.m_lParam3	= iType;
	msg.m_lParam4	= iDouble;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PLAYER_DELTA_COMBO(long lTargetID, int iNumber)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_PLAYER_DELTA_COMBO;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= iNumber;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_DELTA_EXP(long lTargetID, long lNumber)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_DELTA_EXP;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= lNumber;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_DELTA_HP(long lTargetID, int64_t lNumber, int iDirection)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_DELTA_HP;	// 描述,回血，目前只考虑正值
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lNumber;
    //msg.m_lParam3   = rand() % 2;
	msg.m_lParam3	= iDirection;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_DELTA_MP(long lTargetID, int64_t lNumber, int iDirection)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_DELTA_MP;
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lNumber;
    //msg.m_lParam3   = rand() % 2;
	msg.m_lParam3	= iDirection;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_DELTA_SP(long lTargetID, int64_t lNumber, int iDirection)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_DELTA_SP;
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lNumber;
	//msg.m_lParam3   = rand() % 2;
	msg.m_lParam3	= iDirection;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_DELTA_ATTRI(long lTargetID, long kType, long val, int iDirection)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_DELTA_ATTR;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= kType;
	msg.m_lParam3	= val;
	msg.m_lParam4 = iDirection;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_PLAYER_HEADALL(int m_iActiveLevel)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SHOW_PLAYER_HEADALL;
	msg.m_lParam1	= m_iActiveLevel;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_PLAYER_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SHOW_PLAYER_NAME;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_HIDE_PLAYER_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_HIDE_PLAYER_NAME;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_CHANGE_PLAYER_TITLE(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_CHANGE_PLAYER_TITLE;
	msg.m_lParam1	= lTargetID;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_PLAYER_BLOOD(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SHOW_PLAYER_BLOOD;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_HIDE_PLAYER_BLOOD(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_HIDE_PLAYER_BLOOD;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_UPDATE_PLAYER_BLOOD(long lTargetID, int iParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_UPDATE_PLAYER_BLOOD;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= iParam;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_MONSTER_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SHOW_MONSTER_NAME;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_DROPITEM_NAME(long lTargetID, long firstShow, long ft)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SHOW_DROPITEM_NAME;
	msg.m_lParam1	= lTargetID;	
	msg.m_lParam2	= firstShow;
	msg.m_lParam3	= ft;
	return msg;
}
inline MG_MessageBase CreateMsg_UE_HIDE_DROPITEM_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_HIDE_DROPITEM_NAME;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_HIDE_MONSTER_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_HIDE_MONSTER_NAME;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_REMOVE_DISPLAY_CHEST(long lTargetID, int iClientDisplay)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_REMOVE_DISPLAY_CHEST;
	msg.m_lParam1	= lTargetID;	
	msg.m_lParam2	= iClientDisplay;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_RESETTIME_DROPITEM_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_RESET_TIME_DROPITEM_NAME;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= 0;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_DETIME_DROPITEM_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_RESET_TIME_DROPITEM_NAME;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= 1;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SET_DROPITEM_NAME_TIME(long lTargetID, long Owner)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SET_DROPITEM_NAME_TIME;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= Owner;
	msg.m_lParam3	= 1;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_MONSTER_BLOOD(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SHOW_MONSTER_BLOOD;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_HIDE_MONSTER_BLOOD(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_HIDE_MONSTER_BLOOD;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_UPDATE_MONSTER_BLOOD(long lTargetID, int iParam)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_UPDATE_MONSTER_BLOOD;
	msg.m_lParam1	= lTargetID;	
	msg.m_lParam2	= iParam;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_NPC_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_SHOW_MONSTER_NAME;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_HIDE_NPC_NAME(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MSG_UE_HIDE_MONSTER_NAME;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SHOW_TASK(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_SHOW_TASK;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_HIDE_TASK(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_HIDE_TASK;
	msg.m_lParam1	= lTargetID;	
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_FINISH_TASK(long lTargetID, long index)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_FINISH_TASK;
	msg.m_lParam1	= lTargetID;	
	msg.m_lParam2	= index;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_FAIL_TASK(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_FAIL_TASK;
	msg.m_lParam1	= lTargetID;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_VIP_LEVELUP(long lTargetID, long lVipLevel)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_VIP_LEVELUP;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= lVipLevel;
	return msg;
}

//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_VIP_NEW_LEVELUP(long lTargetID, long lVipLevel)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_VIP_NEW_LEVELUP;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= lVipLevel;
	return msg;
}

inline MG_MessageBase CreateMsg_UE_COMMANDER_LEVELUP(long lTargetID, long lLevel)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_UE_COMMANDER_LEVELUP;
	msg.m_lParam1	= lTargetID;
	msg.m_llParam	= lLevel;
	return msg;
}

//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_TASKCHAPTER_OPEN(long lTargetID, long lChapter)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_TASKCHAPTER_OPEN;
	msg.m_lParam1	= lTargetID;
	msg.m_lParam2	= lChapter;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_NAME_FRAME_STATE_NORMAL(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MSG_UE_CHANGE_NAME_FRAME_STATE;
	msg.m_lParam1 = lTargetID;
	msg.m_lParam2 = 1;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_NAME_FRAME_STATE_HOVER(long lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MSG_UE_CHANGE_NAME_FRAME_STATE;
	msg.m_lParam1 = lTargetID;
	msg.m_lParam2 = 2;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_SET_INTERACTIVE_BUTTON_VISIBLE(long lTargetID, bool bVisible)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MSG_UE_SET_INTERACTIVE_BUTTON_VISIBLE;
	msg.m_lParam1 = lTargetID;
	msg.m_lParam2 = bVisible;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_PLACE_HOLDER_NAME_REDRAW()
{
	MG_MessageBase msg;
	msg.m_eMsgType = MSG_UE_PLACE_HOLDER_NAME_REDRAW;
	return msg;
}
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UE_NAME_REDRAW(object_id_type lTargetID)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MSG_UE_NAME_REDRAW;
	msg.m_lParam1 = lTargetID;
	return msg;
};
//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_UI_UPDATE(long ChannelID)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_UI_UPDATE;
	msg.m_uiParam1 = ChannelID;

	return msg;
}

inline MG_MessageBase CreateMsg_UE_Transcription_Actor_Show_NumberIcon(long lNPCID)
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_UE_SHOW_TRANSCRIPTION_ACTOR_NUMBER_ICON;
	msg.m_lParam1	= lNPCID;
	return msg;
}

//inline MG_MessageBase CreateMsg_UE_LOST_JEWELLERY_ROUND(long lAreaIndex, long lRoundNum)
//{
//    MG_MessageBase msg;
//    msg.m_eMsgType	= MGT_UE_LOST_JEWELLERY_ROUND;
//    msg.m_lParam1	= lAreaIndex;   // 0：峨嵋派；1：金钱帮；2：圣水宫；3：铁掌帮；4：真武观
//    msg.m_lParam2   = lRoundNum;    // 0～3：第一～最终回合
//    return msg;
//}
//inline MG_MessageBase CreateMsg_UE_LOST_JEWELLERY_STATE(long lStateIndex)
//{
//    MG_MessageBase msg;
//    msg.m_eMsgType	= MGT_UE_LOST_JEWELLERY_STATE;
//    msg.m_lParam1	= lStateIndex;  // 0：胜利；1：开始；2：失败
//    return msg;
//}

inline MG_MessageBase CreateMsg_Windows_Lost_Focus( bool bActive )
{
	MG_MessageBase msg;
	msg.m_eMsgType	= MGT_WINDOWS_LOST_FOCUS;
	msg.m_lParam1	= bActive ? 1 : 0;
	return msg;
}

inline MG_MessageBase CreateMsg_UICutScenes_Condition(int iType, long lTaskID, int iState, float fShowTime = 10.0f)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_UICUTSCENES_CONDITION;
	msg.m_lParam1 = iType;
	msg.m_lParam2 = lTaskID;
	msg.m_lParam3 = iState;
	msg.m_fParam4 = fShowTime;
	return msg;
}

inline MG_MessageBase CreateMsg_NPC_Born_Begin(object_id_type _rtid, int lCharType)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_NPC_BORN_BEGIN;
	msg.m_lParam1 = _rtid;
	msg.m_lParam2 = lCharType;
	return msg;
}

inline MG_MessageBase CreateMsg_NPC_Born_End(object_id_type _rtid, int lCharType)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_NPC_BORN_END;
	msg.m_lParam1 = _rtid;
	msg.m_lParam2 = lCharType;
	return msg;
}

inline MG_MessageBase CreateMsg_NPC_Dead_Begin(object_id_type _rtid, int lCharType)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_NPC_DEAD_BEGIN;
	msg.m_lParam1 = _rtid;
	msg.m_lParam2 = lCharType;
	return msg;
}

//------------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_CombatRecord_Damage(object_id_type lSrcID, object_id_type lTargetID, int64_t iFinalDamage)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_COMBATRECORD_DAMAGE;
	msg.m_lParam1 = lSrcID;
	msg.m_lParam2 = lTargetID;
	msg.m_llParam = iFinalDamage;
	return msg;
}

inline MG_MessageBase CreateMsg_CombatRecord_Heal(object_id_type lSrcID, int64_t iFinalHeal)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_COMBATRECORD_HEAL;
	msg.m_lParam1 = lSrcID;
	msg.m_llParam = iFinalHeal;
	return msg;
}
//< yunying begin
//-----------------------------------------------------------------------------
inline MG_MessageBase CreateMsg_NEW_ROLE(const void * charInfo)	//< PS_CharViewInfo
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_YUNYING_NEW_ROLE;
	msg.m_pbyParam1	= (BYTE *)charInfo;
	return msg;
}
inline MG_MessageBase CreateMsg_PLAYER_LOGIN(const void * charInfo)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_YUNYING_PLAYER_LOGIN;
	msg.m_pbyParam1	= (BYTE *)charInfo;
	return msg;
}
inline MG_MessageBase CreateMsg_LOGIC_PLAYER_LEVELUP(int newLV, unique_id_type * puuid, const StringType&  role_name, int role_gender, int role_profession, int role_power)
{
	MG_MessageBase msg;
	msg.m_eMsgType = MGT_YUNYING_PLAYER_LEVELUP;
	msg.m_lParam1	= newLV;
	msg.m_pbyParam2	= (BYTE *)puuid;
	msg.m_pbyParam3 = (BYTE *)role_name.c_str();
	msg.m_lParam4	= role_gender;
	msg.m_lParam5	= role_profession;
	msg.m_lParam6	= role_power;
	return msg;
}

inline MG_MessageBase CreateMsg_ADD_DUOWAN_GET_REPORTABLE_TASK(int sceneId, const char * sid, const char * rolename)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_YUNYING_ADD_DUOWAN_GET_REPORTABLE;
	msg.m_lParam1    = sceneId;
	msg.m_pbyParam2  = (BYTE*)sid;
	msg.m_pbyParam3  = (BYTE*)rolename;
	return msg;
}
inline MG_MessageBase CreateMsg_ADD_DUOWAN_REPORT_CHAT_TASK(const char * chatContent, const char * toUserName, const char * fromUserName, int bMultipy, int sceneId)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_YUNYING_ADD_DUOWAN_REPORT_CHAT;
	msg.m_pbyParam1  = (BYTE*)chatContent;
	msg.m_pbyParam2  = (BYTE*)toUserName;
	msg.m_pbyParam3  = (BYTE*)fromUserName;
	msg.m_lParam4    = bMultipy;
	msg.m_lParam5    = sceneId;
	return msg;
}
inline MG_MessageBase CreateMsg_ADD_DUOWAN_REPORT_LEVELUP_TASK(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power)
{
	MG_MessageBase msg;
	msg.m_eMsgType   = MGT_YUNYING_ADD_DUOWAN_REPORT_LEVELUP;
	msg.m_lParam1    = newLevel;
	msg.m_pbyParam2  = (BYTE*)&uuid;
	msg.m_pbyParam3  = (BYTE*)role_name;
	msg.m_lParam4    = role_gender;
	msg.m_lParam5    = role_profession;
	msg.m_lParam6    = role_power;
	return msg;
}
//< yunying end


#endif
