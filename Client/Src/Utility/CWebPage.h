#ifndef		__CWEB_PAGE_H__
#define		__CWEB_PAGE_H__


#ifdef	__cplusplus
	extern "C"
	{
#endif


	long EmbedBrowserObject(HWND hwnd);
	void UnEmbedBrowserObject(HWND hwnd);
	long DisplayHTMLPage(HWND hwnd, LPTSTR webPageName);


#ifdef	__cplusplus
	}
#endif


#endif	//__CWEB_PAGE_H__