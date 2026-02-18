#include "stdafx.h"
#include "TimeLineCtrl.h"
#include "i3UIAnimation2DSprite.h"
#define MAX_TIME 60;

IMPLEMENT_DYNAMIC(CTimeLineCtrl, i3TDKTimeSeqCtrl)

BEGIN_MESSAGE_MAP(CTimeLineCtrl, i3TDKTimeSeqCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

CTimeLineCtrl::CTimeLineCtrl()
{
	m_bUseSlideCustomSize = true;
}

CTimeLineCtrl::~CTimeLineCtrl()
{
}

void CTimeLineCtrl::SetTimeSequence(i3UIAnimation2DSprite* pAnimation)
{
	m_pNode = pAnimation;

	DeleteTimeSequence();
	m_MaxDuration = MAX_TIME;

	i3TimeSequenceInfo * pInfo = i3TimeSequenceInfo::new_object_ref();

	i3Node* pNode = i3Node::new_object_ref();
	pNode->SetName(pAnimation->GetName());

	pInfo->SetStartTime(0.0f);
	pInfo->SetNode(pNode);

	m_pTimeSeq = i3TimeSequence::new_object();
	AddSequence(pInfo);
	pInfo->SetDuration(m_pNode->GetLifeTime());


}

void CTimeLineCtrl::DeleteTimeSequence()
{
	if (m_pTimeSeq) I3_SAFE_RELEASE(m_pTimeSeq);
	Invalidate();
}

void CTimeLineCtrl::GetSeqTime(REAL32& startTime, REAL32& endTime)
{
	if (m_pTimeSeq)
	{
		i3TimeSequenceInfo* pSeq = m_pTimeSeq->GetSequence(0);
		startTime = pSeq->GetStartTime();
		endTime = pSeq->GetDuration();
	}
}

void CTimeLineCtrl::SetStartTime(REAL32 fTime)
{
	if (m_pTimeSeq)
	{
		i3TimeSequenceInfo* pSeq = m_pTimeSeq->GetSequence(0);
		pSeq->SetStartTime(fTime);
		Invalidate();
	}
}

void CTimeLineCtrl::SetDuration(REAL32 fTime)
{
	if (m_pTimeSeq)
	{
		i3TimeSequenceInfo* pSeq = m_pTimeSeq->GetSequence(0);
		pSeq->SetDuration(fTime);
		Invalidate();
	}
}

void CTimeLineCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_CurrentNcHit != NC_HIT_SLIDE_BODY)
	{
		i3TDKTimeSeqCtrl::OnMouseMove(nFlags, point);

		REAL32 startTime, endTime;
		char chStartTime[MAX_PATH];
		char chEndTime[MAX_PATH];

		switch (m_MouseMode)
		{
		case MOUSE_MODE_SLIDE_MOVE:
		{
			GetSeqTime(startTime, endTime);

			m_pNode->SetLifeTime(endTime);
		}
		break;
		}
	}
}


void CTimeLineCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{

}