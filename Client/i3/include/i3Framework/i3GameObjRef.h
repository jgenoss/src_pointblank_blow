#if !defined( __I3_GAMEOBJ_REF_H)
#define __I3_GAMEOBJ_REF_H

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3GameObjRef : public i3EventReceiver
{
	I3_EXPORT_CLASS_DEFINE( i3GameObjRef, i3EventReceiver);
protected:
	i3GameObj *			m_pRefObj = nullptr;

public:
	virtual ~i3GameObjRef(void);

	i3GameObj *			getRefObject(void)					{ return m_pRefObj; }
	void				setRefObject( i3GameObj * pObj)		{ I3_REF_CHANGE( m_pRefObj, pObj); }

	virtual void		SetName( const i3::rc_string& strName);
};

#endif
