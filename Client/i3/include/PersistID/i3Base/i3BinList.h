#if !defined( __CLBINLIST_H)
#define __CLBINLIST_H

#include "i3List.h"
#include "i3MetaTemplate.h"

class I3_EXPORT_BASE i3BinList : public i3List 
{
	I3_CLASS_DEFINE( i3BinList);

protected:
	COMPAREPROC m_pCompProc;

protected:
	INT32 GetAddPosition( void * p);

public:
	i3BinList( INT32 AllocUnit = 16) ;
	virtual ~i3BinList(void);

	void SetCompareProc( COMPAREPROC pUserProc)						{ m_pCompProc = pUserProc; }
	COMPAREPROC GetCompareProc(void)								{ return m_pCompProc; }

	virtual INT32 Add( void * p);
	virtual INT32 FindItem( void * p);
};

#endif
