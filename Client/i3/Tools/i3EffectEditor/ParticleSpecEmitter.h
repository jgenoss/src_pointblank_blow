#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CParticleSpecEmitter dialog

class CParticleSpecEmitter : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CParticleSpecEmitter)

public:
	CParticleSpecEmitter(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleSpecEmitter();

// Dialog Data
	enum { IDD = IDD_PARTICLESPEC_EMITTER };

protected:
	i3ParticleInfo *	m_pInfo;

public:
	void	SetParticle( i3ParticleInfo * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_Type;

	CEdit m_AngleCtrl;
	CSpinButtonCtrl m_AngleSpinCtrl;
	CEdit m_UnitCtrl;
	CSpinButtonCtrl m_UnitSpinCtrl;
	CButton m_MeshSelCtrl;
	CEdit m_MeshNameCtrl;
	CButton m_RandomMeshCtrl;
	
	virtual BOOL OnInitDialog();

protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnCbnSelchangeCbEmitType();
	afx_msg void OnBnClickedBtnMesh();
	CEdit m_OffsetXCtrl;
	CEdit m_OffsetYCtrl;
	CEdit m_OffsetZCtrl;
	CEdit m_DirXCtrl;
	CEdit m_DirYCtrl;
	CEdit m_DirZCtrl;
	CEdit m_UpXCtrl;
	CEdit m_UpYCtrl;
	CEdit m_UpZCtrl;
	CEdit m_IntervalCtrl;
	afx_msg void OnEnKillFocus();
	afx_msg void OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedMeshRandomCtrl();

private:
	void InitSpinCtrl();
	void MeshControlEnable(BOOL isEnable);
};
