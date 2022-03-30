#ifndef _LC_ROTARYTABLE_H_
#define _LC_ROTARYTABLE_H_

#include "LC_Define.h"
#include "SystemError.h"
#include "char_data.pb.h"

class LC_ServerPlayer;
class CRotaryTable
{
public:
	CRotaryTable();
	~CRotaryTable();

	void LoadData(const char_data_define::PB_RotaryTable& data);
	void SaveData(char_data_define::PB_RotaryTable* pkData);

	//��������ת�̻�Ľ���ʱ��	
	void InitRotaryTableData(int32_t time);
	//ת�̳�齱
	void GetRotaryTableReward(LC_ServerPlayer* pkOwnerPlayer);
	//���������������
	void SetRotaryTableSkipAnimation(LC_ServerPlayer* pkOwnerPlayer, bool flag);
	//����ת����תʱ��
	void UpdateRotaryTableReward(LC_ServerPlayer* pkOwnerPlayer, bool force=false);

	//��¼��ʱ��ͬ��ת����������
	void SyncAllRotaryTableInfo(LC_ServerPlayer* pkOwnerPlayer);
	
	void SetRotaryTableCDStartTime(int64_t time);

	void UpdateRotaryTableVaildTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t value);

protected:
private:
	void SyncRotaryTableVaildTimes(LC_ServerPlayer* pkOwnerPlayer);
	void SyncRotaryTableRewardRecord(LC_ServerPlayer* pkOwnerPlayer);
	void SyncRotaryTableEndTime(LC_ServerPlayer* pkOwnerPlayer);
	void SyncRotaryTableSkipAnimation(LC_ServerPlayer* pkOwnerPlayer);

	ResultType CheckRotaryTableVaild();

	//�齱����
	int32_t	GetRotaryTableVaildTimes();
	int32_t	GetRotaryTableTimes();

	ResultType RandomRotaryTableRewardIndex(int32_t times, int32_t& rewardIndex);
	void UpdateRotaryTableRewardIndex(LC_ServerPlayer* pkOwnerPlayer, int32_t rewardIndex);
	bool	CheckRotaryTableRewardIndexVaild(int32_t rewardIndex);
	void SetCurrentRotaryTableRewardIndex(int32_t rewardIndex);

	int32_t m_nRotaryTableEndTime;//ת�̻������ʱ��

	int64_t m_nRotaryTableCDStartTime;//ת�̿�ʼת����ʱ��
	int32_t m_nCurrentRTRewardIndex;//��������Ľ���index

	// ��db
	int32_t m_nRotaryTableVaildTimes;//ת����Ч�齱����
	mem::vector<int32_t> m_nRotaryTableRewardIndex;//�Ѿ���ù���ת�̽���
	bool m_bSkipAnimation;//�Ƿ���������(true:���� false:������)
};

#endif
