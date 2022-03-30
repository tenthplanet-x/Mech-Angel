#pragma once
//�ͻ����ʼ�����
#include "LC_MailAsset.h"

namespace GameLogic
{
	class LC_ClientMailManager :	public Utility::UT_Singleton<LC_ClientMailManager>,
									public Memory::MM_BaseObject
	{
	public:
		LC_ClientMailManager();
		virtual ~LC_ClientMailManager();
		DECL_RTTI_NAME( LC_ClientMailManager );

		// �ͻ�����������Լ����ʼ��ʲ�
		ResultType RequestMailAsset();
		static void PostGetGlobalMail(Utility::timer_id_type timerID);

		// �ż����ݼ�飬���������ʣ���Ϊ��Ҫ��������ʾ�����Էֲ����.
		ResultType RequestWriteMailContentCheck(const StringType& strReceiverName, 
												const StringType& strSubject, const StringType& strContent,
												MoneyType iGold, MoneyType iSilver,	MoneyType iCopper, bool bIsPay);
		// ���ʼ��
		ResultType RequestWriteMailPostageCheck(MoneyType iGold, MoneyType iSilver, MoneyType iCopper);

		// ִ��д�ʼ�����
		void RequestWriteMailExcute(const StringType& strReceiverName, 
									const StringType& strSubject, const StringType& strContent,
									MoneyType iGold, MoneyType iSilver, MoneyType iCopper, bool bIsPay);
		

		// �����ţ��ͻ�������ȡ��Ʒ����
		ResultType RequestItemAffix(int iIndex);

		// �����ţ��ͻ�������ȡ������Ʒ����
		ResultType RequestAllItemAffix();
		ResultType RequestAllItemAffixByMailIndex(int iIndex);
		int		   GetItemAffixCount(int iIndex);

		// �����ţ��ͻ��������ȡ��Ǯ����
		ResultType RequestCashAffix();

		// �ͻ�����������
		ResultType RequestUntreadMail();
		ResultType RequestUntreadMailByMailID(uint64_t MailID);

		// �ͻ���������ʼ�
		ResultType RequestOpenMail();

		// �ͻ�������رմ򿪵��ż�
		ResultType RequestCloseMail();

		// ��д�ţ��ͻ�����������ʼ�����
		ResultType RequestAddAffix(int32_t iPos, int32_t iPackType, int32_t iIndex);

		// ��д�ţ��ͻ�������ɾ�����丽��
		ResultType RequestDeleletAffix(int32_t iPos);
		
		// ����ʼ��ʲ�, �����ʼ�
		void Reset();
		
		// �յ��ʼ�ͷ
		void ReceiveMailSimpleInfo(const Protocol::MG_RLT_MailAsset& rkMailInfo);

		// �յ�һ���ʼ�
		void ReceiveMail(const Protocol::PS_MailInfo& kMail);

		// ɾ��һ���ʼ�
		void DeleteMail(uint64_t lMailID);

		// �ɹ���ȡһ����Ʒ����
		void GetItemAffix(uint64_t iMailID, int iIndex);

		// �ɹ���ȡһ����Ǯ����
		void GetCashAffix(uint64_t iMailID);

		// ��ȡ�ʼ��ʲ�
		LC_MailAsset* GetMailAsset();

		// ���õ�ǰѡ���ʼ�
		void SetSelectedMailIndex(int iIndex);

		// ��ȡ��ǰѡ���ʼ�
		int GetSelectedMailIndex();

		// �������
		void CompleteWriteMail();

		// ���д��װ����־
		void ClearWriteFlag();

		bool GetSendingFlag();
		bool GetCompleteFlag();

		void SetSendingFlag(bool bFlag);
		void SetCompleteFlag(bool bFlag);

		// ��ȡ��Ʒ������Ϣ
		LC_ClientItemAffixInfo* GetClientItemAffixInfo();

		// ��������򽹵�ʧȥ������������Ƿ����
		void CheckReceiverName(const StringType& strName);
		
		// ��ȡ�ʼ�������
		MoneyType ComputeMailCharge(MoneyType iGold, MoneyType iSilver, MoneyType iCopper);
		StringType GetMailCharge(MoneyType iGold, MoneyType iSilver, MoneyType iCopper);

	public:
		bool IsExistPendingName(const StringType& strName);
		void AddPendingName(const StringType& strName);
		void ErasePendingName(const StringType& strName);

		ResultType SendPendingMail();

		void OnLoginRequest()
		{
			if(!m_bIsLoginRequest)
			{
				RequestMailAsset();
				m_bIsLoginRequest = true;
			}
		}
	public:
		void	SetMailHintFlag(bool bHint);
		bool	GetMailHintFlag();

		void	SetMailboxFullFlag(bool bFlag);
		bool	GetMailboxFullFlag();
		void	SetIsReadingMailFlag(bool bFlag);
		bool	GetIsReadingMailFlag();

	private:

	private:
		LC_MailAsset			m_kMailAsset;		
		int						m_iSelectedIndex;
		uint64_t				m_iSelectedMailID;

		LC_ClientItemAffixInfo	m_kClientItemAffixInfo;

		bool					m_bIsREQMailAsset;
		bool					m_bIsSending;
		bool					m_bIsComplete;

		//Ϊ�˵�½����
		bool					m_bIsLoginRequest;
		int64_t					m_iPreRequestTimeStamp;

		TStringList				m_kPendingTargetList;		// д��Ŀ���б�

		StringType				m_strPendingReceiverName;
		StringType				m_strPendingSubject;
		StringType				m_strPendingContent;
		MoneyType				m_iPendingGold;
		MoneyType				m_iPendingSilver;
		MoneyType				m_iPendingCopper;
		bool					m_bIsPay;
		bool					m_bMailHint;
		bool					m_bMailboxFull;
		bool					m_bIsReadingMail;
	};
}
