#ifndef _LC_CLIENT_ITEM_H
#define _LC_CLIENT_ITEM_H

#include "LC_ItemBase.h"

class GD_ClientActorModel;

class LC_ClientItem : public GameLogic::LC_ItemBase
{
public:
	enum	ItemSuitPosition
	{
		ISP_HEAD = 0,		// ͷ����װ
		ISP_BODY = 1,		// ���廻װ
		ISP_LEG  = 2,		// �Ȳ���װ
		ISP_LEFT_HAND = 3,	// ���ֻ�����
		ISP_RIGHT_HAND= 4,	// ���ֻ�����
		ISP_BOTH_HAND = 5,	// ˫�ֻ�����
		ISP_EXTRA1	  = 6,	// �����װ���Ҽ�����ߵȣ�
		ISP_BOW		  = 7,	// ����
		ISP_RIDE_WEAPON = 8,// ��ս����
		ISP_WING = 9,		//����

		ISP_COUNT,
	};

public:
	LC_ClientItem();
	virtual ~LC_ClientItem();

	//��װ����
  bool OperationAvatarFunc(long lCharType, GD_ClientActorModel* pkActor, GameLogic::LC_BackPackEntry* pkItemEntry, float fCurrentTime);
	bool UnoperationAvatarFunc(long lCharType, GD_ClientActorModel* pkActor, GameLogic::LC_BackPackEntry* pkItemEntry, float fCurrentTime);

	void CalcSubordinateAppendixMapModify( int nType, int32_t nStage, LC_ActorAttributeMap* kAttrMap);

protected:

	//�����޸Ĺ���
	//virtual bool	_operationAttributeFunc(int iFuncID,GameLogic::LC_PlayerBase*pkUser,GameLogic::LC_BackPackEntry* pkItemEntry,float fCurrentTime);
	//virtual bool	_unoperationAttributeFunc(int iFuncID,GameLogic::LC_PlayerBase*pkUser,GameLogic::LC_BackPackEntry* pkItemEntry,float fCurrentTime);	
};

#endif