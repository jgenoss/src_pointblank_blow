#if !defined( __I3_TDK_LISTCTRL_EDIT_H)
#define __I3_TDK_LISTCTRL_EDIT_H

//	=================================================================================
//	i3TDKListCtrlEdit
//	Programed : komet
//	Build Date : 2006.6.14
//	note : 
//	=================================================================================
#pragma once


// i3TDKListCtrlEdit
//#define DEFAULT_EDIT_BUFFER_SIZE		256

#define LCEB_KILLFOCUSDESTROY	0x00000001		//	포커스를 잃어도 제거되지 않는다.
#define LCEB_FISRTSELECTION		0x00000002		//	처음 Open시 텍스트가 선택되어 나온다.

struct LCEBInitStruct
{
	INT32			_nItem = 0;				//	i3SubItem의 Row 번호
	INT32			_nSubItem = 0;			//	i3SubItem의 Column 번호
	const TCHAR*	_pszInitText = nullptr;		//	초기화 Label
	UINT32			_nType = 0;

} ; // i3TDKListCtrlEdit 초기화 구조체

//////////////////////////////////////////////////////////////////////////////////////////////////////

class i3TDKListCtrlEdit : public CEdit
{
	DECLARE_DYNAMIC(i3TDKListCtrlEdit)

protected:
	INT32		m_nItem = -1;
	INT32		m_nSubItem = -1;

#ifdef _UNICODE
	i3::wstring m_strText;
#else
	i3::string	m_strText;
#endif

	UINT32		m_nType = 0;

	BOOL		m_bActive = FALSE;

public:
	i3TDKListCtrlEdit() {}
	virtual ~i3TDKListCtrlEdit() {}

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID);
	void	Init( LCEBInitStruct * pInitStruct);
	void	Excute( BOOL bBool, const TCHAR* pszInitText);

	void	SetEditText( const TCHAR* pszText);
	const TCHAR* GetEditText( void)	const	{	return m_strText.c_str();		}
	INT32	GetEditTextSize( void) const		{	return (INT32) m_strText.size();	}

	BOOL	IsActive( void)					{	return m_bActive;				}
	INT32	GetItemNum( void)				{	return m_nItem;					}
	INT32	GetSubItemNum( void)			{	return m_nSubItem;				}


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#endif //__I3_TDK_LISTCTRL_EDIT_H


