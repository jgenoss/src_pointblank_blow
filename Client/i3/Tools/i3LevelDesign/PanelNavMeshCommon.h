#pragma once

#include "i3Level/i3NavMeshUtil.h"

//class CNavMesh;

class CPanelNavMeshCommon : public i3TDKDialogBase
{
	DECLARE_DYNAMIC( CPanelNavMeshCommon )

public:
	CPanelNavMeshCommon( CWnd* pParent = NULL );
	virtual ~CPanelNavMeshCommon();

public:
	enum 
	{
		IDD = IDD_NAVMESH_COMMON 
	};

public:
	void	LevelEditModeChanged( LEVEL_EDIT_MODE editMode )
	{

	}

public:
	CStatic			m_filePath;
	CButton			m_chkBtnDisableTex;
	CButton			m_chkBtnWireframe;
	CButton			m_chkBtnVisible;
	
	CMFCButton		m_btnBlankTexColor;

private:
	std::string			m_path;
	std::string			m_ext;

private:
	struct _BindTexture
	{
		i3TextureBindAttr*	pbind = nullptr;
		i3Texture*			ptex = nullptr;
	};

private:
	i3Texture*						m_pBlankTex;
	i3::vector< _BindTexture >		m_BindTextureList;

	COLORREAL	m_crBlankTex;

private:
	void		_SetBlankTexture();
	void		_UnSetBlankTexture();

private:
	DECLARE_MESSAGE_MAP()

private:
	virtual void	DoDataExchange(CDataExchange* pDX) override;

	virtual BOOL	OnInitDialog() override;

	virtual void	OnOK() override {}
	virtual void	OnCancel() override {}

public:
	afx_msg void 	OnBnClickedNavmeshNmsave();	
	afx_msg void 	OnBnClickedNavmeshNmsaveAs();	
	afx_msg void 	OnBnClickedNavmeshNmload();	
	
	afx_msg void 	OnBnClickedNavmeshChkBlankTexture();
	afx_msg void 	OnBnClickedNavmeshBlankTextureColor();
	afx_msg void 	OnBnClickedNavmeshChkWireframe();
	afx_msg void 	OnBnClickedNavmeshChkVisible();

	afx_msg void 	OnBnClickedNavmeshReset();
};
