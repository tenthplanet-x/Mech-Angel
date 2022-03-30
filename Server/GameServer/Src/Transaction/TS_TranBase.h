#pragma once

/********************************************************************
purpose:	规范事务原子操作的接口
notice:		事务的上下文
*********************************************************************/
#include "MM_BaseObject.h"
#include "MG_PACKET.h"
#include "SystemError.h"
using namespace Protocol;
enum emTranOperate
{
	TRAN_OP_NEXT,
	TRAN_OP_FAILED,
};

enum enTranStatus
{		
	TRAN_STATUS_RUNNING,
	TRAN_STATUS_TIMEOUT,
};

class TS_AtomOperate : public Memory::MM_BaseObject
{
public:	
	TS_AtomOperate();
	virtual ~TS_AtomOperate() {}

	//! 执行事务
	virtual emTranOperate Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID) = 0;

	// 发送相应消息	
	virtual void RespondRltMsg() {}

	//! 回顾事务
	virtual void Rollback() {}

	//! 重置操作数据
	virtual void Reset();

	//! 超时判断
	virtual bool TimeOut() {return false;}

public:
	void SetResultID(uint32_t iErrorID);
	void SetUserID(uint32_t iUserID);
	void SetCharID(const unique_id_impl& iCharID);
	void SetContentFlag(bool bFlag);

	uint32_t GetResultID() const;
	uint32_t GetUserID() const;
	const unique_id_impl& GetCharID() const;
	bool GetContentFlag() const;

private:
	uint32_t	m_iResultID;			//! 结果编号
	user_id_type	m_uiUserID;				//! 当前事务涉及账号ID
	unique_id_impl	m_uiCharID;				//! 当前操作涉及角色ID
	bool		m_bContentFlag;			//! 是否有上下文	
};

typedef hash_map<uint32_t, TS_AtomOperate*> StepAtomMap;

//typedef mem::map<uint32_t, TS_AtomOperate*>		StepAtomMap;
typedef StepAtomMap::iterator					StepAtomMapIter;

/********************************************************************
purpose:	1. 主要针对网络操作的的规范化，
			2. 事务相关接口定义
			事务是顺序执行的!!!
*********************************************************************/
class TS_PostCommit : public Memory::MM_BaseObject
{
public:
	TS_PostCommit(){};
	virtual ~TS_PostCommit(){};

	virtual void operator()(void){};
};

class TS_TranBase : public Memory::MM_BaseObject
{
public:
	TS_TranBase();
	virtual ~TS_TranBase();

	//! 事务对消息的处理
	virtual ResultType HandlerMsg(Protocol::SS_MG_HEAD& pkHead, Protocol::MG_BODY* pkMsg);

	//! 事务的更新
	virtual enTranStatus Update();
	
	virtual uint32_t GetType() = 0;

	virtual void Reset();

	void   SetOwnerID(uint32_t uiUserID, const unique_id_impl& uiCharID);

public:
	void SetID(uint32_t uiSequence);
	void SetStep(uint32_t uiStep);

	uint32_t GetID() const;
	uint32_t GetStep() const;

	void RegPostCommit(TS_PostCommit* pkPostCommit);

private:
	void _rollbackAllStep();

protected:				
	uint32_t	m_uiID;						//! 事务关联的序列号，相当于私钥，用于找回相应事务
	uint32_t	m_uiStep;					//! 事务当前执行到第几步	
	float		m_fTranHandleTime;			//! 当前事务执行时间
	StepAtomMap	m_kStepTranMap;				//! 该事务操作的组成部分

	//for 
	TS_PostCommit*	m_pPostCommit;
};

typedef hash_map<uint32_t, TS_TranBase*>	SequnceTranMap;

//typedef mem::map<uint32_t, TS_TranBase*>	SequnceTranMap;
typedef SequnceTranMap::iterator			SequnceTranMapIter;

#include "TS_TranBase.inl"
