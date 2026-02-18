#if !defined( __I3_GAMEOBJ_PROPERTY_DLG_H)
#define __I3_GAMEOBJ_PROPERTY_DLG_H

#if defined( I3_WINDOWS) && defined( I3_DEBUG)

#include "i3GameObjBase.h"

class I3_EXPORT_FRAMEWORK i3GameObjPropertyDialog : public CDialog
{
	DECLARE_DYNAMIC(i3GameObjPropertyDialog)

protected:
	i3ElementBase *			m_pObject = nullptr;

protected:
	DECLARE_MESSAGE_MAP()

public:
	i3GameObjPropertyDialog(UINT id, CWnd * pParent = nullptr) : CDialog(id, pParent) {}
	virtual ~i3GameObjPropertyDialog(void);

	i3ElementBase *			getObject(void)							{ return m_pObject; }
	virtual void			SetObject( i3ElementBase * pObj);
};
#endif

#endif
