// SpecAIState.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "SpecAIState.h"
#include "GICShapeAI.h"
#include ".\specaistate.h"
#include "i3Base/i3StringListBuffer.h"

#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecAIState message handlers

BEGIN_MESSAGE_MAP(CSpecAIState, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_CB_ANIM, OnCbnSelchangeCbAnim)
	ON_CBN_SELCHANGE(IDC_CB_BLENDTYPE, OnCbnSelchangeCbBlendtype)
	ON_CBN_SELCHANGE(IDC_CB_NEXTSTATE, OnCbnSelchangeCbNextstate)
	ON_BN_CLICKED(IDC_CHK_ANIM_LOOP, OnBnClickedChkAnimLoop)
	ON_BN_CLICKED(IDC_CHK_TERM_TIME, OnBnClickedChkTermTime)
	ON_BN_CLICKED(IDC_CHK_TERM_WITHANIM, OnBnClickedChkTermWithanim)
	ON_EN_KILLFOCUS(IDC_ED_BLEND, OnEnKillfocusEdBlend)
	ON_EN_KILLFOCUS(IDC_ED_COMMENTTEXT, OnEnKillfocusEdCommenttext)
	ON_EN_KILLFOCUS(IDC_ED_FINISH_SCRIPT, OnEnKillfocusEdFinishScript)
	ON_EN_KILLFOCUS(IDC_ED_ID, OnEnKillfocusEdId)
	ON_EN_KILLFOCUS(IDC_ED_NAME, OnEnKillfocusEdName)
	ON_EN_KILLFOCUS(IDC_ED_RUN_SCRIPT, OnEnKillfocusEdRunScript)
	ON_EN_KILLFOCUS(IDC_ED_SRCTEXT, OnEnKillfocusEdSrctext)
	ON_EN_KILLFOCUS(IDC_ED_TIMELENGTH, OnEnKillfocusEdTimelength)
	ON_CBN_KILLFOCUS(IDC_CB_FILTER, OnCbnKillfocusCbFilter)
	ON_BN_CLICKED(IDC_CHK_VIEWPATH, OnBnClickedChkViewpath)
	ON_BN_CLICKED(IDC_CHK_ONLYNAME, &CSpecAIState::OnBnClickedChkOnlyname)
	ON_EN_KILLFOCUS(IDC_ED_ANIMNAME, &CSpecAIState::OnEnKillfocusEdAnimname)
END_MESSAGE_MAP()


// CSpecAIState dialog

IMPLEMENT_DYNAMIC(CSpecAIState, i3TDKDialogBase)
CSpecAIState::CSpecAIState(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecAIState::IDD, pParent)
{
}

CSpecAIState::~CSpecAIState()
{
}

void CSpecAIState::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_NAME, m_NameCtrl);
	DDX_Control(pDX, IDC_CB_ANIM, m_AnimCtrl);
	DDX_Control(pDX, IDC_ED_ID, m_IDCtrl);
	DDX_Control(pDX, IDC_ED_RUN_SCRIPT, m_RunScriptCtrl);
	DDX_Control(pDX, IDC_ED_FINISH_SCRIPT, m_FinishScriptCtrl);
	DDX_Control(pDX, IDC_CHK_TERM_WITHANIM, m_TermAnimCtrl);
	DDX_Control(pDX, IDC_CHK_TERM_TIME, m_TermTimeCtrl);
	DDX_Control(pDX, IDC_ED_TIMELENGTH, m_TimeLengthCtrl);
	DDX_Control(pDX, IDC_CB_NEXTSTATE, m_NextStateCtrl);
	DDX_Control(pDX, IDC_ED_BLEND, m_BlendTimeCtrl);
	DDX_Control(pDX, IDC_CB_BLENDTYPE, m_BlendTypeCtrl);
	DDX_Control(pDX, IDC_CHK_ANIM_LOOP, m_LoopCtrl);
	DDX_Control(pDX, IDC_ED_SRCTEXT, m_SrcTextCtrl);
	DDX_Control(pDX, IDC_ED_COMMENTTEXT, m_CommentTextCtrl);
	DDX_Control(pDX, IDC_CB_FILTER, m_FilterCtrl);
	DDX_Control(pDX, IDC_CHK_VIEWPATH, m_DisplayPathCtrl);
	DDX_Control(pDX, IDC_ED_ANIMNAME, m_ED_AnimName);
	DDX_Control(pDX, IDC_CHK_ONLYNAME, m_CHK_OnlyName);
}

void CSpecAIState::_UpdateAnimComboList(void)
{
	char szFilter[MAX_PATH];
	char conv[MAX_PATH];
//	char	conv2[MAX_PATH];
	BOOL	bShowPath;
	INT32 i, idx;

	m_FilterCtrl.GetWindowText( szFilter, sizeof(szFilter) - 1);

	if( m_DisplayPathCtrl.GetCheck() == BST_CHECKED)
		bShowPath = TRUE;
	else
		bShowPath = FALSE;

	m_AnimCtrl.ResetContent();

	// 셋팅을 지울수 있는 null 상태 추가.
	idx = m_AnimCtrl.AddString( "(null)");
	m_AnimCtrl.SetItemData( idx, NULL);

	// Chara가 소유하고 있는 모든 Animation들을 등록..
	{
		i3Animation * pAnim;

		for( i = 0; i < g_pChara->getAnimCount(); i++)
		{
			pAnim = g_pChara->getAnim( i);
			I3ASSERT( pAnim->hasName());

			if( szFilter[0] != 0)
			{
				// 편의를 위해 경로 비교할때 대소문자 가리지 않도록 komet
				if( i3::icontain_string( pAnim->GetName(), szFilter) == -1)
					continue;
			}

			if( bShowPath)
			{
				strcpy( conv, pAnim->GetName());
			}
			else
			{
				i3::stack_string title, dir;
				i3::extract_directoryname_filename(pAnim->GetName(), dir, title);
				i3::extract_filename(dir, dir);
				i3::remove_ext(title);
				i3::sprintf(conv, "%s\\%s", dir, title);

				/*
				char * pName = i3String::SplitFileName( pAnim->GetName(), conv, FALSE);
				if( pName != NULL )
				{
					INT32 nLength = i3::generic_string_size( pName );
					if( nLength > 0 )
					{
						char temp[MAX_PATH] = {0,};
					//	i3String::InsertStr( temp, (char*)pAnim->GetName(), 0, i3::generic_string_size(pAnim->GetName()) - (nLength+1) );
						i3::generic_string_ncopy(temp, pAnim->GetName(), i3::generic_string_size(pAnim->GetName()) - (nLength+1) ); 

						i3String::SplitFileName( temp, conv2, FALSE );
						strcpy( temp, conv );
						sprintf( conv, "%s\\%s", conv2, temp );
					}
				}
				*/
			}

			idx = m_AnimCtrl.AddString( conv);
			m_AnimCtrl.SetItemData( idx, (DWORD_PTR) pAnim);
		}
	}
}

void CSpecAIState::SetObject( i3ElementBase * pObj)
{
	INT32 i, idx;

	if( m_pObject == pObj)
		return;

	I3_REF_CHANGE(m_pObject, pObj);

	GICShapeAI * pShape = (GICShapeAI *) pObj;

	_UpdateAnimComboList();

	// 현재 AI가 소유하고 있는 모든 AI-State를 등록...
	I3ASSERT( m_pCurAI != NULL);
	{
		i3AI * pAI = m_pCurAI->getAI();

		m_NextStateCtrl.ResetContent();

		//	Next State에 NULL을 선택할 수 있도록 추가 by KOMET 2006.10.19
		idx = m_NextStateCtrl.AddString( "<NONE>");
		m_NextStateCtrl.SetItemData( idx, NULL);

		for( i = 0; i < pAI->getAIStateCount(); i++)
		{
			i3AIState * pState = pAI->getAIState( i);

			if( pShape->getAIState() == pState)
				continue;

			idx = m_NextStateCtrl.AddString( pState->GetName());
			m_NextStateCtrl.SetItemData( idx, (DWORD_PTR) pState);
		}
	}

	if( pShape != NULL)
	{
		UpdateAIStatePanel();
	}
	else
	{
		i3TDK::EnableChildCtrl( m_hWnd, FALSE);
	}
}



BOOL CSpecAIState::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	I3TRACE( "%x\n", m_SrcTextCtrl.m_hWnd);

	i3TDK::RegisterUpdate( m_hWnd, AI::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, GICShapeAI::static_meta(), I3_TDK_UPDATE_EDIT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecAIState::OnCancel()
{
}

void CSpecAIState::OnOK()
{
	char conv[256];

	if( getObject() == NULL)
		return;

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	// Name
	m_NameCtrl.GetWindowText( conv, sizeof(conv) - 1);
	if( conv[0] != 0)
		pState->SetName( conv);
	else
		pState->SetName( "");

	// Blending
	{
		// Time
		m_BlendTimeCtrl.GetWindowText( conv, sizeof(conv) - 1);
		pState->setBlendTime( (REAL32) atof( conv));
	}

	// ID
	m_IDCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pState->setID( atoi( conv));

	// Run Script
	m_RunScriptCtrl.GetWindowText( conv, sizeof(conv) - 1);
	if( conv[0] != 0)
		pState->SetRunLuaProcName( conv);
	else
		pState->SetRunLuaProcName( NULL);

	// Finish Script
	m_FinishScriptCtrl.GetWindowText( conv, sizeof(conv) - 1);
	if( conv[0] != 0)
		pState->SetFinishLuaProcName( conv);
	else
		pState->SetFinishLuaProcName( NULL);

	// Time Length
	m_TimeLengthCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pState->setDuration( (REAL32) atof( conv));

	// Source
	{
		CString txt;

		m_SrcTextCtrl.GetWindowText( txt);
	//	pShape->getSourceText()->CopyFromBuffer( (char *) LPCTSTR( txt));
		CopyFromBufferToStringList(pShape->getSourceText(), LPCTSTR(txt));
	}

	// Comment
	{
		CString txt;

		m_CommentTextCtrl.GetWindowText( txt);
	//	pShape->getCommentText()->CopyFromBuffer( (char *) LPCTSTR( txt));
		CopyFromBufferToStringList(pShape->getCommentText(), LPCTSTR(txt));
	}

	//_UpdateAnimComboList();


	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( i3::kind_of<AI*>(pInfo->m_pMeta)) //->IsTypeOf( AI::static_meta()))
		{
			if( pInfo->m_pObject != NULL)
			{
				m_pCurAI = (AI *) pInfo->m_pObject;
			}
			else
			{
				m_pCurAI = NULL;
				SetObject( NULL);
			}
		}
		else if( i3::kind_of<GICShapeAI*>(pInfo->m_pMeta)) //->IsTypeOf( GICShapeAI::static_meta()))
		{
			if( pInfo->m_pObject != NULL)
			{
				SetObject( pInfo->m_pObject);
			}
			else
			{
				SetObject( NULL);
			}
		}
	}
	else if(pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		UpdateAIStatePanel();
	}
}

LRESULT CSpecAIState::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CSpecAIState::OnCbnSelchangeCbAnim()
{
	INT32 idx;

	if( getObject() == NULL)
		return;

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	// Animation
	idx = m_AnimCtrl.GetCurSel();
	if( idx != -1)
	{
		i3Animation * pAnim = (i3Animation *) m_AnimCtrl.GetItemData( idx);

		pState->SetAnim( pAnim);

		if( pState->hasName() == false)
		{
			pShape->SetName( pAnim->GetName());
			pState->SetName( pAnim->GetName());
		}

		if(pAnim && pAnim->GetName())
			pState->setAnimName(pAnim->GetName());
		else
			pState->setAnimName("");
	}
	else
	{
		pState->SetAnim( NULL);
		pState->setAnimName("");
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnCbnSelchangeCbBlendtype()
{
	if( getObject() == NULL)
		return;

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	// Type
	if( m_BlendTypeCtrl.GetCurSel() == 0)
		pState->removeStyle( I3_AISTATE_STYLE_BLEND_WITH_STOP);
	else
		pState->addStyle( I3_AISTATE_STYLE_BLEND_WITH_STOP);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnCbnSelchangeCbNextstate()
{
	INT32 idx;

	if( getObject() == NULL)
		return;

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	// Next AI-State
	idx = m_NextStateCtrl.GetCurSel();
	if( idx != -1)
	{
		i3AIState * pNextState = (i3AIState *) m_NextStateCtrl.GetItemData( idx);

		pState->setNext( pNextState);
	}
	else
	{
		pState->setNext( NULL);
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnBnClickedChkAnimLoop()
{
	if( getObject() == NULL)
		return;

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	// Loop
	if( m_LoopCtrl.GetCheck() == BST_CHECKED)
		pState->addStyle( I3_AISTATE_STYLE_ANIM_LOOP);
	else
		pState->removeStyle( I3_AISTATE_STYLE_ANIM_LOOP);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnBnClickedChkTermTime()
{
	if( getObject() == NULL)
		return;

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	// Term. Time
	if( m_TermTimeCtrl.GetCheck() == BST_CHECKED)
		pState->addStyle( I3_AISTATE_STYLE_FIN_TIME);
	else
		pState->removeStyle( I3_AISTATE_STYLE_FIN_TIME);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnBnClickedChkTermWithanim()
{
	if( getObject() == NULL)
		return;

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	// Term. anim
	if( m_TermAnimCtrl.GetCheck() == BST_CHECKED)
		pState->addStyle( I3_AISTATE_STYLE_FIN_ANIM);
	else
		pState->removeStyle( I3_AISTATE_STYLE_FIN_ANIM);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdBlend()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();
	char conv[256];

	// Blending
	{
		// Time
		m_BlendTimeCtrl.GetWindowText( conv, sizeof(conv) - 1);
		pState->setBlendTime( (REAL32) atof( conv));
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdCommenttext()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	CString txt;

	m_CommentTextCtrl.GetWindowText( txt);

	CopyFromBufferToStringList(pShape->getCommentText(), LPCTSTR( txt));	//	pShape->getCommentText()->CopyFromBuffer( (char *) LPCTSTR( txt));
	
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdFinishScript()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();
	char conv[256];

	// Finish Script
	m_FinishScriptCtrl.GetWindowText( conv, sizeof(conv) - 1);
	if( conv[0] != 0)
		pState->SetFinishLuaProcName( conv);
	else
		pState->SetFinishLuaProcName( NULL);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdId()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();
	char conv[256];

	// ID
	m_IDCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pState->setID( atoi( conv));

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdName()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();

	if( pShape == NULL)
		return;

	i3AIState * pState = pShape->getAIState();
	char conv[256];

	m_NameCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pShape->SetName( conv);
	pState->SetName( conv);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdRunScript()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();
	char conv[256];

	// Run Script
	m_RunScriptCtrl.GetWindowText( conv, sizeof(conv) - 1);
	if( conv[0] != 0)
		pState->SetRunLuaProcName( conv);
	else
		pState->SetRunLuaProcName( NULL);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdSrctext()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	CString txt;

	m_SrcTextCtrl.GetWindowText( txt);

	CopyFromBufferToStringList(pShape->getSourceText(), LPCTSTR( txt));
	//pShape->getSourceText()->CopyFromBuffer( (char *) LPCTSTR( txt));
	
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, getObject());
}

void CSpecAIState::OnEnKillfocusEdTimelength()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();
	char conv[256];

	// Time Length
	m_TimeLengthCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pState->setDuration( (REAL32) atof( conv));
}

void CSpecAIState::OnCbnKillfocusCbFilter()
{
	_UpdateAnimComboList();
}

void CSpecAIState::OnBnClickedChkViewpath()
{
	_UpdateAnimComboList();
}

void CSpecAIState::OnBnClickedChkOnlyname()
{
	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	if( m_CHK_OnlyName.GetCheck() == BST_CHECKED)
	{
		pState->addStyle( I3_AISTATE_STYLE_ANIM_TEMPLATE);

		if( pState->getAnim() != NULL)
		{
			i3Animation * pAnim = pState->getAnim();

			pState->setAnimName( (char *) pAnim->GetName());
			m_ED_AnimName.SetWindowText( pAnim->GetName());
		}

		//pState->SetAnim( NULL);
		//m_AnimCtrl.SetCurSel( -1);
	}
	else
	{
		pState->removeStyle( I3_AISTATE_STYLE_ANIM_TEMPLATE);
	}
}

void CSpecAIState::OnEnKillfocusEdAnimname()
{
	char conv[256];

	GICShapeAI * pShape = (GICShapeAI *) getObject();
	i3AIState * pState = pShape->getAIState();

	m_ED_AnimName.GetWindowText( conv, sizeof(conv));

	pState->setAnimName( conv);
}

void CSpecAIState::UpdateAIStatePanel()
{
	INT32 idx;
	char conv[256];
	GICShapeAI * pShape = (GICShapeAI *)m_pObject;

	if(!pShape)
		return;

	i3AIState * pState = pShape->getAIState();

	// Name
	if( pState->hasName())
	m_NameCtrl.SetWindowText( pState->GetName());
	else
	m_NameCtrl.SetWindowText( "<NONE>");

	// Only Animation Name
	if( pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE)
	{
		m_CHK_OnlyName.SetCheck( BST_CHECKED);
	}
	else
	{
		m_CHK_OnlyName.SetCheck( BST_UNCHECKED);
	}

	// Animation Name
	m_ED_AnimName.SetWindowText( pState->getAnimName());

	// Animation
	if( pState->getAnim() != NULL)
	{
		idx = i3TDK::FindItemByData( &m_AnimCtrl, (DWORD_PTR) pState->getAnim());

		m_AnimCtrl.SetCurSel( idx);
	}
	else
	{
		m_AnimCtrl.SetCurSel( -1);
	}

	// Blending
	{
		i3::ftoa( pState->getBlendTime(), conv);
		m_BlendTimeCtrl.SetWindowText( conv);

		if( pState->getStyle() & I3_AISTATE_STYLE_BLEND_WITH_STOP)
			m_BlendTypeCtrl.SetCurSel( 0);
		else
			m_BlendTypeCtrl.SetCurSel( 1);

		if( pState->getStyle() & I3_AISTATE_STYLE_ANIM_LOOP)
			m_LoopCtrl.SetCheck( BST_CHECKED);
		else
			m_LoopCtrl.SetCheck( BST_UNCHECKED);
	}

	// ID
	sprintf( conv, "%d", pState->getID());
	m_IDCtrl.SetWindowText( conv);

	// Run Script
	if( pState->getRunLuaProcName() != NULL)
	m_RunScriptCtrl.SetWindowText( pState->getRunLuaProcName());
	else
	m_RunScriptCtrl.SetWindowText( "");

	// Finish Script
	if( pState->getFinishLuaProcName() != NULL)
	m_FinishScriptCtrl.SetWindowText( pState->getFinishLuaProcName());
	else
	m_FinishScriptCtrl.SetWindowText( "");

	// Term. anim
	if( pState->getStyle() & I3_AISTATE_STYLE_FIN_ANIM)
	m_TermAnimCtrl.SetCheck( BST_CHECKED);
	else
	m_TermAnimCtrl.SetCheck( BST_UNCHECKED);

	// Term. Time
	if( pState->getStyle() & I3_AISTATE_STYLE_FIN_TIME)
	m_TermTimeCtrl.SetCheck( BST_CHECKED);
	else
	m_TermTimeCtrl.SetCheck( BST_UNCHECKED);

	// Time Length
	i3::ftoa( pState->getDuration(), conv);
	m_TimeLengthCtrl.SetWindowText( conv);

	// Next AI-State
	if( pState->getNext() != NULL)
	{
		idx = i3TDK::FindItemByData( &m_NextStateCtrl, (DWORD_PTR) pState->getNext());

		m_NextStateCtrl.SetCurSel( idx);
	}
	else
	{
		m_NextStateCtrl.SetCurSel( -1);
	}

	// Source
	if(	 GetStringListBufferSize( pShape->getSourceText() )	> 0)
	{
		INT32 sz =	GetStringListBufferSize(pShape->getSourceText()); //	pShape->getSourceText()->GetBufferSize();
		char * pszBuf = new char[ sz + 1];

		//	pShape->getSourceText()->CopyToBuffer( pszBuf, sz + 1);
		CopyFromStringListToBuffer(pShape->getSourceText(), pszBuf, sz+1);

		m_SrcTextCtrl.SetWindowText( pszBuf);
		delete pszBuf;
	}
	else
	{
		m_SrcTextCtrl.SetWindowText( "");
	}

	// Comment
	if( GetStringListBufferSize(pShape->getCommentText()) > 0)
	{
		INT32 sz = GetStringListBufferSize(pShape->getCommentText());
		char * pszBuf = new char[ sz + 1];

		CopyFromStringListToBuffer(pShape->getCommentText(), pszBuf, sz+1);

		//	pShape->getCommentText()->CopyToBuffer( pszBuf, sz + 1);

		m_CommentTextCtrl.SetWindowText( pszBuf);
		delete pszBuf;
	}
	else
	{
		m_CommentTextCtrl.SetWindowText( "");
	}

}

void CSpecAIState::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	i3TDK::BatchAlignCtrls(i3TDK::EBATCH_ALGNCTL_RIGHT, 5, 0, this, 
		&m_AnimCtrl, &m_NameCtrl, &m_ED_AnimName, &m_NextStateCtrl,
		&m_RunScriptCtrl, &m_FinishScriptCtrl, &m_SrcTextCtrl, NULL);
}
