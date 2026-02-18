#pragma once

#include "i3TDKTimeSeqCtrl.h"

class cMyTDKTimeSeqCtrl : public i3TDKTimeSeqCtrl
{
	DECLARE_DYNAMIC(cMyTDKTimeSeqCtrl)
public:
	cMyTDKTimeSeqCtrl(void);
	virtual ~cMyTDKTimeSeqCtrl(void);

	void ReDraw();
	MOUSE_MODE GetMouseState() { return m_MouseMode; }


	void		OnLoadSound( POINT * pPoint);
	void		_PrepareTimeSeq(void);

	virtual	void	OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx);
	virtual	void	OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx);
	virtual void	OnChangeCurrentTime( REAL32 tm);
	virtual	void	OnChangeTimeSequence(void);
protected:
	GICShapeAI *		m_pShape;
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

};
