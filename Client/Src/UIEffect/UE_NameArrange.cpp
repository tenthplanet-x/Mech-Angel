#include "ClientPCH.h"
#include <math.h>
#include "UE_NameArrange.h"
#include "NiMain.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "UE_UiEffectManager.h"

bool CompareY (const UE_NameArrange::MyGUIRect& lhs, const UE_NameArrange::MyGUIRect& rhs)
{
	return lhs.m_kNameRect.m_bottom > rhs.m_kNameRect.m_bottom;
}

class CompareY_GUIRect
{
public:
	bool operator() (const GUISys::GUIRect& lhs, const GUISys::GUIRect& rhs)
	{
		return lhs.m_bottom > rhs.m_bottom;
	}
};

UE_NameArrange::UE_NameArrange()
: m_bNeedRedraw(false)
{
}

bool UE_NameArrange::GetNameRect(long lTargetID, GUISys::GUIRect& kNameRect)
{
	NameRectMap::iterator it = m_kNameRectMap.find(lTargetID);
	if(it != m_kNameRectMap.end())
	{
		kNameRect = (*it).second;
		return true;
	}
	return false;
}

void UE_NameArrange::_requestRedraw(void)
{
	m_bNeedRedraw = true;
}

void UE_NameArrange::InsertNameRect(long lTargetID, const GUISys::GUIRect& kNameArea)
{
	NameRectMap::iterator it = m_kNameRectMap.find(lTargetID);
	if(it != m_kNameRectMap.end())
		m_kNameRectMap.erase(it);
	if(!m_bNeedRedraw)
	{
		NameRectMap::iterator itBegin = m_kNameRectMap.begin();
		NameRectMap::iterator itEnd = m_kNameRectMap.end();
		for (; itBegin != itEnd; ++itBegin)
		{
			if ((*itBegin).first == lTargetID)
				continue;
			if (GUISys::GUIRect::IsRectIntersect(kNameArea, (*itBegin).second) != GUIRECT_NO_INTERSECT)
			{
				_requestRedraw();
				break;
			}
		}
	}
	m_kNameRectMap[lTargetID] = kNameArea;
}

void UE_NameArrange::DeleteNameRect(long lTargetID)
{
	if(m_kNameRectMap.empty())
		return;
	m_kNameRectMap.erase(lTargetID);
}

void UE_NameArrange::CleanUp(void)
{
	m_kNameRectMap.clear();
}

void UE_NameArrange::Update(float fCurrentTime)
{
	if(!m_bNeedRedraw)
		return;

	//UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
	//GUISys::GUIRect kNameRect;
	//NameRectMap::iterator it = m_kNameRectMap.begin();
	//for (; it != m_kNameRectMap.end(); ++it)
	//{
	//	if(pkUIEffectMgr->GetLocalPlaceHolderNameRect((*it).first, kNameRect))
	//	{
	//		(*it).second = kNameRect;
	//	}
	//}
	//
	//std::vector<std::vector<MyGUIRect>> kVecStages;
	//_stageDivision(kVecStages);
	//NameRectMap kCrossNameRectMap;
	//GUIRectVec kProcessedRectVec;
	//GUISys::GUIPoint2 kPosition;
	//
	//for (int i = 0; i < kVecStages.size(); i++)
	//{
	//	kProcessedRectVec.clear();
	//	_findCrossNameRectsInStage(i - 1, kVecStages, kCrossNameRectMap);
	//	_updateCrossRectPosition(kCrossNameRectMap, kVecStages[i]);
	//	_addToProcessedVec(kCrossNameRectMap, kProcessedRectVec);
	//	for (int j = 0; j < kVecStages[i].size(); j++)
	//	{
	//		if(kCrossNameRectMap.find(kVecStages[i][j].m_lTargetID) != kCrossNameRectMap.end())
	//			continue;
	//		std::sort(kProcessedRectVec.begin(), kProcessedRectVec.end(), CompareY_GUIRect());
	//		_findAvailablePosition(kVecStages[i][j].m_kNameRect, kProcessedRectVec, kPosition);
	//		kVecStages[i][j].m_kNameRect.SetPosition(kPosition);
	//		_addToProcessedVec(kVecStages[i][j].m_kNameRect, kProcessedRectVec);
	//	}
	//}
	//
	//m_kNameRectMap.clear();
	//for (int i = 0; i < kVecStages.size(); i++)
	//{
	//	for(int j = 0; j < kVecStages[i].size(); j++)
	//	{
	//		m_kNameRectMap[kVecStages[i][j].m_lTargetID] = kVecStages[i][j].m_kNameRect;
	//		pkUIEffectMgr->SetNameRect(kVecStages[i][j].m_lTargetID, kVecStages[i][j].m_kNameRect);
	//	}
	//}
	//
	//MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UE_PLACE_HOLDER_NAME_REDRAW());

	//m_bNeedRedraw = false;
}

void UE_NameArrange::_findAvailablePosition(const GUISys::GUIRect& kNameRect, const GUIRectVec& kProcessedVec, GUISys::GUIPoint2& kRetPosition) const
{
	GUISys::GUIRect kRect = kNameRect;
	_getDestRect(kProcessedVec, kRect);

	kRetPosition.x = kRect.m_left;
	kRetPosition.y = kRect.m_top;
}

void UE_NameArrange::_getDestRect(const GUIRectVec& kNameRectVec, GUISys::GUIRect& kRetNameRect) const
{
	int iSize = kNameRectVec.size();
	GUISys::GUIRect& kRect = kRetNameRect;
	for (int i = 0; i < iSize; i++)
	{
		if(GUISys::GUIRect::IsRectIntersect(kRect, kNameRectVec[i]) != GUIRECT_NO_INTERSECT)
		{
			kRect.Offset(0.0f, (float)(kNameRectVec[i].m_top - kRect.m_bottom - 1.0f));
		}
	}
}

#pragma optimize("", off)

float UE_NameArrange::_findLeftEdge()
{
	float fResult = (float)INT_MAX;
	NameRectMap::iterator it = m_kNameRectMap.begin();
	for (; it != m_kNameRectMap.end(); ++it)
	{
		float left = (*it).second.m_left;
		if(left < fResult)
			fResult = left;
	}
	return fResult;
}

float UE_NameArrange::_findRightEdge()
{
	float fResult = float(-INT_MAX);
	NameRectMap::iterator it = m_kNameRectMap.begin();
	for (; it != m_kNameRectMap.end(); ++it)
	{
		float right = (*it).second.m_right;
		if(right > fResult)
			fResult = right;
	}
	return fResult;
}

void UE_NameArrange::_stageDivision(std::vector<std::vector<MyGUIRect>>& kRetStages)
{
	kRetStages.clear();
	float fLeftEdge = _findLeftEdge();
	float fRightEdge = _findRightEdge();
	if(fLeftEdge >= fRightEdge)
		return;

	float fInterval = 100.0f;	//头顶名字最大宽度
	int iSize = (int)ceil((fRightEdge - fLeftEdge) / fInterval);
	kRetStages.resize(iSize);

	NameRectMap::iterator it = m_kNameRectMap.begin();
	for (; it != m_kNameRectMap.end(); ++it)
	{
		float l = (*it).second.m_left;
		float r = (*it).second.m_right;
		int iStart = (int)((l - fLeftEdge) / fInterval);
		int iEnd = (int)((r - fLeftEdge) / fInterval);

		iStart = UT_MathBase::ClampInt(iStart, 0, kRetStages.size() - 1);
		iEnd   = UT_MathBase::ClampInt(iEnd  , 0, kRetStages.size() - 1);

		MyGUIRect kMyGUIRect = MyGUIRect((*it).first, (*it).second);
		kMyGUIRect.m_bCross = (iStart != iEnd);
		kRetStages[iStart].push_back(kMyGUIRect);
		if (kMyGUIRect.m_bCross)
		{
			kMyGUIRect.m_bCross = false;
			kRetStages[iEnd].push_back(kMyGUIRect);
		}
	}
	for(int i = 0; i < iSize; i++)
	{
		std::sort(kRetStages[i].begin(), kRetStages[i].end(), CompareY);
	}
}

#pragma optimize("", on)


void UE_NameArrange::_findCrossNameRectsInStage(int iStage, const VecNameRectStage& kNameRectStages, NameRectMap& kCrossNameRectMap)
{
	kCrossNameRectMap.clear();
	if(iStage < 0 || iStage >= kNameRectStages.size())
		return;
	const std::vector<MyGUIRect>& kNameRectVet = kNameRectStages[iStage];
	for (int i = 0 ; i < kNameRectVet.size(); i++)
	{
		if (kNameRectVet[i].m_bCross)
		{
			kCrossNameRectMap[kNameRectVet[i].m_lTargetID] = kNameRectVet[i].m_kNameRect;
		}
	}
}

void UE_NameArrange::_updateCrossRectPosition(const NameRectMap& kCrossNameRectMap, std::vector<MyGUIRect>& kNameRectVec)
{
	if(kCrossNameRectMap.empty())
		return;
	NameRectMap::const_iterator it;
	for(int i = 0; i < kNameRectVec.size(); i++)
	{
		long lTargetID = kNameRectVec[i].m_lTargetID;
		it = kCrossNameRectMap.find(lTargetID);
		if(it != kCrossNameRectMap.end())
		{
			kNameRectVec[i].m_kNameRect = (*it).second;
		}
	}
}

void UE_NameArrange::_addToProcessedVec(const NameRectMap& kCrossNameRectMap, GUIRectVec& kRetProcessedVec)
{
	NameRectMap::const_iterator it = kCrossNameRectMap.begin();
	for (; it != kCrossNameRectMap.end(); ++it)
	{
		kRetProcessedVec.push_back((*it).second);
	}
}
void UE_NameArrange::_addToProcessedVec(const GUISys::GUIRect& kNameRect, GUIRectVec& kRetProcessedVec)
{
	kRetProcessedVec.push_back(kNameRect);
}