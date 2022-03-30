//为门派技能捐献材料提高门派贡献度

#pragma once

#include "TS_TranBase.h"
#include "TS_TranType.h"
#include "MG_ProtocolStruct.h"

// 请求捐献材料
class TS_Server_ReqDonateGuildMaterials : public TS_AtomOperate
{
public:
	TS_Server_ReqDonateGuildMaterials();
	~TS_Server_ReqDonateGuildMaterials(){}

	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
	virtual void			Rollback();
	virtual void			Reset();


private:
	uint32_t		m_uiTypeID;
	uint32_t		m_uiNumber;
	uint32_t		m_uiResearch;
};

// 捐献材料结果
class TS_Server_RltDonateGuildMaterials : public TS_AtomOperate
{
public:	
	virtual emTranOperate	Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID);
	virtual void			RespondRltMsg();
};

//
class TS_Server_DonateGuildMaterialsTran : public TS_TranBase
{
public:
	enum WriteMailStep
	{
		STEP_REQ_GGU_DONATE_GUILDMATERIALS = 0,
		STEP_RLT_GGU_DONATE_GUILDMATERIALS,
	};

	virtual uint32_t GetType()
	{
		return TRAN_TYPE_DONATE_MATERIALS;
	}
public:
	TS_Server_DonateGuildMaterialsTran();
	~TS_Server_DonateGuildMaterialsTran();
};
