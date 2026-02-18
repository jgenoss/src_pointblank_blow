#if !defined( __I3_PROJECT_OFFSET_ATTR_H)
#define __I3_PROJECT_OFFSET_ATTR_H
#include "i3Base.h"
#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ProjectOffsetAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE(i3ProjectOffsetAttr, i3RenderAttr);
protected:
	VEC3D	m_vec;
public:
	i3ProjectOffsetAttr(void);
	virtual ~i3ProjectOffsetAttr(void);

	void			Set(VEC3D* pPos)
	{
		i3Vector::Set(&m_vec, pPos->x, pPos->y, pPos->z);
	}

	VEC3D *		Get(void)
	{
		return &m_vec;
	}

	virtual void Apply(i3RenderContext * pContext) override;
	virtual	void CopyTo(i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame(i3RenderAttr * pAttr) override;

};

#endif
