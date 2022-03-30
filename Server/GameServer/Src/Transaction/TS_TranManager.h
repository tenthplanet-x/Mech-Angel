//�������񣬸���Է��������������

#pragma once

#include "TS_TranBase.h"
#include "TS_TranPool.h"

class TS_TranManager : public Memory::MM_BaseObject,
					   public Utility::UT_Singleton<TS_TranManager>
{
public:
	//static TS_TranManager* GetSingletonPtr()
	//{
	//	static TS_TranManager s_hInstance;
	//	return &s_hInstance;
	//}

	//! ע��һ���������
	TS_TranBase* RegisteTran(uint32_t uiTranID, uint32_t uiTranType,uint32_t uiUserID, const unique_id_impl& uiCharID);

	//! ע��һ���������
	bool UnRegisteTran(uint32_t uiTranID);

	//! ��ȡһ���Ѿ�ע���ʧ��
	TS_TranBase* GetTran(uint32_t uiTranID);

	//! ������и��²���
	void  Update();

private:
	SequnceTranMap	 m_kSequnceTranMap;

	TS_TranPool		 m_kTranPool;
};