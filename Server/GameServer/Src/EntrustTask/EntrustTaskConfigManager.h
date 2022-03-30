//委派任务配置相关
#ifndef _H_EntrustTaskConfigManager_H_
#define _H_EntrustTaskConfigManager_H_
#include "LC_Define.h"
#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "../logic/CommandData.h"

struct SEntrustTaskConfig
{
	SEntrustTaskConfig()
	{
		star = 0;
		type = 0;
		needTime = 0x7fffffff;
	}
	int32_t star;	//星级
	int32_t type;	//1英雄 2宝具
	int32_t needTime;	//需要的时间
	mem::vector<SKeyValueConfig> conditionVec;	//条件
	SKeyValueConfig acceptCost;	//派遣消耗
	SKeyValueConfig speedupCost;	//加速消耗
	mem::vector<SItemConfig> rewardVec;	//奖励
};

class LC_ServerPlayer;
class CEntrustTaskConfigManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<CEntrustTaskConfigManager>
{
	public:
		CEntrustTaskConfigManager();
		~CEntrustTaskConfigManager();

		//初始化
		void Init();
		//释放
		void Unit();
		//重加载
		void Reload();

		//得到数据
		SEntrustTaskConfig* GetEnstrustTaskConfig( int32_t _id );

		//得到随机的任务
		bool GetRandomEnstrustTask( mem::list<int32_t>& _list, int32_t _maxNum, int32_t _star = 0 );

	private:
		void InitEntrustType();
		void InitEntrustTask();

	protected:
		mem::map<int32_t, SEntrustTaskConfig> m_EntrustTaskMap;	//委派任务数据
		mem::vector<SKeyValueConfig> m_WeightVec;		//权重
		int32_t m_TotalWeight;	//总权重

		mem::map< int32_t, mem::vector<int32_t> > m_StarMap;	//委派星级对应的任务
};

#endif
