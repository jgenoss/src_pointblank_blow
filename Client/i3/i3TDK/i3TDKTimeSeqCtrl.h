#pragma once

#include "i3TDKUtilGDI.h"

enum
{
	TSQBMP_TRACK_NORMAL_LEFT = 0,
	TSQBMP_TRACK_NORMAL_BODY,
	TSQBMP_TRACK_NORMAL_RIGHT,

	TSQBMP_TRACK_SELECT_LEFT,
	TSQBMP_TRACK_SELECT_BODY,
	TSQBMP_TRACK_SELECT_RIGHT,

	TSQBMP_TRACK_DISABLE_LEFT,
	TSQBMP_TRACK_DISABLE_BODY,
	TSQBMP_TRACK_DISABLE_RIGHT,

	TSQBMP_SLIDE_NORMAL_LEFT,
	TSQBMP_SLIDE_NORMAL_BODY,
	TSQBMP_SLIDE_NORMAL_RIGHT,

	TSQBMP_SLIDE_SELECT_LEFT,
	TSQBMP_SLIDE_SELECT_BODY,
	TSQBMP_SLIDE_SELECT_RIGHT,

	TSQBMP_SLIDE_DISABLE_LEFT,
	TSQBMP_SLIDE_DISABLE_BODY,
	TSQBMP_SLIDE_DISABLE_RIGHT,

	TSQBMP_QUAGE,
	TSQBMP_SLIDE_BACK,

	TSQBMP_NUMS
};

enum	NC_HIT
{
	NC_HIT_NONE = -1,
	NC_HIT_RULER,
	NC_HIT_GUAGE,
	NC_HIT_TRACK,
	NC_HIT_TRACK_ENABLE,
	NC_HIT_SLIDE_HEAD,
	NC_HIT_SLIDE_BODY,
	NC_HIT_SLIDE_TAIL,
	NC_HIT_SLIDE_EMPTY,
};

// i3TDKTimeSeqCtrl

class I3_EXPORT_TDK i3TDKTimeSeqCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKTimeSeqCtrl)

protected:
	enum	NODE_TYPE
	{
		NODE_TYPE_NORMAL = 0,
		NODE_TYPE_SOUND,
		NODE_TYPE_PARTICLE,
	};

	struct NODE_INFO
	{
		NODE_TYPE		m_Type = NODE_TYPE::NODE_TYPE_NORMAL;
		REAL32			m_Duration = 0.0f;
	} ;

	enum MOUSE_MODE
	{
		MOUSE_MODE_NA = 0,
		MOUSE_MODE_SCROLL,
		MOUSE_MODE_TRACK_DRAG,
		MOUSE_MODE_SLIDE_MOVE,
		MOUSE_MODE_SLIDE_RESIZE,
		MOUSE_MODE_GUAGE_MOVE,
	};

protected:
	HPEN			m_hpenBorder = nullptr;
	HBRUSH			m_hbrBack = nullptr;

	HFONT			m_hFont = nullptr;

	HDC				m_hdcDraw = nullptr, m_hdcTemp = nullptr;
	HBITMAP			m_hbmpDraw = nullptr, m_hbmpOld = nullptr;

	INT32			m_ClientWidth = 0, m_ClientHeight = 0;

	i3TimeSequence *	m_pTimeSeq = nullptr;

	HBITMAP			m_hbmpRulerBack = nullptr;
	I3TDK_BMPINFO	m_hbmpTable[TSQBMP_NUMS];

	REAL32			m_ShowStartTime = 0.0f;
	INT32			m_ShowStartIndex = 0;
	REAL32			m_ZoomRate = 1.0f;
	REAL32			m_timeGuage = 0.0f;
	REAL32			m_MaxDuration = 1.0f;

	MOUSE_MODE		m_MouseMode = MOUSE_MODE_NA;
	CPoint			m_DragStartPt;
	INT32			m_DragStartIndex = -1;

	REAL32			m_SavedStartTime = 0.0f;
	INT32			m_SavedStartIndex = 0;

	REAL32			m_SavedDurationTime = 0.0f;
	NC_HIT			m_CurrentNcHit = NC_HIT_NONE;
	bool			m_bUseSlideCustomSize = false;

	HRGN			m_hRegion = nullptr;
	HRGN			m_hRulerRgn = nullptr;

	char			m_chTitle[MAX_PATH] = "Time Line";

public:
	void			_GetRulerRect( RECT * pRect);
	void			_GetGuageRect( RECT * pRect);
	void			_GetTrackRect( INT32 idx, RECT * pRect);
	void			_GetTrackEnableRect( INT32 idx, RECT * pRect);
	void			_GetSlideRect( INT32 idx, RECT * pHeadRect, RECT * pBodyRect, RECT * pTailRect);
	void			_GetSlideRect( INT32 idx, RECT * pRect);

	void			_DrawRuler(void);
	void			_DrawItems(void);
	void			_DrawItem( RECT * pRect, i3TimeSequenceInfo * pInfo);
	void			_Draw(void);

	bool			_Rec_AnalyzeNode( i3Node * pNode, NODE_INFO * pInfo);
	void			_AnalyzeNode( i3Node * pNode, NODE_INFO * pInfo);
	void			_UpdateScrollInfo(void);
	void			_SetTitleText(char* pchtext) {strcpy_s(m_chTitle, pchtext); }
	INT32			_tm2pix( REAL32 tm)
	{
		// Zoom 100%일 때, 2 ms가 1 Pixel에 해당한다. 즉, 1초는 500 Pixel 길이로 표현된다.
		return (INT32)(((tm - m_ShowStartTime) * 500.0f) * m_ZoomRate);
	}

	INT32			_tm2pix_sz( REAL32 tm)
	{
		// Zoom 100%일 때, 2 ms가 1 Pixel에 해당한다. 즉, 1초는 500 Pixel 길이로 표현된다.
		return (INT32)((tm * 500.0f) * m_ZoomRate);
	}

	REAL32			_pix2tm( INT32 pos)
	{
		return (REAL32)((pos * 0.002f / m_ZoomRate) + m_ShowStartTime);
	}

	REAL32			_pix2tm_sz( INT32 pos)
	{
		return (REAL32)(pos * 0.002f / m_ZoomRate);
	}

	virtual void		OnSelectAll(void);
	virtual void		OnSelect( INT32 idx);
	virtual void		OnUnselectAll(void);
	virtual void		OnDelete(void);
	virtual void		OnCopy(void);
	virtual void		OnPaste(void);
	virtual void		OnRename( INT32 idxSel);

public:
	i3TDKTimeSeqCtrl() {}
	virtual ~i3TDKTimeSeqCtrl();

	void			SetTimeSequence( i3TimeSequence * pTimeSeq);

	void			UpdateSequence( i3TimeSequenceInfo * pInfo);
	void			AddSequence( i3TimeSequenceInfo * pInfo);

	REAL32			GetTotalDuration(void)								{ return m_MaxDuration; }

	BOOL Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

	NC_HIT			HitTest( POINT pt);
	INT32			FindItem( POINT pt, NC_HIT * pHitTest = nullptr);
	INT32			FindInsertPos( POINT pt);
	void			EnsureVisible( REAL32 tm);

	virtual	void	OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx);
	virtual	void	OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx);
	virtual void	OnChangeCurrentTime( REAL32 tm);
	virtual	void	OnChangeTimeSequence(void);
	virtual void	OnChangeItemEnable( i3TimeSequenceInfo * pInfo);

	REAL32 GetTimeGauage() { return m_timeGuage; }
	void SetTimeGauage(REAL32 time) { m_timeGuage = time; }

	void CursorStyleChange();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


