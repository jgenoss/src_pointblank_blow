#if !defined( __I3_LOD_SELECTOR_H)
#define __I3_LOD_SELECTOR_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3SceneTracer.h"

class I3_EXPORT_SCENE i3LODSelector : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3LODSelector);

protected:
	REAL32 m_L;
	REAL32 m_K;

public:
	i3LODSelector(void);
	virtual ~i3LODSelector(void);

	void			SetLK( REAL32 l, REAL32 k)			{ m_L = l; m_K = k; }
	REAL32			GetL(void)							{ return m_L; }
	REAL32			GetK(void)							{ return m_K; }

	INT32			SelectCurrentLevel( i3SceneTracer * pTracer);

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
