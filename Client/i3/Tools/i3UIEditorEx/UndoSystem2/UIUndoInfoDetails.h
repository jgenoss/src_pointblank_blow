#if !defined( __I3UICONTROL_UNDO_INFO_DETAIL_H__)
#define __I3UICONTROL_UNDO_INFO_DETAIL_H__

#include "UIUndoInfo.h"

//#if defined( I3_DISTRIBUTE)



class UIUndoInfoDetails : public UIUndoInfo
{
	I3_ABSTRACT_CLASS_DEFINE( UIUndoInfoDetails);

protected:
	UI_CONTROL_INFO*	m_pInfo;
	i3List				m_FriendList;	//m_pElement 를 공유하는 UIUndoInfo들

	void		_rec_AddToFriendList( UIUndoInfo* pInfo, INT32 nElementID, i3List* pList);

public:
	UIUndoInfoDetails();
	virtual ~UIUndoInfoDetails();

	virtual void	setObject( i3UIControl * pObj) = 0;
	virtual bool	Undo( i3UIScene * pScene) = 0;
	virtual bool	Redo( i3UIScene * pScene) = 0;

	void			UpdateFriendList( void);
	void			ShareElementWithFriends( void);

};

//#endif	// I3_DISTRIBUTE

#endif	//__I3UICONTROL_UNDO_INFO_DETAIL_H__
