#pragma once


// i3PropertySheetCtrl

class i3PropertySheetCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3PropertySheetCtrl)

protected:
	i3::vector<CWnd*>		m_WndList;
	CWnd *		m_pCurActive;

public:
	i3PropertySheetCtrl();
	virtual ~i3PropertySheetCtrl();

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

	INT32	AddPanel( CWnd * pWnd);
	INT32	FindPanel( CWnd * pWnd);
	void	SetActivePanel( INT32 idx);
	void	SetActivePanel( CWnd * pWnd);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


