/*/////////////////////////////////////////////////////////////////////////
作用：给物品头顶名字分配合适的屏幕位置
算法：根据默认的大小，将屏幕划分为若干个小格子，然后查找最近的空闲小格子
注意点：当屏幕分辨率发生变化时，需要重新
// chenjiangang [8/27/2013 PC024]
/////////////////////////////////////////////////////////////////////////*/


#ifndef _UE_NAME_ARRANGE_H
#define _UE_NAME_ARRANGE_H

#include <map>
#include "CR_UICommon.h"

class UE_NameArrange
{
public:
	UE_NameArrange();

	void InsertNameRect(long lTargetID, const GUISys::GUIRect& kNameArea);
	void Update(float fCurrentTime);
	void CleanUp();
	void DeleteNameRect(long lTargetID);
	bool GetNameRect(long lTargetID, GUISys::GUIRect& kNameRect);

	class MyGUIRect
	{
	public:
		MyGUIRect(long lTarget, const GUISys::GUIRect& rect)
			:m_kNameRect(rect)
			,m_lTargetID(lTarget)
			,m_bCross(false)
		{}
		bool m_bCross;
		long m_lTargetID;
		GUISys::GUIRect m_kNameRect;
		
	};
	typedef std::vector<GUISys::GUIRect> GUIRectVec;
	typedef std::vector<std::vector<MyGUIRect>> VecNameRectStage;
	typedef std::map<long, GUISys::GUIRect>	NameRectMap;

private:
	void _requestRedraw();

	void _findAvailablePosition(const GUISys::GUIRect& kNameRect, const GUIRectVec& kProcessedVec, GUISys::GUIPoint2& kRetPosition) const;
	void _stageDivision(VecNameRectStage& kRetStages);
	void _findCrossNameRectsInStage(int iStage, const VecNameRectStage& kNameRectStages, NameRectMap& kCrossNameRectMap);
	float _findLeftEdge();
	float _findRightEdge();
	void _updateCrossRectPosition(const NameRectMap& kCrossNameRectMap, std::vector<MyGUIRect>& kNameRectVec);
	void _addToProcessedVec(const NameRectMap& kCrossNameRectMap, GUIRectVec& kRetProcessedVec);
	void _addToProcessedVec(const GUISys::GUIRect& kNameRect, GUIRectVec& kRetProcessedVec);
	void _getDestRect(const GUIRectVec& kNameRectVec, GUISys::GUIRect& kRetNameRect) const;
private:
	bool					m_bNeedRedraw;
	NameRectMap				m_kNameRectMap;
};

#endif