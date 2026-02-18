// TimeSeqCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "TimeSeqCtrl.h"
#include "GlobalVar.h"
#include "DlgSelectSound.h"

// CTimeSeqCtrl

IMPLEMENT_DYNAMIC(CTimeSeqCtrl, i3TDKTimeSeqCtrl)
CTimeSeqCtrl::CTimeSeqCtrl()
{
	m_pShape = NULL;
}

CTimeSeqCtrl::~CTimeSeqCtrl()
{
}

void CTimeSeqCtrl::SetShape( GICShapeAI * pShape)
{
	m_pShape = pShape;

	if( pShape != NULL)
	{
		i3AIState * pState = pShape->getAIState();

		I3TRACE( "Shape : %s\n", pShape->GetName());

		I3ASSERT( pState != NULL);

		SetTimeSequence( pState->getTimeSeq());
	}
	else
	{
		SetTimeSequence( NULL);
	}
}

void CTimeSeqCtrl::_PrepareTimeSeq(void)
{
	i3AIState * pState = m_pShape->getAIState();

	if( pState->getTimeSeq() == NULL)
	{
		i3TimeSequence * pTimeSeq = i3TimeSequence::new_object_ref();

		pState->setTimeSeq( pTimeSeq);

		SetTimeSequence( pTimeSeq);
	}
}

void	CTimeSeqCtrl::OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx)
{
	if( m_pShape == NULL)
		return;

	pPopup->AppendMenu( MF_STRING, 100, "Add Scene-Graph...");
	pPopup->AppendMenu( MF_STRING, 101, "Add Sound...");
}

void	CTimeSeqCtrl::OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx)
{
	switch( cmd)
	{
		case 100 :		// Add Scene-graph
			OnLoadI3S( pPoint);
			break;

		case 101 :		// Add Sound
			OnLoadSound( pPoint);
			break;
	}
}

void	CTimeSeqCtrl::OnChangeCurrentTime( REAL32 tm)
{
	g_pChara->geti3Chara()->OnChangeTime( tm);

	g_pRenderPanel->Invalidate();
}

void	CTimeSeqCtrl::OnChangeTimeSequence(void)
{
	if( m_pShape == NULL)
		return;

	i3AIState * pState = m_pShape->getAIState();

	m_MaxDuration = MAX( m_MaxDuration, pState->getDuration());
	m_MaxDuration = MAX( m_MaxDuration, pState->getAnimDuration());
}

void CTimeSeqCtrl::OnLoadI3S( POINT * pPoint)
{
	if( m_pShape == NULL)
		return;

	CFileDialog	Dlg( TRUE, "I3S", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Scene-Graph File(*.I3S)|*.I3S||", this);

	if( Dlg.DoModal() != IDOK) return;

	i3SceneFile file;

	if( file.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.", LPCTSTR( Dlg.GetPathName()));
		return;
	}

	_PrepareTimeSeq();

	i3TimeSequenceInfo * pInfo = i3TimeSequenceInfo::new_object_ref();
	CPoint pt;
	REAL32 tm;

	tm = _pix2tm( pt.x);

	pInfo->SetStartTime( tm); 
	pInfo->SetNode( file.GetSceneGraph());

	m_pTimeSeq->AddSequence( pInfo);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeSeq);

	Invalidate();
}

void CTimeSeqCtrl::OnLoadSound( POINT * pPoint)
{
	if( m_pShape == NULL)
		return;

	CDlgSelectSound dlg;

	i3Sound * pSound;
	i3SoundPlayInfo * pPlayInfo;
	i3SoundNode * pNode;

	pSound = dlg.Execute();
	if( pSound == NULL)
	{
		return;
	}

	pPlayInfo = i3SoundPlayInfo::new_object_ref();
	pPlayInfo->setSound( pSound);
	pPlayInfo->SetStyle( I3SND_PROP_TYPE_EFFECT | I3SND_RT_3DMODE_DISABLE);
	pPlayInfo->SetDefaultVolume( 100);
	pPlayInfo->setListenerVolume( 100);

	pNode = i3SoundNode::new_object_ref();
	pNode->setSoundPlayInfo( pPlayInfo);
	pNode->setStartTime( 0.0f);

	_PrepareTimeSeq();

	i3TimeSequenceInfo * pInfo = i3TimeSequenceInfo::new_object_ref();
	REAL32 tm;
	CRect rt;

	_GetRulerRect( &rt);
	tm = _pix2tm( pPoint->x - rt.left);
	if( tm < 0)
		tm = 0.0f;

	if( tm > m_MaxDuration)
		tm = m_MaxDuration;

	pInfo->SetStartTime( tm); 
	pInfo->SetNode( pNode);

	m_pTimeSeq->AddSequence( pInfo);

	SetTimeSequence( m_pTimeSeq);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo);
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeSeq);

	Invalidate();
}

void CTimeSeqCtrl::OnSelect( INT32 idx)
{
	i3TDKTimeSeqCtrl::OnSelect( idx);

	i3TimeSequenceInfo * pInfo = m_pTimeSeq->GetSequence( idx);

	if( pInfo->GetNode() != NULL)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo->GetNode());
	}
}

BEGIN_MESSAGE_MAP(CTimeSeqCtrl, i3TDKTimeSeqCtrl)
END_MESSAGE_MAP()



// CTimeSeqCtrl message handlers

