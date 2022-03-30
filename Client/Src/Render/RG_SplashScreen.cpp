#include "ClientPCH.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <process.h>

#include "RG_SplashScreen.h"

using namespace Utility;

/************************************************************************/
struct SlideSlice
{
	float	fPercent;
	DWORD	dwSpendTime;
};
static SlideSlice s_ProgressSlideSlice[] = 
{
	{	  5,  500},
	{36.97f, 4430},
	{42.44f,  655},
	{43.36f,  109},
	{44.00f,  78},
	{45.83f,  219},
	{46.61f,  93},
	{48.18f,  188},
	{70.00f, 2620},
	{95.18f, 3011},
	{100.0f, 577},
};

/************************************************************************/

IJZFlashPlayer*		RG_SplashWindow::ms_pExternalFlashPlayer = NULL;

RG_SplashWindow::RG_SplashWindow( HWND hParent )
 : m_pFlashPlayer(NULL)
{
	//////////////////////////////////////////////////////////////////////////
	m_hParentWnd = hParent;
	m_nPercent = 0;
	m_nShowPercent = 0;
	m_iSlideSliceIndex = -1;

	//download & load
	m_metrics.m_nPercent = 0;
	m_metrics.m_pPercentMsg = NULL;
	m_metrics.m_bDownloadVisible = false;
	m_metrics.m_DownloadSize = 0;
	m_metrics.m_DownloadSpeed = 0;

	if (ms_pExternalFlashPlayer == NULL)
		m_pFlashPlayer = IJZFlashPlayer::create(0, hParent);
	else
		m_pFlashPlayer = ms_pExternalFlashPlayer;
	m_nWidth = 0;
	m_nHeight = 0;
}

RG_SplashWindow::~RG_SplashWindow()
{
	this->Stop();
	this->Hide();

	if (m_pFlashPlayer != NULL && m_pFlashPlayer != ms_pExternalFlashPlayer)
	{
		IJZFlashPlayer::destroy(m_pFlashPlayer);
	}
	m_pFlashPlayer = NULL;
	ms_pExternalFlashPlayer = NULL;
}

bool RG_SplashWindow::isInitlized()
{
	return m_pFlashPlayer != NULL;
}

/************************************************************************/

void RG_SplashWindow::UpdateShapedWindow(void)
{
	float nPercent				= this->m_nPercent;
	IJZFlashPlayer *pFlashPlayer= this->m_pFlashPlayer;
	std::wstring & kPrgMsg		= this->m_ProgressMsg;

	//要求滑动进度条
	if (this->m_iSlideSliceIndex >= 0)
	{
		const SlideSlice & d = s_ProgressSlideSlice[this->m_iSlideSliceIndex];
		DWORD ct = ::GetTickCount();
		DWORD elapsed = ct - this->m_dwSlideBeginTime;
		if (elapsed >= d.dwSpendTime)
			nPercent = d.fPercent;
		else
			nPercent += (d.fPercent - nPercent) * elapsed / d.dwSpendTime;
	}
	this->SetShowProgress(nPercent);

	//< 设置flash
	wchar_t sd_str[32];
	swprintf(sd_str, L"%.2f", nPercent);

	wchar_t tip_str[256];
	swprintf(tip_str, L"%s %d%%", kPrgMsg.c_str(), (int)nPercent);

	pFlashPlayer->setVariable(L"sd", sd_str);
	pFlashPlayer->setVariable(L"tipsStr", tip_str);
	if (this->m_metrics.m_bDownloadVisible)
	{
		swprintf(sd_str, CLIENTAPP_FLASH_TEXT_0, (int)(this->m_metrics.m_DownloadSize >> 10), (int)(this->m_metrics.m_DownloadSpeed >> 10));
		pFlashPlayer->setVariable(L"speedStr", sd_str);
	}
	else
		pFlashPlayer->setVariable(L"speedStr", L"");

	//< flash窗口自适应
	pFlashPlayer->resize();
}

void RG_SplashWindow::Show()
{
	if (!g_dllRunInBrowser)
	{
		ShowWindow(this->m_hParentWnd, SW_SHOWNORMAL);
		UpdateWindow(this->m_hParentWnd);
	}
}

void RG_SplashWindow::Hide()
{
}

void  RG_SplashWindow::Run()
{
	static DWORD lastTime = GetTickCount();
	DWORD cTime = GetTickCount();
	if (cTime - lastTime >= 100)
	{
		if (this->isInitlized())
		{
			this->UpdateShapedWindow();
		}
		lastTime = cTime;
	}
	else
	{
		::Sleep(10);
	}
}

LRESULT CALLBACK RG_SplashWindow::SplashWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	RG_SplashWindow* pInstance = RG_SplashWindow::GetSingletonPtr();

	switch (uMsg)
	{
	case PBM_SETPOS:
		{
			const std::wstring* msg = reinterpret_cast<std::wstring*>( lParam );
			if ( msg && pInstance->m_ProgressMsg != *msg )
			{
				pInstance->m_ProgressMsg = *msg;
			}
			delete msg;
			return 1;
		} break;
	case WM_CLOSE:	//ignore close msg
		return 1;
	}

	return 0;
}
void RG_SplashWindow::SetProgress( float percent, const wchar_t* msg )
{
	m_nPercent = percent;
	m_nShowPercent = percent;
	std::wstring* tempmsg = new std::wstring( msg );
	PostMessage( m_hParentWnd, PBM_SETPOS, 0, reinterpret_cast<LPARAM>(tempmsg) );
}

void RG_SplashWindow::Play()
{
	m_pFlashPlayer->play();
}

void RG_SplashWindow::Stop()
{
	m_pFlashPlayer->stop();
}

void RG_SplashWindow::SetSlideProgress(int iSliceIndex, const wchar_t* msg)
{
	T_ASSERT(iSliceIndex >= 0);
	T_ASSERT(iSliceIndex < _countof(s_ProgressSlideSlice));

	float percent = (iSliceIndex > 0) ? s_ProgressSlideSlice[iSliceIndex - 1].fPercent : 0;
	m_iSlideSliceIndex = iSliceIndex;
	this->SetProgress(percent, msg);
	m_dwSlideBeginTime = ::GetTickCount();
}
