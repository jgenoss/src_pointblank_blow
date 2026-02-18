#if !defined( __I3_VOLUME_H)
#define __I3_VOLUME_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
#include "../i3Base/i3MetaTemplate.h"
#include "../i3Base/i3PersistantElement.h"
#include "i3MathType.h"
#include "i3Vector.h"

class I3_EXPORT_MATH i3Volume : public i3PersistantElement
{
	I3_ABSTRACT_CLASS_DEFINE( i3Volume);

protected:
	BOOL		m_bEmpty;

public:
	i3Volume(void);

	inline
	BOOL	IsEmpty(void)				{ return m_bEmpty; }

	virtual void GetCenter( RT_VEC3D * pVec) = 0;
};

#endif
