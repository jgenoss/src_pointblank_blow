#include "pch.h"
#include "HitMapTimeSeqCtrl.h"
#include "Prop_LvElm_HitMap.h"
#include "i3Base/string/ext/ftoa.h"

IMPLEMENT_DYNAMIC(CHitMapTimeSeqCtrl, i3TDKTimeSeqCtrl)

BEGIN_MESSAGE_MAP(CHitMapTimeSeqCtrl, i3TDKTimeSeqCtrl)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
CHitMapTimeSeqCtrl::CHitMapTimeSeqCtrl()
{
	m_bUseSlideCustomSize = true;
}


CHitMapTimeSeqCtrl::~CHitMapTimeSeqCtrl()
{
}


 void CHitMapTimeSeqCtrl::SetTimeSequence(REAL32 fPlayTime)
 {
	 DeleteTimeSequence();

	 i3Node* pNode = i3Node::new_object_ref();
	 pNode->SetName("Play Time");
	 i3TimeSequenceInfo * pInfo = i3TimeSequenceInfo::new_object_ref();
	 m_MaxDuration = fPlayTime;
	 pInfo->SetStartTime(0.0f);
	 pInfo->SetNode(pNode);

	 m_pTimeSeq = i3TimeSequence::new_object();
	 AddSequence(pInfo);
	 pInfo->SetDuration(fPlayTime);

	 char chTemp[MAX_PATH];
	 sprintf_s(chTemp, "Time Line : %dMinute", (int)fPlayTime);
	 _SetTitleText(chTemp);
 }

 void CHitMapTimeSeqCtrl::DeleteTimeSequence()
 {
	 if (m_pTimeSeq) I3_SAFE_RELEASE(m_pTimeSeq);
	 Invalidate();
 }

 void CHitMapTimeSeqCtrl::GetSeqTime(REAL32& startTime, REAL32& endTime)
 {
	 if (m_pTimeSeq)
	 {
		 i3TimeSequenceInfo* pSeq = m_pTimeSeq->GetSequence(0);
		 startTime = pSeq->GetStartTime();
		 endTime = pSeq->GetDuration();
	 }
 }

 void CHitMapTimeSeqCtrl::SetStartTime(REAL32 fTime)
 {
	 if (m_pTimeSeq)
	 {
		 i3TimeSequenceInfo* pSeq = m_pTimeSeq->GetSequence(0);
		 pSeq->SetStartTime(fTime);
		 Invalidate();
	 }
 }

 void CHitMapTimeSeqCtrl::SetDuration(REAL32 fTime)
 {
	 if (m_pTimeSeq)
	 {
		 i3TimeSequenceInfo* pSeq = m_pTimeSeq->GetSequence(0);
		 pSeq->SetDuration(fTime);
		 Invalidate();
	 }
 }

 void CHitMapTimeSeqCtrl::OnMouseMove(UINT nFlags, CPoint point)
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
			 i3::ftoa(startTime, chStartTime);
			 i3::ftoa(endTime + startTime, chEndTime);

			 m_pHitMapCommonProperty->SetSeqTime(chStartTime, chEndTime);
		 }
		 break;
	 }
 }
