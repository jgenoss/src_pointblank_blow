#if !defined( __I3_GAMEOBJ_PROPERTY_DLG_H)
#define __I3_GAMEOBJ_PROPERTY_DLG_H

#if defined( I3_DEBUG) && defined( I3_WINDOWS)

#include "i3GameObjBase.h"

class I3_EXPORT_FRAMEWORK i3GameObjPropertyDialog : public CDialog
{
protected:
	i3ElementBase *			m_pObject;

public:
	i3GameObjPropertyDialog( UINT id, CWnd * pParent = NULL);
	virtual ~i3GameObjPropertyDialog(void);

	i3ElementBase *			getObject(void)							{ return m_pObject; }
	virtual void			SetObject( i3ElementBase * pObj);
};
#endif

#endif
