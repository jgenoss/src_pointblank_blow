#if !defined( __I3_ATTR_STACK_MANAGER_H)
#define __I3_ATTR_STACK_MANAGER_H

#include "i3Base.h"
#include "i3RenderAttr.h"
#include "i3AttrStack.h"
#include "i3AttrID.h"
#include "i3RenderAttrListAttr.h"

class I3_EXPORT_SCENE i3AttrStackManager : public i3ElementBase
{
	I3_CLASS_DEFINE( i3AttrStackManager);

protected:
	bool					m_bEnable;
	i3AttrStack *			m_pAttrStacks[I3_ATTR_MAX];
	i3AttrStack *			m_pHeadAS;
	i3AttrStack *			m_pTailAS;
	i3RenderContext *		m_pContext;

	BOOL					m_bFlushed[I3_ATTR_MAX];
	i3RenderAttr *			m_pLastAttrTrack[ I3_ATTR_MAX];

	static BOOL				m_DefaultMap[I3_ATTR_MAX];

protected:
	void				addModifiedList( i3AttrStack * pStack);
	void				resetModifiedList(void);

public:
	i3AttrStackManager(void);
	virtual ~i3AttrStackManager(void);

	bool		isEnabled(void)												{ return m_bEnable; }
	void		setEnable( bool bFlag)										{ m_bEnable = bFlag; }

	void		SetRenderContext( i3RenderContext * pContext)
	{
		m_pContext = pContext;
	}

	void Reset(void);

	i3AttrStack *		GetAttrStackByID( UINT32 ID)
	{
		return m_pAttrStacks[ ID];
	}

	void				SetFlushState( UINT32 ID)
	{
		m_bFlushed[ID] = TRUE;
	}

	i3AttrStack * GetStack( I3_ATTRID id)				{ return m_pAttrStacks[id]; }

	void			PushAttr( i3RenderAttr * pAttr, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			PushAttrs( i3List * pList, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			PopAttr( UINT32 id, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			PopAttrs( i3List * pList, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			FlushPushedAttrs( i3RenderAttrListAttr * pOutput);

	void			EnableAttrs( INT32 * pIDTable, INT32 count, BOOL bFlag);
};

#endif
