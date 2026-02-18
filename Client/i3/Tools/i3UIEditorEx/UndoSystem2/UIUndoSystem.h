#if !defined( __I3UICONTROL_UNDO_SYSTEM_H__)
#define __I3UICONTROL_UNDO_SYSTEM_H__

#include "UIUndoInfo.h"

//#if defined( I3_DISTRIBUTE)

class UIUndoSystem : public i3ElementBase
{
	I3_CLASS_DEFINE( UIUndoSystem, i3ElementBase);

	friend class UIUndoInfo;

protected:
	i3::vector<UIUndoInfo*>	m_UndoList;
	i3::vector<UIUndoInfo*>	m_RedoList;

	i3UIScene*			m_pScene;

	bool				m_isUserAction;			//User에 의한 Action여부
	bool				m_bUndoBatchEnable;		//여러개의 동작을 한번에 수행하는경우 플래그
	bool				m_bPendingEnable;

	UIUndoInfo	*	m_pBatchParentInfo;

public:
	UIUndoSystem(void);
	virtual ~UIUndoSystem(void);

	void		ResetSystem(void);
	void		SetCurrentScene( i3UIScene* pScene);

protected:
	void		_BeginUndo(void)							{ m_isUserAction = false;}
	void		_EndUndo(void)								{ m_isUserAction = true;}

	void		getInfoList( i3::vector<UIUndoInfo*>& Out);

public:
	void		BeginBatch(void);
	void		EndBatch(void);
	bool		SetPendingEnable( bool bPending = false)	{ m_bPendingEnable = bPending; return true;}

	bool		AddUndoInfo( UIUndoInfo * pInfo);

	bool		Undo(void);
	bool		Redo(void);

	INT32		getUndoCount(void)	{	return (INT32)m_UndoList.size();}
	INT32		getRedoCount(void)	{	return (INT32)m_RedoList.size();}

	// User의 Action에의한 행동인지 Undo/Redo에의한 행동인지 
	bool		isUserAction(void)							{ return m_isUserAction;}

protected:
	void		_rec_ADDUNDO_Create( UIUndoInfo* pInfo, i3UIControl* pObj);	// Parent부터 순서대로 
	void		_rec_ADDUNDO_Delete( UIUndoInfo* pInfo, i3UIControl* pObj);	// Child부터 순서대로

public:
	bool		ADDUNDO_Create( i3UIControl * pObj);
	bool		ADDUNDO_Create( const i3::vector<i3UIControl*>& ObjList);

	bool		ADDUNDO_Delete( i3UIControl * pObj);
	bool		ADDUNDO_Delete( const i3::vector<i3UIControl*>& ObjList);

	bool		ADDUNDO_Transform( i3UIControl * pObj);
	bool		ADDUNDO_Transform( const i3::vector<i3UIControl*>& ObjList);

	bool		ADDUNDO_Edit( i3UIControl * pObj);
	bool		ADDUNDO_Edit( const i3::vector<i3UIControl*>& ObjList);

#ifdef _DEBUG
	const i3::vector<UIUndoInfo*>&		getUndoList( void) const	{	return m_UndoList;	}
	const i3::vector<UIUndoInfo*>&		getRedoList( void) const 	{	return m_RedoList;	}
#endif
};


namespace UIUndo
{
	UIUndoSystem*	getUndoSystem( void);

	bool	Init(i3UIScene * pScene);
	bool	Reset(void);
	bool	Close(void);

	bool	PendingEnable(bool bPending = true);

	bool	BeginBatch(void);
	bool	EndBatch(void);

	bool	Undo(void);
	bool	Redo(void);

	INT32	GetUndoCount(void);
	INT32	GetRedoCount(void);

	bool	ADD_Create( i3UIControl * pObj);
	bool	ADD_Create( const i3::vector<i3UIControl*>& ObjList);

	bool	ADD_Delete( i3UIControl * pObj);
	bool	ADD_Delete( const i3::vector<i3UIControl*>& ObjList);

	bool	ADD_Transform( i3UIControl * pObj);
	bool	ADD_Transform( const i3::vector<i3UIControl*>& ObjList);

	bool	ADD_Edit( i3UIControl * pObj);
	bool	ADD_Edit( const i3::vector<i3UIControl*>& ObjList);

#ifdef _DEBUG
	void	TraceCurrentStack( void);
#endif

	/*bool	Create( i3UIControl* pObj);
	bool	Delete( i3UIControl* pObj);*/
};

extern I3_EXPORT_FRAMEWORK UIUndoSystem * g_pUIUndoSystem;

//#endif	// I3_DISTRIBUTE

#endif	//__I3UICONTROL_UNDO_SYSTEM_H__
