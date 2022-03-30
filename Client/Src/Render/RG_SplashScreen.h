#pragma once

#include <GdiPlus.h>
#include "UT_Singleton.h"

#define JZFLASH_EXPORTS
#include "Ijzflashplayer.h"


struct SplashInfo
{
	UINT			m_nPercent;			// from 0 to 100
	const wchar_t*	m_pPercentMsg;
	bool			m_bDownloadVisible;
	long			m_DownloadSize;		//byte
	long			m_DownloadSpeed;	//byte
};

class RG_SplashWindow : public Memory::MM_BaseObject
					  , public Utility::UT_Singleton<RG_SplashWindow>
{
public:
	RG_SplashWindow( HWND hParent );
	~RG_SplashWindow();	

	bool isInitlized();
	void Show();
	void Hide();
	void Run();
	void Play();
	void Stop();

	void SetProgress(float percent, const wchar_t* msg );
	void SetSlideProgress(int iSliceIndex, const wchar_t* msg);
	SplashInfo & GetProgressArg() { return m_metrics; }
	void  SetShowProgress(float f) { m_nShowPercent = f; }
	float GetShowProgress() { return m_nShowPercent; }

private:
	RG_SplashWindow(const RG_SplashWindow&);
	RG_SplashWindow& operator=(const RG_SplashWindow&);
	void UpdateShapedWindow(void);

protected:
	float					m_nPercent; // from 0.00 to 100.00
	float					m_nShowPercent; // from 0.00 to 100.00
	int						m_iSlideSliceIndex;
	DWORD					m_dwSlideBeginTime;

	HWND							m_hParentWnd;
	std::wstring					m_ProgressMsg;

	IJZFlashPlayer*					m_pFlashPlayer;
	unsigned int					m_nWidth;
	unsigned int					m_nHeight;

	SplashInfo						m_metrics;

public:
	static 	IJZFlashPlayer*			ms_pExternalFlashPlayer;
	static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};