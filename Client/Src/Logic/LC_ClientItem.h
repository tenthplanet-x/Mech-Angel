#ifndef _LC_CLIENT_ITEM_H
#define _LC_CLIENT_ITEM_H

#include "LC_ItemBase.h"

class GD_ClientActorModel;

class LC_ClientItem : public GameLogic::LC_ItemBase
{
public:
	enum	ItemSuitPosition
	{
		ISP_HEAD = 0,		// 头部换装
		ISP_BODY = 1,		// 身体换装
		ISP_LEG  = 2,		// 腿部换装
		ISP_LEFT_HAND = 3,	// 左手换武器
		ISP_RIGHT_HAND= 4,	// 右手换武器
		ISP_BOTH_HAND = 5,	// 双手换武器
		ISP_EXTRA1	  = 6,	// 额外服装（挂件、马具等）
		ISP_BOW		  = 7,	// 弩炮
		ISP_RIDE_WEAPON = 8,// 骑战武器
		ISP_WING = 9,		//神翼

		ISP_COUNT,
	};

public:
	LC_ClientItem();
	virtual ~LC_ClientItem();

	//换装功能
  bool OperationAvatarFunc(long lCharType, GD_ClientActorModel* pkActor, GameLogic::LC_BackPackEntry* pkItemEntry, float fCurrentTime);
	bool UnoperationAvatarFunc(long lCharType, GD_ClientActorModel* pkActor, GameLogic::LC_BackPackEntry* pkItemEntry, float fCurrentTime);

	void CalcSubordinateAppendixMapModify( int nType, int32_t nStage, LC_ActorAttributeMap* kAttrMap);

protected:

	//属性修改功能
	//virtual bool	_operationAttributeFunc(int iFuncID,GameLogic::LC_PlayerBase*pkUser,GameLogic::LC_BackPackEntry* pkItemEntry,float fCurrentTime);
	//virtual bool	_unoperationAttributeFunc(int iFuncID,GameLogic::LC_PlayerBase*pkUser,GameLogic::LC_BackPackEntry* pkItemEntry,float fCurrentTime);	
};

#endif