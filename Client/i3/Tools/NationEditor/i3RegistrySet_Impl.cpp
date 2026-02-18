#include "stdafx.h"

#include "i3RegistrySet_Impl.h"

#include "NationEditorDlg.h"

I3_CLASS_INSTANCE( i3RegistrySet_Impl); //, i3RegistrySet);

void i3RegistrySet_Impl::_XML_ConvertDataToXML_Name(UINT32 I3Reg_Type, 
											   size_t Index, const char* szPreName,
											   char * _DstBuf, size_t _SizeInBytes)
{
	if( Index == size_t(-1) )
		sprintf_s( _DstBuf, _SizeInBytes, "%s", szPreName);
	else
	{
		CHListBox* List = m_pNationListPtr->GetNationCtrl();
		sprintf_s( _DstBuf, _SizeInBytes, "%s_%s", szPreName, 
			List->GetCurrentString( Index ).GetString() );
	}
}