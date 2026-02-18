#pragma once
class CTimeLineCtrl : public i3TDKTimeSeqCtrl
{
	DECLARE_DYNAMIC(CTimeLineCtrl)

protected:
	i3UIAnimation2DSprite* m_pNode;
public:
	CTimeLineCtrl();
	virtual ~CTimeLineCtrl();

	void SetTimeSequence(i3UIAnimation2DSprite* pAnimation);
	void DeleteTimeSequence();
	void GetSeqTime(REAL32& startTime, REAL32& endTime);
	void SetStartTime(REAL32 fTime);
	void SetDuration(REAL32 fTime);


	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
};

