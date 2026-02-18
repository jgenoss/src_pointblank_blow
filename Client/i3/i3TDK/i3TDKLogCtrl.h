#if !defined( __I3_TDK_LOG_CTRL_H)
#define __I3_TDK_LOG_CTRL_H

//	=================================================================================
//	i3TDKLogCtrl
//	Programed : komet
//	Build Date : 2006.6
//	note : 
//	=================================================================================

#pragma once


// i3TDKLogCtrl

class I3_EXPORT_TDK i3TDKLogCtrl : public CEdit
{
	DECLARE_DYNAMIC(i3TDKLogCtrl)

protected:
	DWORD			m_dwStyle = 0;
	CFont			m_Font;

	TCHAR 			m_szMaxLongText[1024] = { 0 };

public:
	i3TDKLogCtrl() {}
	virtual ~i3TDKLogCtrl() {}

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

	INT32	AddText( const TCHAR* pszString, ...);		
	void	ClearText( void)			{	SetWindowText(_T("") );								}
	
	const TCHAR* GetText( INT32 nLine) const;

	void	SetLogFont( const LOGFONT* pLogFont);

protected:
	DECLARE_MESSAGE_MAP()

	void	checkScrollBar( void);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


#endif //__I3_TDK_LOG_CTRL_H