#include "LC_ServerNPCOwnerList.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPCManager.h"

using namespace GameLogic;
//-----------------------------------------------------------------
LC_ServerNPCOwnerList::LC_ServerNPCOwnerList()
: LC_ActorOwnerListBase()
{

}
//-----------------------------------------------------------------
LC_ServerNPCOwnerList::~LC_ServerNPCOwnerList()
{

}

void LC_ServerNPCOwnerList::AddOwnerList(const object_id_set& ids)
{
	m_kOwnerIDList.insert(m_kOwnerIDList.end(), ids.begin(), ids.end());
}

void LC_ServerNPCOwnerList::AddDamageID(object_id_type id)
{
	m_kDamageList.insert(id);
}

void LC_ServerNPCOwnerList::GetDamageList(object_id_set& id)
{
	for(object_id_set::iterator it=m_kDamageList.begin(); it!=m_kDamageList.end(); ++it)
	{
		id.insert(*it);
	}
}

void LC_ServerNPCOwnerList::ResolveTotalLevel(GameLogic::LC_MapBase* pkMap)
{
	m_iTotalLevel = 0;
	if (NULL == pkMap)
	{
		return;
	}
	object_id_vector::const_iterator cit = m_kOwnerIDList.begin();	
	for (; cit!=m_kOwnerIDList.end(); ++cit)
	{
		LC_ActorBase* pkActor = (LC_ActorBase*)pkMap->FindObject(*cit);
		if (pkActor)
		{
			m_iTotalLevel += pkActor->GetLevel();
		}
	}
}
//-----------------------------------------------------------------
void LC_ServerNPCOwnerList::AddOwnerID(object_id_type lOwnerID, bool bConsiderGroupMember /*= true*/)
{
	if(!IS_PLAYER_ID(lOwnerID) && !IS_NPC_ID(lOwnerID))
		return;

	//ֻ��¼��һ�����õĹ�������һ����
	if(!m_kOwnerIDList.empty())
		return;

	if(IS_NPC_ID(lOwnerID))
	{
		LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();

		LC_NPCBase* pkNPC = pkNPCManager->FindNPC(lOwnerID);
		if(NULL == pkNPC)
			return;

		if(!IS_PLAYER_ID(pkNPC->GetFollowedTargetID()))
			return;

		AddOwnerID(pkNPC->GetFollowedTargetID(), bConsiderGroupMember);
	
		return;
	}

	if(!IS_PLAYER_ID(lOwnerID))
		return;

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if(!pkServerPlayerManager)
		return;

	LC_PlayerBase* pkOwner = pkServerPlayerManager->FindPlayer(lOwnerID);
	if(!pkOwner)
		return;
	
	//�ȼ�¼��һ������
	_addOwnerID(lOwnerID);

	if (bConsiderGroupMember)
	{
		//��������
		LC_PlayerGroupBase* pkPlayerGroup = pkOwner->GetPlayerGroup();
		if(NULL != pkPlayerGroup)
		{
			//������
			for (int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++i)
			{
				LC_PlayerGroupEntry* pkMember = pkPlayerGroup->GetGroupEntryInMemberList(i);
				if (pkMember)
				{
					if (pkMember->GetValid() && pkMember->GetPlayerID() != lOwnerID)
						_addOwnerID(pkMember->GetPlayerID());
				}
			}
			//���÷���ԭ��
			SetRollItemGrade(pkPlayerGroup->GetItemGradeRealVal());
		}
	}
	SetAddOwnerConsiderGroupMember(bConsiderGroupMember);
}
//-----------------------------------------------------------------
void LC_ServerNPCOwnerList::Update(float fCurrentTime,int lNPCMapLogicID,const Utility::UT_Vec3Int& rkNPCLocation,bool bBeforDead,bool bCheckDistance)
{
	if(m_kOwnerIDList.empty())
		return;
	
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if(!pkPlayerManager)
		return;

	object_id_type lOwnerID = 0;
	bool bValidOwner;
	

	if(!bBeforDead)
	{
		if(m_bAvoidUpdateFlag)		//��������������¹����������ڣ�
			return;

		bool bIsAllOwnerInvalid = true;
		
		int iOwnerCount = (int)m_kOwnerIDList.size();
		
		for(int iOwnerIndex = 0; iOwnerIndex < iOwnerCount; iOwnerIndex++)
		{
			lOwnerID = m_kOwnerIDList[iOwnerIndex];

			if(!bCheckDistance) //���������ֻ�в��ڸ����ڵ���ұ�����
				bValidOwner = _isDistanceAndStateValidToOwn(pkPlayerManager,lOwnerID,lNPCMapLogicID,rkNPCLocation,true,-1);
			else
				bValidOwner = _isDistanceAndStateValidToOwn(pkPlayerManager,lOwnerID,lNPCMapLogicID,rkNPCLocation,true,CHEST_MAX_DIST_TO_ROLL);

			if(bValidOwner)
			{
				bIsAllOwnerInvalid = false;
				break;
			}
		}
		
		//ֻ��ȫ������Ч�ˣ����������
		if(bIsAllOwnerInvalid)
			m_kOwnerIDList.clear();
	}
	else
	{
		//����ʱ����Ч��������������
		object_id_vector_iter IterBe = m_kOwnerIDList.begin();
  
		for( ; IterBe != m_kOwnerIDList.end(); )
		{
			lOwnerID = *IterBe;
			
			if(!bCheckDistance) //���������ֻ�в��ڸ����ڵ���ұ�����
				bValidOwner = _isDistanceAndStateValidToOwn(pkPlayerManager,lOwnerID,lNPCMapLogicID,rkNPCLocation,false,-1);
			else
				bValidOwner = _isDistanceAndStateValidToOwn(pkPlayerManager,lOwnerID,lNPCMapLogicID,rkNPCLocation,false,CHEST_MAX_DIST_TO_ROLL);

			if(bValidOwner)
				++IterBe;
			else
				IterBe = m_kOwnerIDList.erase(IterBe);
		}
	}
}
//-----------------------------------------------------------------
bool LC_ServerNPCOwnerList::_isDistanceAndStateValidToOwn(LC_ServerPlayerManager* pkPlayerManager,object_id_type lPlayerID,int lNPCMapLogicID,const Utility::UT_Vec3Int& rkNPCLocation,bool bCheckDead,float fCheckDistance)
{
	if(!IS_PLAYER_ID(lPlayerID))
		return false;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkPlayerManager->FindPlayer(lPlayerID));
	
	if(!pkPlayer)
		return false;

	if(pkPlayer->GetMapLogicID() != lNPCMapLogicID)
		return false;
	
	if(fCheckDistance > 0.0f)
	{
		if(UT_MathBase::LineLengthXYInt(rkNPCLocation,pkPlayer->GetCurrentLocation()) >= fCheckDistance)
			return false;
	}

	return true;
}