#if !defined( __I3_GAMEOBJ_REF_H)
#define __I3_GAMEOBJ_REF_H

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3GameObjRef : public i3EventReceiver
{
	I3_CLASS_DEFINE( i3GameObjRef);
protected:
	i3GameObj *			m_pRefObj;

public:
	i3GameObjRef(void);
	virtual ~i3GameObjRef(void);

	i3GameObj *			getRefObject(void)					{ return m_pRefObj; }
	void				setRefObject( i3GameObj * pObj)		{ I3_REF_CHANGE( m_pRefObj, pObj); }

	virtual void		SetName( const char * pszName);
};

#endif
