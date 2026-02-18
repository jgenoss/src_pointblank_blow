#include "StdAfx.h"
#include "MyTDKTimeSeqCtrl.h"
#include "DlgSelectSound.h"

IMPLEMENT_DYNAMIC(cMyTDKTimeSeqCtrl, i3TDKTimeSeqCtrl)

cMyTDKTimeSeqCtrl::cMyTDKTimeSeqCtrl(void)
{
}

cMyTDKTimeSeqCtrl::~cMyTDKTimeSeqCtrl(void)
{
}

BEGIN_MESSAGE_MAP(cMyTDKTimeSeqCtrl, i3TDKTimeSeqCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

void cMyTDKTimeSeqCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	//if(g_pViewer->IsState( I3_VIEWER_STATE_PAUSED))
		_Draw();

	::BitBlt( dc.m_hDC, 0, 0, m_ClientWidth, m_ClientHeight, m_hdcDraw, 0, 0, SRCCOPY);
}

void cMyTDKTimeSeqCtrl::OnSize(UINT nType, int cx, int cy)
{
	i3TDKTimeSeqCtrl::OnSize(nType,cx,cy);
	ReDraw();
}

void cMyTDKTimeSeqCtrl::ReDraw()
{
	CPaintDC dc(this); // device context for painting
	_Draw();
	::BitBlt( dc.m_hDC, 0, 0, m_ClientWidth, m_ClientHeight, m_hdcDraw, 0, 0, SRCCOPY);

}

BOOL cMyTDKTimeSeqCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( nFlags & MK_CONTROL)
	{
		// Zoom
		m_ZoomRate += ((REAL32) zDelta * 0.001f);

		if (m_ZoomRate < 0.28f)
			m_ZoomRate = 0.28f;
		else if (m_ZoomRate > 6.0f)
			m_ZoomRate = 6.0f;

		Invalidate();
	}
	ReDraw();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void cMyTDKTimeSeqCtrl::_PrepareTimeSeq(void)
{
	i3AIState * pState = m_pShape->getAIState();

	if( pState->getTimeSeq() == NULL)
	{
		i3TimeSequence * pTimeSeq = i3TimeSequence::new_object_ref();

		pState->setTimeSeq( pTimeSeq);

		SetTimeSequence( pTimeSeq);
	}
}

void	cMyTDKTimeSeqCtrl::OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx)
{
	if( m_pShape == NULL)
		return;

	pPopup->AppendMenu( MF_STRING, 100, "Add Scene-Graph...");
	pPopup->AppendMenu( MF_STRING, 101, "Add Sound...");
}

void	cMyTDKTimeSeqCtrl::OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx)
{
	switch( cmd)
	{
	case 101 :		// Add Sound
		OnLoadSound( pPoint);
		break;
	}
}

void	cMyTDKTimeSeqCtrl::OnChangeCurrentTime( REAL32 tm)
{
	g_pChara->geti3Chara()->OnChangeTime( tm);

	//g_pRenderPanel->Invalidate();
}

void	cMyTDKTimeSeqCtrl::OnChangeTimeSequence(void)
{
	if( m_pShape == NULL)
		return;

	i3AIState * pState = m_pShape->getAIState();

	m_MaxDuration = MAX( m_MaxDuration, pState->getDuration());
	m_MaxDuration = MAX( m_MaxDuration, pState->getAnimDuration());
}

void cMyTDKTimeSeqCtrl::OnLoadSound( POINT * pPoint)
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
