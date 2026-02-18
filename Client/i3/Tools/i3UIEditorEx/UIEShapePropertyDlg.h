#pragma once
#include "afxwin.h"


// CUIEShapePropertyDlg dialog

class CUIEShapePropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(CUIEShapePropertyDlg)

protected:
	I3UI_SHAPE_INFO	*	m_pInfo;

	i3::rc_string	m_strInfoString;
	REAL32	m_X, m_Y;
	REAL32	m_Width;
	REAL32	m_Height;
	INT32	m_shapeCount;



protected:
	void	_UpdateInfo(void);

public:
	CUIEShapePropertyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUIEShapePropertyDlg();

	bool	Execute( REAL32 width, REAL32 height);

// Dialog Data
	enum { IDD = IDD_DLG_SHAPEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edInfoString;
	CEdit m_edWidth;
	CEdit m_edHeight;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog() override;

	void	setOnlyRectMode(bool bEnable)	{	m_bOnlyRectMode = bEnable;}

public:
	void	setShapeInfo( I3UI_SHAPE_INFO	*	pInfo)	{	m_pInfo = pInfo;}
	void	GetShapeInfo( I3UI_SHAPE_INFO * pInfo);
	INT32	GetShapeCount()								{	return m_shapeCount;}
	afx_msg void OnEnKillfocusEditShapeinfoString();
	afx_msg void OnEnKillfocusEditShapeinfoWidth();
	afx_msg void OnEnKillfocusEditShapeinfoHeight();
	CEdit m_edPosX;
	CEdit m_edPosY;
	CEdit m_edCount;
	afx_msg void OnEnKillfocusEditShapeinfoPosx();
	afx_msg void OnEnKillfocusEditShapeinfoPosy();
	afx_msg void OnEnKillfocusEditShapeCount();

protected:
	bool	m_bOnlyRectMode;
};
