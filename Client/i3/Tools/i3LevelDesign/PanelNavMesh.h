#pragma once

#include "PanelNavMeshCommon.h"
#include "PanelNavMeshEdit.h"
#include "PanelNavMeshGenerator.h"


class CPanelNavMesh : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelNavMesh)

public:
	CPanelNavMesh( CWnd* pParent = NULL );
	virtual ~CPanelNavMesh();

public:
	enum 
	{
		IDD = IDD_PANEL_NAVMESH 
	};

public:
	void		LevelEditModeChanged( LEVEL_EDIT_MODE editMode )
	{
		m_common.LevelEditModeChanged( editMode );
		m_edit.LevelEditModeChanged( editMode );
		m_generator.LevelEditModeChanged( editMode );
	}

private:
	i3TDKWndFoldCtrl		m_foldCtrl;

public:
	CPanelNavMeshCommon		m_common;
	CPanelNavMeshEdit		m_edit;
	CPanelNavMeshGenerator	m_generator;

protected:
	DECLARE_MESSAGE_MAP()

private:	
	virtual void	DoDataExchange( CDataExchange* pDX ) override;

	virtual BOOL	OnInitDialog() override;

	virtual void	OnOK() override {}
	virtual void	OnCancel() override {}

	virtual LRESULT		WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) override;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
