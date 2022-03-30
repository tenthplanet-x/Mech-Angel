#include "BuddyAttrManager.h"
#include "CF_Buddy.h"
#include "CF_BuddyUpgrade.h"
#include "CF_LevelUp.h"
#include "SK_Define.h"
#include "UT_Log.h"
#include "UT_ServerHelper.h"
#include "GlobalSettings.h"
#include "BuddyHelper.h"
#include "CF_BuddyTransform.h"
#include "BuddyInstance.h"
#include "CF_SkillSpecialAttrib.h"
#include "LC_ServerPlayer.h"
#include "LC_Define.h"
BuddyAttrMgr::BuddyAttrMgr(): m_pConfig(NULL),m_configId(0)
{
	Clear();
}
BuddyAttrMgr::~BuddyAttrMgr()
{

}
void BuddyAttrMgr::init(CSVFile::CF_Buddy::DataEntry* _pConfig)
{
	if(NULL == _pConfig)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "BuddyAttrMgr::init _pConfig is NULL \n");
		return;
	}
	
	m_pConfig = _pConfig;
	m_configId = m_pConfig->_cID;
}

void BuddyAttrMgr::Clear()
{
	m_configId = 0;
	m_pConfig = NULL;
	m_NewFinalAttributeMap.Clear();
	m_HeroAllAttributeMap.Clear();
	m_GetChainAllAttributeMap.Clear();
	m_LevelAttributeMap.Clear();
	m_RuneAttributeMap.Clear();
	m_EquipAttributeMap.Clear();
	m_SkinAttributeMap.Clear();
	memset(m_nBuddyCombatScore ,0, sizeof(m_nBuddyCombatScore));
}
void BuddyAttrMgr::RefreshAllAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy)
{
	if(NULL==pkPlayer || pBuddy==NULL)
	{
		return;
	}

	RefreshSkinAttrMap(pkPlayer,pBuddy);
	RefreshEquipAttrMap(pkPlayer,pBuddy);
	RefreshRuneAttrMap(pBuddy);
	RefreshLevelAttrMap(pkPlayer,pBuddy);
	RefreshHeroAllAttributeMap(pkPlayer,pBuddy);
}
void BuddyAttrMgr::RefreshNewFinalAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy, bool login, int32_t ReasonType)
{
	if(NULL == pkPlayer|| NULL == pBuddy)
	{
		return;
	}

	m_NewFinalAttributeMap.Clear();

	mem::map<int, int>* pkSuitInfo = pkPlayer->GetControllerMoldingSpiritSuitInfo();
	if(NULL == pkSuitInfo)
	{
		return;
	}

	mem::map<int, int> controllerSuitAttrPercent;
	LC_ActorAttributeMap tempMap;
	LC_Helper::GetCommanderSuitAttrInfoFromItemEffectFunc(pkSuitInfo, &tempMap, controllerSuitAttrPercent);
	//Ӣ������=��Ӣ�ۻ���+ָ�ӹ���������+ָ�ӹ����������(��Ҫ�ų��Լ����ṩ��������ԣ���ֹ������������)��*��1+���ǹ���Ѫ��ǿ%��*(1+���߼���%+Ƥ���ٷֱ�+ָ�ӹ���װ�İٷֱ� ))
	
	for(int i = 0; i < ATT_MAX_COUNT; i++)
	{
		//Ӣ�ۻ���
		m_NewFinalAttributeMap.AddAttributeMapByType(i,m_HeroAllAttributeMap);
		// ��Ӣ�����������
		m_NewFinalAttributeMap.AddAttributeMapByType(i,m_GetChainAllAttributeMap);
		// ָ�ӹ���������
		m_NewFinalAttributeMap.AddAttributeMapByType(i,*(pkPlayer->GetCommanderAttrMgr().GetCommanderAllAttributeMap()));
	}
	m_NewFinalAttributeMap.PrintAttrMap("m_NewFinalAttributeMap1111");
	// Ӣ�����������ٷֱ�
	LC_Helper::GetBuddyUpgradeRate(m_configId,pBuddy->GetStarLevel(),m_NewFinalAttributeMap);
	m_NewFinalAttributeMap.PrintAttrMap("m_NewFinalAttributeMap2222");
	// Ӣ��Ƥ�� �ٷֱ�
	for(int i=0; i<pBuddy->GetBuddyInstanceData().GetBuddyInstanceData_Base()->m_nActiveTransformRecord.size(); ++i)
	{
		CF_BuddyTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(pBuddy->GetBuddyInstanceData().GetBuddyInstanceData_Base()->m_nActiveTransformRecord[i]);
		 if(NULL == pkData)
			 continue;
		 for(int k=0; k<pkData->_kAttributeGlobal.size(); ++k)
		 {
			 if(pkData->_kAttributeGlobal[k].GetFlag() != 1)
				 continue;
			Utility::UT_SIMDataInfo data = pkData->_kAttributeGlobal[k];
			//����:��λid:����id:����ֵ(0Ϊ����ϵͳ��1Ϊ���壻��λid��Ӧ1��2��3��4������λ��2��4���ٷֱȣ�������ֵ������ƣ�����ã�
			//data._cID: 0: ����ϵͳ 1: ����
			//data.ParamA(): ���Բ�λID
			//data.ParamB(): ����ID
			//data.ParamC(): ����ֵ
			LC_Helper::AddValueToPartABCDbyType(data.ParamA(),data.ParamB(),data.ParamC(),m_NewFinalAttributeMap);	
		 }
	}
	m_NewFinalAttributeMap.PrintAttrMap("m_NewFinalAttributeMap3333");
	// ָ�ӹ���װ�ٷֱ�
	for(mem::map<int, int>::iterator itc = controllerSuitAttrPercent.begin();itc !=controllerSuitAttrPercent.end(); ++itc)
	{
		LC_Helper::AddValueToPartABCDbyType(4,itc->first,itc->second,m_NewFinalAttributeMap);
	}
	m_NewFinalAttributeMap.PrintAttrMap("m_NewFinalAttributeMap4444");
	
	// Ӣ�۱��߼��ܰٷֱ�
	Utility::UT_SIMDataList skillAttr;
	pBuddy->GetRuneSkillAttr(skillAttr);
	for(int i=0; i<skillAttr.size(); ++i)
	{
		if(skillAttr[i].GetFlag() != 1)
			continue;
		Utility::UT_SIMDataInfo& data = skillAttr[i];
		LC_Helper::AddValueToPartABCDbyType(data.ParamA(),data.ParamB(),data.ParamC(),m_NewFinalAttributeMap);	
	}
	// ����Ż�������������
	int nResult = false;
	const mem::map<int16_t, PetInstance*> &petMap =  pkPlayer->GetPetRawData(nResult);
	if(nResult)
	{
		CF_Pet* pPetConfig = CF_Pet::GetSingletonPtr();
		for(mem::map<int16_t, PetInstance*>::const_iterator itc = petMap.begin();itc != petMap.end();++itc )
		{
			PetInstance * pPet = itc->second;
			if(NULL ==  pPet)
				continue;
			int StarLevel = pPet->GetPetInstanceData().m_Data_Base.m_nStarLevel;
			int nPetId = pPet->GetTemplateId();
			CF_Pet::DataEntry* pConfig111 = pPetConfig->GetEntryPtr(nPetId);
			if (pConfig111 == NULL)
				continue;
			if(m_pConfig->_ielementdamagetype != pConfig111->_ielementdamagetype)
				continue;
			const CF_PetUpgrade::DataEntry* pStarData = BuddyHelper::GetPetUpgradeData(nPetId, StarLevel);
			if (pStarData == NULL)
				continue;
			
			for(int i=0; i<pStarData->Attribute.size(); ++i)
			{
				const Utility::UT_SIMDataInfo& data = pStarData->Attribute[i];
				LC_Helper::AddValueToPartABCDbyType(data.ParamA(),data.ParamB(),data.ParamC(),m_NewFinalAttributeMap);	
			}
		}
	}
	// ���� �ȳ� ���� ��ӵ���������
	LC_Helper::GetCimeliaSkinPercentageAttr(pkPlayer,LC_SUB_TP_VEHICLE,m_pConfig->_ielementdamagetype,m_NewFinalAttributeMap);
	LC_Helper::GetCimeliaSkinPercentageAttr(pkPlayer,LC_SUB_TP_DEVA_EYE,m_pConfig->_ielementdamagetype,m_NewFinalAttributeMap);
	LC_Helper::GetCimeliaSkinPercentageAttr(pkPlayer,LC_SUB_TP_MUHUN,m_pConfig->_ielementdamagetype,m_NewFinalAttributeMap);

	m_NewFinalAttributeMap.RecomputeAll();
	uint64_t nFinalSocre =  0;
	nFinalSocre += LC_Helper::CalcAttributeMapCombatScore(&m_NewFinalAttributeMap,true);
	nFinalSocre += pBuddy->GetBuddySkillCombatScore();
	nFinalSocre += pkPlayer->GetCommanderAttrMgr().GetCommanderCombatScoreByType(CST_SubSkillScore);
	SetBuddyCombatScoreByType(pkPlayer, BCST_FinalBuddy,nFinalSocre);
	pkPlayer->GetCommanderAttrMgr().UpdateBuddyFinalSocre(pkPlayer, m_configId,nFinalSocre);
	m_NewFinalAttributeMap.PrintAttrMap("m_NewFinalAttributeMap");
	pBuddy->SyncBuddyAttributes(SPAT_BuddyAllAttr,m_NewFinalAttributeMap, login, GetBuddyCombatScoreByType(BCST_FinalBuddy), ReasonType);
}
void BuddyAttrMgr::RefreshHeroGiveToOtherCoreAttrMap(LC_ServerPlayer* pkPlayer,bool bRefresh )
{
	if (NULL ==  pkPlayer ||false ==  pkPlayer->CheckBuddyChain( m_configId ) )
		return;
	m_GiveToOtherCoreAttributeMap.Clear();
	for(int i = 0; i < ATT_MAX_COUNT; i++)
	{
		m_GiveToOtherCoreAttributeMap.AddAttributeMapByType(i,m_LevelAttributeMap);
		m_GiveToOtherCoreAttributeMap.AddAttributeMapByType(i,m_RuneAttributeMap);
		m_GiveToOtherCoreAttributeMap.AddAttributeMapByType(i,m_EquipAttributeMap);
	}
	//m_GiveToOtherCoreAttributeMap.RecomputeAll();
	
	m_GiveToOtherCoreAttributeMap.PrintAttrMap("m_GiveToOtherCoreAttributeMap");
}
void BuddyAttrMgr::RefreshHeroAllAttributeMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy)
{
	m_HeroAllAttributeMap.Clear();
	GetBaseAttrMap(m_HeroAllAttributeMap);
	for(int i = 0; i < ATT_MAX_COUNT; i++)
	{
		m_HeroAllAttributeMap.AddAttributeMapByType(i,m_LevelAttributeMap);
		m_HeroAllAttributeMap.AddAttributeMapByType(i,m_RuneAttributeMap);
		m_HeroAllAttributeMap.AddAttributeMapByType(i,m_EquipAttributeMap);
		m_HeroAllAttributeMap.AddAttributeMapByType(i,m_SkinAttributeMap);	
	}
	m_HeroAllAttributeMap.PrintAttrMap("m_HeroAllAttributeMap");
}


void BuddyAttrMgr::GetBaseAttrMap(LC_ActorAttributeMap & attrMap)
{
	attrMap.AddAttributeBaseValue(ATT_MAX_HP,	m_pConfig->_lMaxHP);
	attrMap.AddAttributeBaseValue(ATT_MAX_ATK,	m_pConfig->_lMaxPHA);
	attrMap.AddAttributeBaseValue(ATT_DEF,		m_pConfig->_lPHD);
	attrMap.AddAttributeBaseValue(ATT_DOG_RATE,	m_pConfig->_lDogRATE);
	attrMap.AddAttributeBaseValue(ATT_CIR_RATE,	m_pConfig->_lCirRATE);
	attrMap.SetAttributeBase(ATT_MOVE_SPEED, (int)CHAR_MOVE_SPEED);
	attrMap.AddAttributeBaseValue(ATT_ATK_RATE, m_pConfig->_lATKRateAdd);
	attrMap.AddAttributeBaseValue(ATT_CIR_DEF, m_pConfig->_lCirDefAdd);
	// ����û�����߼������Ը���
	attrMap.SetAttributeBase(ATT_MAX_MP,	0);
	attrMap.SetAttributeBase(ATT_RES_FREEZ, 0);
	attrMap.SetAttributeBase(ATT_RES_SLOW, 0);
	attrMap.SetAttributeBase(ATT_RES_SILENCE, 0);
	attrMap.SetAttributeBase(ATT_RES_DIZZ, 0);
	attrMap.SetAttributeBase(ATT_RES_SLEEP, 0);
	// Move from _PlayerBase
	attrMap.SetAttributeBase(ATT_MAX_AMM,	0);
	attrMap.SetAttributeBase(ATT_MAX_STM,	0);
	attrMap.SetAttributeBase(ATT_MP_RECOVER,	0);
	attrMap.SetAttributeBase(ATT_CIR_DAM, m_pConfig->_lCirDAM);
	//attrMap.SetAttributeBase(ATT_MOVE_SPEED, CHAR_MOVE_SPEED);
	attrMap.SetAttributeBase(ATT_PICK_CRASH_RNG, GetGlobalSetting.iGoldPickRadius);

	attrMap.AddAttributePercentFunc(ATT_MAX_HP, 0);//m_pConfig->_lMaxHPRate);
	attrMap.AddAttributePercentFunc(ATT_MAX_ATK, 0);//m_pConfig->_lMaxAtkRate);
	attrMap.AddAttributePercentFunc(ATT_DEF, 0);//m_pConfig->_lMaxDefRate);

	//����ָ���Ǳ��ʵĸı�
	attrMap.SetAttributeBase(ATT_MP_COST_RATE, 0);
	attrMap.SetAttributeBase(ATT_HP_COST_RATE, 0);
	attrMap.SetAttributeBase(ATT_MP_RECOVER_RATE, 0);
	attrMap.SetAttributeBase(ATT_HP_RECOVER_RATE, 0);
	attrMap.SetAttributeBase(ATT_SP_RECOVER_RATE, 10000);
	attrMap.SetAttributeBase(ATT_STM_RECOVER_RATE, 0);//m_pConfig->_lSTMReg);
	attrMap.SetAttributeBase(ATT_AMM_RECOVER_RATE, 0);//m_pConfig->_lAMMReg);
	//����ָ���ǰٷֱ�*10000�����������
	attrMap.SetAttributeBase(ATT_EXP_RATE, 10000);
	attrMap.SetAttributeBase(ATT_CASH_RATE, 10000);
	attrMap.SetAttributeBase(ATT_MF_RATE, 0);//m_pConfig->_lMFRate + 10000);

	//�˺�����Ĳ���
	attrMap.SetAttributeBase(ATT_DMG_RATE, 10000);
	// Ŀǰ���ǧ���ƣ�ԭ���������
	attrMap.SetAttributeBase(ATT_DMG_RATE, m_pConfig->_lDMG_RATE);
	attrMap.SetAttributeBase(ATT_DMG_DERATE,m_pConfig->_lDMG_DERATE );
	attrMap.SetAttributeBase(ATT_IGNORE_DEF, m_pConfig->_lIGNORE_DEF );
	attrMap.SetAttributeBase(ATT_DMG_REBOUND, m_pConfig->_lDMG_REBOUND);
	attrMap.SetAttributeBase(ATT_CRI_DMG_DERATE, m_pConfig->_lCRI_DMG_DERATE);
	attrMap.SetAttributeBase(ATT_HP_HURT, m_pConfig->_HpHurt);
	//attrMap.RecomputeAll();
	attrMap.PrintAttrMap("GetBaseAttrMap");
}


void BuddyAttrMgr::RefreshLevelAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy)
{
	if(NULL == pkPlayer|| NULL == pBuddy)
		return;
	int lv = pBuddy->GetLevel();
	int starLv = pBuddy->GetStarLevel();
	const CF_LevelUp::DataEntry* pLevelData =  CF_LevelUp::GetSingletonPtr()->GetEntryPtr(lv);
	const CF_BuddyUpgrade::DataEntry* pStarData = BuddyHelper::GetBuddyUpgradeData(m_configId,starLv);
	if(NULL == pLevelData|| NULL == pStarData )
	{
		const unique_id_impl& instance = pkPlayer->GetInstance();
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "BuddyAttrMgr::RefreshLevelAttrMap pkLevelData or pStarData  is NULL lv:%d ,starLv:%d, c_cid:%d, c_citizenship:%d, c_uid:%d\n",
			lv, starLv, instance.detail(), instance.catagory(), pkPlayer->GetUserID());
		return;
	}
	m_LevelAttributeMap.Clear();
	m_LevelAttributeMap.SetAttributeBase(ATT_MAX_HP,	 pLevelData->_lMaxHPAdd * pStarData->_lMaxHP);
	m_LevelAttributeMap.SetAttributeBase(ATT_MAX_ATK,	 pLevelData->_lMaxATKAdd * pStarData->_lMaxPHA);
	m_LevelAttributeMap.SetAttributeBase(ATT_DEF,		pLevelData->_lPHDAdd * pStarData->_lPHD);
	m_LevelAttributeMap.SetAttributeBase(ATT_DOG_RATE,	 pLevelData->_lDogRateAdd * pStarData->_lDogRATE);
	m_LevelAttributeMap.SetAttributeBase(ATT_CIR_RATE,	 pLevelData->_lCirRateAdd * pStarData->_lCirRATE);
	m_LevelAttributeMap.SetAttributeBase(ATT_ATK_RATE, pLevelData->_lATKRateAdd * pStarData->_lATKRateAdd);
	m_LevelAttributeMap.SetAttributeBase(ATT_CIR_DEF, pLevelData->_lCirDefAdd * pStarData->_lCirDefAdd);
	m_LevelAttributeMap.SetAttributeBase(ATT_HP_RECOVER,	pLevelData->_lHPReg);
	m_LevelAttributeMap.SetAttributeBase(ATT_CIR_DAM,	pLevelData->_lCirDamAdd);
	
	m_LevelAttributeMap.PrintAttrMap("m_LevelAttributeMap");

	//ˢ��buddyҪ�ϰ�Ļ���ս��
	RefreshBuddyRankCombatScore(pBuddy);
}

void BuddyAttrMgr::RefreshRuneAttrMap(BuddyInstance* pBuddy)
{
	if(NULL == pBuddy)
	{
		return;
	}

	m_RuneAttributeMap.Clear();
	pBuddy->RefreshBuddyRuneAttrMap();

	LC_ActorAttributeMap* pkAttr = pBuddy->GetRunePack().GetAttributeMap();
	if(NULL == pkAttr)
	{
		return;
	}

	m_RuneAttributeMap.CopyFrom(pkAttr);

	//ˢ��buddyҪ�ϰ�Ļ���ս��
	RefreshBuddyRankCombatScore(pBuddy);
}

void BuddyAttrMgr::RefreshEquipAttrMap(LC_ServerPlayer* pkPlayer, BuddyInstance*pBuddy)
{
	if(NULL == pkPlayer|| NULL == pBuddy)
	{
		return;
	}
	
	pBuddy->RefreshBuddyEquipAttrMap();

	m_EquipAttributeMap.Clear();
	LC_ActorAttributeMap* pkAttr = pBuddy->GetEquipPack().GetAttributeMap();
	if(NULL == pkAttr)
	{
		return;
	}
	m_EquipAttributeMap.CopyFrom(pkAttr);

	m_EquipAttributeMap.RecomputeAll();
	uint64_t buddyEquipScore = LC_Helper::CalcAttributeMapCombatScore(&m_EquipAttributeMap, false);
	SetBuddyCombatScoreByType(pkPlayer, BCST_BuddyEquip, buddyEquipScore);
	
	//ˢ��buddyҪ�ϰ�Ļ���ս��
	RefreshBuddyRankCombatScore(pBuddy);
}

void BuddyAttrMgr::RefreshSkinAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy)
{
	m_SkinAttributeMap.Clear();
	for(int i=0; i<pBuddy->GetBuddyInstanceData().GetBuddyInstanceData_Base()->m_nActiveTransformRecord.size(); ++i)
	{
		CF_BuddyTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(pBuddy->GetBuddyInstanceData().GetBuddyInstanceData_Base()->m_nActiveTransformRecord[i]);
		 if(NULL == pkData)
			 continue;
		 for(int k=0; k<pkData->_kAttribute.size(); ++k)
		 {
			 if(pkData->_kAttribute[k].GetFlag() != 1)
				 continue;
			m_SkinAttributeMap.ModifyAttributeByPart(pkData->_kAttribute[k].GetPart(), pkData->_kAttribute[k].GetAttrID(), pkData->_kAttribute[k].GetValue(), true);
		 }
	}
	m_SkinAttributeMap.PrintAttrMap("m_SkinAttributeMap");
}

uint64_t BuddyAttrMgr::GetBuddyCombatScoreByType(uint8_t uType)
{
	if(uType >= BUDDY_COBAT_SCORE)
		return 0;
	return m_nBuddyCombatScore[uType];
}

void BuddyAttrMgr::SetBuddyCombatScoreByType(LC_ServerPlayer* pkPlayer, uint8_t uType,uint64_t nValue)
{
	if(uType >= BUDDY_COBAT_SCORE || NULL==pkPlayer)
		return ;
	m_nBuddyCombatScore[uType] = nValue;

	int32_t rankType = -1;
	switch(uType)
	{
	case BCST_BuddyEquip:
		rankType = Rank_Type_BuddyEquip;
		break;
	case BCST_FinalBuddy:
		rankType = LC_Helper::GetRankTypeByBuddyID(m_configId);
		break;
	default:
		break;
	}

	if(-1 != rankType)
	{
		pkPlayer->UpdateRankUserValueByRankType(rankType);
	}

	if(BCST_FinalBuddy == uType)
	{
		if(pkPlayer->IsActivateBuddy(m_configId))
		{
			AchievementEvent kEvent1;
			kEvent1.SetNum(pkPlayer->GetActivateBuddyCombatScore());
			pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ACTIVATE_BUDDY_COMBATSCORE, kEvent1);
		}
	}
}

void BuddyAttrMgr::RefreshBuddyRankCombatScore(BuddyInstance* pkBuddy)
{
	if(NULL == pkBuddy)
		return;

	LC_ServerPlayer* pkOwnerPlayer = pkBuddy->GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	LC_ActorAttributeMap buddyBaseAttrMap;
	GetBaseAttrMap(buddyBaseAttrMap);
	LC_ActorAttributeMap buddyRankAttrMap;
	for(int i = 0; i < ATT_MAX_COUNT; i++)
	{
		//Ӣ�۳�ʼ����
		buddyRankAttrMap.AddAttributeMapByType(i,buddyBaseAttrMap);
		//Ӣ����������
		buddyRankAttrMap.AddAttributeMapByType(i, m_LevelAttributeMap);
		//Ӣ��װ����װ������+װ����λ����+װ����λ���ǣ�
		buddyRankAttrMap.AddAttributeMapByType(i, m_EquipAttributeMap);
		//�������ԣ����߻���+��������+�������ǣ�
		buddyRankAttrMap.AddAttributeMapByType(i, m_RuneAttributeMap);
	}
	//Ӣ�����ǵİٷֱ�
	LC_Helper::GetBuddyUpgradeRate(m_configId, pkBuddy->GetStarLevel(),buddyRankAttrMap);

	buddyRankAttrMap.RecomputeAll();
	uint64_t attrMapSocre = LC_Helper::CalcAttributeMapCombatScore(&buddyRankAttrMap, true);
	uint64_t skillCombatScore = pkBuddy->GetBuddySkillCombatScore();
	uint64_t nFinalSocre = attrMapSocre + skillCombatScore;
	SetBuddyCombatScoreByType(pkOwnerPlayer, BCST_BuddyRank, nFinalSocre);
}
