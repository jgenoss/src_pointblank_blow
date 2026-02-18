#if !defined( __I3_ATTR_STACK_MANAGER_H)
#define __I3_ATTR_STACK_MANAGER_H

#include "i3Base.h"
#include "i3RenderAttr.h"
#include "i3AttrStack.h"
#include "i3AttrID.h"
#include "i3RenderAttrListAttr.h"

class I3_EXPORT_SCENE i3AttrStackManager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3AttrStackManager, i3ElementBase);

protected:

	i3AttrStack *			m_pAttrStacks[I3_ATTR_MAX];
	i3AttrStack *			m_apHeadAS[2] = { nullptr, nullptr };
	i3AttrStack *			m_apTailAS[2] = { nullptr, nullptr };
	i3RenderContext *		m_pContext = nullptr;

	i3RenderAttr *			m_apLastAttrTrack[2][ I3_ATTR_MAX];	//m_bActivateAlphaSort 로 배열분기..

	bool					m_bFlushed[I3_ATTR_MAX];
	bool					m_bIgnoreAlphaSort[I3_ATTR_MAX];
	bool					m_bEnable = true;
	bool					m_bActivateAlphaSort = false;


protected:
	void				addModifiedList( i3AttrStack * pStack);
	void				resetModifiedList(void);

public:
	i3AttrStackManager(void);
	virtual ~i3AttrStackManager(void);

	bool		isEnabled(void)												{ return m_bEnable; }
	void		setEnable( bool bFlag)										{ m_bEnable = bFlag; }

	bool		IsAlphaSortActivated() const								{ return m_bActivateAlphaSort; }

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
		m_bFlushed[ID] = true;
	}

	i3AttrStack * GetStack( I3_ATTRID id)				{ return m_pAttrStacks[id]; }

	void			PushAttr( i3RenderAttr * pAttr, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			PushAttrs( i3::vector<i3RenderAttr*>& List, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			PopAttr( UINT32 id, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			PopAttrs( i3::vector<i3RenderAttr*>& List, INT32 Priority = I3_DEF_ATTR_PRIORITY);
	void			FlushPushedAttrs( i3RenderAttrListAttr * pOutput);

	INT32			GatherCurrentAttrsForAlphaSort( i3RenderAttrListAttr * pOutput);
	void			ResetCurrentAttrsForAlphaSort( i3RenderAttrListAttr* pIn, INT32 endDefaultIndex);


	void			EnableAttrs( INT32 * pIDTable, INT32 count, bool bFlag);
};

#endif
