#if !defined( __OBJECT_TEMPLATE_H)
#define __OBJECT_TEMPLATE_H

#include "ObjectState.h"

class CObjectTemplate : public i3GameObj
{
	I3_CLASS_DEFINE( CObjectTemplate, i3GameObj);
protected:
	REAL32			m_MaxHP;
	i3::vector<CObjectState*>	m_StateList;
	INT32			m_idxOldState;
	INT32			m_idxCurState;

	i3::vector<i3SceneGraphInfo*>	m_EffectList;

	REAL32			m_timeLocal;

protected:
	void			_SetCurrentSg(void);

public:
	CObjectTemplate(void);
	virtual ~CObjectTemplate(void);

	REAL32			getMaxHP(void)						{ return m_MaxHP; }
	void			setMaxHP( REAL32 hp)				{ m_MaxHP = hp; }

	INT32			getStateCount(void)					{ return (INT32)m_StateList.size(); }
	CObjectState *	getState( INT32 idx)				{ return m_StateList[idx]; }
	void			RemoveAllState(void);
	void			AddState( CObjectState * pState);
	void			LoadAndAddState( const char * pszPath);
	void			ChangeStateSg( CObjectState * pState, const char * pszPath);
	void			SetCurrentState( INT32 idx);
	INT32			GetStateIndex( CObjectState * pObj)	{ return i3::vu::int_index_of(m_StateList, pObj); }

	INT32			LoadEffect( const char * pszEffect);
	INT32			AddEffect( i3SceneGraphInfo * pInfo);
	void			RemoveAllEffect(void);
	INT32			FindEffect( const char * pszPath);
	INT32			getEffectCount(void)					{ return (INT32)m_EffectList.size(); }
	i3SceneGraphInfo *	getEffect( INT32 idx)				{ return m_EffectList[ idx]; }

	void			BuildConvex( INT32 idx);

	bool			Flush(void);
	bool			Export( const char * pszPath);

	void			OnUpdate( REAL32 tm);
	virtual	bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif
