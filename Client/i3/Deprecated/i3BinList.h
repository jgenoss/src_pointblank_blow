#if !defined( __CLBINLIST_H)
#define __CLBINLIST_H

#include "../Deprecated/i3List.h"
#include "../include/i3Base/i3MetaTemplate.h"

class I3_EXPORT_BASE i3BinList : public i3List 
{
	I3_CLASS_DEFINE( i3BinList);

protected:
	COMPAREPROC m_pCompProc;

public: //protected:
	INT32 GetAddPosition( void * p) const;		// 이제 외부에서도 삽입위치를 알수있도록 조정합니다...

public:
	i3BinList( INT32 AllocUnit = 16) ;
	virtual ~i3BinList(void);

	void SetCompareProc( COMPAREPROC pUserProc)						{ m_pCompProc = pUserProc; }
	COMPAREPROC GetCompareProc(void)								{ return m_pCompProc; }

	virtual INT32 Add( void * p);
	virtual INT32 FindItem( void * p);
};

#endif
