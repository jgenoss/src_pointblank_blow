#pragma once

#if defined( TEST_AUTOMATION)

#include "CaseRunSDK.h"

using namespace CaseRun;

class CaseRunCapture : public i3InputDispatchManager
{
	I3_CLASS_DEFINE(CaseRunCapture, i3InputDispatchManager);

protected:
	CGameFramework *	m_pFramework;
	i3InputDispatchManager *	m_pOldManager;
	Interface *			m_pInterface;

	bool			m_bCapturing;
	vector<Node_KeyDown*>	m_KeyDownList;
	vector<NodeBase*>	m_CaptureList;
	INT64				m_tmStart;
	i3UICaptionControl * m_pEditBox;

	i3UIControl *		m_pMousePickedCtrl;
protected:

	REAL32			GetElapsed(void);
	void			AddNode(NodeBase * pNode);
	void			RemoveNode(INT32 idx);
	void			RemoveAllNodes();
	void			GetFullPath(i3UIControl * pCtrl, std::string & str);
	void			KeyboardCapture(REAL32 tm);
	void			CaptureInputAndRelease(void);
	Node_KeyDown *	IsInKeyDownList(I3I_KEY key);

	bool			PreMouseCapture( POINT pt, BUTTON btn, bool bDblClick);
	bool			PostMouseCapture( BUTTON btn, bool bDblClick);

	// CaseRunCapture Refining
	void			RefineCapture(void);
	INT32			FindEscapeSeq(INT32 idxStart);
	bool			IsEscSeq(NodeBase * pNode);

public:
	CaseRunCapture(void);
	virtual ~CaseRunCapture(void);

	bool			Create(CGameFramework * pFramework, Interface * pInterface);
	bool			Start(void);
	bool			End(void);

	bool			isCapturing(void) const			{ return m_bCapturing; }

	void			OnInput( REAL32 tm);

	virtual void	OnChangeFocus( i3Framework * pFramework, i3EventReceiver * pFrom, i3EventReceiver * pTo) override;
	virtual bool	DispatchLButtonDown(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchLButtonUp(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchLButtonDblClk(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchMButtonDown(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchMButtonUp(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchRButtonDown(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchRButtonUp(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchMouseMove(UINT32 nFlag, POINT point) override;
	virtual bool	DispatchMouseWheel(UINT nFlag, short zDelta, POINT point) override;
	virtual bool	DispatchKeyDown(UINT32 nKey) override;
	virtual bool	DispatchKeyUp(UINT32 nKey) override;
	virtual bool	DispatchActivate( bool bActivate) override;
	virtual bool	DispatchSetFocus(void) override;
	virtual bool	DispatchKillFocus(void) override;
	virtual bool	DispatchInput(void) override;
};

#endif