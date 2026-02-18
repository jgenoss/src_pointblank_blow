#pragma once
#include "afxcmn.h"
#include "i3TDKOptSpecBase.h"

// i3TDKOptSpecChangeCollisionGroup dialog

class I3_EXPORT_TDK i3TDKOptSpecChangeCollisionGroup : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecChangeCollisionGroup)

public:
	i3TDKOptSpecChangeCollisionGroup(CWnd* pParent = nullptr) : i3TDKOptSpecBase(IDD_DLG_CHANGECOLLISIONGROUP, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecChangeCollisionGroup() {}

	enum { IDD = IDD_DLG_CHANGECOLLISIONGROUP };

protected:
	CEdit			m_Ed_CollisionGroupIdx;
	UINT16			m_nCollisionGroup = 0;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
