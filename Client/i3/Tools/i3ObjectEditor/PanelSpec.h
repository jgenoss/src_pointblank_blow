#pragma once

#include "SpecState.h"
#include "SpecObject.h"
#include "SpecActionEmitEffect.h"
#include "SpecActionVisible.h"
#include "SpecActionDynamicState.h"
#include "SpecActionPath.h"
#include "SpecActionExplosion.h"
#include "SpecActionSound.h"
#include "SpecActionDisable.h"

// CPanelSpec dialog

class CPanelSpec : public i3TDKDialogContainer
{
	DECLARE_DYNAMIC(CPanelSpec)
protected:
	CSpecState				m_SpecState;	
	CSpecObject				m_SpecObject;

	CSpecActionEmitEffect	m_SpecActionEmitEffect;
	CSpecActionVisible		m_SpecActionVisible;
	CSpecActionDynamicState	m_SpecActionDynamicState;
	SpecActionPath			m_SpecActionPath;
	CSpecActionExplosion	m_SpecActionExplosion;
	CSpecActionSound		m_SpecActionSound;
	SpecActionDisable		m_SpecActionDisable;

public:

public:
	CPanelSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSpec();

// Dialog Data
	enum { IDD = IDD_PANEL_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;
	virtual void OnCancel() override;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
};
