#if !defined( __I3_DISPLAY_ATTR_LIST_H)
#define __I3_DISPLAY_ATTR_LIST_H

#include "i3Base.h"
#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3RenderAttrListAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3RenderAttrListAttr, i3RenderAttr);

protected:
	i3::vector<i3RenderAttr*>	m_List;
	REAL32				m_Dist = 1.0f;

protected:
	INT32				_FindInsertPoint( REAL32 dist);

public:
	i3RenderAttrListAttr(void);
	virtual ~i3RenderAttrListAttr(void);

	i3RenderAttr *	Get( UINT32 idx)
	{
		return m_List[idx];
	}
	
	REAL32			getDist(void)							{ return m_Dist; }
	void			setDist( REAL32 z)						{ m_Dist = z; }

	INT32			GetCount(void)						{ return INT32(m_List.size()); }

	i3::vector<i3RenderAttr*>&			GetAttrList(void)					{ return m_List; }
	const i3::vector<i3RenderAttr*>&	GetAttrList(void) const				{ return m_List; }

	void		AppendAttr( i3RenderAttr * pAttr);
	void		AddGroup( i3RenderAttrListAttr * pAttrList);
	void		Reset(void);

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override { return false; }

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
	#endif

};

#endif
