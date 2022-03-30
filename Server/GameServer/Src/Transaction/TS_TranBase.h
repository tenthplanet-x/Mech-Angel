#pragma once

/********************************************************************
purpose:	�淶����ԭ�Ӳ����Ľӿ�
notice:		�����������
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

	//! ִ������
	virtual emTranOperate Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID) = 0;

	// ������Ӧ��Ϣ	
	virtual void RespondRltMsg() {}

	//! �ع�����
	virtual void Rollback() {}

	//! ���ò�������
	virtual void Reset();

	//! ��ʱ�ж�
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
	uint32_t	m_iResultID;			//! ������
	user_id_type	m_uiUserID;				//! ��ǰ�����漰�˺�ID
	unique_id_impl	m_uiCharID;				//! ��ǰ�����漰��ɫID
	bool		m_bContentFlag;			//! �Ƿ���������	
};

typedef hash_map<uint32_t, TS_AtomOperate*> StepAtomMap;

//typedef mem::map<uint32_t, TS_AtomOperate*>		StepAtomMap;
typedef StepAtomMap::iterator					StepAtomMapIter;

/********************************************************************
purpose:	1. ��Ҫ�����������ĵĹ淶����
			2. ������ؽӿڶ���
			������˳��ִ�е�!!!
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

	//! �������Ϣ�Ĵ���
	virtual ResultType HandlerMsg(Protocol::SS_MG_HEAD& pkHead, Protocol::MG_BODY* pkMsg);

	//! ����ĸ���
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
	uint32_t	m_uiID;						//! ������������кţ��൱��˽Կ�������һ���Ӧ����
	uint32_t	m_uiStep;					//! ����ǰִ�е��ڼ���	
	float		m_fTranHandleTime;			//! ��ǰ����ִ��ʱ��
	StepAtomMap	m_kStepTranMap;				//! �������������ɲ���

	//for 
	TS_PostCommit*	m_pPostCommit;
};

typedef hash_map<uint32_t, TS_TranBase*>	SequnceTranMap;

//typedef mem::map<uint32_t, TS_TranBase*>	SequnceTranMap;
typedef SequnceTranMap::iterator			SequnceTranMapIter;

#include "TS_TranBase.inl"
