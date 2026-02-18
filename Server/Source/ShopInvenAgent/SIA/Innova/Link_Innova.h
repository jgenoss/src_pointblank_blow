#ifndef ___LINK_INNOVA_H__
#define ___LINK_INNOVA_H__

#include "LinkBase.h"

class cLink_Innova : public CLinkBase
{

public		: 
	cLink_Innova(); 
	virtual ~cLink_Innova(); 

	virtual BOOL 			OnCreate( INT32 i32Idx, NSM_CRingBuffer* pInputBuffer, NSM_CRingBuffer* pOutputBuffer );
	virtual void			OnDestroy(void);

	virtual BOOL			InputProcess( LINK_INPUT * pInput );
};

#endif	// __MODULE_LINK_HTTP_

