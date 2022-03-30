//��Ƶ��������ֻ����1x�ٲ���
#pragma once

#include <bink.h>
#include <radbase.h>

class UT_VideoPlayer : public Memory::MM_BaseObject
{
public:
	//////////////////////////////////////////////////////////////////////////

	UT_VideoPlayer(void);
	~UT_VideoPlayer(void);

	bool Play(const char *cpFilename, unsigned int uLoopCount = 1);
	void End(void);
	bool Pause(bool b);

	//////////////////////////////////////////////////////////////////////////

	bool BeginUpdate(float fTime, float fDeltaTime);
	void EndUpdate(void);

	//////////////////////////////////////////////////////////////////////////

	void CopyImage(void* destBuf, S32 destpitch);
	void CopyImageRect(void* destBuf, 
		S32 destpitch, U32 destheight, U32 destx, U32 desty,
		U32 src_x, U32 src_y, U32 src_w, U32 src_h
		);

public:
	//////////////////////////////////////////////////////////////////////////

	int	getWidth(void) const;
	int	getHeight(void) const;
	const StringType & getFilename(void) const { return m_kFilename; }

	bool	isValid(void) const { return m_hBink != 0; }
	bool	isEnd(void) const { return m_uCurLoop >= m_uLoopCount; }
	bool	isCurLoopEnd(void) const { return m_bCurLoopEnd; }

private:
	//////////////////////////////////////////////////////////////////////////

	HBINK			m_hBink;	//bink���
    int				m_bPause;	//��ͣ��־λ
	StringType		m_kFilename;//��Ƶ�ļ���

	unsigned int	m_uCurLoop;	//��ǰѭ������
	unsigned int	m_uLoopCount;//ѭ������
	bool			m_bCurLoopEnd;	//��ǰѭ���Ƿ����
	bool			m_bInernalUpdateing;	//�ڲ���־λ
};
