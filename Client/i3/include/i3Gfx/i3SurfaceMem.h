#if !defined( __I3G_SURFACEMEM_H)
#define __I3G_SURFACEMEM_H


#include "i3Surface.h"
#include "i3Clut.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)

class i3SurfaceMem : public i3Surface
{
	I3_CLASS_DEFINE( i3SurfaceMem, i3Surface);
protected:
	char *				m_pBuffer = nullptr;

	friend class i3Texture;

public:
	virtual ~i3SurfaceMem(void);

	virtual bool		Lock( I3G_LOCKMETHOD lock = 0) override;
	virtual void		Unlock(void) override;

	virtual void		MakeRuntimeForm( bool bMakeLinear) override;

	void				CreateBuffer( INT32 size);
	char *				getBuffer()								{ return m_pBuffer; };
};		

#endif // __I3G_SURFACEMEM_H

