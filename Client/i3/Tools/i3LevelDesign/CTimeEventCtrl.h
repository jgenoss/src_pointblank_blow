#if !defined ( _TIME_EVENT_CTRL_H__)
#define _TIME_EVENT_CTRL_H__

#define			WM_USER_TIME_EVENT_EDITED			WM_USER + 186

class CTimeEventCtrl : public i3TDKTimeEventCtrl
{
protected:
	i3Action		*	m_CurAction;

protected:
	void			_OnActionPlaySound( i3TimeEventInfo * pInfo, i3ActionPlaySound * pAction);
	void			_OnActionSetWorldSection( i3TimeEventInfo * pInfo, i3ActionSetWorldSection * pAction);
	void			_OnActionQuerySpecDialog( i3TimeEventInfo * pInfo, i3Action * pAction);
	void			_OnActionSetMapSector( i3TimeEventInfo * pInfo, i3ActionSetMapSector * pAction);

public:
	virtual	void	OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx) override;
	virtual	void	OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx) override;

	void			OnPopupHotEdit( INT32 idx);

	CTimeEventCtrl(void);
	virtual ~CTimeEventCtrl(void);
};
#endif // _TIME_EVENT_CTRL_H__