#include "ClientPCH.h"
#include "BM_Manager.h"
#include "IM_Manager.h"
#include "LC_ClientLogicManager.h"
#include "MG_MessageBase.h"
#include "UT_GlobalSettings.h"
#include "LC_ClientGamePlayerManager.h"
#include "FX_ClientFxManager.h"
#include "CF_CharType.h"


BM_Manager::BM_Manager(void)
{
}

BM_Manager::~BM_Manager(void)
{
}

void BM_Manager::Init()
{
	ReSet();
}

void BM_Manager::UnInit()
{
	ReSet();
}

void BM_Manager::ReSet()
{
	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
	mem::map<object_id_type, ModelInfo>::iterator itor = m_mActors.begin();
	for (;itor!=m_mActors.end();)
	{
		_releaseFxAll(itor->second);
		pkInteractManager->DestoryDeadInteractModel(itor->first);
		itor = m_mActors.erase(itor);
		continue;
	}
}

bool BM_Manager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
	END_MESSAGE_HANDLE

	return true;
}

void BM_Manager::_eventTimerTick(MG_MessageBase& rkMessage)
{
	Update(rkMessage.m_fParam1, rkMessage.m_fParam2);
}

void BM_Manager::_updateFragmentMove(object_id_type lLogicID, ModelInfo& Info, float fCurrentTime, float fDeltaTime)
{
	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
	SE_SceneManager* pkSceneMgr = SE_SceneManager::GetSingletonPtr();
	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

	NiNodePtr pkNode = Info.m_Scene->GetRootNode();
	NiAVObject* pkObject = pkNode->GetAt(0);
	NiNodePtr pkRoot = NiDynamicCast(NiNode, pkObject);
	int iChildCount = pkRoot->GetChildCount();
	NiAVObject* pkHeart = pkRoot->GetAt(0);
	if (!pkHeart)
	{
		return;
	}

	if (Info.m_iEndNum >= (iChildCount-1))
	{
		return;
	}

	NiPoint3 nHeartCenter = pkHeart->GetWorldBound().GetCenter();
	NiPoint3 g = NiPoint3(0, 0, g_settings.NPC_DEAD_BREAK_G);
	float m = g_settings.NPC_DEAD_BREAK_M;
	float minWeight = g_settings.NPC_DEAD_BREAK_MINWEIGHT;
	float impulse = g_settings.NPC_DEAD_BREAK_IMPULSE;
	float explore = g_settings.NPC_DEAD_BREAK_EXPLORE;
	float rotate = g_settings.NPC_DEAD_BREAK_ROTATE;
	float drag =g_settings.NPC_DEAD_BREAK_DRAG;
	int iMaxSkipTime = g_settings.NPC_DEAD_BREAK_SKIPTIMES;
	bool bNeedSpeed = false;
	if (Info.m_vSpeeds.empty())
	{
		bNeedSpeed = true;
		Info.m_vSpeeds.resize(iChildCount-1);
		Info.m_vEndList.resize(iChildCount-1);
		Info.m_vAxisDirection.resize(iChildCount-1);
		Info.m_vRotatingSpeed.resize(iChildCount-1);
		Info.m_vRotating.resize(iChildCount-1);
		Info.m_vSkipTimesList.resize(iChildCount-1);
		Info.m_vFxIdList.resize(iChildCount-1);
	}

	for (int i = 1; i < iChildCount; ++i)
	{
		NiAVObject* pkNiObject = pkRoot->GetAt(i);
		NiTriStrips * pkMesh = NiDynamicCast(NiTriStrips, pkNiObject);
		if (pkMesh == NULL)
			continue;
		/*NiFixedString str = pkNiObject->GetName();*/
		if (!bNeedSpeed && Info.m_vEndList[i-1])
		{
			continue;
		}

		NiPoint3 m_kAxisDirection;
		if (bNeedSpeed)
		{
			//线速度
			NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, pkNiObject);
			float icount = (float)pkGeometry->GetVertexCount();
			float weight = _getWeight(minWeight, icount) * m;
			NiPoint3 iImpulseSpeed = Info.m_ImpulseDir * (impulse / weight);
			NiPoint3 iExploerDir = pkNiObject->GetWorldBound().GetCenter() - nHeartCenter;
			iExploerDir.Unitize();
			NiPoint3 iExploreSpeed = iExploerDir * (explore / weight);
			NiPoint3 iSpeed = iImpulseSpeed + iExploreSpeed;
			Info.m_vSpeeds[i-1] = iSpeed;
			//旋转轴
			m_kAxisDirection = Info.m_ImpulseDir.Cross(iExploerDir);
			Info.m_vAxisDirection[i-1] = m_kAxisDirection;
			//角速度
			float m_fRotatingSpeed = rotate / weight;
			Info.m_vRotatingSpeed[i-1] = m_fRotatingSpeed;
			//特效
			int iFlyingFxId = rkFxManager.CreateFx("S_yanwu.gfx", true);
			Info.m_vFxIdList[i-1] = iFlyingFxId;
			rkFxManager.BindFxToDummyNode(iFlyingFxId, pkNiObject, true, false);
			rkFxManager.PlayFx(iFlyingFxId, fCurrentTime);
		}
		else
		{
			Info.m_vSpeeds[i-1] += g * fDeltaTime;
			m_kAxisDirection = Info.m_vAxisDirection[i-1];
			float t = fCurrentTime - Info.m_fBeginTime;
		}
		//旋转角度
		Info.m_vRotating[i-1] += Info.m_vRotatingSpeed[i-1] * fDeltaTime;

		NiMatrix3 kRotateMatrix;
		kRotateMatrix.MakeRotation(Info.m_vRotating[i-1], m_kAxisDirection); 
		pkNiObject->SetWorldRotate(kRotateMatrix);

		NiPoint3 WorldTranslate = pkNiObject->GetWorldTranslate();
		NiPoint3 nWillTranslate = WorldTranslate + Info.m_vSpeeds[i-1] * fDeltaTime;
		const SE_SceneTerrainLocalInfo& kTerrainLocalInfo = pkSceneMgr->GetWalkableInfo(nWillTranslate.x, nWillTranslate.y);
		float Height = kTerrainLocalInfo._fLocalHeight;
		NiPoint3 rkNormal = kTerrainLocalInfo._kLocalNormal;
		Info.m_vEndList[i-1] = false;
		if (nWillTranslate.z < Height)
		{
			nWillTranslate.z = Height;
			Info.m_vSkipTimesList[i-1]++;
			if (Info.m_vSkipTimesList[i-1] >= iMaxSkipTime)
			{
				rkFxManager.ReleaseFx(Info.m_vFxIdList[i-1], false);
				Info.m_iEndNum++;
				Info.m_vEndList[i-1] = true;
			}
			else
			{
				Info.m_vSpeeds[i-1] -= 2 * Info.m_vSpeeds[i-1].Dot(rkNormal) * rkNormal;
				Info.m_vSpeeds[i-1] -= (Info.m_vSpeeds[i-1] * rkNormal) * 0.5f * rkNormal;

				Info.m_vRotatingSpeed[i-1] *= 0.5f;
			}
		}

		pkNiObject->SetWorldTranslate(nWillTranslate);

		if (Info.m_iEndNum >= (iChildCount-1))
		{
			return;
		}
	}
}

void BM_Manager::_releaseFxAll(const ModelInfo& mMode)
{
	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();
	for (int i = 0; i < mMode.m_vFxIdList.size(); ++i)
	{
		rkFxManager.ReleaseFx(mMode.m_vFxIdList[i], false);
	}
	rkFxManager.ReleaseFx(mMode.m_iDeadFx, false);
}

float BM_Manager::_getWeight(float fMinWeight, float fWeight)
{
	float f = fWeight;
	
	if (fWeight < fMinWeight)
		f = fMinWeight;
	else if (fWeight > 3 * fMinWeight && fWeight < 10 * fMinWeight)
		f = 3 * fMinWeight;
	else if (fWeight > 30 * fMinWeight)
		f = 30 * fMinWeight;

	return f;
}

void BM_Manager::Update(float fCurrentTime, float fDeltaTime)
{
	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
	mem::map<object_id_type, ModelInfo>::iterator itor = m_mActors.begin();
	for (;itor!=m_mActors.end();)
	{
		if (itor->second.m_fBeginTime > 0)
		{
			if (fCurrentTime - itor->second.m_fBeginTime >= 3.0f)
			{
				_releaseFxAll(itor->second);
				pkInteractManager->DestoryDeadInteractModel(itor->first);
				itor = m_mActors.erase(itor);
				continue;
			}
			_updateFragmentMove(itor->first, itor->second, fCurrentTime, fDeltaTime);
		}
		else if (itor->second.m_fBeginTime <= 0 && itor->second.m_Scene->IsInitedWithLoaded())
		{
			itor->second.m_fBeginTime = fCurrentTime;
		}
		
		++itor;
	}
}

void BM_Manager::Begin(long lCharType, object_id_type lLogicID, const NiPoint3& kPos, const Utility::UT_Vec2Float& kDir, object_id_type lAttackerID, int lAttackSkillType)
{
	const CF_CharType::DataEntry* pkData = CF_CharType::GetSingleton().GetEntryPtr(lCharType);
	if (pkData->_sResExplodeName.empty())
	{
		return;
	}

	LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
	LC_PlayerBase* pkAttacker = pkPlayerManager->FindPlayer(lAttackerID);
	if (!pkAttacker)
	{
		return;
	}

	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
	pkInteractManager->DestroyInteractModel(lLogicID);

	ModelInfo pkInfo;

	NiMatrix3 kRot;
	//根据新的当前方向，计算新的当前旋转矩阵
	NiPoint3 kAxisZ = NiPoint3::UNIT_Z;
	NiPoint3 kAxisY = NiPoint3(-kDir.x,-kDir.y,0.0f);
	NiPoint3 AxisX = kAxisY.Cross(kAxisZ);
	AxisX.Unitize();
	kRot = NiMatrix3(AxisX,kAxisY,kAxisZ);

	pkInfo.m_Scene = (GD_ClientBreakModel*)pkInteractManager->CreateInteractDeadModel(pkData->_sResExplodeName, lLogicID, kPos, kRot);

	pkInfo.m_ImpulseDir = kPos - UT_EngineMath::ConvertVec3IntToNiPoint3(pkAttacker->GetVisibleLocation());
	float fDirLength = pkInfo.m_ImpulseDir.Unitize();
	if (fDirLength <= UT_MathBase::Epsilon)
	{
		pkInfo.m_ImpulseDir = NiPoint3(-kDir.x, -kDir.y, 0.0f);
		pkInfo.m_ImpulseDir.Unitize();
	}

	pkInfo.m_AttackSkillType = lAttackSkillType;

	FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
	int iFxID = pkFxManager->CreateFx("N_死亡.gfx", true);
	pkFxManager->BindFxToLocation(iFxID, kPos);
	pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	pkInfo.m_iDeadFx = iFxID;

	m_mActors[lLogicID] = pkInfo;
}
