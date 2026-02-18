// PanelSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelSpec.h"
#include "GICShapeAI.h"
#include "LOD.h"

// CPanelSpec dialog

IMPLEMENT_DYNAMIC(CPanelSpec, i3TDKDialogContainer)
CPanelSpec::CPanelSpec(CWnd* pParent /*=NULL*/)
	: i3TDKDialogContainer(CPanelSpec::IDD, pParent)
{
}

CPanelSpec::~CPanelSpec()
{
}

void CPanelSpec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelSpec, i3TDKDialogContainer)
END_MESSAGE_MAP()


// CPanelSpec message handlers

BOOL CPanelSpec::OnInitDialog()
{
	i3TDKDialogContainer::OnInitDialog();

	{
		m_SpecAI.Create( CSpecAI::IDD, this);
		AddSpecDialog( AI::static_meta(), &m_SpecAI);

		m_SpecAIState.Create( CSpecAIState::IDD, this);
		AddSpecDialog( GICShapeAI::static_meta(), &m_SpecAIState);

		m_SpecLOD.Create( CSpecLOD::IDD, this);
		AddSpecDialog( CLOD::static_meta(), &m_SpecLOD);

		m_SpecChara.Create( CSpecChara::IDD, this);
		AddSpecDialog( CChara::static_meta(), &m_SpecChara);

		m_SpecAnim.Create( CSpecAnim::IDD, this);
		AddSpecDialog( i3Animation::static_meta(), &m_SpecAnim);

		m_SpecSoundNode.Create( CSpecSoundNode::IDD, this);
		AddSpecDialog( i3SoundNode::static_meta(), &m_SpecSoundNode);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
