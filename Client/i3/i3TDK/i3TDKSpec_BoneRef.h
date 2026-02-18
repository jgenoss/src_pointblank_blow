#pragma once
#include "afxwin.h"


// i3TDKSpec_BoneRef dialog

class i3TDKSpec_BoneRef : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_BoneRef)

protected:
	i3BoneRef *		m_pBoneRef = nullptr;

protected:
	i3Body *		_FindBody(void);

	void			_UpdateBoneList( i3Body * pBody);

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_BoneRef(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_BoneRef::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_BoneRef();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_BONEREF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CB_Bones;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnCbnSelchangeTdkCbBone();
};
