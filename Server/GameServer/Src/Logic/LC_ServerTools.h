#pragma once

#include "gameplatform_types.h"
#include "TS_TranBase.h"
#include "LC_CommonStatAsset.h"
#include "LC_ServerPlayer.h"
#include "IdDefine.h"
using namespace GameLogic;

void on_common_stat_op_successfully(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iKey, uint32_t opFlag, int32_t opPara);

class CommonStatOPCBArg : public TS_PostCommit
{
public:
	uint32_t	m_uiRslt;
	uint32_t	m_uiCharID;
	int32_t		m_iType;
	int32_t		m_iKey;
	LC_CommonStatConfigDataEntry* m_pkCSConfigCEntry;
	LC_CommonStatRewardDataEntry* m_pkCSRewardEntry;
	LC_CommonStatReplevyDataEntry* m_pkCSReplevyEntry;
	uint32_t	m_uiOPFlag;
	int32_t		m_iOPPara;
	float		m_fDiscountExpo;
	float		m_fRewardExpo;
	int         nLogCode;
	std::vector<int> nLogParams;

public:
	CommonStatOPCBArg(uint32_t uiCharID, int32_t iType, int32_t iKey, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, 
		LC_CommonStatReplevyDataEntry* pkCSReplevyEntry, uint32_t uiOPFlag, int32_t iOPPara);

	inline void SetDiscountExpo(float fExpo) { m_fDiscountExpo = fExpo;}
	inline void SetRewardExpo(float fExpo) { m_fRewardExpo = fExpo;}

	virtual void operator()(void);
};

int32_t gain_CommonStatDraw(LC_ServerPlayer* pkPlayer, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, int32_t iType, float fRewardExpo = 1.0);
int32_t gain_CommonStatDraw(mem::list<LC_ServerPlayer*>& rPlayerList, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, int32_t iType);
int32_t gain_CommonStatDraw(const unique_id_list& rCharList, LC_CommonStatConfigDataEntry* pkCSConfigCEntry, LC_CommonStatRewardDataEntry* pkCSRewardEntry, int32_t iType);