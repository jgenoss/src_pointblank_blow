#pragma once
#include "afxwin.h"


// CUIUVSelectorDlg dialog
enum COLOR_CHANNEL
{
	COLOR_CHANNEL_A = 0,
	COLOR_CHANNEL_R,
	COLOR_CHANNEL_G,
	COLOR_CHANNEL_B,
	COLOR_CHANNEL_RGB,
	COLOR_CHANNEL_RGBA,
};

enum RECT_EXPANDTYPE {
	RECTEXPAND_LEFT = 0,
	RECTEXPAND_RIGHT,
	RECTEXPAND_TOP,
	RECTEXPAND_BOTTOM,
};


class CUIUVSelectorDlg : public CDialog
{
	DECLARE_DYNAMIC(CUIUVSelectorDlg)

protected:
	char		m_szWindowCaption[MAX_PATH];

	i3::vector<RECT*> m_listRect;
	i3Texture	*	m_pTexture;
	REAL32		m_u,m_v,m_w,m_h;
	COLORREAL	m_ColorKey;

	HBITMAP		m_hBitmap, m_hOldBitmap;
	HDC			m_MemDC;

	COLOR_CHANNEL	m_ColorChannel;
	bool			m_bColorKeySelecting;

	////////////////////////////////////////
	POINT			m_posStart;
	POINT			m_posScroll;
	REAL32			m_Zoom;

	bool			m_bDblClk;

	bool			m_bLButtonDown;


	//LeftDrag
	bool			m_bLButtonDrag;
	POINT			m_LDragStart;
	POINT			m_LDragMove;

	//RightDrag
	BOOL			m_RButtonDown;
	POINT			m_RDragStart;

	CSize			m_ImageAreaSize;
	CSize			m_WndSize;
	CRect			m_Rect;						// scroll에 의해 invalidate되는 경우에 쓰이는 영역

	bool			m_bHasInitialValue;
	bool			m_bSelected;
	i3::vector<i3::pack::RECT*>	m_SelectedUVList;		//i3::pack::RECT*	(기존의 i3::pack::RECT m_SelectedUV를 제거)

	CUIShapeGripManager m_pGrip;

public:
	CUIUVSelectorDlg(CWnd* pParent = NULL);		// standard constructor
	virtual ~CUIUVSelectorDlg();

// Dialog Data
	enum { IDD = IDD_UV_SELECTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	bool	_isSameColor( COLORREAL * pCol1, COLORREAL * pKey);
	void	_SaveColorKey(void);
	void	_LoadColorKey(void);


	bool	_PtInRect( i3::pack::RECT * pRT, POINT * pt);
	RECT*	 MakeNewRect(POINT pos, i3::vector<RECT*>& listRect);
	bool	CalculRect(i3Texture * pTexture, RECT *rt);
	void	ResultRect(i3Texture * pTexture, RECT *rt);
	void	RectExpend(RECT *rt, RECT_EXPANDTYPE type);

	void	_SetImage();
	void	_DeleteImage();
	void	_AnalysisImage( i3Texture * pTexture);
	void	_AnalysisPickImage(i3Texture * pTexture);
	INT32	_FindBefore( POINT *pt, i3::vector<RECT*>& listRect);

	bool		Scroll(int nBar , int ScrollAmount , LONG& pos);
	void		ResetScroll(int nBar , LONG& pos);
	void		SetScroll();

public:
	void	SetWindowCaption( const char * pszCaption){	i3::string_ncopy_nullpad( m_szWindowCaption, pszCaption, MAX_PATH);}
	void	SetSourceTexture( i3Texture * pTexture)	{	I3_REF_CHANGE( m_pTexture, pTexture);}
	void	setInitialTexCoord( REAL32 u, REAL32 v, REAL32 w, REAL32 h);
	void	SetTextureCoordinate( i3UIShape * pShape);
	void	SetTextureCoordinateFromFirstUV( i3UIShape* pShape);

	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewOption(UINT id);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CButton m_btnOK;
	CButton m_btnCancel;
	CButton	m_btnSelColorKey;
	CStatic m_stColor;
	i3TDKColorCtrl m_stColorCtrl;
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnBnClickedSelkey();


//LeftDrag
protected:
	bool			m_bLButtonDownOutSide;
	bool			m_bLDrag;
	POINT			m_PrevPt;

public:
	void			WillSelectRects( CPoint* startPt);
	void			IsSelectingRects( CPoint* curPt);
	void			DidSelectRects( CPoint* endPt);

	void			GetRects( CRect * pViewRect, CRect * pImageRect);

protected:
	CEdit m_edUVLeft;
	CEdit m_edUVTop;
	CEdit m_edUVRight;
	CEdit m_edUVBottom;

	void		UpdateUVValueToEdit( void);
public:
	afx_msg void OnEnKillfocusUVEdit();
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;

	
	i3::pack::RECT*		_OnSelectUV( i3::pack::RECT* pRt);	
	void		_ClearUVList( void);

public:
	INT32		getSelectedUVCount( void)		{	return (INT32)m_SelectedUVList.size();	}
	void		UnSelectFirstUV( void);

};
