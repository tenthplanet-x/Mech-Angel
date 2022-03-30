#ifndef _LC_SREVER_PLACEHOLDER_H
#define _LC_SREVER_PLACEHOLDER_H

#include "LC_PlaceHolderBase.h"
#include "MG_PlaceHolderProtocol.h"


class LC_ServerPlayer;
class LC_ServerPlaceHolder : public GameLogic::LC_PlaceHolderBase
{
public:
	LC_ServerPlaceHolder(int protected_time = 60);
	virtual ~LC_ServerPlaceHolder();

	virtual bool	Empty();

	virtual void	Update( float fCurrentTime,float fDeltaTime );

	void			RequestPickupItem(LC_ServerPlayer* pkPlayer , Protocol::MG_RLT_PickUpPlaceHolder& msg, bool checkCD=true);
	void			PickupGold(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg);
	void			PickupEffect(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg);
	//void			PickupSP(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg);
	void			PickupBossJP(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg);
	void			PickupUnbindyuanbao(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg);
	void			PickupRuneCream(LC_ServerPlayer* pkPlayer  , Protocol::MG_RLT_PickUpPlaceHolder& msg);
	void			SetMapID(int32_t map_id){m_MapID = map_id;}
	int32_t			GetMapID(void){return m_MapID;}
	void			SetNPCID(int32_t nNpcId){m_nNpcId = nNpcId;}
	int32_t			GetNPCID(void){return m_nNpcId;}

	inline void		SetProtectTime(int fTime){m_protected_time = fTime;}
	inline int32_t	GetProtectTime(void){return m_protected_time;}

protected:

	enum ePickResult{
		PR_PICK_OK = 0,
		PR_BACKPACK_FULL = 1,
		PR_PICK_FAIL = 2,

	}PICKRESULT;
	
	bool			_pickUpItem(LC_ServerPlayer* pkPlayer , Protocol::MG_RLT_PickUpPlaceHolder& msg, bool checkCD);

	int				_performAddToPackage(object_id_type lPlayer ,Protocol::MG_RLT_PickUpPlaceHolder& msg, bool checkCD);
	void			_removeResolvedItems();

	void			_resetOwners(object_id_type lPlayer);

	void			_updateItemsTimeout(float fTime);
		
	LC_ServerPlayer* _findPlayer(object_id_type lPlayerID);

	void			_sendMsgToPlayer(object_id_type lPlayerID,int32_t iMsgType,Protocol::MG_BODY* pkMsgBody);

	bool			_isDistanceAndStateValidToHold(object_id_type lPlayerID);
	bool			_isDistanceAndStateValidToRoll(object_id_type lPlayerID,int lWeaponType);
	int32_t			m_MapID;
	int32_t			m_protected_time;
	int32_t         m_nNpcId;
};

typedef mem::vector<LC_ServerPlaceHolder *> PlaceHolderListType;

#endif