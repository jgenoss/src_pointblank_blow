#pragma once

#include "SpecAI.h"
#include "SpecAISTate.h"
#include "SpecLOD.h"
#include "SpecChara.h"
#include "SpecAnim.h"
#include "SpecSoundNode.h"

// CPanelSpec dialog

class CPanelSpec : public i3TDKDialogContainer
{
	DECLARE_DYNAMIC(CPanelSpec)

protected:
	CSpecAI			m_SpecAI;
	CSpecAIState	m_SpecAIState;
	CSpecLOD		m_SpecLOD;
	CSpecChara		m_SpecChara;
	CSpecAnim		m_SpecAnim;
	CSpecSoundNode	m_SpecSoundNode;

public:
	CPanelSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSpec();

// Dialog Data
	enum { IDD = IDD_PANEL_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
