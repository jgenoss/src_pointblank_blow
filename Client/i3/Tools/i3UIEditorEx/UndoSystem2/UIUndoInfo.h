#if !defined( __I3UICONTROL_UNDO_INFO_H__)
#define __I3UICONTROL_UNDO_INFO_H__

//#include "../i3UIControl.h"
//#include "../i3UIScene.h"

//#if defined( I3_DISTRIBUTE)

//**************************************************************************************************************
// NOTE
//
// UndoInfo는 Reference Counting 방식을 이용하여 m_pElement를 관리한다.
// 예를들어, 어떤 컨트롤이 삭제될 때, 
// 그 컨트롤은 disable되고 부모에서 제거되기만 할 뿐, 
// 메모리 release는 발생하지 않는다.
//
//**************************************************************************************************************

class UIUndoInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( UIUndoInfo, i3NamedElement);

protected:
	i3::vector<UIUndoInfo*>	m_ChildList;		//하위 undo list   undo --+-- undo
											//						  +-- undo
	i3UIControl		*	m_pElement;

public:
	UIUndoInfo(void);
	virtual ~UIUndoInfo(void);

	virtual void	setObject( i3UIControl * pObj)	{	I3_REF_CHANGE(m_pElement, pObj);	}
	i3UIControl	*	getObject(void)					{	return m_pElement;	}

	INT32	GetChildInfoCount(void) const {	return (INT32)m_ChildList.size(); }

	const i3::vector<UIUndoInfo*>&	GetChildInfoList( void) const	{	return m_ChildList;	}

	INT32			GetElementList( i3::vector<i3UIControl*>& List);

public:
	void			AddChildUndoInfo( UIUndoInfo * pChildInfo);

	virtual bool	Undo( i3UIScene* pScene);
	virtual bool	Redo( i3UIScene* pScene);

	//Debugging purpose
	virtual void	PrintUndoInfo( void);
};

//#endif	// I3_DISTRIBUTE

#endif	//__I3UICONTROL_UNDO_INFO_H__
