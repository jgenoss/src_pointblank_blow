#if !defined( __I3_REF_STACK_H)
#define __I3_REF_STACK_H

#include "i3List.h"

class I3_EXPORT_BASE i3RefStack : public i3List
{
	I3_CLASS_DEFINE( i3RefStack);
protected:

public:
	i3RefStack(void);
	virtual ~i3RefStack(void);

	void			Push( i3ElementBase * pNode);
	void			Pop( i3ElementBase ** pNode);
	void			Reset(void)							{ Clear(); }

	i3ElementBase *	getTop(void)						
	{ 
		if( GetCount() == 0)
		{
			return NULL;
		}
		return (i3ElementBase *) GetItem( GetCount() - 1);
	}
	bool			IsEmpty(void)
	{
		return	(GetCount() == 0);
	}
};

#endif
