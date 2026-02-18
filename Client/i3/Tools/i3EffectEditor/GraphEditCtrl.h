#pragma once
#include "i3EditCtrl.h"

// CGraphEditCtrl

struct CONTROLPOINT
{
	RECT rect;
};

class CGraphEditCtrl : public CWnd
{
	DECLARE_DYNAMIC(CGraphEditCtrl)

protected:
	i3KeyframeTable	*		m_pTable;
	i3::vector<CONTROLPOINT*>	m_CPList;
	REAL32					m_Min, m_Max;
	REAL32					m_XFactor, m_YFactor;
	INT32					m_YOrg;
	BOOL					m_bDrawZeroLine;

	HPEN					m_hAxisPen;
	HPEN					m_hLinePen;
	HBRUSH					m_hGraphBackBrush;
	HBRUSH					m_hCPBackBrush;
	HFONT					m_hFont;
	HCURSOR					m_hArrowCursor;
	HCURSOR					m_hCrossCursor;
	HCURSOR					m_hMoveCursor;

	CRect					m_ClientRect;
	CRect					m_GraphRect;

	i3EditCtrl				m_StepCtrl;

	bool					m_bDragging;
	INT32					m_DragCP;

	REAL32					m_LimitMin, m_LimitMax;
	bool					m_bLimitEnable;

protected:
	void	SetCP( CONTROLPOINT * pCP, INT32 x, INT32 y);
	void	AddCP( INT32 x, INT32 y);
	void	RemoveCP( INT32 idx);
	void	RemoveAllCP(void);
	void	ResetCP(void);
	INT32	FindCP( POINT pt);
	void	BuildCP(void);
	void	UpdateParams(void);
	void	SetTableCount( INT32 count);

	void	SetClientRect( INT32 cx, INT32 cy);

	void	DrawAxis( HDC dc);
	void	DrawPlotLine( HDC dc);
	void	DrawCP( HDC dc);
	void	DrawGraph( HDC dc);

	void	SetMouseCursor( HCURSOR hCursor)
	{
		SetCursor( hCursor);
		::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) hCursor);
	}

public:
	CGraphEditCtrl();
	virtual ~CGraphEditCtrl();

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

	i3KeyframeTable *	GetTable(void)				{ return m_pTable; }
	void	SetTable( i3KeyframeTable * pTable, REAL32 TimeLength = 0.0f, REAL32 limitMin = 0.0f, REAL32 limitMax = 0.0f, bool bLimit = false);


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


