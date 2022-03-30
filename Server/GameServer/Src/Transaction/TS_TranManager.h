//管理事务，负责对服务器内事务调用

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

	//! 注册一个事务服务
	TS_TranBase* RegisteTran(uint32_t uiTranID, uint32_t uiTranType,uint32_t uiUserID, const unique_id_impl& uiCharID);

	//! 注销一个事务服务
	bool UnRegisteTran(uint32_t uiTranID);

	//! 获取一个已经注册的失误
	TS_TranBase* GetTran(uint32_t uiTranID);

	//! 事务进行更新策略
	void  Update();

private:
	SequnceTranMap	 m_kSequnceTranMap;

	TS_TranPool		 m_kTranPool;
};