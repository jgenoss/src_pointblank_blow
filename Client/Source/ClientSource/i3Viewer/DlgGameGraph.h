#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxwin.h"
#include "i3TDK.h"

// CDlgGameGraph dialog

class CDlgGameGraph : public CDialog
{
	DECLARE_DYNAMIC(CDlgGameGraph)

protected:
	i3TDKGameGraphTreeCtrl		m_Ctrl;

protected:
	void	PlaceCtrls( INT32 cx, INT32 cy);
	void	UpdateAll(void);
	void	_Rec_Update( i3GameNode * pNode, HTREEITEM hParent);

public:
	CDlgGameGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGameGraph();

// Dialog Data
	enum { IDD = IDD_GAMEGRAPH};

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
};

#endif
