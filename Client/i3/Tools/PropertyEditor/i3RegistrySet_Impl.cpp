#include "stdafx.h"

#include "i3TDKListCtrl_Impl.h"
#include "i3RegistrySet_Impl.h"

I3_CLASS_INSTANCE( i3RegistrySet_Impl); //, i3RegistrySet);

INT32 i3RegistrySet_Impl::GetNationCount()
{
	return (INT32)(m_pListCtrlPtr->GetNationListRef()->size());
}

void i3RegistrySet_Impl::_XML_ConvertDataToXML_Name(UINT32 I3Reg_Type, 
											   size_t Index, const char* szPreName,
											   char * _DstBuf, size_t _SizeInBytes)
{
	if( Index == size_t(-1) )
		sprintf_s( _DstBuf, _SizeInBytes, "%s", szPreName);
	else
	{
		std::vector< std::string >* List = m_pListCtrlPtr->GetNationListRef();

		sprintf_s( _DstBuf, _SizeInBytes, "%s_%s", szPreName, (*List)[Index].c_str() );
	}
}