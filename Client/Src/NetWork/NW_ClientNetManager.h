#ifndef _NW_CLIENT_NET_MANAGER_H
#define _NW_CLIENT_NET_MANAGER_H
//�����ͷ���������Ϣ��


#define LOGIN_TIMEOUT 15				//��¼��ʱ��ֵ������Ϊ��λ

class NW_ClientNetManager : public NetWork::NW_NetManagerBase
{
public:
	NW_ClientNetManager();
	~NW_ClientNetManager();

	virtual bool Init();
	virtual void UnInit();
	virtual void Update();

	void StartCheckLogin(bool bCheck);

private:
	bool m_bCheckLogin;				//��������¼��ʱ
	float m_fLoginStartTime;		//��ʼ���ӷ�������ʱ��
};

#endif