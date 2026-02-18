#pragma once


// i3ClutTableDlg dialog

class i3ClutTableDlg : public CDialog
{
	DECLARE_DYNAMIC(i3ClutTableDlg)

public:
	i3ClutTableDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~i3ClutTableDlg();

// Dialog Data
	enum { IDD = IDD_CLUT_TABLE };
protected:
	i3Clut * m_pClut;
	
	CPoint	 m_OldMousePos;
	INT32	 m_OldIndex;
	CRect	 m_rtClutRegion;
public:
	void	SetClut( i3Clut * pClut );
	void	SetColorValue( CPoint point );
	void	SetColorValue( UINT8	nIndex );

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
private:
	UINT m_CtrlRed;
	UINT m_CtrlGreen;
	UINT m_CtrlBlue;
	UINT m_CtrlAlpha;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
