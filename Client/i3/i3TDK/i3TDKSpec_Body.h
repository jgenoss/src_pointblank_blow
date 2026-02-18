#pragma once
#include "afxwin.h"


// i3TDKSpec_Body dialog

class i3TDKSpec_Body : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Body)
protected:
	i3Body *		m_pBody = nullptr;
	i3ElementBase * m_pOldObj = nullptr;
	REAL32			m_fDist = 0.0f;			//오브젝트와 카메라와의 거리
	VEC3D		m_Pos;
	VEC3D		m_OffsetPos;

protected:
	INT32			GetTotalBoneCount(void);

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Body(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Body::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Body();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_BODY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	MATRIX* GetRootMatrix();
	void SetTextRotate(bool bZero = false);
	void SetTextPosition();
	void CalculateDist();

public:
	CButton m_CHK_AutoLOD;
	CEdit m_ED_Slope;
	CEdit m_ED_Const;
	CEdit m_ED_Anim;

	CEdit m_ED_RotateX, m_ED_RotateY, m_ED_RotateZ, m_ED_RotateW;
	CEdit m_ED_PosX, m_ED_PosY, m_ED_PosZ;

	CSpinButtonCtrl m_SP_RotateX, m_SP_RotateY, m_SP_RotateZ;
	CSpinButtonCtrl m_SP_PosX, m_SP_PosY, m_SP_PosZ;
	//CButton m_BTN_RotateInit;

	QUATERNION m_Quat;

	MATRIX m_matOri;
	

	afx_msg void OnBnClickedTdkAutolod();
	afx_msg void OnEnKillfocusTdkEdSlope();
	afx_msg void OnEnKillfocusTdkEdConst();
	afx_msg void OnBnClickedTdkBtnFrameskel();
	afx_msg void OnBnClickedTdkBtnAnim();
	CButton m_BTN_Skel;
	CButton m_BTN_Anim;
	CStatic m_ST_BoneCount;
	afx_msg void OnDeltaposSpinRotateX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinRotateY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinRotateZ(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDeltaposSpinPosX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinPosY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinPosZ(NMHDR *pNMHDR, LRESULT *pResult);

	//afx_msg void OnDeltaposSpinValueX(NMHDR *pNMHDR, LRESULT *pResult);

	void UpdateRotate(VEC3D vecAxis);
	void UpdateTranslate(VEC3D vecPos);

	//afx_msg void OnBnClickedTdkBtnRotateinit();
	afx_msg void OnBnClickedTdkCheckboxUseRotate();
	void UseObjectRotate(bool bCheck);
	afx_msg void OnBnClickedBtnSkeletonbone();
	REAL32 GetDist(){ return m_fDist; }
};
