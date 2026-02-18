#pragma once

#include "afxcmn.h"
#include "afxwin.h"

class i3NavMeshDataMgr;

class CCustomPorgressCtrl : public CProgressCtrl
{
public:
	virtual BOOL	OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult ) override
	{
		return CProgressCtrl::OnNotify( wParam, lParam, pResult );
	}
};


class CDlgNavmeshGenerator : public CDialog
{
	DECLARE_DYNAMIC( CDlgNavmeshGenerator )

public:
	CDlgNavmeshGenerator( CWnd* pParent = NULL );
	virtual ~CDlgNavmeshGenerator();

	enum { IDD = IDD_NAVMESH_GENERATOR_PROGRESS };

public:
	CCustomPorgressCtrl		m_progress;
	CStatic					m_txt;

public:
	CWinThread*			m_pTrhead;
	CRITICAL_SECTION	m_cs;

	int			m_pos;

public:
	navMeshConfig		 m_cfg;

	i3NavMeshDataMgr*			m_dataMgr;
	i3World*					m_pWorld;	

public:
	bool		DoAutoGenerate();

	void		UpdateLoadingRatio( float f, const char* sz );

public:
	INT_PTR		DoModal( i3NavMeshDataMgr* dataMgr, i3World* world, const navMeshConfig& cfg );

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void	DoDataExchange( CDataExchange* pDX ) override;

public:
	virtual BOOL	OnInitDialog() override;
	virtual void	OnOK() override {}
	virtual void	OnCancel() override {}

	afx_msg void	OnTimer(UINT_PTR nIDEvent);
};
