#if !defined( __I3G_SURFACEMEM_H)
#define __I3G_SURFACEMEM_H


#include "i3Surface.h"
#include "i3Clut.h"

class i3SurfaceMem : public i3Surface
{
	I3_CLASS_DEFINE( i3SurfaceMem);
protected:
	char *				m_pBuffer;

	friend class i3Texture;

public:
	i3SurfaceMem(void);
	virtual ~i3SurfaceMem(void);

	virtual void		Lock( I3G_LOCKMETHOD lock = 0);
	virtual void		Unlock(void);

	virtual void		MakeRuntimeForm( bool bMakeLinear);

	void				CreateBuffer( INT32 size);
	char *				getBuffer()								{ return m_pBuffer; };
};		

#endif // __I3G_SURFACEMEM_H

