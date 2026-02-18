#pragma once
#include "afxcmn.h"

// i3TDKOptSpecBase dialog

class I3_EXPORT_TDK i3TDKOptSpecBase : public CDialog
{
	DECLARE_DYNAMIC(i3TDKOptSpecBase)

protected:
	i3SceneOptimizer *		m_pOpt = nullptr;

public:
	i3TDKOptSpecBase(UINT id, CWnd* pParent = nullptr) : CDialog(id, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecBase() {}

	void			SetOptimizer( i3SceneOptimizer * pOpt)		{ m_pOpt = pOpt; }
};
