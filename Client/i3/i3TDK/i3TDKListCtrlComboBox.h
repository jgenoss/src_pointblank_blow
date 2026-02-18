#if !defined( __I3_TDK_LISTCTRL_COMBOBOX_H)
#define __I3_TDK_LISTCTRL_COMBOBOX_H
//	=================================================================================
//	i3TDKListCtrlComboBox
//	Programed : komet
//	Build Date : 2006.6.14
//	note : 
//	=================================================================================
#pragma once


#define LCCB_KILLFOCUSDESTROY	0x00000001		//	포커스를 잃어도 제거되지 않는다.

// Init Structure
struct LCCBInitStruct
{
	INT32			_nItem = 0;				//	i3SubItem의 Row 번호
	INT32			_nSubItem = 0;			//	i3SubItem의 Column 번호	
	UINT32			_nType = 0;

} ; // i3TDKListCtrlComboBox 초기화 구조체

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class i3TDKListCtrlComboBox : public CComboBox
{
	DECLARE_DYNAMIC(i3TDKListCtrlComboBox)
protected:
	INT32		m_nItem = -1;
	INT32		m_nSubItem = -1;
	UINT32		m_nType = 0;
	BOOL		m_bActive = FALSE;

public:
	i3TDKListCtrlComboBox() {}
	virtual ~i3TDKListCtrlComboBox() {}

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID);

	//	pInitStruct 구조체에 아이템 리스트 _pItemList가 포함되어야 아이템 요소들이 셋팅된다.
	void	Init( LCCBInitStruct * pInitStruct);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL	IsActive( void)					{	return m_bActive;				}
	INT32	GetItemNum( void)				{	return m_nItem;					}
	INT32	GetSubItemNum( void)			{	return m_nSubItem;				}

	//	ComboBox Element
	INT32	AddElement( const TCHAR * pszString)					{	return AddString( pszString);				}
	INT32	AddElements( const TCHAR** pszStrings, INT32 nCount)	
			{	
				INT32 count = 0;
				for( INT32 i = 0; i < nCount; i++)
					count = AddElement( pszStrings[ i]);
				
				return count;
			}
	INT32	InsertElement( INT32 idx, const TCHAR* pszString)	{	return InsertString( idx, pszString);			}
	INT32	DeleteElement( INT32 idx)							{	return DeleteString( idx);						}
	void	ClearElement( void)									{	ResetContent();									} 
	INT32	GetElementCount( void)								{	return GetCount();								}	
	INT32	GetElement( INT32 idx, TCHAR * pBuffer)				{	return GetLBText( idx, pBuffer);				}
	INT32	GetSelectedElement( TCHAR * pBuffer)					{	return GetLBText( GetSelectedIdx(), pBuffer);	}
	INT32	GetSelectedIdx( void)									{	return GetCurSel();							}
	INT32	SelectElement( INT32 idx)								{	return SetCurSel( idx);						}
	INT32	SelectElement( INT32 startidx, const TCHAR * pszString)	{	return SelectString( startidx, pszString);	}
	INT32	FindElement( INT32 startidx, const TCHAR * pszString)	{	return FindString( startidx, pszString);	}
	
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnCbnCloseup();
	afx_msg void OnCbnSelchange();
};

#endif //__I3_TDK_LISTCTRL_COMBOBOX_H



