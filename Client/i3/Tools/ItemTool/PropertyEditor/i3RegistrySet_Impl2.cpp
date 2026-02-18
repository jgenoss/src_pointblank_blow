#include "StdAfx.h"
#include "i3RegistrySet_Impl2.h"
#include "i3TDKRegistryCtrl_Impl2.h"


I3_CLASS_INSTANCE( i3RegistrySet_Impl2); //, i3RegistrySet_Impl);


i3RegistrySet_Impl2::i3RegistrySet_Impl2(void)
{
}

i3RegistrySet_Impl2::~i3RegistrySet_Impl2(void)
{
}


void i3RegistrySet_Impl2::SelectData( i3RegKey* pKey )
{
	if( m_pListCtrlPtr )
		m_pListCtrlPtr->SetRegKey(pKey, CListSubCtrl::ACT_COPYCELL);
}
