#include "i3FrameworkPCH.h"
#include "i3GameObjRef.h"


I3_CLASS_INSTANCE( i3GameObjRef);


i3GameObjRef::~i3GameObjRef(void)
{
	I3_SAFE_RELEASE( m_pRefObj);
}

void i3GameObjRef::SetName(  const i3::rc_string& strName)
{
	i3EventReceiver::SetName( strName);

	if( m_pRefObj != nullptr)
	{
		m_pRefObj->SetName( strName);
	}
}
