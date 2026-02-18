#if !defined( __I3_DISPLAY_ATTR_LIST_H)
#define __I3_DISPLAY_ATTR_LIST_H

#include "i3Base.h"
#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3RenderAttrListAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3RenderAttrListAttr);

protected:
	i3List				m_List;
	REAL32				m_Dist;

protected:
	INT32				_FindInsertPoint( REAL32 dist);

public:
	i3RenderAttrListAttr(void);
	virtual ~i3RenderAttrListAttr(void);

	i3RenderAttr *	Get( UINT32 idx)
	{
		return (i3RenderAttr *) m_List.Items[idx];
	}
	
	REAL32			getDist(void)							{ return m_Dist; }
	void			setDist( REAL32 z)						{ m_Dist = z; }

	INT32			GetCount(void)						{ return m_List.GetCount(); }

	i3List *		GetAttrList(void)					{ return &m_List; }

	void		AppendAttr( i3RenderAttr * pAttr);
	void		AddGroup( i3RenderAttrListAttr * pAttrList);
	void		Reset(void);

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump(void);
	#endif

};

#endif
