#include "ClientPCH.h"
#include "GD_DynamicCollisionManager.h"
#include "IM_Manager.h"
#include "MG_MessageBase.h"
#include "PF_ClientPathManager.h"

#include "LC_ClientGamePlayer.h"
#include "LC_ClientLogicManager.h"

GD_DynamicCollisionManager::GD_DynamicCollisionManager(void)
{
}

GD_DynamicCollisionManager::~GD_DynamicCollisionManager(void)
{
}

void GD_DynamicCollisionManager::OnMapReloaded(bool bMapChanged)
{
    m_kDCEnablingNpcIDs.clear();
    m_kDCRegisterdNpcIDs.clear();
    m_kDCEnabledNpcIDs.clear();
}

bool GD_DynamicCollisionManager::EnableDynamicCollision(long lID)
{
    if (m_kDCRegisterdNpcIDs.find(lID) == m_kDCRegisterdNpcIDs.end()	//���DisableDynamicCollision��ʱ������bUnregisterDCShape�������m_kDCRegisterdNpcIDs��ɾ����npc
        && m_kDCEnablingNpcIDs.find(lID) == m_kDCEnablingNpcIDs.end())	//�´�����ٸ���npc����EnableDynamicCollision���򲻻����ɹ�
    {
        m_kDCEnablingNpcIDs.insert(lID);
		return true;
    }
	return false;
}

void GD_DynamicCollisionManager::DisableDynamicCollision(long lID, bool bUnregisterDCShape /*= false*/)
{
    m_kDCEnablingNpcIDs.erase(lID);

    if (bUnregisterDCShape)
    {
        DynamicCollisionEnabledNpcIDSet::iterator itr = m_kDCRegisterdNpcIDs.find(lID);
        if (itr != m_kDCRegisterdNpcIDs.end())
        {
            PF_ClientPathManager::GetSingleton().UnregisterDynamicCollisionShape(lID);

            m_kDCRegisterdNpcIDs.erase(lID);
            m_kDCEnabledNpcIDs.erase(lID);
        }
    }
    else
    {
        DynamicCollisionEnabledNpcIDSet::iterator itr = m_kDCEnabledNpcIDs.find(lID);
        if (itr != m_kDCEnabledNpcIDs.end())
        {
            PF_ClientPathManager::GetSingleton().EnableDynamicCollisionShape(lID, false);

            m_kDCEnabledNpcIDs.erase(lID);
        }
    }
}

bool GD_DynamicCollisionManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
        MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick);
	END_MESSAGE_HANDLE

	return true;
}

void GD_DynamicCollisionManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
    for (DynamicCollisionEnabledNpcIDSet::iterator itr = m_kDCEnablingNpcIDs.begin(); itr != m_kDCEnablingNpcIDs.end(); )
    {
        long lID = (*itr);
        if (m_kDCRegisterdNpcIDs.find(lID) == m_kDCRegisterdNpcIDs.end())
        {
            GD_ClientActorModel* pkNpcModel = IM_Manager::GetSingleton().GetActor(lID);
            if (pkNpcModel != NULL)
            {
                NiPoint3 kCenterPoint;
                NiPoint2 akVertices[4];
                if (pkNpcModel->GetDynamicCollisionShape(kCenterPoint, akVertices[0], akVertices[1], akVertices[2], akVertices[3]))
                {
                    if (PF_ClientPathManager::GetSingleton().RegisterDynamicCollisionShape(lID, kCenterPoint, akVertices, true))
                    {
						// ��δ���Ϊ�˴�����ҳ�����Ѱ·������м���ֶ�̬�赲����Чǽ�����ܻ������Ҵ�Խ��̬�赲�����ﲻ�õ��������
						// ��������������Χ���ֶ�̬�赲�������������Ѱ·����·������ֹ�������
						LC_ClientLogicManager* pkLogicManager = (LC_ClientLogicManager*)CLIENT_GET_LOGIC_MANAGER();
						LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
						if (pkGamePlayer && pkGamePlayer->IsPathFinding())
						{
							UT_Vec3Int tt = pkGamePlayer->GetTargetLocation();
							pkGamePlayer->StopMove();
							pkGamePlayer->PathToPoint(tt);
						}
						//
                        itr = m_kDCEnablingNpcIDs.erase(itr);

                        m_kDCRegisterdNpcIDs.insert(lID);
                        m_kDCEnabledNpcIDs.insert(lID);
                    }
                    else
                    {
                        ++itr;
                    }
                }
                else
                {
                    ++itr;
                }
            }
            else
            {
                ++itr;
            }
        }
        else
        {
            itr = m_kDCEnablingNpcIDs.erase(itr);

            if (m_kDCEnabledNpcIDs.find(lID) == m_kDCEnabledNpcIDs.end())
            {
                PF_ClientPathManager::GetSingleton().EnableDynamicCollisionShape(lID, true);
                m_kDCEnabledNpcIDs.insert(lID);
            }
        }
    }
}
