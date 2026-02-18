#pragma once

#include "GICShapeAI.h"
#include "MyTDKTimeSeqCtrl.h"
// CTimeSeqCtrl

class CTimeSeqCtrl : public cMyTDKTimeSeqCtrl
{
	DECLARE_DYNAMIC(CTimeSeqCtrl)

protected:
	GICShapeAI *		m_pShape;

protected:
	void		OnLoadI3S( POINT * pPoint);
	void		OnLoadSound( POINT * pPoint);

	void		_PrepareTimeSeq(void);

public:
	CTimeSeqCtrl();
	virtual ~CTimeSeqCtrl();

	void			SetShape( GICShapeAI * pShape);

	virtual	void	OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx);
	virtual	void	OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx);
	virtual void	OnChangeCurrentTime( REAL32 tm);
	virtual	void	OnChangeTimeSequence(void);

	virtual void	OnSelect( INT32 idx);

protected:
	DECLARE_MESSAGE_MAP()
};


