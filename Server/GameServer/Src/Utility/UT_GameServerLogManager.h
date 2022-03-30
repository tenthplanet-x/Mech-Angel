//����LogStream����Ҫ�������������LogStream��
//			����LogStreamע�ᵽ����LogTag��

#pragma once

#include "MM_BaseObject.h"
#include "StringDefine.h"
#include <UT_Singleton.h>
#include "UT_Log.h"
#ifndef ACTIVATION_CODE
#include "CF_SkillSpecialAttrib.h"
#include "LC_ActorBase.h"

class LC_ItemFlopInfoList;
#endif
class UT_GameServerLogManager :	public Memory::MM_BaseObject, 
								public Utility::UT_Singleton<UT_GameServerLogManager>
{
public:
	UT_GameServerLogManager(void);
	virtual ~UT_GameServerLogManager(void);
#ifndef ACTIVATION_CODE
	//Begin ��ӡ������֤��Ϣ
	void SKA_LogActorInfo(GameLogic::LC_ActorBase* pkActor, const StringType& rkRemark, const StringType& rkFieldPrefix);
	void SKA_LogSkillInfo(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkTarget, CSVFile::CF_SkillSpecialAttrib::DataEntry* pkSkillData, float fFinalAttackRate, float fFinalCriticalRate, bool bAttack, bool bCritical, attr_value_type iDamage, int iDefence, int iMPCost, attr_value_type iFinalDamage, const StringType& kRemark);
	void SKA_LogRemark(const StringType& kRemark);
	//End	��ӡ������֤��Ϣ

	//��ӡ�������֤��Ϣ
	void LogFlopInfo(LC_ItemFlopInfoList* pkFlopInfoList);
#endif

	void set_console( bool flag );

	void Flush(){ m_pkLogManager->Flush(); }

private:
	void _registerLog(void);			//��LogStreamע�ᵽ����LogTag��

	Utility::UT_LogManager*		m_pkLogManager;
	Utility::UT_LogStreamConsoleDebug * console_debug;

	bool m_bPrintSkillValidateInfo;
};
