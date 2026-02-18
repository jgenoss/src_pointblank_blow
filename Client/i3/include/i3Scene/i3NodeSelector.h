#if !defined( __I3_NODE_SELECTOR_H)
#define __I3_NODE_SELECTOR_H

#include "i3Base.h"
#include "i3Node.h"

class I3_EXPORT_SCENE i3NodeSelector : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3NodeSelector, i3Node);

protected:
	UINT64		m_Mask = 0;

public:
	i3NodeSelector(void);

	UINT64		GetMask(void)						{ return m_Mask;}
	bool		IsMask( UINT64 mask)				{ return (m_Mask & mask) != 0; }
	void		SetMask( UINT64 mask)				{ m_Mask = mask; }
	void		AddMask( UINT64 mask)				{ m_Mask |= mask; }
	void		RemoveMask( UINT64 mask)			{ m_Mask &= ~(mask); }

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
