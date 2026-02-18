#pragma once
class CProp_LvElm_HitMap;
class CHitMapTimeSeqCtrl : public i3TDKTimeSeqCtrl
{
	DECLARE_DYNAMIC(CHitMapTimeSeqCtrl)

protected:
	CProp_LvElm_HitMap* m_pHitMapCommonProperty;

public:
	CHitMapTimeSeqCtrl();
	virtual ~CHitMapTimeSeqCtrl();

	void SetTimeSequence(REAL32 fPlayTime);
	void DeleteTimeSequence();
	void GetSeqTime(REAL32& startTime, REAL32& endTime);
	void SetStartTime(REAL32 fTime);
	void SetDuration(REAL32 fTime);

	void SetHitMapCommonProperty(CProp_LvElm_HitMap* pProperty) {m_pHitMapCommonProperty = pProperty;}

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	DECLARE_MESSAGE_MAP()
};


