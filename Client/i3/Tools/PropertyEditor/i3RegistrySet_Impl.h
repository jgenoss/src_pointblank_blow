#pragma once

//#include "i3RegistrySet.h"

class i3TDKListCtrl_Impl;

class i3RegistrySet_Impl :	public i3RegistrySet
{
	I3_CLASS_DEFINE( i3RegistrySet_Impl, i3RegistrySet );

public:
	virtual INT32 GetNationCount();

protected:
	i3TDKListCtrl_Impl* m_pListCtrlPtr;

protected:
	virtual void _XML_ConvertDataToXML_Name(UINT32 I3Reg_Type, 
		size_t Index, const char* szPreName,
		char * _DstBuf, size_t _SizeInBytes);

public:
	void SetListCtrlPtr(i3TDKListCtrl_Impl* Ptr)	{	m_pListCtrlPtr = Ptr;	}
};