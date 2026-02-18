#pragma once

//class CNavMesh;

class CPanelNavMeshGenerator : public i3TDKDialogBase
{
	DECLARE_DYNAMIC( CPanelNavMeshGenerator )

public:
	CPanelNavMeshGenerator( CWnd* pParent = NULL );
	virtual ~CPanelNavMeshGenerator();

public:
	enum 
	{
		IDD = IDD_NAVMESH_GENERATOR
	};

	enum CONFIG_DATA_TYPE
	{
		CFG_CELL_SIZE			= 0,
		CFG_CELL_HEIGHT,
		CFG_EDGE_MAXLENGTH,
		CFG_EDGE_MAXERROR,
		CFG_REGION_MINSIZE,
		CFG_REGION_MERGESIZE,

		MAX_CONFIG_DATA_TYPE,
	};

public:
	void	InitCtrls();

	void	InitSliderCtrl();
	void	LevelEditModeChanged( LEVEL_EDIT_MODE editMode );

public:
	navMeshConfig		m_cfg;

public:
	CComboBox		m_comboType;

	CButton		m_btnReCreate;
	CButton		m_btnReset;
	CButton		m_btnMonotonePartition;

	CSliderCtrl			m_slider[ MAX_CONFIG_DATA_TYPE ];
	CStatic				m_text[ MAX_CONFIG_DATA_TYPE ];

private:
	float*		m_data[ MAX_CONFIG_DATA_TYPE ];
	float		m_min[ MAX_CONFIG_DATA_TYPE ];
	float		m_max[ MAX_CONFIG_DATA_TYPE ];
	float		m_tick[ MAX_CONFIG_DATA_TYPE ];

	bool		m_bNumber[ MAX_CONFIG_DATA_TYPE ];

private:
	DECLARE_MESSAGE_MAP()

private:
	virtual void	DoDataExchange(CDataExchange* pDX) override;

	virtual BOOL	OnInitDialog() override;

	virtual void	OnOK() override {}
	virtual void	OnCancel() override {}

public:
	afx_msg void	OnBnClickedReCreateNavmesh();
	afx_msg void	OnBnClickedNavmeshResetCfg();

	afx_msg void	OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );	
};
