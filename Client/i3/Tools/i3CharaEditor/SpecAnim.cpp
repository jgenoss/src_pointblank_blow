// SpecAnim.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "SpecAnim.h"
#include ".\specanim.h"
#include "i3Base/string/ext/ftoa.h"

// CSpecAnim dialog

IMPLEMENT_DYNAMIC(CSpecAnim, i3TDKDialogBase)
CSpecAnim::CSpecAnim(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecAnim::IDD, pParent)
{
}

CSpecAnim::~CSpecAnim()
{
}

void CSpecAnim::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_TRACKCOUNT, m_TrackCountCtrl);
	DDX_Control(pDX, IDC_ED_DURATION, m_DurationCtrl);
	DDX_Control(pDX, IDC_TRACKLIST, m_TrackListCtrl);
}

void CSpecAnim::_UpdateTrack( INT32 idx, i3TransformSequence * pSeq)
{
	char conv[64];

	m_TrackListCtrl.SetItemData( idx, (DWORD_PTR) pSeq);

	// Keys
	sprintf( conv, "%d", pSeq->GetKeyframeCount());
	m_TrackListCtrl.SetItemText( idx, 1, conv);

	// Duration
	i3::ftoa( pSeq->GetDuration(), conv);
	m_TrackListCtrl.SetItemText( idx, 2, conv);

	// Attrs
	conv[0] = 0;

	if( pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE)
		strcat( conv, "FT");
	else if( pSeq->GetKeyframeChannels() & i3TransformSequence::TRANSLATE)
		strcat( conv, "T");

	if( pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE)
		strcat( conv, "_FR");
	else if( pSeq->GetKeyframeChannels() & i3TransformSequence::ROTATE)
		strcat( conv, "_R");

	if( pSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE)
		strcat( conv, "_FS");
	else if( pSeq->GetKeyframeChannels() & i3TransformSequence::SCALE)
		strcat( conv, "_S");

	if( pSeq->GetKeyframeChannels() & i3TransformSequence::TIME)
		strcat( conv, "_TM");
	m_TrackListCtrl.SetItemText( idx, 3, conv);
}

void CSpecAnim::SetObject( i3ElementBase * pObj)
{
	i3Animation * pAnim;
	char conv[64];
	INT32 i, idx;

	I3ASSERT( i3::kind_of<i3Animation*>(pObj)); //->IsTypeOf( i3Animation::static_meta()));
	I3_REF_CHANGE(m_pObject, pObj);
	
	pAnim = (i3Animation *) pObj;

	if( pAnim != NULL)
	{
		// # of tracks
		sprintf( conv, "%d", pAnim->GetTrackCount());
		m_TrackCountCtrl.SetWindowText( conv);

		// duration
		i3::ftoa( pAnim->GetDuration(), conv);
		m_DurationCtrl.SetWindowText( conv);

		// Tracks
		m_TrackListCtrl.DeleteAllItems();

		for( i = 0; i < pAnim->GetTrackCount(); i++)
		{
			i3TransformSequence * pSeq = pAnim->GetTrack( i);

			idx = m_TrackListCtrl.InsertItem( i, pAnim->GetTrackBoneName( i), 0);

			_UpdateTrack( idx, pSeq);
		}
	}
	else
	{
		m_TrackCountCtrl.SetWindowText( "");
		m_DurationCtrl.SetWindowText( "");
		m_TrackListCtrl.DeleteAllItems();
	}
}

void CSpecAnim::OnDelete(void)
{
	INT32 i, cnt;
	i3Animation * pAnim = (i3Animation *) m_pObject;
	i3::pack::animation::TRACK_INFO * pNewTable, * pOldTable;

	I3ASSERT( pAnim != NULL);

	for( i = 0, cnt = 0; i < m_TrackListCtrl.GetItemCount(); i++)
	{
		if( m_TrackListCtrl.GetItemState( i, LVIS_SELECTED) == 0)
		{
			cnt++;
		}
	}

	if( cnt <= 0)
	{
		AfxMessageBox( "선택된 Track이 없습니다.");
		return;
	}

	pNewTable = (i3::pack::animation::TRACK_INFO *) i3MemAlloc( sizeof(i3::pack::animation::TRACK_INFO) * cnt);
	I3ASSERT( pNewTable != NULL);

	pOldTable = pAnim->getTrackArray();

	for( i = 0, cnt = 0; i < m_TrackListCtrl.GetItemCount(); i++)
	{
		if( m_TrackListCtrl.GetItemState( i, LVIS_SELECTED) == 0)
		{
			memcpy( &pNewTable[cnt], &pOldTable[i], sizeof( i3::pack::animation::TRACK_INFO));
			cnt++;
		}
		else
		{
			I3_SAFE_RELEASE( pOldTable[i].m_pSeq);
		}
	}

	for( i = 0; i < m_TrackListCtrl.GetItemCount(); i++)
	{
		if( m_TrackListCtrl.GetItemState( i, LVIS_SELECTED))
		{
			m_TrackListCtrl.DeleteItem( i);
			i--;
		}
	}

	pAnim->SetTrackCount( cnt);
	pAnim->setTrackArray( pNewTable);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pAnim);
}

BEGIN_MESSAGE_MAP(CSpecAnim, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_KEYDOWN, IDC_TRACKLIST, OnLvnKeydownTracklist)
	ON_NOTIFY(NM_RCLICK, IDC_TRACKLIST, OnNMRclickTracklist)
END_MESSAGE_MAP()


// CSpecAnim message handlers

BOOL CSpecAnim::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_TrackListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_TrackListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 100);
		m_TrackListCtrl.InsertColumn( 1, "Keys", LVCFMT_LEFT, 40);
		m_TrackListCtrl.InsertColumn( 2, "Duration", LVCFMT_LEFT, 55);
		m_TrackListCtrl.InsertColumn( 3, "Attrs", LVCFMT_LEFT, 70);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecAnim::OnCancel()
{
}

void CSpecAnim::OnOK()
{
}

void CSpecAnim::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TrackListCtrl.m_hWnd))
	{
		m_TrackListCtrl.SetWindowPos( NULL, 0, 0, cx - 6, cy - 32, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	}
}

void CSpecAnim::OnLvnKeydownTracklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	
	if( pLVKeyDown->wVKey == VK_DELETE)
	{
		OnDelete();
	}

	*pResult = 0;
}

void CSpecAnim::OnNMRclickTracklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
