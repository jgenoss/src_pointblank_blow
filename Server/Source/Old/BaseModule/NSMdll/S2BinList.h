#if !defined( __CLBINLIST_H)
#define __CLBINLIST_H

#include "S2List.h"

class EXPORT_BASE S2BinList : public S2List 
{
protected:
	COMPAREPROC m_pCompProc;

	INT32 GetAddPosition( void* p ) const;

public:
	S2BinList( INT32 i32AllocUnit = 16 );
	virtual ~S2BinList(void);

	void SetCompareProc( COMPAREPROC pUserProc )				{ m_pCompProc = pUserProc; }
	COMPAREPROC GetCompareProc(void)							{ return m_pCompProc; }

	virtual INT32 Add( void* p );
	virtual INT32 FindItem( void* p );
};

#endif
