#pragma once


// CGraphViewCtrl

class CGraphViewCtrl : public CWnd
{
	DECLARE_DYNAMIC(CGraphViewCtrl)

protected:
	HBITMAP	m_hBitmap, m_hOldBitmap;
	HDC		m_DC;
	HFONT	m_hFont, m_hOldFont;
	
	REAL32						m_Duration;

	i3ParticleInfo::FUNC_TYPE	m_Func[2];
	REAL32						m_Init[2];
	REAL32						m_Factor[2];
	i3KeyframeTable *			m_pTable[2];

	BOOL						m_bClamp;
	REAL32						m_MinValue;
	REAL32						m_MaxValue;

protected:
	void						Simulate( INT32 count, REAL32 * pResult);
	void						Draw(void);

public:
	void	Set( REAL32 duration, i3ParticleInfo::FUNC_TYPE func0, REAL32 init0, REAL32 factor0,
		i3ParticleInfo::FUNC_TYPE func1, REAL32 init1, REAL32 factor1,
		BOOL bClamp = FALSE, REAL32 minval = 0.0f, REAL32 maxval = 0.0f);

public:
	CGraphViewCtrl();
	virtual ~CGraphViewCtrl();

	BOOL	Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


