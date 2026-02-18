#if !defined( __I3_VOLUME_H)
#define __I3_VOLUME_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
#include "../i3Base/i3PersistantElement.h"
#include "i3MathType.h"
#include "i3Vector.h"

class I3_EXPORT_MATH i3Volume : public i3PersistantElement
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3Volume, i3PersistantElement);

protected:
	bool		m_bEmpty = true;

public:
	inline
	bool	IsEmpty(void)				{ return m_bEmpty; }

	virtual void GetCenter( VEC3D * pVec) = 0;
};

#endif
