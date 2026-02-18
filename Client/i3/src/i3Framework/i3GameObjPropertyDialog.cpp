#include "i3FrameworkPCH.h"
#include "i3GameObjPropertyDialog.h"

#if defined( I3_WINDOWS) && defined( I3_DEBUG)
IMPLEMENT_DYNAMIC(i3GameObjPropertyDialog, CDialog)

i3GameObjPropertyDialog::~i3GameObjPropertyDialog(void)
{
	I3_SAFE_RELEASE( m_pObject);
}

BEGIN_MESSAGE_MAP(i3GameObjPropertyDialog, CDialog)
END_MESSAGE_MAP()

void i3GameObjPropertyDialog::SetObject( i3ElementBase * pObj)
{
	I3_REF_CHANGE( m_pObject, pObj);
}

#endif
