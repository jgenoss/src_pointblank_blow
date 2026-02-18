// SpecTextureSequence.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "SpecTextureSequence.h"
#include ".\spectexturesequence.h"
#include "GlobalVariable.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecTextureSequence dialog

IMPLEMENT_DYNAMIC(CSpecTextureSequence, CDialog)
CSpecTextureSequence::CSpecTextureSequence(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecTextureSequence::IDD, pParent)
	, m_TextureWidth(0)
	, m_TextureHeight(0)
{
}

CSpecTextureSequence::~CSpecTextureSequence()
{
}

void CSpecTextureSequence::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TIME, m_StaticTimeCtrl);
	DDX_Control(pDX, IDC_REPEAT, m_RepeatCtrl);
	DDX_Control(pDX, IDC_RANDOM, m_RandomCtrl);
	DDX_Control(pDX, IDC_FRAME_COUNT, m_FrameCountCtrl);
	DDX_Control(pDX, IDC_FRAME_INTERVAL, m_FrameIntervalCtrl);
	DDX_Control(pDX, IDC_FRAME_WIDTH, m_FrameWidthCtrl);
	DDX_Control(pDX, IDC_FRAME_HEIGHT, m_FrameHeightCtrl);
	DDX_Text(pDX, IDC_TEXTURE_WIDTH, m_TextureWidth);
	DDX_Text(pDX, IDC_TEXTURE_HEIGHT, m_TextureHeight);
	DDX_Control(pDX, IDC_CHECK1, m_FixedFrameCtrl);
	DDX_Control(pDX, IDC_FRAME_STARTINDEX, m_StartIndexCtrl);
}

BOOL CSpecTextureSequence::Execute( i3Particle * pParticle)
{
	m_pParticle = pParticle;

	if( DoModal() == IDCANCEL)
		return FALSE;

	return TRUE;
}

void CSpecTextureSequence::AssignValues(void)
{
	i3TextureSequence * pTexSeq;
	i3Texture * pTex;
	char conv[256];

	pTexSeq = m_pParticle->GetTextureSequence();

	if( pTexSeq != NULL)
	{
		// Static TIme
		if( pTexSeq->GetUseStaticTime())
			m_StaticTimeCtrl.SetCheck( BST_CHECKED);
		else
			m_StaticTimeCtrl.SetCheck( BST_UNCHECKED);

		// Repeat
		if( pTexSeq->IsRepeat())
			m_RepeatCtrl.SetCheck( BST_CHECKED);
		else
			m_RepeatCtrl.SetCheck( BST_UNCHECKED);

		// Random
		if( pTexSeq->IsRandomMode())
			m_RandomCtrl.SetCheck( BST_CHECKED);
		else
			m_RandomCtrl.SetCheck( BST_UNCHECKED);

		// Fixed Frame Mode
		if( pTexSeq->IsFixedFrameMode())
			m_FixedFrameCtrl.SetCheck( BST_CHECKED);
		else
			m_FixedFrameCtrl.SetCheck( BST_UNCHECKED);

		// Frame Count
		sprintf( conv, "%d", pTexSeq->GetCount());
		m_FrameCountCtrl.SetWindowText( conv);

		// Statt Inddex
		sprintf( conv, "%d", pTexSeq->GetStartIndex());
		m_StartIndexCtrl.SetWindowText( conv);

		// Frame Interval
		i3::ftoa( pTexSeq->GetStaticInterval() * 1000.0f, conv);
		m_FrameIntervalCtrl.SetWindowText( conv);

		// Frame Width
		sprintf( conv, "%d", (INT32)pTexSeq->GetFrameWidth());
		m_FrameWidthCtrl.SetWindowText( conv);

		// Frame Height
		sprintf( conv, "%d", (INT32)pTexSeq->GetFrameHeight());
		m_FrameHeightCtrl.SetWindowText( conv);
	}
	else
	{
		m_StaticTimeCtrl.SetCheck( BST_CHECKED);
		m_RepeatCtrl.SetCheck( BST_CHECKED);

		m_FrameCountCtrl.SetWindowText( "0");
		m_StartIndexCtrl.SetWindowText( "0");
		m_FrameWidthCtrl.SetWindowText( "0");
		m_FrameHeightCtrl.SetWindowText( "0");
		m_FrameIntervalCtrl.SetWindowText( "16.6");
	}

	pTex = m_pParticle->getTexture();
	if( pTex != NULL)
	{
		// Texture Width
		m_TextureWidth = pTex->GetWidth();

		// Texture Height
		m_TextureHeight = pTex->GetHeight();
	}
	else
	{
		m_TextureWidth = 0;
		m_TextureHeight = 0;
	}

	UpdateData( FALSE);
}

BEGIN_MESSAGE_MAP(CSpecTextureSequence, CDialog)
END_MESSAGE_MAP()


// CSpecTextureSequence message handlers

BOOL CSpecTextureSequence::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignValues();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecTextureSequence::OnOK()
{
	i3TextureSequence * pTexSeq;
	char conv[256];

	if( m_pParticle->getTexture() == NULL)
	{
		AfxMessageBox( "Texture가 지정되어 있지 않슴다. 반드시 해야함다");
		return;
	}

	EnterRenderCS();

	pTexSeq = m_pParticle->GetTextureSequence();

	if( pTexSeq == NULL)
	{
		pTexSeq = i3TextureSequence::new_object_ref();

		m_pParticle->SetTextureSequence( pTexSeq);
	}

	// Static TIme
	if( m_StaticTimeCtrl.GetCheck() == BST_CHECKED)
		pTexSeq->SetUseStaticTime( TRUE);
	else
		pTexSeq->SetUseStaticTime( FALSE);

	// Repeat
	if( m_RepeatCtrl.GetCheck() == BST_CHECKED)
		pTexSeq->SetRepeat( TRUE);
	else
		pTexSeq->SetRepeat( FALSE);

	// Random
	if( m_RandomCtrl.GetCheck() == BST_CHECKED)
		pTexSeq->SetRandomMode( TRUE);
	else
		pTexSeq->SetRandomMode( FALSE);

	// Fixed Frame
	if( m_FixedFrameCtrl.GetCheck() == BST_CHECKED)
		pTexSeq->SetFixedFrameMode( TRUE);
	else
		pTexSeq->SetFixedFrameMode( FALSE);

	// Frame Count
	m_FrameCountCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pTexSeq->SetCount( (UINT32) atoi( conv));

	// Start Index
	m_StartIndexCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pTexSeq->SetStartIndex( atoi( conv));

	// Frame Interval
	m_FrameIntervalCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pTexSeq->SetStaticInterval( (REAL32) (atof( conv) * 0.001));

	// Frame Width & Height
	{
		REAL32 w, h;

		m_FrameWidthCtrl.GetWindowText( conv, sizeof(conv) - 1);
		w = (REAL32) atof( conv);

		m_FrameHeightCtrl.GetWindowText( conv, sizeof( conv) - 1);
		h = (REAL32) atof( conv);

		pTexSeq->SetFrameSize( w, h);
	}

	pTexSeq->SetTexture( m_pParticle->getTexture());

	LeaveRenderCS();

	CDialog::OnOK();
}
